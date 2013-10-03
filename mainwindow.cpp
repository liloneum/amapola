#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "myfilereader.h"
#include "myfilewriter.h"
#include "mysubtitlefileparser.h"
#include "myattributesconverter.h"
#include "SubtitlesParsers/SubRip/subripparser.h"
#include "SubtitlesParsers/DcSubtitle/dcsubparser.h"
#include <QFileDialog>
#include <QColorDialog>
#include <QString>
#include <QKeyEvent>
#include <QMessageBox>
#include <QTimer>


#define SEC_TO_MSEC 1000


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Init UI
    ui->setupUi(this);

    // Hide columns to user
    for ( qint32 i = 5; i < ui->subTable->columnCount(); i++ ) {
        ui->subTable->setColumnHidden(i, true);
    }

    // Install an event filter on all the MainWindow
    //this->installEventFilter(this);
    qApp->installEventFilter(this);

    // Init flags
    mTextPosChangedBySoft = false;
    mTextPosChangedByUser = false;
    mTextFontChangedBySoft = false;
    mTextFontChangedByUser = false;
    mVideoPositionChanged = false;

    // Init the subtitles database
    ui->subTable->initStTable(0);

    //ui->stEditDisplay->setStyleSheet("background: transparent; color: yellow");


    // Init default properties
    qApp->setProperty("prop_FontSize_pt", ui->fontSizeSpinBox->cleanText());
    qApp->setProperty("prop_FontName", ui->fontIdComboBox->currentText());

    if ( ui->fontItalicButton->isChecked() ) {
        qApp->setProperty("prop_FontItalic", "yes");
    }
    else {
        qApp->setProperty("prop_FontItalic", "no");
    }

    if ( ui->fontUnderlinedButton->isChecked() ) {
        qApp->setProperty("prop_FontUnderlined", "yes");
    }
    else {
        qApp->setProperty("prop_FontUnderlined", "no");
    }

    if ( ui->fontColorRButton->isChecked() ) {
        qApp->setProperty("prop_FontColor_rgba", "FFFF0000");
    }
    else if ( ui->fontColorGButton->isChecked() ) {
        qApp->setProperty("prop_FontColor_rgba", "FF00FF00");
    }
    else if ( ui->fontColorBButton->isChecked() ) {
        qApp->setProperty("prop_FontColor_rgba", "FF0000FF");
    }
    else if ( ui->fontColorYButton->isChecked() ) {
        qApp->setProperty("prop_FontColor_rgba", "FFFFFF00");
    }
    else if ( ui->fontColorBlButton->isChecked() ) {
        qApp->setProperty("prop_FontColor_rgba", "FF000000");
    }
    else if ( ui->fontColorWButton->isChecked() ) {
        qApp->setProperty("prop_FontColor_rgba", "FFFFFFFF");
    }
    else if ( ui->fontColorOtherButton->isChecked() ) {
        qApp->setProperty("prop_FontColor_rgba",  ui->fontColorOtherText->text());
    }

    qApp->setProperty("prop_Valign", ui->vAlignBox->currentText());
    qApp->setProperty("prop_Halign", ui->hAlignBox->currentText());
    qApp->setProperty("prop_Vposition_percent", QString::number(ui->vPosSpinBox->value(), 'f', 1));
    qApp->setProperty("prop_Hposition_percent", QString::number(ui->hPosSpinBox->value(), 'f', 1));

    ui->stEditDisplay->defaultSub();

    ui->waveForm->openFile("", "");

    // Init text edit parameter from tool boxes
    this->updateTextPosition();
    this->updateTextFont(false);

    // Init display infos timer
    mpDisplayInfoTimer = new QTimer(this);
    mpDisplayInfoTimer->setInterval(2000);

    // Init the SIGNAL / SLOT connections
    connect(ui->videoPlayer, SIGNAL(durationChanged(qint64)),this, SLOT(updateStEditSize()));
    connect(ui->videoPlayer, SIGNAL(positionChanged(qint64)), this, SLOT(videoPositionChanged(qint64)));
    connect(ui->videoPlayer, SIGNAL(playerStateInfos(QString)), this, SLOT(displayInfo(QString)));

    connect(ui->waveForm, SIGNAL(markerPositionChanged(qint64)), this, SLOT(waveformMarerkPosChanged(qint64)));
    connect(ui->waveForm, SIGNAL(ctrlLeftClickEvent(qint64)), this, SLOT(changeCurrentSubStartTime(qint64)));
    connect(ui->waveForm, SIGNAL(ctrlRightClickEvent(qint64)), this, SLOT(changeCurrentSubEndTime(qint64)));
    connect(ui->waveForm, SIGNAL(shiftLeftClickEvent(qint64)), this, SLOT(shiftCurrentSubtitle(qint64)));

    connect(ui->stEditDisplay, SIGNAL(cursorPositionChanged()), this, SLOT(updateSubTableText()));
    connect(ui->stEditDisplay, SIGNAL(subDatasChanged(MySubtitles)), this, SLOT(updateSubTableDatas(MySubtitles)));
    connect(ui->stEditDisplay, SIGNAL(textLineFocusChanged()), this, SLOT(updateToolBox()));

    connect(ui->subTable, SIGNAL(itemSelectionChanged(qint64)), this, SLOT(currentItemChanged(qint64)));
    connect(ui->subTable, SIGNAL(newTextToDisplay(MySubtitles)), this, SLOT(updateTextEdit(MySubtitles)));

    connect(this->mpDisplayInfoTimer, SIGNAL(timeout()), this, SLOT(eraseInfo()));
}

MainWindow::~MainWindow() {
    delete ui;
}


bool MainWindow::eventFilter(QObject *watched, QEvent *event) {

    // Catch shortcut key events
    if ( event->type() == QEvent::KeyPress ) {

        MySubtitles new_subtitle;
        QList<MySubtitles> subtitle_list;
        qint32 current_subtitle_index;
        qint64 current_position_ms;

        QKeyEvent* key_event = static_cast<QKeyEvent*>(event);

        if ( key_event->key() == Qt::Key_F1 ) {

            current_position_ms = ui->waveForm->currentPositonMs();

            if ( key_event->modifiers() == Qt::ShiftModifier ) {
                // Shift + F1 : shift subtitle to the current position
                this->shiftCurrentSubtitle(current_position_ms);
            }
            else {
                // F1 set current subtitle start time
                this->changeCurrentSubStartTime(current_position_ms);
            }
            return true;
        }
        else if ( key_event->key() == Qt::Key_F2 ) {

            // F2 set current subtitle end time
            current_position_ms = ui->waveForm->currentPositonMs();
            this->changeCurrentSubEndTime(current_position_ms);
            return true;
        }
        else if ( key_event->key() == Qt::Key_F3 ) {

            qint64 end_time_ms = ui->waveForm->currentPositonMs();
            current_subtitle_index = ui->subTable->currentIndex();

            // F3 set current subtitle end time + ( add new subtitle entry or move the next subtitle start time)

            // Change the current subtitle "end time", continue if ok
            if ( this->changeCurrentSubEndTime(end_time_ms) == true ) {

                qint64 start_time_ms = end_time_ms + ( ( (qreal)SEC_TO_MSEC / qApp->property("prop_FrameRate_fps").toReal() ) * qApp->property("prop_SubMinInterval_frame").toReal() );

                // If next subtitle exist
                if ( ( current_subtitle_index + 1 ) < ui->subTable->subtitlesCount() ) {

                    // Move the next subtitle "start time" at the current subtitle "end time" + X frames,
                    // where X is the minimum interval between two subtitles
                    if ( ui->subTable->setStartTime(start_time_ms, ( current_subtitle_index + 1 ) ) == false ) {
                        QString error_msg = ui->subTable->errorMsg();
                        QMessageBox::warning(this, "Set start time", error_msg);
                    }
                    else {
                        // Redraw the subtitle zone in the waveform
                        ui->waveForm->changeZoneStartTime( ( current_subtitle_index + 1 ), start_time_ms);
                    }
                } // If next subtitle doesn't exist (current is the last)
                else {

                    new_subtitle = ui->stEditDisplay->getDefaultSub();

                    // Inserte new subtitle start_time at X frames of the current subtitle
                    // where X is the minimum interval in frame between two subtitle (defined in "settings" by the user)
                    if ( ui->subTable->insertNewSub( new_subtitle , start_time_ms  ) == false ) {
                        QString error_msg = ui->subTable->errorMsg();
                        QMessageBox::warning(this, "Insert subtitle", error_msg);
                    }
                    else {
                        // Draw subtitle zone in the waveform
                        subtitle_list.append(new_subtitle);
                        current_subtitle_index = ui->subTable->currentIndex();
                        ui->waveForm->drawSubtitlesZone(subtitle_list, current_subtitle_index);
                        ui->waveForm->changeZoneColor(current_subtitle_index);
                    }
                }
            }
            return true;
        }
        else if ( key_event->key() == Qt::Key_F4 ) {

            // F4 add new subtitle entry after current subtitle
            new_subtitle = ui->stEditDisplay->getDefaultSub();

            if ( ui->subTable->insertNewSubAfterCurrent( new_subtitle ) == false ) {
                QString error_msg = ui->subTable->errorMsg();
                QMessageBox::warning(this, "Insert subtitle", error_msg);
            }
            else {
                subtitle_list.append(new_subtitle);
                current_subtitle_index = ui->subTable->currentIndex();
                ui->waveForm->drawSubtitlesZone(subtitle_list, current_subtitle_index);
                ui->waveForm->changeZoneColor(current_subtitle_index);
            }
            return true;
        }
        else if ( key_event->key() == Qt::Key_Delete ) {

            // Delete key : remove current subtitle
            // Remove the current subtitle item from the waveform
            current_subtitle_index = ui->subTable->currentIndex();
            ui->waveForm->removeSubtitleZone(current_subtitle_index);

            // Remove the current subtitle from the database
            ui->subTable->deleteCurrentSub();

            // Current subtitle changed, change the item color in the waveform
            current_subtitle_index = ui->subTable->currentIndex();
            ui->waveForm->changeZoneColor(current_subtitle_index);
            return true;
        }
        else if ( key_event->key() == Qt::Key_PageUp ) {

            // PageUp key : Select previous subtitle
            current_subtitle_index = ui->subTable->currentIndex();

            if ( current_subtitle_index > 0 ) {
                ui->subTable->selectRow(current_subtitle_index - 1);
            }
            return true;
        }
        else if ( key_event->key() == Qt::Key_PageDown ) {

            // PageDown key : Select next subtitle
            current_subtitle_index = ui->subTable->currentIndex();

            if ( (current_subtitle_index + 1) < ui->subTable->subtitlesCount() ) {
                ui->subTable->selectRow(current_subtitle_index + 1);
            }
            return true;
        }
        else if ( ( ( key_event->key() == Qt::KeypadModifier ) && ( key_event->key() == Qt::Key_2 ) ) ||
            ( key_event->key() == Qt::Key_F12 ) ) {

            // "[NUM]2" or "F12" key : move current position 1 second after
            current_position_ms = ui->waveForm->currentPositonMs();
            ui->waveForm->updatePostionMarker(current_position_ms + 1000);
            return true;
        }
        else if ( ( ( key_event->key() == Qt::KeypadModifier ) && ( key_event->key() == Qt::Key_1 ) ) ||
         ( key_event->key() == Qt::Key_F11 ) ) {

            // "[NUM]1" or "F11" key : move current position 1 second before
            current_position_ms = ui->waveForm->currentPositonMs();
            ui->waveForm->updatePostionMarker(current_position_ms - 1000);
            return true;
        }
        else if ( ( ( key_event->key() == Qt::KeypadModifier ) && ( key_event->key() == Qt::Key_8 ) ) ||
            ( key_event->key() == Qt::Key_F10 ) ) {

            // "[NUM]8" or "F10" key : move current position 1 frame after
            current_position_ms = ui->waveForm->currentPositonMs();
            qint16 frame_duration_ms = (qint16)( (qreal)SEC_TO_MSEC / qApp->property("prop_FrameRate_fps").toReal() );
            ui->waveForm->updatePostionMarker(current_position_ms + frame_duration_ms);
            return true;
        }
        else if ( ( ( key_event->key() == Qt::KeypadModifier ) && ( key_event->key() == Qt::Key_7 ) ) ||
         ( key_event->key() == Qt::Key_F9 ) ) {

            // "[NUM]7" or "F9" key : move current position 1 frame before
            current_position_ms = ui->waveForm->currentPositonMs();
            qint16 frame_duration_ms = (qint16)( (qreal)SEC_TO_MSEC / qApp->property("prop_FrameRate_fps").toReal() );
            ui->waveForm->updatePostionMarker(current_position_ms - frame_duration_ms);
            return true;
        }
        else if ( ( ( key_event->key() == Qt::KeypadModifier ) && ( key_event->key() == Qt::Key_Minus ) ) ||
         ( key_event->key() == Qt::Key_F7 ) ) {

            // "[NUM]-" or "F7" key : slow down video (min x1)

            current_position_ms = ui->waveForm->currentPositonMs();
            mMarkerPosChanged = true;

            // Change video player plaback rate
            if ( ui->videoPlayer->changePlaybackRate(false) == true ) {
                // QMediaplayer is bugged, position jump when playback rate change
                // Replace the position marker to force video position
                ui->waveForm->updatePostionMarker(current_position_ms);
            }
            else {
                mMarkerPosChanged = false;
            }
            return true;
        }
        else if ( ( ( key_event->key() == Qt::KeypadModifier ) && ( key_event->key() == Qt::Key_Plus ) ) ||
         ( key_event->key() == Qt::Key_F8 ) ) {

            // "[NUM]+" or "F8" key : speed up video (max x4)

            current_position_ms = ui->waveForm->currentPositonMs();
            mMarkerPosChanged = true;

            if ( ui->videoPlayer->changePlaybackRate(true) == true ) {
                // QMediaplayer is bugged, position jump when playback rate change
                // Replace the position marker to force video position
                ui->waveForm->updatePostionMarker(current_position_ms);
            }
            else {
                mMarkerPosChanged = false;
            }
            return true;
        }
        else if ( ( ( key_event->key() == Qt::KeypadModifier ) && ( key_event->key() == Qt::Key_4 ) ) ||
         ( ( key_event->modifiers() == Qt::ControlModifier ) && ( key_event->key() == Qt::Key_P ) ) ) {

            //"[NUM]4" or "Ctrl + P" key : switch play / pause video player

            current_position_ms = ui->waveForm->currentPositonMs();
            mMarkerPosChanged = true;
            ui->videoPlayer->on_playButton_clicked();
            // QMediaplayer is bugged, position jump when "play" after "pause"
            // Replace the position marker to force video position
            ui->waveForm->updatePostionMarker(current_position_ms);

            return true;
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

// Current item selection changed in the database.
void MainWindow::currentItemChanged(qint64 positionMs) {

    // Update the waveform marker position
    ui->waveForm->updatePostionMarker(positionMs);
}

// Update the subtitle text in the database
void MainWindow::updateSubTableText() {

    qint64 current_position_ms = ui->waveForm->currentPositonMs();

    // There are no subtitle for the current time. Try to add new subtitle entry
    if ( ui->subTable->isNewEntry( current_position_ms ) ) {

        MySubtitles new_subtitle;
        TextLine new_line = this->getPosToolBox();

        new_line.setLine(ui->stEditDisplay->text().first().Line());

        new_subtitle.setText(new_line, this->getFontToolBox(false));

        if ( ui->subTable->insertNewSub(new_subtitle, current_position_ms) == false ) {

            QString error_msg = ui->subTable->errorMsg();
            QMessageBox::warning(this, "Insert subtitle", error_msg);
            MySubtitles empty_subtitle;
            updateTextEdit(empty_subtitle);
        }
        else {
            // Draw subtitle zone in the waveform
            QList<MySubtitles> subtitle_list;
            subtitle_list.append(new_subtitle);
            qint32 current_subtitle_index = ui->subTable->currentIndex();
            ui->waveForm->drawSubtitlesZone(subtitle_list, current_subtitle_index);
            ui->waveForm->changeZoneColor(current_subtitle_index);
        }
    } // There are a subtitle for the current time. Update the text in the database
    else {
        ui->subTable->updateText( ui->stEditDisplay->text());
    }
}

// Update the subtitle datas in the database
void MainWindow::updateSubTableDatas(MySubtitles subtitleDatas) {

    qint64 current_position_ms = ui->waveForm->currentPositonMs();

    // There are no subtitle for the current time. Avoid
    if ( ui->subTable->isNewEntry( current_position_ms ) ) {
        return;
    }
    else {
        ui->subTable->updateDatas(subtitleDatas);
    }
}

// Video positon changed. Update the waveform marker position
void MainWindow::videoPositionChanged(qint64 positionMs) {

    // Update only if it's not the marker that commanded video position changment
    if ( mMarkerPosChanged == false ) {
        mVideoPositionChanged = true;
        ui->waveForm->updatePostionMarker(positionMs);
    }
    else {
        mMarkerPosChanged = false;
    }
}

// Interface to update the position of the player
void MainWindow::updateVideoPosition(qint64 positionMs) {

    if ( ui->videoPlayer->videoAvailable() ) {
        ui->videoPlayer->setPosition(positionMs);
    }
    else {
        mMarkerPosChanged = false;
    }
}

// Manage the waveform marker position changment
void MainWindow::waveformMarerkPosChanged(qint64 positionMs) {

    // Change the video positon if its not the mediaplayer that has changed the waveform marker position
    if ( mVideoPositionChanged == false ) {
        mMarkerPosChanged = true;
        this->updateVideoPosition(positionMs);
    }
    else {
        mVideoPositionChanged = false;
    }

    // Change the current item selection
    ui->subTable->updateStDisplay(positionMs);
}

// Interface to update the text of the text edit area
void MainWindow::updateTextEdit(MySubtitles subtitle) {

    ui->stEditDisplay->setText(subtitle);
    ui->waveForm->changeZoneColor(ui->subTable->currentIndex());
}

// Change the current subtitle "start time"
bool MainWindow::changeCurrentSubStartTime(qint64 positionMs) {

    qint32 current_subtitle_index = ui->subTable->currentIndex();

    // Check if there are a subtitle selected
    if ( current_subtitle_index >= 0 ) {
        // Check if there are a subtitle before
        if ( ( current_subtitle_index - 1 ) >= 0) {

            MySubtitles previous_subtitle = ui->subTable->getSubInfos(current_subtitle_index - 1);
            qint64 previous_sub_start_time_ms = MyAttributesConverter::timeStrHMStoMs( previous_subtitle.startTime() );

            // Check time validity
            if ( previous_sub_start_time_ms >= 0 ) {

                // If the current sub new "start time" is before the previous sub "start time" (previous sub completely recover)
                // Abord
                if ( positionMs <= previous_sub_start_time_ms ) {
                    QMessageBox::warning(this, "Set start time", "start time < previous start time");
                    return false;
                }

                qint64 previous_sub_end_time_ms = MyAttributesConverter::timeStrHMStoMs( previous_subtitle.endTime() );

                // Check time validity
                if ( previous_sub_end_time_ms >=0 ) {

                    // If the current sub new "start time" is between the previous sub "start/end time"
                    // Move the previous subtitle "end time" at the current subtitle "start time" - X frames,
                    // where X is the minimum interval between two subtitles
                    if ( positionMs <= previous_sub_end_time_ms) {

                        qint64 new_end_time_ms = positionMs - ( ( (qreal)SEC_TO_MSEC / qApp->property("prop_FrameRate_fps").toReal() ) * qApp->property("prop_SubMinInterval_frame").toReal() );

                        if ( ui->subTable->setEndTime( new_end_time_ms , ( current_subtitle_index - 1 ) ) == false ) {
                            QMessageBox::warning(this, "Set start time", "start time < previous start time");
                            return false;
                        }
                        else {
                            // If ok, redraw the subtitle zone in the waveform
                            ui->waveForm->changeZoneEndTime( ( current_subtitle_index - 1 ), new_end_time_ms);
                        }
                    }
                } // Time not valid, abord
                else return false;
            }   // Time not valid, abord
            else return false;
        }

        // Change the current sub "start time"
        if ( ui->subTable->setStartTime(positionMs, current_subtitle_index) == false ) {
            QString error_msg = ui->subTable->errorMsg();
            QMessageBox::warning(this, "Set start time", error_msg);
            return false;
        }
        else {
            // If ok, redraw the subtitle zone in the waveform
            ui->waveForm->changeZoneStartTime(current_subtitle_index, positionMs);
        }
    }
    return true;
}

// Change the current subtitle "start time"
bool MainWindow::changeCurrentSubEndTime(qint64 positionMs) {

    qint32 current_subtitle_index = ui->subTable->currentIndex();

    // Check if there are a subtitle selected
    if ( current_subtitle_index < ui->subTable->subtitlesCount() ) {
        // Check if there are a subtitle after
        if ( ( current_subtitle_index + 1 ) < ui->subTable->subtitlesCount() ) {

            MySubtitles next_subtitle = ui->subTable->getSubInfos(current_subtitle_index + 1);
            qint64 next_sub_end_time_ms = MyAttributesConverter::timeStrHMStoMs( next_subtitle.endTime() );

            // Check time validity
            if ( next_sub_end_time_ms >= 0 ) {

                // If the current sub new "end time" is after the next sub "end time" (next sub completely recover)
                // Abord
                if ( positionMs >= next_sub_end_time_ms ) {
                    QMessageBox::warning(this, "Set end time", "end time > next end time");
                    return false;
                }

                qint64 next_sub_start_time_ms = MyAttributesConverter::timeStrHMStoMs( next_subtitle.startTime() );

                // Check time validity
                if ( next_sub_start_time_ms >=0 ) {

                    // If the current sub new "end time" is between the next sub "start/end time"
                    // Move the next subtitle "start time" at the current subtitle "end time" + X frames,
                    // where X is the minimum interval between two subtitles
                    if ( positionMs >= next_sub_start_time_ms) {

                        qint64 new_start_time_ms = positionMs + ( ( (qreal)SEC_TO_MSEC / qApp->property("prop_FrameRate_fps").toReal() ) * qApp->property("prop_SubMinInterval_frame").toReal() );

                        if ( ui->subTable->setStartTime( new_start_time_ms , ( current_subtitle_index + 1 ) ) == false ) {
                            QMessageBox::warning(this, "Set end time", "end_time > next end time");
                            return false;
                        }
                        else {
                            // If ok, redraw the subtitle zone in the waveform
                            ui->waveForm->changeZoneStartTime( ( current_subtitle_index + 1 ), new_start_time_ms);
                        }
                    }
                } // Time not valid, abord
                else return false;
            } // Time not valid, abord
            else return false;
        }

        // Change the current sub "end time"
        if ( ui->subTable->setEndTime(positionMs, current_subtitle_index) == false ) {
            QString error_msg = ui->subTable->errorMsg();
            QMessageBox::warning(this, "Set end time", error_msg);
            return false;
        }
        else {
            // If ok, redraw the subtitle zone in the waveform
            ui->waveForm->changeZoneEndTime(current_subtitle_index, positionMs);
        }
    }
    return true;
}

// Move the current subtitle at given position
void MainWindow::shiftCurrentSubtitle(qint64 positionMs) {

    qint32 current_subtitle_index = ui->subTable->currentIndex();

    MySubtitles current_subtitle = ui->subTable->getSubInfos(current_subtitle_index);
    qint64 current_sub_start_time_ms = MyAttributesConverter::timeStrHMStoMs( current_subtitle.startTime() );
    qint64 current_sub_end_time_ms = MyAttributesConverter::timeStrHMStoMs( current_subtitle.endTime() );

    // Check time validity
    if ( ( current_sub_start_time_ms >= 0 ) &&  ( current_sub_end_time_ms >= 0 ) ) {

        qint32 current_sub_durationMs = current_sub_end_time_ms - current_sub_start_time_ms;

        // Check if the new "start time" is after the actual "start time"
        if ( positionMs > current_sub_start_time_ms ) {

            // Move the "end time" first
            if ( this->changeCurrentSubEndTime( positionMs + current_sub_durationMs ) == true ) {
                // If "end time" well moved, move the "start time"
                this->changeCurrentSubStartTime(positionMs);
            }
        }// Check if the new "start time" is before the actual "start time"
        else if ( positionMs < current_sub_start_time_ms ) {
            // Move the "start time" first
            if ( this->changeCurrentSubStartTime( positionMs ) == true ) {
                // If "start time" well moved, move the "end time"
                this->changeCurrentSubEndTime( positionMs + current_sub_durationMs );
            }
        }
    }
}

// Manage the openning of video file when "Open" button is triggered
void MainWindow::on_actionOpen_triggered()
{
    // Open a video file with the palyer
    QString video_file_name = ui->videoPlayer->openFile();

    // If the media player has openned the video
    if ( !video_file_name.isEmpty() ) {

        // Open the corresponding waveform
        QString wf_file_name = video_file_name;
        wf_file_name.truncate(wf_file_name.lastIndexOf("."));
        wf_file_name.append(".wf");

        ui->waveForm->openFile(wf_file_name, video_file_name);
    }
}

// Resize the subtitles edit zone in function of the current video size
void MainWindow::updateStEditSize() {

    QSizeF video_item_size;
    QSizeF video_native_size;
    QSizeF video_current_size;
    qreal video_item_ratio;
    qint32 video_x_pos;
    qint32 video_y_pos;

    // Video item size is the size of the zone where is displayed the video
    video_item_size = ui->videoPlayer->videoItemSize();
    video_item_ratio = video_item_size.width() / video_item_size.height();

    // Video native size is the real full size of the media
    video_native_size = ui->videoPlayer->videoItemNativeSize();

    // Video current size is the displayed size of the video,
    // scaled on the video item with its ratio kept

    // When no video loaded, the subtitles edit zone should be mapped on the video item
    // 9 is the margins value of the mediaplayer;
    video_current_size = video_item_size;
    video_x_pos = 9;
    video_y_pos = 9;

    // If a video is loaded
    if ( video_native_size.isValid() ) {

        // Retrieve the video current size (native size scaled in item size)
        video_current_size = video_native_size.scaled(video_item_size, Qt::KeepAspectRatio);

        qreal video_native_ratio = video_native_size.width() / video_native_size.height();

        // Compare the ratio of native video and video item
        // to knwon if the video is full scale on width or on height of the item
        if ( video_native_ratio > video_item_ratio ) {
            video_x_pos = 9;
            video_y_pos = (qint32)( qRound( video_item_size.height() - video_current_size.height() ) / 2.0 ) + 9;
        }
        else if ( video_native_ratio < video_item_ratio ) {
            video_x_pos = (qint32)( qRound( video_item_size.width() - video_current_size.width() ) / 2.0 ) + 9;
            video_y_pos = 9;
        }
    }

    // Set the edit region size and position mapped on video.
    ui->stEditDisplay->setFixedSize(video_current_size.toSize());
    ui->stEditDisplay->move(video_x_pos, video_y_pos);

    ui->playerInfoLabel->move(video_x_pos, video_y_pos);

}

void MainWindow::resizeEvent(QResizeEvent* event) {

    updateStEditSize();
    QMainWindow::resizeEvent(event);
}


void MainWindow::on_actionQuit_triggered()
{
    close();
}

// Manage the subtitles import when "Import" button triggered
void MainWindow::on_actionImport_Subtitles_triggered()
{
    QString selected_filter;

    // Open a subtitle file
    QString file_name = QFileDialog::getOpenFileName(this, tr("Open Subtitles"),QDir::homePath() + "/Videos",
                                                     tr("SubRip (*.srt);;DCSubtitle (*.xml);;All Files(*)"),
                                                     &selected_filter);


    MyFileReader file_reader(file_name, "UTF-8");

    if ( file_reader.readFile(file_name, "UTF-8") == false ) {
        QString error_msg = file_reader.errorMsg();
        QMessageBox::warning(this, "Import subtitles", error_msg);
        return;
    }

    // Choose a parser to parse the file
    MySubtitleFileParser* parser;

    if ( selected_filter ==  ("SubRip (*.srt)") ) {
        parser = new SubRipParser();
    }
    else if ( selected_filter ==  ("DCSubtitle (*.xml)") ) {
        parser = new DcSubParser();
    }
    else {
        //TO DO
    }

    // Parse the file
    QList<MySubtitles> subtitles_list = parser->open(file_reader);

    // If parsing is successfull, load the subtitles list in the database
    if ( !subtitles_list.isEmpty() ) {

        ui->subTable->loadSubtitles(subtitles_list);

        // Remove all and draw subtitles zones in the waveform
        ui->waveForm->removeAllSubtitlesZones();
        qint32 current_subtitle_index = ui->subTable->currentIndex();
        ui->waveForm->drawSubtitlesZone(subtitles_list, current_subtitle_index);
        ui->waveForm->changeZoneColor(current_subtitle_index);
    }
}

// Manage the subtitles export when "Export" button triggered
void MainWindow::on_actionExport_Subtitles_triggered() {

    QString selected_filter;

    // Create or open file to write datas
    QString file_name = QFileDialog::getSaveFileName(this, tr("Open Subtitles"),QDir::homePath() + "/Videos",
                                                     tr("SubRip (*.srt);;DCSubtitle (*.xml);;All Files(*)"),
                                                     &selected_filter);


    MyFileWriter file_writer(file_name, "UTF-8");

    // Choose the good parser
    MySubtitleFileParser* parser;

    if ( selected_filter ==  ("SubRip (*.srt)") ) {
        parser = new SubRipParser();
    }
    else if ( selected_filter ==  ("DCSubtitle (*.xml)") ) {
        parser = new DcSubParser();
    }
    else {
        //TO DO
    }

    // Retreive the subtitles datas from databases
    QList<MySubtitles> subtitles_list = ui->subTable->saveSubtitles();

    // If there are datas, write it to asked format
   if ( !subtitles_list.isEmpty() ) {

       parser->save(file_writer, subtitles_list);

       if ( file_writer.toFile() == false ) {
           QString error_msg = file_writer.errorMsg();
           QMessageBox::warning(this, "Export subtitles", error_msg);
       }
   }
}

void MainWindow::on_settingsButton_clicked()
{
    switch ( ui->stackedWidget->currentIndex() ) {
    case 0 :
        ui->stackedWidget->setCurrentIndex(1);
        break;
    case 1 :
        ui->stackedWidget->setCurrentIndex(0);
        break;
    default:
        ui->stackedWidget->setCurrentIndex(0);
        break;
    }
}

void MainWindow::displayErrorMsg(QString errorMsg) {

    QMessageBox::warning(this, "Error", errorMsg);
}

void MainWindow::displayInfo(QString info) {

    ui->playerInfoLabel->setText(info);
    mpDisplayInfoTimer->start(2000);
}

void MainWindow::eraseInfo() {
    ui->playerInfoLabel->setText("");
}

// ******************************** Tool Box ***************************************************************//

// Update the parameter tool boxes
void MainWindow::updateToolBox() {

    MySubtitles current_subtitle_datas;
    TextLine text_line;
    TextFont text_font;
    qint16 line_nbr;

    // Check the line number (1 or 2)
    line_nbr = ui->stEditDisplay->lastFocused();
    // Retrieve the current text edit zones paramaters
    current_subtitle_datas = ui->stEditDisplay->subtitleData();

    if ( line_nbr <= current_subtitle_datas.text().count() ) {

        // Retrieve font and position of the current line
        text_line = current_subtitle_datas.text().at(line_nbr -1);
        text_font = text_line.Font();

        // Update the tool boxes with this parameters
        this->updatePosToolBox(text_line);
        this->updateFontToolBox(text_font);
    }
}

// Update the "position" tool boxes
void MainWindow::updatePosToolBox(TextLine textLine) {

    // Update only if the user is not changing the parameters with tool boxes
    if ( mTextPosChangedByUser != true ) {

        // Set the flag to indicate that toolboxes states are changing
        mTextPosChangedBySoft = true;

        // Set the parameters to the boxes
        ui->hAlignBox->setCurrentText( textLine.textHAlign() );
        ui->hPosSpinBox->setValue( textLine.textHPosition().toDouble() );
        ui->vAlignBox->setCurrentText( textLine.textVAlign() );
        ui->vPosSpinBox->setValue( textLine.textVPosition().toDouble() );

    }
    mTextPosChangedBySoft = false;
}

// Change the "position" of the current "edit line" from the toolboxes parameters
void MainWindow::updateTextPosition() {

    mTextPosChangedByUser = true;

    // Retrieve position parameters from tool boxes
    TextLine new_text_line = this->getPosToolBox();

    // Change the text edit zone position
    ui->stEditDisplay->updateTextPosition( new_text_line );

    qint64 current_psotion_ms = ui->waveForm->currentPositonMs();

    // If there is subtitle indexed in the table for the current position
    if ( !ui->subTable->isNewEntry(current_psotion_ms) ) {

        // Retrieve the current subtitle datas
        MySubtitles current_subtitle_datas;
        current_subtitle_datas = ui->subTable->getSubInfos( ui->subTable->currentIndex() );

        // Check the line number (1 or 2)
        qint16 line_nbr = ui->stEditDisplay->lastFocused();

        if ( line_nbr > 0 ) {

            // Retrieve the current text properties lines list
            QList<TextLine> current_text_lines = current_subtitle_datas.text();
            // Retrieve the line to change
            TextLine current_line = current_text_lines.at(line_nbr - 1);
            // Retrieve its font
            TextFont current_text_font = current_line.Font();

            // Push the font properties in the new line
            new_text_line.setFont(current_text_font);
            // Replace the line in the list
            current_text_lines.replace(line_nbr - 1, new_text_line);
            // Push the changed list in the subtitle container
            current_subtitle_datas.setText(current_text_lines);
        }

        // Push the new datas in the table
        ui->subTable->updateDatas(current_subtitle_datas);
    }

    mTextPosChangedByUser = false;
}

// Get the position paramters from the tool boxes
TextLine MainWindow::getPosToolBox() {

    TextLine text_line;

    text_line.setTextHAlign( ui->hAlignBox->currentText() );

    QString hPos_str = QString::number(ui->hPosSpinBox->value(), 'f', 1);
    text_line.setTextHPosition( hPos_str );

    text_line.setTextVAlign( ui->vAlignBox->currentText() );

    QString vPos_str = QString::number(ui->vPosSpinBox->value(), 'f', 1);
    text_line.setTextVPosition( vPos_str );

    return text_line;
}

// Vertical alignment toolbox value changed
void MainWindow::on_vAlignBox_activated(const QString &value) {

    // Tool box changed by user
    if ( mTextPosChangedBySoft == false ) {

        // If there is not subtitle indexed in the table for the current position
        if ( ui->subTable->isNewEntry( ui->waveForm->currentPositonMs() ) ) {
            // Save the parameter as default parameter
            qApp->setProperty("prop_Valign", value);
            ui->stEditDisplay->defaultSub();
        }

        // Update the text edit position
        updateTextPosition();
    }
}

// Vertical position toolbox value changed
void MainWindow::on_vPosSpinBox_valueChanged(const QString &value) {

    if ( mTextPosChangedBySoft == false ) {

        if ( ui->subTable->isNewEntry( ui->waveForm->currentPositonMs() ) ) {
            qApp->setProperty("prop_Vposition_percent", QString::number(ui->vPosSpinBox->value(), 'f', 1));
            ui->stEditDisplay->defaultSub();
        }

        updateTextPosition();
    }
}

// Horizontal alignment toolbox value changed
void MainWindow::on_hAlignBox_activated(const QString &value) {

    if ( mTextPosChangedBySoft == false ) {

        if ( ui->subTable->isNewEntry( ui->waveForm->currentPositonMs() ) ) {
            qApp->setProperty("prop_Halign", value);
            ui->stEditDisplay->defaultSub();
        }

        updateTextPosition();
    }
}

// Horizontal position toolbox value changed
void MainWindow::on_hPosSpinBox_valueChanged(const QString &value) {

    if ( mTextPosChangedBySoft == false ) {

        if ( ui->subTable->isNewEntry( ui->waveForm->currentPositonMs() ) ) {
            qApp->setProperty("prop_Hposition_percent", QString::number(ui->hPosSpinBox->value(), 'f', 1));
            ui->stEditDisplay->defaultSub();
        }

        updateTextPosition();
    }
}

// Update the "font" tool boxes
void MainWindow::updateFontToolBox(TextFont textFont) {

    // Update only if the user is not changing the parameters with tool boxes
    if ( mTextFontChangedByUser != true ) {

        // Set the flag to indicate that toolboxes states are changing
        mTextFontChangedBySoft = true;

        // Set the parameters to the boxes
        QFont font(textFont.fontId());
        ui->fontIdComboBox->setCurrentFont(font);
        ui->fontSizeSpinBox->setValue(textFont.fontSize().toInt());

        QString font_color = textFont.fontColor();

        qApp->setProperty("prop_FontColor_rgba", font_color);

        ui->fontColorOtherText->setEnabled(false);

        if ( font_color == "FFFF0000" ) {
            ui->fontColorRButton->setChecked(true);
        }
        else if ( font_color == "FF00FF00" ) {
            ui->fontColorGButton->setChecked(true);
        }
        else if ( font_color == "FF0000FF" ) {
            ui->fontColorBButton->setChecked(true);
        }
        else if ( font_color == "FFFFFF00" ) {
            ui->fontColorYButton->setChecked(true);
        }
        else if ( font_color == "FF000000" ) {
            ui->fontColorBlButton->setChecked(true);
        }
        else if ( font_color == "FFFFFFFF") {
            ui->fontColorWButton->setChecked(true);
        }
        else {
            ui->fontColorOtherButton->setChecked(true);
            ui->fontColorOtherText->setEnabled(true);
            ui->fontColorOtherText->setText(textFont.fontColor());
        }

        if ( textFont.fontItalic().contains("yes", Qt::CaseInsensitive) == true ) {
            ui->fontItalicButton->setChecked(true);
        }
        else {
            ui->fontItalicButton->setChecked(false);
        }

        if ( textFont.fontUnderlined().contains("yes", Qt::CaseInsensitive) == true ) {
            ui->fontUnderlinedButton->setChecked(true);
        }
        else {
            ui->fontUnderlinedButton->setChecked(false);
        }

    }
    mTextFontChangedBySoft = false;
}

// Get the font parameters from the tool boxes
TextFont MainWindow::getFontToolBox(bool customColorClicked) {

    TextFont text_font;
    QString font_color_str;

    if ( ui->fontLabel->isEnabled() == true )  {

        QFont font = ui->fontIdComboBox->currentFont();
        text_font.setFontId( font.family() );

        text_font.setFontSize(ui->fontSizeSpinBox->cleanText());

        ui->fontColorOtherText->setEnabled(false);

        if ( ui->fontColorRButton->isChecked() ) {
            font_color_str = "FFFF0000";
        }
        else if ( ui->fontColorGButton->isChecked() ) {
            font_color_str = "FF00FF00";
        }
        else if ( ui->fontColorBButton->isChecked() ) {
            font_color_str = "FF0000FF";
        }
        else if ( ui->fontColorYButton->isChecked() ) {
            font_color_str = "FFFFFF00";
        }
        else if ( ui->fontColorBlButton->isChecked() ) {
            font_color_str = "FF000000";
        }
        else if ( ui->fontColorWButton->isChecked() ) {
            font_color_str = "FFFFFFFF";
        }
        else if ( ui->fontColorOtherButton->isChecked() ) {

            if ( customColorClicked == true ) {
                bool ok;
                QColor init_color = QColor::fromRgba( ui->fontColorOtherText->text().toUInt(&ok, 16) );
                QColor font_color = QColorDialog::getColor(init_color, 0, "Select Color", QColorDialog::ShowAlphaChannel);
                font_color_str = QString::number( font_color.rgba(), 16 ).toUpper();
                ui->fontColorOtherText->setText(font_color_str);
            }
            else {
                font_color_str = ui->fontColorOtherText->text();
            }
            ui->fontColorOtherText->setEnabled(true);
        }

        text_font.setFontColor(font_color_str);

        if ( ui->subTable->isNewEntry( ui->waveForm->currentPositonMs() ) ) {
            qApp->setProperty("prop_FontColor_rgba", font_color_str);
            ui->stEditDisplay->defaultSub();
        }


        if ( ui->fontItalicButton->isChecked() ) {
            text_font.setFontItalic("yes");
        }
        else {
            text_font.setFontItalic("no");
        }

        if ( ui->fontUnderlinedButton->isChecked() ) {
            text_font.setFontUnderlined("yes");
        }
        else {
            text_font.setFontUnderlined("no");
        }
    }

    return text_font;
}

// Change the "font" of the current "edit line" from the font toolboxes parameters
void MainWindow::updateTextFont(bool customColorClicked) {

    mTextFontChangedByUser = true;

    // Retrieve font parameters from tool boxes
    TextFont new_font = this->getFontToolBox(customColorClicked);

    // Change the text edit zone font
    ui->stEditDisplay->updateTextFont(new_font, this->getPosToolBox() );

    qint64 current_psotion_ms = ui->waveForm->currentPositonMs();

    // If there is subtitle indexed in the table for the current time
    if ( !ui->subTable->isNewEntry(current_psotion_ms) ) {

        // Retrieve the current subtitle datas
        MySubtitles current_subtitle_datas;
        current_subtitle_datas = ui->subTable->getSubInfos( ui->subTable->currentIndex() );

        // Check the line number (1 or 2)
        qint16 line_nbr = ui->stEditDisplay->lastFocused();

        if ( line_nbr > 0 ) {

            // Retrieve the current text properties lines list
            QList<TextLine> current_text_lines = current_subtitle_datas.text();
            // Retrieve the line to change
            TextLine current_line = current_text_lines.at(line_nbr - 1);

            // Push the text font properties in the current line
            current_line.setFont(new_font);
            // Replace the line in the list
            current_text_lines.replace(line_nbr - 1, current_line);
            // Push the changed list in the subtitle container
            current_subtitle_datas.setText(current_text_lines);
        }

        // Push the new datas in the table
        ui->subTable->updateDatas(current_subtitle_datas);
    }

    mTextFontChangedByUser = false;
}

// Font size toolbox changed
void MainWindow::on_fontSizeSpinBox_valueChanged(const QString &value) {

    // Tool box changed by user
    if ( mTextFontChangedBySoft == false ) {

        // If there is no subtitle indexed in the table for the current time
        if ( ui->subTable->isNewEntry( ui->waveForm->currentPositonMs() ) ) {
            // Save the parameter as default parameter
            qApp->setProperty("prop_FontSize_pt", ui->fontSizeSpinBox->cleanText());
            ui->stEditDisplay->defaultSub();
        }

        // Update the text edit font
        updateTextFont(false);
    }
}

// Font color selected (the 7 colors boxes are exclusive, if one is checked, the others are unchecked)
void MainWindow::on_fontColorRButton_toggled(bool checked) {

    if ( ( mTextFontChangedBySoft == false ) && ( checked == true ) ) {
        updateTextFont(false);
    }
}

void MainWindow::on_fontColorGButton_toggled(bool checked) {

    if ( ( mTextFontChangedBySoft == false ) && ( checked == true ) ) {
        updateTextFont(false);
    }
}

void MainWindow::on_fontColorBButton_toggled(bool checked) {

    if ( ( mTextFontChangedBySoft == false ) && ( checked == true ) ) {
        updateTextFont(false);
    }
}

void MainWindow::on_fontColorYButton_toggled(bool checked) {

    if ( ( mTextFontChangedBySoft == false ) && ( checked == true ) ) {
        updateTextFont(false);
    }
}

void MainWindow::on_fontColorBlButton_toggled(bool checked) {

    if ( ( mTextFontChangedBySoft == false ) && ( checked == true ) ) {
        updateTextFont(false);
    }
}

void MainWindow::on_fontColorWButton_toggled(bool checked) {

    if ( ( mTextFontChangedBySoft == false ) && ( checked == true ) ) {
        updateTextFont(false);
    }
}

void MainWindow::on_fontColorOtherButton_clicked() {

    if ( mTextFontChangedBySoft == false ) {
        updateTextFont(true);
    }
}

// Italic value changed
void MainWindow::on_fontItalicButton_toggled(bool checked) {

    if ( mTextFontChangedBySoft == false ) {

        if ( ui->subTable->isNewEntry( ui->waveForm->currentPositonMs() ) ) {

            if ( checked ) {
                qApp->setProperty("prop_FontItalic", "yes");
            }
            else {
                qApp->setProperty("prop_FontItalic", "no");
            }

            ui->stEditDisplay->defaultSub();
        }

        updateTextFont(false);
    }
}

// Underlined value changed
void MainWindow::on_fontUnderlinedButton_toggled(bool checked) {

    if ( mTextFontChangedBySoft == false ) {

        if ( ui->subTable->isNewEntry( ui->waveForm->currentPositonMs() ) ) {

            if ( checked ) {
                qApp->setProperty("prop_FontUnderlined", "yes");
            }
            else {
                qApp->setProperty("prop_FontUnderlined", "no");
            }

            ui->stEditDisplay->defaultSub();
        }

        updateTextFont(false);
    }
}

// Font name changed
void MainWindow::on_fontIdComboBox_currentFontChanged(const QFont &f) {

    if ( mTextFontChangedBySoft == false ) {

        if ( ui->subTable->isNewEntry( ui->waveForm->currentPositonMs() ) ) {

            qApp->setProperty("prop_FontName", ui->fontIdComboBox->currentText());
            ui->stEditDisplay->defaultSub();
        }

        updateTextFont(false);
    }
}
