#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "myfilereader.h"
#include "myfilewriter.h"
#include "mysubtitlefileparser.h"
#include "myattributesconverter.h"
#include "SubtitlesParsers/SubRip/subripparser.h"
#include "SubtitlesParsers/DcSubtitle/interop/dcsubinteropparser.h"
#include "SubtitlesParsers/DcSubtitle/smpte/dcsubsmpteparser.h"
#include "SubtitlesParsers/EBU/ebuparser.h"
#include <QFileDialog>
#include <QColorDialog>
#include <QFontDialog>
#include <QString>
#include <QKeyEvent>
#include <QMessageBox>
#include <QTimer>
#include <QTime>
#include <QAction>
#include "amapolaprojfileparser.h"
#include "subexportdialog.h"
#include "subimportmanager.h"
#include "inputsizedialog.h"


#define SEC_TO_MSEC 1000


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Init UI
    ui->setupUi(this);

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

    // Init history (undo/redo)
    this->clearHistory();

    // Save the database current state in history
    this->saveToHistory("Init");

    //ui->stEditDisplay->setStyleSheet("background: transparent; color: yellow");

    // Init the toolbar
    // Resolution
    QLabel* resolution_label = new QLabel();
    resolution_label->setText("Resolution : ");
    mResolutionComboBox = new QComboBox();
    mResolutionComboBox->addItem("NTSC 720x480", QSize(720,480));
    mResolutionComboBox->addItem("PAL 720x576", QSize(720,576));
    mResolutionComboBox->addItem("HD 720p 1280x720", QSize(1280,720));
    mResolutionComboBox->addItem("HD 1080p 1920x1080", QSize(1920,1080));
    mResolutionComboBox->addItem("2k 2048x1080", QSize(2048,1080));
    mResolutionComboBox->addItem("UHD 3840x2160", QSize(3840,2160));
    mResolutionComboBox->addItem("4k 4096x2160", QSize(4096,2160));
    mResolutionComboBox->addItem(tr("Other..."));

    ui->mainToolBar->addWidget(resolution_label)->setVisible(true);
    ui->mainToolBar->addWidget(mResolutionComboBox)->setVisible(true);
    connect(mResolutionComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(resolutionComboBox_currentIndexChanged(int)));
    mResolutionChangedNySoft = false;
    resolutionComboBox_currentIndexChanged(mResolutionComboBox->currentIndex());

    // Frame rate
    QLabel* frame_rate_label = new QLabel;
    frame_rate_label->setText("Frame rate : ");
    mFrameRateComboBox = new QComboBox();
    mFrameRateComboBox->addItem("23.976", 23.976);
    mFrameRateComboBox->addItem("24", 24);
    mFrameRateComboBox->addItem("25", 25);
    mFrameRateComboBox->addItem("29.97", 29.97);
    mFrameRateComboBox->addItem("30", 30);

    ui->mainToolBar->addWidget(frame_rate_label)->setVisible(true);
    ui->mainToolBar->addWidget(mFrameRateComboBox)->setVisible(true);
    connect(mFrameRateComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(frameRateComboBox_currentIndexChanged(int)));
    mFrameRateChangedBySoft = false;

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

    if ( ui->fontBorderCheckBox->isChecked() ) {
        qApp->setProperty("prop_FontBorder", "yes");
    }
    else {
        qApp->setProperty("prop_FontBorder", "no");
    }

    if ( ui->fontShadowCheckBox->isChecked() ) {
        qApp->setProperty("prop_FontShadow", "yes");
    }
    else {
        qApp->setProperty("prop_FontShadow", "no");
    }

    if ( ui->fontBackgroundCheckBox->isChecked() ) {
        qApp->setProperty("prop_FontBackground", "yes");
    }
    else {
        qApp->setProperty("prop_FontBackground", "no");
    }

    // Effects colors
    mTextBorderColor = MyAttributesConverter::stringToColor("FF000000");
    mTextShadowColor = MyAttributesConverter::stringToColor("FF000000");
    mTextBackgroundColor = MyAttributesConverter::stringToColor("FF000000");

    MyAttributesConverter::setColorToButton(ui->fontBorderColor, MyAttributesConverter::stringToColor("FF000000") );
    MyAttributesConverter::setColorToButton(ui->fontShadowColor, MyAttributesConverter::stringToColor("FF000000") );
    MyAttributesConverter::setColorToButton(ui->fontBackgroundColor, MyAttributesConverter::stringToColor("FF000000") );

    qApp->setProperty("prop_FontBorderColor", "FF000000");
    qApp->setProperty("prop_FontShadowColor", "FF000000");
    qApp->setProperty("prop_FontBackgroundColor", "FF000000");

    // Colors
    mColor1 = MyAttributesConverter::stringToColor("FFFFFFFF");
    mColor2 = MyAttributesConverter::stringToColor("FFFFFF00");
    mColor3 = MyAttributesConverter::stringToColor("FFFF0000");
    mColor4 = MyAttributesConverter::stringToColor("FFFF00FF");
    mColor5 = MyAttributesConverter::stringToColor("FF00FFFF");
    mColor6 = MyAttributesConverter::stringToColor("FF00FF00");
    mColor7 = MyAttributesConverter::stringToColor("FF0000FF");

    MyAttributesConverter::setColorToButton(ui->fontColor1Button, mColor1 );
    MyAttributesConverter::setColorToButton(ui->fontColor2Button, mColor2 );
    MyAttributesConverter::setColorToButton(ui->fontColor3Button, mColor3 );
    MyAttributesConverter::setColorToButton(ui->fontColor4Button, mColor4 );
    MyAttributesConverter::setColorToButton(ui->fontColor5Button, mColor5 );
    MyAttributesConverter::setColorToButton(ui->fontColor6Button, mColor6 );
    MyAttributesConverter::setColorToButton(ui->fontColor7Button, mColor7 );

    qApp->setProperty("prop_Color1","FFFFFFFF");
    qApp->setProperty("prop_Color2","FFFFFF00");
    qApp->setProperty("prop_Color3","FFFF0000");
    qApp->setProperty("prop_Color4","FFFF00FF");
    qApp->setProperty("prop_Color5","FF00FFFF");
    qApp->setProperty("prop_Color6","FF00FF00");
    qApp->setProperty("prop_Color7","FF0000FF");


    if ( ui->fontColor1Button->isChecked() ) {
        qApp->setProperty("prop_FontColor_rgba", "FFFFFFFF");
    }
    else if ( ui->fontColor2Button->isChecked() ) {
        qApp->setProperty("prop_FontColor_rgba", "FFFFFF00");
    }
    else if ( ui->fontColor3Button->isChecked() ) {
        qApp->setProperty("prop_FontColor_rgba", "FFFF0000");
    }
    else if ( ui->fontColor4Button->isChecked() ) {
        qApp->setProperty("prop_FontColor_rgba", "FFFF00FF");
    }
    else if ( ui->fontColor5Button->isChecked() ) {
        qApp->setProperty("prop_FontColor_rgba", "FF00FFFF");
    }
    else if ( ui->fontColor6Button->isChecked() ) {
        qApp->setProperty("prop_FontColor_rgba", "FF00FF00");
    }
    else if ( ui->fontColor7Button->isChecked() ) {
        qApp->setProperty("prop_FontColor_rgba", "FF0000FF");
    }

    // Vertical position
    qApp->setProperty("prop_Valign", ui->vAlignBox->currentText());
    qApp->setProperty("prop_Halign", ui->hAlignBox->currentText());
    qApp->setProperty("prop_Vposition_percent", QString::number(ui->vPosSpinBox->value(), 'f', 1));
    qApp->setProperty("prop_Hposition_percent", QString::number(ui->hPosSpinBox->value(), 'f', 1));

    ui->stEditDisplay->updateDefaultSub();

    qApp->setProperty("currentVideoFileName","");

    ui->waveForm->openFile("", "");

    // Init text edit parameter from tool boxes
    this->updateTextPosition();
    this->updateTextFont();

    // Init display infos timer
    mpDisplayInfoTimer = new QTimer(this);
    mpDisplayInfoTimer->setInterval(2000);

    // Init the SIGNAL / SLOT connections
    connect(ui->videoPlayer, SIGNAL(durationChanged(qint64)),this, SLOT(updateStEditSize()));
    connect(ui->videoPlayer, SIGNAL(positionChanged(qint64)), this, SLOT(videoPositionChanged(qint64)));
    connect(ui->videoPlayer, SIGNAL(playerStateInfos(QString)), this, SLOT(displayInfo(QString)));
    connect(ui->videoPlayer, SIGNAL(nativeSizeChanged(QSizeF)), this, SLOT(updateResoltionBox(QSizeF)));

    connect(ui->waveForm, SIGNAL(markerPositionChanged(qint64)), this, SLOT(waveformMarerkPosChanged(qint64)));
    connect(ui->waveForm, SIGNAL(ctrlLeftClickEvent(qint64)), this, SLOT(onCtrlLeftClickEvent(qint64)));
    connect(ui->waveForm, SIGNAL(ctrlRightClickEvent(qint64)), this, SLOT(onCtrlRightClickEvent(qint64)));
    connect(ui->waveForm, SIGNAL(shiftLeftClickEvent(qint64)), this, SLOT(shiftSubtitles(qint64)));
    connect(ui->waveForm, SIGNAL(waveFormFileReady(QString)), ui->videoPlayer, SLOT(loadWaveForm(QString)));

    connect(ui->stEditDisplay, SIGNAL(cursorPositionChanged()), this, SLOT(updateSubTableText()));
    connect(ui->stEditDisplay, SIGNAL(subDatasChanged(MySubtitles)), this, SLOT(updateSubTableDatas(MySubtitles)));
    connect(ui->stEditDisplay, SIGNAL(textLineFocusChanged()), this, SLOT(updateToolBox()));

    connect(ui->subTable, SIGNAL(itemSelectionChanged(qint64)), this, SLOT(currentSelectionChanged(qint64)));
    connect(ui->subTable, SIGNAL(newTextToDisplay(MySubtitles)), this, SLOT(currentSubChanged(MySubtitles)));
    connect(ui->subTable, SIGNAL(endTimeCodeChanged(qint32,qint64)), ui->waveForm, SLOT(changeZoneEndTime(qint32,qint64)));

    connect(this->mpDisplayInfoTimer, SIGNAL(timeout()), this, SLOT(eraseInfo()));

    connect(ui->settings, SIGNAL(frameRateChanged(qreal)), this, SLOT(updateFrameRate(qreal)));
    connect(ui->settings, SIGNAL(marginChanged()), this, SLOT(updateMarginsDraw()));
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
                this->shiftSubtitles(current_position_ms);
            }
            else {
                // F1 set current subtitle start time
                this->changeSubStartTime(current_position_ms);
            }
            return true;
        }
        else if ( key_event->key() == Qt::Key_F2 ) {

            // F2 set current subtitle end time
            current_position_ms = ui->waveForm->currentPositonMs();
            this->changeSubEndTime(current_position_ms);
            return true;
        }
        else if ( key_event->key() == Qt::Key_F3 ) {

            qint64 end_time_ms = ui->waveForm->currentPositonMs();
            current_subtitle_index = ui->subTable->currentIndex();

            // F3 set current subtitle end time + ( add new subtitle entry or move the next subtitle start time)

            // Change the current subtitle "end time", continue if ok
            if ( this->changeSubEndTime(end_time_ms, current_subtitle_index, QList<MySubtitles>(), false) == true ) {

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
                    new_subtitle.setDurationAuto(true);

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
                        ui->waveForm->changeZoneColor(ui->subTable->selectedIndex(), current_subtitle_index);
                    }
                }
            }
            return true;
        }
        else if ( key_event->key() == Qt::Key_F4 ) {

            // F4 add new subtitle entry after current subtitle
            new_subtitle = ui->stEditDisplay->getDefaultSub();
            new_subtitle.setDurationAuto(true);

            if ( ui->subTable->insertNewSubAfterCurrent( new_subtitle ) == false ) {
                QString error_msg = ui->subTable->errorMsg();
                QMessageBox::warning(this, "Insert subtitle", error_msg);
            }
            else {
                subtitle_list.append(new_subtitle);
                current_subtitle_index = ui->subTable->currentIndex();
                ui->waveForm->drawSubtitlesZone(subtitle_list, current_subtitle_index);
                ui->waveForm->changeZoneColor(ui->subTable->selectedIndex(), current_subtitle_index);
            }
            return true;
        }
        else if ( ( key_event->modifiers() == Qt::ControlModifier ) && ( key_event->key() == Qt::Key_Delete ) ) {

            // Delete key : remove subtitles selected
            this->removeSubtitles();

            return true;
        }
        else if ( key_event->key() == Qt::Key_PageUp ) {

            // PageUp key : Select previous subtitle
            current_subtitle_index = ui->subTable->currentIndex();

            if ( current_subtitle_index >= 0 ) {

                qint64 current_sub_end_time_ms = MyAttributesConverter::timeStrHMStoMs(ui->subTable->getSubInfos(current_subtitle_index).endTime() );

                if ( ui->waveForm->currentPositonMs() > current_sub_end_time_ms ) {

                    ui->subTable->clearSelection();
                    ui->waveForm->updatePostionMarker(current_sub_end_time_ms);
                }
                else if ( current_subtitle_index > 0 ){

                    ui->subTable->selectRow(current_subtitle_index - 1);
                }
            }
            return true;
        }
        else if ( key_event->key() == Qt::Key_PageDown ) {

            // PageDown key : Select next subtitle
            current_subtitle_index = ui->subTable->currentIndex();

            if ( current_subtitle_index < ui->subTable->subtitlesCount() ) {

                qint64 current_sub_start_time_ms = MyAttributesConverter::timeStrHMStoMs(ui->subTable->getSubInfos(current_subtitle_index).startTime() );

                if ( ui->waveForm->currentPositonMs() < current_sub_start_time_ms ) {

                    ui->subTable->clearSelection();
                    ui->waveForm->updatePostionMarker(current_sub_start_time_ms);

                }
                else if ( (current_subtitle_index + 1) < ui->subTable->subtitlesCount() ){

                    ui->subTable->selectRow(current_subtitle_index + 1);
                }
            }
            return true;
        }
        else if ( ( ( key_event->modifiers() == Qt::KeypadModifier ) && ( key_event->key() == Qt::Key_2 ) ) ||
            ( key_event->key() == Qt::Key_F12 ) ) {

            // "[NUM]2" or "F12" key : move current position 1 second after
            current_position_ms = ui->waveForm->currentPositonMs();
            ui->waveForm->updatePostionMarker(current_position_ms + 1000);
            return true;
        }
        else if ( ( ( key_event->modifiers() == Qt::KeypadModifier ) && ( key_event->key() == Qt::Key_1 ) ) ||
         ( key_event->key() == Qt::Key_F11 ) ) {

            // "[NUM]1" or "F11" key : move current position 1 second before
            current_position_ms = ui->waveForm->currentPositonMs();
            qint64 new_position_ms = current_position_ms - 1000;
            if ( new_position_ms < 0 ) {
                new_position_ms = 0;
            }
            ui->waveForm->updatePostionMarker(new_position_ms);
            return true;
        }
        else if ( ( ( key_event->modifiers() == Qt::KeypadModifier ) && ( key_event->key() == Qt::Key_8 ) ) ||
            ( key_event->key() == Qt::Key_F10 ) ) {

            // "[NUM]8" or "F10" key : move current position 1 frame after
            current_position_ms = ui->waveForm->currentPositonMs();
            qint16 frame_duration_ms = (qint16)( (qreal)SEC_TO_MSEC / qApp->property("prop_FrameRate_fps").toReal() );
            ui->waveForm->updatePostionMarker(current_position_ms + frame_duration_ms);
            return true;
        }
        else if ( ( ( key_event->modifiers() == Qt::KeypadModifier ) && ( key_event->key() == Qt::Key_7 ) ) ||
         ( key_event->key() == Qt::Key_F9 ) ) {

            // "[NUM]7" or "F9" key : move current position 1 frame before
            current_position_ms = ui->waveForm->currentPositonMs();
            qint16 frame_duration_ms = (qint16)( (qreal)SEC_TO_MSEC / qApp->property("prop_FrameRate_fps").toReal() );
            qint64 new_position_ms = current_position_ms - frame_duration_ms;

            if ( new_position_ms < 0 ) {
                new_position_ms = 0;
            }
            ui->waveForm->updatePostionMarker(new_position_ms);
            return true;
        }
        else if ( ( ( key_event->modifiers() == Qt::KeypadModifier ) && ( key_event->key() == Qt::Key_Minus ) ) ||
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
        else if ( ( ( key_event->modifiers() == Qt::KeypadModifier ) && ( key_event->key() == Qt::Key_Plus ) ) ||
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
        else if ( ( ( key_event->modifiers() == Qt::KeypadModifier ) && ( key_event->key() == Qt::Key_4 ) ) ||
         ( ( key_event->modifiers() == Qt::ControlModifier ) && ( key_event->key() == Qt::Key_P ) ) ) {

            //"[NUM]4" or "Ctrl + P" key : switch play / pause video player

            mMarkerPosChanged = true;
            ui->videoPlayer->on_playButton_clicked();

            return true;
        }
        else if ( ( key_event->key() == Qt::Key_Z ) && ( key_event->modifiers() & Qt::ControlModifier ) ) {

            bool status;

            if ( key_event->modifiers() & Qt::ShiftModifier ) {

                status = this->redo();
            }
            else {
                status = this->undo();
            }

            if ( status == true ) {

                // Remove all and draw subtitles zones in the waveform
                ui->waveForm->removeAllSubtitlesZones();
                ui->waveForm->drawSubtitlesZone(ui->subTable->saveSubtitles(), ui->subTable->currentIndex());
                ui->waveForm->changeZoneColor(ui->subTable->selectedIndex(), ui->subTable->currentIndex());
            }
            return true;
        }
    }

    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::onCtrlLeftClickEvent(qint64 positionMs) {

    this->changeSubStartTime(positionMs);
}

void MainWindow::onCtrlRightClickEvent(qint64 positionMs) {

    this->changeSubEndTime(positionMs);
}

// Current item selection changed in the database.
void MainWindow::currentSelectionChanged(qint64 positionMs) {

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

        new_subtitle.setText(new_line, this->getFontToolBox());

        new_subtitle.setDurationAuto(true);

        if ( ui->subTable->insertNewSub(new_subtitle, current_position_ms) == false ) {

            QString error_msg = ui->subTable->errorMsg();
            QMessageBox::warning(this, "Insert subtitle", error_msg);
            MySubtitles empty_subtitle;
            ui->stEditDisplay->setText(empty_subtitle);
        }
        else {

            // Save the database current state in history
            this->saveToHistory("Insert new subtitle");

            // Draw subtitle zone in the waveform
            QList<MySubtitles> subtitle_list;
            subtitle_list.append(new_subtitle);
            qint32 current_subtitle_index = ui->subTable->currentIndex();
            ui->waveForm->drawSubtitlesZone(subtitle_list, current_subtitle_index);
            ui->waveForm->changeZoneColor(ui->subTable->selectedIndex(), current_subtitle_index);

        }
    } // There are a subtitle for the current time. Update the text in the database
    else {

        if ( ui->stEditDisplay->text().count() == 1 ) {

            ui->stEditDisplay->updateTextFont(this->getFontToolBox(), this->getPosToolBox());
        }
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
        ui->videoPlayer->updateTime(positionMs);
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

    ui->waveForm->changeZoneColor(ui->subTable->selectedIndex(), ui->subTable->currentIndex());
}

// Interface to manage current sub changing
void MainWindow::currentSubChanged(MySubtitles subtitle) {

    // Display current subtitle text
    ui->stEditDisplay->setText(subtitle);

    // Change auto duration checkbox state
    if ( subtitle.isValid() ) {
        ui->durationAutoCheckBox->setEnabled(true);
        ui->durationAutoCheckBox->setChecked( subtitle.isDurationAuto() );
    }
    else {
        ui->durationAutoCheckBox->setEnabled(false);
    }
}

// Change the current subtitle "start time"
bool MainWindow::changeSubStartTime(qint64 &positionMs, qint32 refIndex, QList<MySubtitles> subList, bool multiChange,bool applyChange, bool movePrevious) {

    qint32 current_subtitle_index;
    qint32 ref_subtitle_index;
    QList<qint32> selected_indexes;
    qint32 delta_ms;
    QTime time_base(0, 0, 0, 0);
    qint32 changed_count = 0;
    qint32 prev_changed_count = 0;
    qint32 failed_count = 0;
    QString error_msgs;
    QList<MySubtitles> sub_list;

    // Use the reference subtitle index passed in argument else used the current index as reference
    if ( refIndex == -1 ) {
        ref_subtitle_index = ui->subTable->currentIndex();
    }
    else {
        ref_subtitle_index = refIndex;
    }

    // Use the subtitles list passed in argument else used the current subtitle list
    if ( subList.isEmpty() ) {

        // Get the subtitles list from the database
        sub_list = ui->subTable->saveSubtitles();
    }
    else {
        sub_list = subList;
    }

    if ( ( ref_subtitle_index >= sub_list.count() ) ||
         ( ref_subtitle_index < 0 ) ) {
        return false;
    }

    // Get the current subtitle start time and compute the time shift
    MySubtitles current_subtitle = sub_list[ref_subtitle_index];
    qint64 current_sub_start_time_ms = MyAttributesConverter::timeStrHMStoMs( current_subtitle.startTime() );

    delta_ms = positionMs - current_sub_start_time_ms;

    // Changment apply on all selected subtitle or only on current
    if ( multiChange == true ) {
        selected_indexes = ui->subTable->selectedIndex();
        qSort(selected_indexes.begin(), selected_indexes.end());
    }
    else {
        selected_indexes.append(ref_subtitle_index);
    }

    qint32 sub_min_interval_ms = ( ( (qreal)SEC_TO_MSEC / qApp->property("prop_FrameRate_fps").toReal() ) * qApp->property("prop_SubMinInterval_frame").toReal() );

    QList<qint32>::iterator it;
    for ( it = selected_indexes.begin(); it != selected_indexes.end(); ++it ) {

        current_subtitle_index = *it;

        // Check if there are a subtitle selected
        if ( current_subtitle_index >= 0 ) {

            current_subtitle = sub_list[current_subtitle_index];

            current_sub_start_time_ms = MyAttributesConverter::timeStrHMStoMs( current_subtitle.startTime() );

            // Deactivate the duration auto for the reference subtitle
            if ( current_subtitle_index == ref_subtitle_index ) {
                current_subtitle.setDurationAuto(false);
            }

            // Compute the new position relative to this subtitle start time
            positionMs = current_sub_start_time_ms + delta_ms;

            // Check if there are a subtitle before
            if ( ( current_subtitle_index - 1 ) >= 0) {

                MySubtitles previous_subtitle = sub_list[current_subtitle_index - 1];
                qint64 previous_sub_start_time_ms = MyAttributesConverter::timeStrHMStoMs( previous_subtitle.startTime() );
                qint64 previous_sub_end_time_ms = MyAttributesConverter::timeStrHMStoMs( previous_subtitle.endTime() );

                // Check time validity
                if ( previous_sub_start_time_ms >= 0 ) {

                    // If the current sub new "start time" is before the previous sub "start time" (previous sub completely recover)
                    // Abord
                    if ( positionMs <= previous_sub_start_time_ms ) {
//                        error_msgs.append("Subtitle n° " +QString::number(current_subtitle_index + 1) +" : start time < previous start time \n");
//                        failed_count++;
//                        continue;
                        positionMs = previous_sub_end_time_ms + sub_min_interval_ms;
                    }

                    // Check time validity
                    if ( previous_sub_end_time_ms >=0 ) {

                        // If the current sub new "start time" is between the previous sub "start/end time"
                        // Move the previous subtitle "end time" at the current subtitle "start time" - X frames,
                        // where X is the minimum interval between two subtitles
                        if ( positionMs < ( previous_sub_end_time_ms + sub_min_interval_ms ) ) {

                            if ( movePrevious == true ) {

                                qint64 new_end_time_ms = positionMs - sub_min_interval_ms;

                                if ( new_end_time_ms <=  MyAttributesConverter::timeStrHMStoMs( previous_subtitle.startTime() ) ) {
                                    error_msgs.append("Subtitle n° " +QString::number(current_subtitle_index + 1) +" : start time < previous start time \n");
                                    failed_count++;
                                    continue;
                                }
                                else {
                                    // If ok, change the previous sub end timecode
                                    previous_subtitle.setEndTime( time_base.addMSecs(new_end_time_ms).toString("hh:mm:ss.zzz") );
                                    sub_list.replace(current_subtitle_index - 1, previous_subtitle );
                                    error_msgs.append("Subtitle n° " +QString::number(current_subtitle_index) +" : end time code changed \n");
                                    prev_changed_count++;
                                }
                            }
                            else {
                                positionMs = previous_sub_end_time_ms + sub_min_interval_ms;
                            }
                        }
                    } // Time not valid, abord
                    else {
                        error_msgs.append("Subtitle n° " +QString::number(current_subtitle_index) +" : end time code invalid \n");
                        failed_count++;
                        continue;
                    }
                }   // Time not valid, abord
                else {
                    error_msgs.append("Subtitle n° " +QString::number(current_subtitle_index) +" : start time code invalid \n");
                    failed_count++;
                    continue;
                }
            }

            // Bound negative start time code to 0
            if ( positionMs < 0 ) {
                positionMs = 0;
            }

            // Check if the new start time is not after its end time
            if ( positionMs >= MyAttributesConverter::timeStrHMStoMs( current_subtitle.endTime() ) ) {
                error_msgs.append("Subtitle n° " +QString::number(current_subtitle_index + 1) +" : new start time code > end time code \n");
                failed_count++;
                continue;
            }
            else {
                // If ok, change the start timecode
                current_subtitle.setStartTime( time_base.addMSecs(positionMs).toString("hh:mm:ss.zzz") );
                sub_list.replace(current_subtitle_index, current_subtitle );
                changed_count++;
            }
        }
    }

    if ( applyChange == true ) {

        // If there are some change failed or some subtitle end time code forced to change
        // Display a warning message
        if ( ( failed_count > 0 ) || ( prev_changed_count > 0) ) {

            QMessageBox msg_box(this);
            msg_box.setText( "<b>Change Start Timecode</b><br><br>"
                             "- Start timecodes changed successfully : " +QString::number(changed_count) +"<br>"
                             "- End time code forced to change : " +QString::number(prev_changed_count)  +"<br>"
                             "- Failed : " +QString::number(failed_count) );
            msg_box.setInformativeText("Do you want to keep changes ?");
            msg_box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msg_box.setDefaultButton(QMessageBox::No);
            if ( failed_count > 0 ) {
                msg_box.button(QMessageBox::Yes)->setEnabled(false);
            }
            msg_box.setDetailedText(error_msgs);

            if ( msg_box.exec() == QMessageBox::No ) {
                return false;
            }
        }

        // Load modified subtitles in the database
        ui->subTable->loadSubtitles(sub_list);

        // Save the database current state in history
        this->saveToHistory("Change subtitle start timecode");

        // Remove all and draw subtitles zones in the waveform
        ui->waveForm->removeAllSubtitlesZones();
        ui->waveForm->drawSubtitlesZone(sub_list, ui->subTable->currentIndex());
        ui->waveForm->changeZoneColor(ui->subTable->selectedIndex(), ui->subTable->currentIndex());
    }

    if ( changed_count > 0 )
        return true;
    else {
        return false;
    }
}

// Change the current subtitle "start time"
bool MainWindow::changeSubEndTime(qint64 &positionMs, qint32 refIndex, QList<MySubtitles> subList, bool multiChange, bool applyChange, bool moveNext) {

    qint32 current_subtitle_index;
    qint32 ref_subtitle_index;
    QList<qint32> selected_indexes;
    qint32 delta_ms;
    QTime time_base(0, 0, 0, 0);
    qint32 changed_count = 0;
    qint32 next_changed_count = 0;
    qint32 failed_count = 0;
    QString error_msgs;
    QList<MySubtitles> sub_list;

    // Use the reference subtitle index passed in argument else used the current index as reference
    if ( refIndex == -1 ) {
        ref_subtitle_index = ui->subTable->currentIndex();
    }
    else {
        ref_subtitle_index = refIndex;
    }

    // Use the subtitles list passed in argument else used the current subtitle list
    if ( subList.isEmpty() ) {

        // Get the subtitles list from the database
        sub_list = ui->subTable->saveSubtitles();
    }
    else {
        sub_list = subList;
    }

    if ( ( ref_subtitle_index >= sub_list.count() ) ||
         ( ref_subtitle_index < 0 ) ) {
        return false;
    }

    // Get the current subtitle start time and compute the time shift
    MySubtitles current_subtitle = sub_list[ref_subtitle_index];

    qint64 current_sub_end_time_ms = MyAttributesConverter::timeStrHMStoMs( current_subtitle.endTime() );

    delta_ms = positionMs - current_sub_end_time_ms;

    // Changment apply on all selected subtitle or only on current
    if ( multiChange == true ) {
        selected_indexes = ui->subTable->selectedIndex();
        qSort(selected_indexes.begin(), selected_indexes.end(), qGreater<qint32>());
    }
    else {
        selected_indexes.append(ref_subtitle_index);
    }

    qint32 sub_min_interval_ms = ( ( (qreal)SEC_TO_MSEC / qApp->property("prop_FrameRate_fps").toReal() ) * qApp->property("prop_SubMinInterval_frame").toReal() );

    QList<qint32>::iterator it;
    for ( it = selected_indexes.begin(); it != selected_indexes.end(); ++it ) {

        current_subtitle_index = *it;

        current_subtitle = sub_list[current_subtitle_index];

        current_sub_end_time_ms = MyAttributesConverter::timeStrHMStoMs( current_subtitle.endTime() );

        // Deactivate the duration auto for the reference subtitle
        if ( current_subtitle_index == ref_subtitle_index ) {
            current_subtitle.setDurationAuto(false);
        }

        // Compute the new position relative to this subtitle start time
        positionMs = current_sub_end_time_ms + delta_ms;

        // Check if there are a subtitle selected
        if ( current_subtitle_index < ui->subTable->subtitlesCount() ) {

            // Check if there are a subtitle after
            if ( ( current_subtitle_index + 1 ) < ui->subTable->subtitlesCount() ) {

                MySubtitles next_subtitle = sub_list[current_subtitle_index + 1];

                qint64 next_sub_end_time_ms = MyAttributesConverter::timeStrHMStoMs( next_subtitle.endTime() );
                qint64 next_sub_start_time_ms = MyAttributesConverter::timeStrHMStoMs( next_subtitle.startTime() );

                // Check time validity
                if ( next_sub_end_time_ms >= 0 ) {

                    // If the current sub new "end time" is after the next sub "end time" (next sub completely recover)
                    // Abord
                    if ( positionMs >= next_sub_end_time_ms ) {

//                        error_msgs.append("Subtitle n° " +QString::number(current_subtitle_index + 1) +" : end time > next end time \n");
//                        failed_count++;
//                        continue;
                        positionMs = next_sub_start_time_ms - sub_min_interval_ms;
                    }

                    // Check time validity
                    if ( next_sub_start_time_ms >=0 ) {

                        // If the current sub new "end time" is between the next sub "start/end time"
                        // Move the next subtitle "start time" at the current subtitle "end time" + X frames,
                        // where X is the minimum interval between two subtitles
                        if ( positionMs > ( next_sub_start_time_ms - sub_min_interval_ms ) )  {

                            if ( moveNext == true ) {

                                qint64 new_start_time_ms = positionMs + sub_min_interval_ms;

                                if ( new_start_time_ms >=  MyAttributesConverter::timeStrHMStoMs( next_subtitle.endTime() ) ) {
                                    error_msgs.append("Subtitle n° " +QString::number(current_subtitle_index + 1) +" : end time > next end time \n");
                                    failed_count++;
                                    continue;
                                }
                                else {
                                    // If ok, change the next sub start timecode
                                    next_subtitle.setStartTime( time_base.addMSecs(new_start_time_ms).toString("hh:mm:ss.zzz") );
                                    sub_list.replace(current_subtitle_index + 1, next_subtitle );
                                    error_msgs.append("Subtitle n° " +QString::number(current_subtitle_index + 2) +" : start time code changed \n");
                                    next_changed_count++;
                                }
                            }
                            else {
                                positionMs = next_sub_start_time_ms - sub_min_interval_ms;
                            }
                        }
                    } // Time not valid, abord
                    else {
                        error_msgs.append("Subtitle n° " +QString::number(current_subtitle_index + 2) +" : start time code invalid \n");
                        failed_count++;
                        continue;
                    }
                }   // Time not valid, abord
                else {
                    error_msgs.append("Subtitle n° " +QString::number(current_subtitle_index + 2) +" : end time code invalid \n");
                    failed_count++;
                    continue;
                }
            }

            // Check if the new end time is not before the subtitle start time
            if ( positionMs <= MyAttributesConverter::timeStrHMStoMs( current_subtitle.startTime() ) ) {
                error_msgs.append("Subtitle n° " +QString::number(current_subtitle_index + 1) +" : new end timecode < satrt timecode \n");
                failed_count++;
                continue;
            }
            else {
                // If ok, change the end timecode
                current_subtitle.setEndTime( time_base.addMSecs(positionMs).toString("hh:mm:ss.zzz") );
                sub_list.replace(current_subtitle_index, current_subtitle );
                changed_count++;
            }
        }
    }

    if ( applyChange == true ) {

        // If there are some change failed or some subtitle end time code forced to change
        // Display a warning message
        if ( ( failed_count > 0 ) || ( next_changed_count > 0) ) {

            QMessageBox msg_box(this);
            msg_box.setText( "<b>Change End Timecode</b>" "<br><br>"
                             "- End timecodes changed successfully : " +QString::number(changed_count) +"<br>"
                             "- Start time code forced to change : " +QString::number(next_changed_count) +"<br>"
                             "- Failed : " +QString::number(failed_count) );
            msg_box.setInformativeText("Do you want to keep changes ?");
            msg_box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msg_box.setDefaultButton(QMessageBox::No);
            if ( failed_count > 0 ) {
                msg_box.button(QMessageBox::Yes)->setEnabled(false);
            }
            msg_box.setDetailedText(error_msgs);

            if ( msg_box.exec() == QMessageBox::No ) {
                return false;
            }
        }

        // Load modified subtitles in the database
        ui->subTable->loadSubtitles(sub_list);

        // Save the database current state in history
        this->saveToHistory("Change subtitle end timecode");

        // Remove all and draw subtitles zones in the waveform
        ui->waveForm->removeAllSubtitlesZones();
        ui->waveForm->drawSubtitlesZone(sub_list, ui->subTable->currentIndex());
        ui->waveForm->changeZoneColor(ui->subTable->selectedIndex(), ui->subTable->currentIndex());
    }

    if ( changed_count > 0 )
        return true;
    else {
        return false;
    }
}

// Move the current subtitle at given position
void MainWindow::shiftSubtitles(qint64 positionMs, qint32 index) {

    qint32 current_subtitle_index;
    qint32 ref_subtitle_index;
    qint32 delta_ms;
    QTime time_base(0, 0, 0, 0);
    qint64 new_sub_start_time_ms;
    qint64 new_sub_end_time_ms;

    if ( index == -1 ) {
        ref_subtitle_index = ui->subTable->currentIndex();
    }
    else {
        ref_subtitle_index = index;
    }

    QList<MySubtitles> sub_list;
    sub_list = ui->subTable->saveSubtitles();

    if ( ( ref_subtitle_index >= sub_list.count() ) ||
         ( ref_subtitle_index < 0 ) ) {
        return;
    }

    MySubtitles current_subtitle = ui->subTable->getSubInfos(ref_subtitle_index);
    qint64 current_sub_start_time_ms = MyAttributesConverter::timeStrHMStoMs( current_subtitle.startTime() );
    qint64 current_sub_end_time_ms = MyAttributesConverter::timeStrHMStoMs( current_subtitle.endTime() );

    delta_ms = positionMs - current_sub_start_time_ms;

    QList<qint32> selected_indexes = ui->subTable->selectedIndex();

    if ( positionMs > current_sub_start_time_ms ) {
        qSort(selected_indexes.begin(), selected_indexes.end(), qGreater<qint32>());
    }
    else if ( positionMs < current_sub_start_time_ms ) {
        qSort(selected_indexes.begin(), selected_indexes.end());
    }
    else return;

    QList<qint32>::iterator it;
    for ( it = selected_indexes.begin(); it != selected_indexes.end(); ++it ) {

        current_subtitle_index = *it;

        current_subtitle = ui->subTable->getSubInfos(current_subtitle_index);
        current_sub_start_time_ms = MyAttributesConverter::timeStrHMStoMs( current_subtitle.startTime() );
        current_sub_end_time_ms = MyAttributesConverter::timeStrHMStoMs( current_subtitle.endTime() );

        // Check time validity
        if ( ( current_sub_start_time_ms >= 0 ) && ( current_sub_end_time_ms >= 0 ) ) {

            qint32 current_sub_durationMs = current_sub_end_time_ms - current_sub_start_time_ms;
            positionMs = current_sub_start_time_ms + delta_ms;

            // If the new subtitle position is negative, avoid.
            if ( positionMs < 0 ) {
                if ( ( positionMs + current_sub_durationMs ) <= 0 ) {
                    return;
                } // If just the start timecode is negative, bound to 0
                else {
                    current_sub_durationMs = current_sub_durationMs + positionMs;
                    positionMs = 0;
                }
            }

            new_sub_start_time_ms = positionMs;
            new_sub_end_time_ms = positionMs + current_sub_durationMs;

            // Check if the new "start time" is after the actual "start time"
            if ( positionMs > current_sub_start_time_ms ) {
                // Move the "end time" first
                if ( this->changeSubEndTime( new_sub_end_time_ms, current_subtitle_index, sub_list, false, false, false ) == false ) {
                    // If "end time" not well moved, avoid
                    return;
                }
                else {
                    new_sub_start_time_ms = new_sub_end_time_ms - current_sub_durationMs;
                }
            }// Check if the new "start time" is before the actual "start time"
            else if ( positionMs < current_sub_start_time_ms ) {
                // Move the "start time" first
                if ( this->changeSubStartTime( new_sub_start_time_ms, current_subtitle_index, sub_list, false, false, false ) == false ) {
                    // If "start time" not well moved, avoid
                    return;
                }
                else {
                    new_sub_end_time_ms = new_sub_start_time_ms + current_sub_durationMs;
                }
            }

            current_subtitle.setStartTime( time_base.addMSecs(new_sub_start_time_ms).toString("hh:mm:ss.zzz") );
            current_subtitle.setEndTime( time_base.addMSecs(new_sub_end_time_ms).toString("hh:mm:ss.zzz") );
            sub_list.replace(current_subtitle_index, current_subtitle );
        }
    }

    ui->subTable->loadSubtitles(sub_list);

    // Save the database current state in history
    this->saveToHistory("Shift subtitle");

    // Remove all and draw subtitles zones in the waveform
    ui->waveForm->removeAllSubtitlesZones();
    ui->waveForm->drawSubtitlesZone(sub_list, ui->subTable->currentIndex());
    ui->waveForm->changeZoneColor(ui->subTable->selectedIndex(), ui->subTable->currentIndex());
}

// Remove the selected subtitles
void MainWindow::removeSubtitles() {

    QList<qint32> selected_indexes = ui->subTable->selectedIndex();

    QList<MySubtitles> sub_list = ui->subTable->saveSubtitles();

    qSort(selected_indexes.begin(), selected_indexes.end(), qGreater<qint32>());

    QList<qint32>::iterator it;
    for ( it = selected_indexes.begin(); it != selected_indexes.end(); ++it ) {

        qint32 index = *it;
        sub_list.removeAt(index);
    }

    ui->subTable->loadSubtitles(sub_list, true);

    // Save the database current state in history
    this->saveToHistory("Remove subtitle");

    qint32 current_sub_index = ui->subTable->currentIndex();

    // If there no more subtitle in the database, erase the text in edition zone
    if ( current_sub_index < 0 ) {
        ui->stEditDisplay->setText(ui->subTable->getSubInfos(current_sub_index));
    }

    // Remove all and draw subtitles zones in the waveform
    ui->waveForm->removeAllSubtitlesZones();
    ui->waveForm->drawSubtitlesZone(sub_list, current_sub_index);
    ui->waveForm->changeZoneColor(ui->subTable->selectedIndex(), ui->subTable->currentIndex());
}

// Frame rate change managment
void MainWindow::updateFrameRate(qreal frameRate) {

    this->updateFrameRateBox(frameRate);

    QList<MySubtitles> sub_list = ui->subTable->saveSubtitles();

    MyAttributesConverter::roundSubListToFrame(frameRate, sub_list);

    ui->subTable->loadSubtitles(sub_list);

    // Save the database current state in history
    this->saveToHistory("Change frame rate");

    // Remove all and draw subtitles zones in the waveform
    ui->waveForm->removeAllSubtitlesZones();
    qint32 current_subtitle_index = ui->subTable->currentIndex();
    ui->waveForm->drawSubtitlesZone(sub_list, current_subtitle_index);
    ui->waveForm->changeZoneColor(ui->subTable->selectedIndex(), current_subtitle_index);
}

// When "Open" action is triggered
void MainWindow::on_actionOpen_triggered() {

    QString current_video_file_name;

    // Open an Amapola Project File (.apf)
    QString file_name = QFileDialog::getOpenFileName(this, tr("Open Amapola project"),QDir::homePath(),
                                                     "AmpaProj (*.apf)");


    // Read the file
    MyFileReader file_reader(file_name, "UTF-8");

    if ( file_reader.readFile(file_name, "UTF-8") == false ) {
        QString error_msg = file_reader.errorMsg();
        QMessageBox::warning(this, "Open Amapola project", error_msg);
        return;
    }

    // Remember the current project file path name
    qApp->setProperty("currentProjFile",file_name);

    // Remember the current video file
    current_video_file_name = qApp->property("currentVideoFileName").toString();


    // Parse the file
    AmapolaProjFileParser* parser = new AmapolaProjFileParser();

    QList<MySubtitles> subtitles_list = parser->open(file_reader);

    // Update the default parameters
    ui->stEditDisplay->updateDefaultSub();
    // Update the settings
    ui->settings->updateDisplayAll();
    updateResoltionBox(qApp->property("prop_resolution_px").toSizeF());
    updateFrameRateBox(qApp->property("prop_FrameRate_fps").toDouble());

    // Load the subtitles list in the database
    ui->subTable->loadSubtitles(subtitles_list, false);
    // Save the database current state in history
    this->clearHistory();
    this->saveToHistory("Init");
    // Remove all and draw subtitles zones in the waveform
    ui->waveForm->removeAllSubtitlesZones();

    if ( !subtitles_list.isEmpty() ) {

        // Update the subtitles draw zones in the waveform
        qint32 current_subtitle_index = ui->subTable->currentIndex();
        ui->waveForm->drawSubtitlesZone(subtitles_list, current_subtitle_index);
        ui->waveForm->changeZoneColor(ui->subTable->selectedIndex(), current_subtitle_index);
    }
    else {

        // Erase the text edit zone
        MySubtitles empty_sub;
        ui->stEditDisplay->setText(empty_sub);
    }

    // Retrieive the new video file path name
    QString new_video_file_name = qApp->property("currentVideoFileName").toString();

    if ( !new_video_file_name.isEmpty() ) {

        // If the video have changed, load the new one
        if ( new_video_file_name != current_video_file_name ) {

            this->openVideo(new_video_file_name);
        }
    }
    else {

        // Unload all media, erase the waveform
        ui->waveForm->openFile("", "");
        ui->videoPlayer->unloadMedia();
    }
}

// When "Save as" action is triggered
void MainWindow::on_actionSave_as_triggered() {

    // Create or open file to write datas
    QString file_name = QFileDialog::getSaveFileName(this, tr("Save Amapola project"),QDir::homePath(),
                                                     tr("AmpaProj (*.apf)") );

    if ( !file_name.isEmpty() ) {

        // Add the ".apf" extension if it miss
        if ( !file_name.contains(".apf") ) {
            file_name.append(".apf");
        }

        // Save the project
        this->saveProject(file_name);
    }
}

// When "Save" action is triggered
void MainWindow::on_actionSave_triggered() {

    // If there are no file specified to save the project
    if ( qApp->property("currentProjFile").toString().isEmpty() ) {

        // Call "Save as" action to open a file dialog
        this->on_actionSave_as_triggered();
    }
    else {

        this->saveProject( qApp->property("currentProjFile").toString() );
    }
}

// Save the current state of Amapola application into a ".apf" file
void MainWindow::saveProject(QString fileName) {

    MyFileWriter file_writer(fileName, "UTF-8");

    AmapolaProjFileParser* parser = new AmapolaProjFileParser();

    // Retreive the subtitles datas from databases
    QList<MySubtitles> subtitles_list = ui->subTable->saveSubtitles();

    // Save the subtitles, the video opened, all default parameters and current settings
    parser->save(file_writer, subtitles_list, qApp->property("currentVideoFileName").toString());

    if ( file_writer.toFile() == false ) {
        QString error_msg = file_writer.errorMsg();
        QMessageBox::warning(this, "Save project", error_msg);
    }
    else {

        // Remember the current project file path name
        qApp->setProperty("currentProjFile", fileName);
    }
}

// When "Open" action is triggered
void MainWindow::on_actionOpen_video_triggered() {

    // Open video with no file name specified to open a file dialog
    this->openVideo("");
}

// Manage the openning of video file
void MainWindow::openVideo(QString fileName ) {

    // Open a video file with the palyer
    QString video_file_name = ui->videoPlayer->openFile(fileName);

    // If the media player has openned the video
    if ( !video_file_name.isEmpty() ) {

        qApp->setProperty("currentVideoFileName",video_file_name);

        // Open the corresponding waveform
        QString wf_file_name = video_file_name;
        wf_file_name.truncate(wf_file_name.lastIndexOf("."));
        wf_file_name.append(".awf");

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
    qreal ratio;

    // Video item size is the size of the zone where is displayed the video
    video_item_size = ui->videoPlayer->videoItemSize();
    video_item_ratio = video_item_size.width() / video_item_size.height();

    // Video current size is the displayed size of the video,
    // scaled on the video item with its ratio kept
    video_native_size = qApp->property("prop_resolution_px").toSizeF();
    video_current_size = video_native_size.scaled(video_item_size, Qt::KeepAspectRatio);

    ratio = video_native_size.width() / video_native_size.height();

    // Compare the ratio of native video and video item
    // to knwon if the video is full scale on width or on height of the item
    if ( ratio > video_item_ratio ) {
        video_x_pos = 10;
        video_y_pos = (qint32)( qRound( video_item_size.height() - video_current_size.height() ) / 2.0 ) + 10;
    }
    else if ( ratio < video_item_ratio ) {
        video_x_pos = (qint32)( qRound( video_item_size.width() - video_current_size.width() ) / 2.0 ) + 10;
        video_y_pos = 10;
    }


    qApp->setProperty("prop_editWidgetSize_px", video_current_size.toSize());

    // Set the edit region size and position mapped on video.
    ui->stEditDisplay->setFixedSize(video_current_size.toSize());
    ui->stEditDisplay->move(video_x_pos, video_y_pos);

    ui->playerInfoLabel->move(video_x_pos, video_y_pos);

}

void MainWindow::updateMarginsDraw() {

    ui->stEditDisplay->repaint();
}

void MainWindow::resizeEvent(QResizeEvent* event) {

    updateStEditSize();
    if ( ui->subTable->columnWidth(6) < 400 ) {
        ui->subTable->setColumnWidth(6, 400);
    }
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
                                                     tr("All Files(*);;SubRip (*.srt);;DCSubtitle (*.xml)"),
                                                     &selected_filter);


    MyFileReader file_reader(file_name, "UTF-8");

    MySubtitleFileParser* parser;

    if ( file_name.section(".", -1) == "stl" ) {

        if ( file_reader.readRawData(file_name) == false ) {
            QString error_msg = file_reader.errorMsg();
            QMessageBox::warning(this, "Import subtitles", error_msg);
            return;
        }

        parser = new EbuParser();
    }
    else {

        if ( file_reader.readFile(file_name, "UTF-8") == false ) {
            QString error_msg = file_reader.errorMsg();
            QMessageBox::warning(this, "Import subtitles", error_msg);
            return;
        }

        // Choose a parser to parse the file
        parser = SubImportManager::findSubFileParser(file_reader);
    }

    if ( parser != NULL ) {

        // Parse the file
        QList<MySubtitles> subtitles_list = parser->open(file_reader);

        // If parsing is successfull, load the subtitles list in the database
        if ( !subtitles_list.isEmpty() ) {

            // Round the timecodes to the current frame rate and load subtitles
            MyAttributesConverter::roundSubListToFrame(qApp->property("prop_FrameRate_fps").toDouble(), subtitles_list);
            ui->subTable->loadSubtitles(subtitles_list, false);

            // Save the database current state in history
            this->saveToHistory("Import subtitles");

            // Remove all and draw subtitles zones in the waveform
            ui->waveForm->removeAllSubtitlesZones();
            qint32 current_subtitle_index = ui->subTable->currentIndex();
            ui->waveForm->drawSubtitlesZone(subtitles_list, current_subtitle_index);
            ui->waveForm->changeZoneColor(ui->subTable->selectedIndex(), current_subtitle_index);
        }
    }
    else {

        QMessageBox::warning(this, "Import subtitles", "This file format is not supported");
        return;
    }
}

// Manage the subtitles export when "Export" button triggered
void MainWindow::on_actionExport_Subtitles_triggered() {

    SubExportDialog export_dialog(ui->subTable->saveSubtitles(), ui->subTable->selectedIndex(), this);

    if ( export_dialog.exec() == QDialog::Accepted ) {
        // Ok
    }
    else {
        // Rejected or error
    }
}

// Save the current state in "history", to allow undo/redo
void MainWindow::saveToHistory(QString changeReason) {

    qint32 begin_index = mHistoryCurrentIndex + 1;
    qint32 end_index = mSubListHistory.count();

    // Remove all saved states from current index + 1 to the last
    for ( qint32 i = begin_index; i < end_index; i++ ) {
        mHistoryReasons.removeLast();
        mSubListHistory.removeLast();
        mPropertyHistory.removeLast();
    }

    // If the last action was "Shift subtitle", delete one more state ("Change subtile start/end")
    // before to save the current state
    if ( changeReason == "Shift subtitle") {
        mHistoryReasons.removeLast();
        mSubListHistory.removeLast();
        mPropertyHistory.removeLast();
        mHistoryCurrentIndex--;
    }

    // Register the current state
    mHistoryReasons.append(changeReason);
    mSubListHistory.append(ui->subTable->saveSubtitles());
    mPropertyHistory.append(ui->settings->getCurrentProp());
    mHistoryCurrentIndex++;
}

void MainWindow::clearHistory() {

    // Init history (undo/redo)
    mSubListHistory.clear();
    mHistoryReasons.clear();
    mPropertyHistory.clear();
    mHistoryCurrentIndex = -1;
}

bool MainWindow::undo() {

    // If previous state exist in history
    if ( (mHistoryCurrentIndex - 1) >= 0 ) {

        // If the action to undo is "Change frame rate" update the frame rate combo box
        if ( mHistoryReasons.at(mHistoryCurrentIndex) == "Change frame rate") {

            qreal frame_rate = mPropertyHistory[mHistoryCurrentIndex - 1].frameRate();
            ui->settings->setFrameRate(frame_rate);
            this->updateFrameRateBox(frame_rate);
        }

        // Reload the subtitles in database
        ui->subTable->loadSubtitles(mSubListHistory.at(mHistoryCurrentIndex - 1));
        mHistoryCurrentIndex--;

        ui->stEditDisplay->setText(ui->subTable->getSubInfos(ui->subTable->currentIndex()));

        return true;
    }
    return false;
}

bool MainWindow::redo() {

    // If next state exist in history
    if ( (mHistoryCurrentIndex + 1) < mSubListHistory.count() ) {

        // If the action to redo is "Change frame rate" update the frame rate combo box
        if ( mHistoryReasons.at(mHistoryCurrentIndex + 1) == "Change frame rate") {

            qreal frame_rate = mPropertyHistory[mHistoryCurrentIndex + 1].frameRate();
            ui->settings->setFrameRate(frame_rate);
            this->updateFrameRateBox(frame_rate);
        }

        // Reload the subtitles in database
        ui->subTable->loadSubtitles(mSubListHistory.at(mHistoryCurrentIndex + 1));
        mHistoryCurrentIndex++;
        return true;
    }
    return false;
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

// Toll bar

// Resolution
void MainWindow::resolutionComboBox_currentIndexChanged(int index) {

    if ( mResolutionChangedNySoft == false ) {

        if ( mResolutionComboBox->currentText() == "Other...") {

            QSize resolution_size = InputSizeDialog::getSize(this, tr("Set resolution"));
            updateResoltionBox(resolution_size);
        }
        else {

            qApp->setProperty("prop_resolution_px", mResolutionComboBox->itemData(index));
            this->updateStEditSize();
        }
    }

    mResolutionChangedNySoft = false;
}

void MainWindow::updateResoltionBox(QSizeF videoResolution) {

    QSize video_resolution = videoResolution.toSize();

    for ( qint16 i = 0; i < mResolutionComboBox->count(); i++ ) {

        if ( mResolutionComboBox->itemData(i).toSizeF() == video_resolution ) {

            mResolutionComboBox->setCurrentIndex(i);
            return;
        }
    }

    QString resolution_str = QString::number(video_resolution.width()) +"x" +QString::number(video_resolution.height());
    mResolutionChangedNySoft = true;
    mResolutionComboBox->insertItem(mResolutionComboBox->count() - 1, resolution_str, video_resolution);
    mResolutionChangedNySoft = false;
    mResolutionComboBox->setCurrentIndex(mResolutionComboBox->count() - 2);
}

void MainWindow::frameRateComboBox_currentIndexChanged(int index) {

    if ( mFrameRateChangedBySoft == false ) {

        ui->settings->setFrameRate(mFrameRateComboBox->itemData(index).toDouble());
    }

    mFrameRateChangedBySoft = false;
}

void MainWindow::updateFrameRateBox(qreal frameRate) {

    for ( qint16 i = 0; i < mFrameRateComboBox->count(); i++ ) {

        if ( qFuzzyCompare(mFrameRateComboBox->itemData(i).toDouble(), frameRate ) ) {

            mFrameRateChangedBySoft = true;
            mFrameRateComboBox->setCurrentIndex(i);
            return;
        }
    }

    QString frame_rate_str = QString::number(frameRate,'f', 3);
    mFrameRateComboBox->insertItem(mFrameRateComboBox->count(), frame_rate_str, frameRate);
    mFrameRateChangedBySoft = true;
    mFrameRateComboBox->setCurrentIndex(mFrameRateComboBox->count() - 1);
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

    if ( line_nbr < current_subtitle_datas.text().count() ) {

        // Retrieve font and position of the current line
        text_line = current_subtitle_datas.text().at(line_nbr);
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

        // Check the line number
        qint16 line_nbr = ui->stEditDisplay->lastFocused();

        if ( line_nbr >= 0 ) {

            // Retrieve the current subtitle line "line_nbr" font
            TextFont current_text_font = current_subtitle_datas.text()[line_nbr].Font();

            // Push the font properties in the new line
            new_text_line.setFont(current_text_font);

            // Replace the line in the subtitle container
            current_subtitle_datas.text()[line_nbr] = new_text_line;
        }

        // Push the new datas in the table
        ui->subTable->updateDatas(current_subtitle_datas);

        // Save the database current state in history
        this->saveToHistory("Change subtitle position");
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
            ui->stEditDisplay->updateDefaultSub();
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
            ui->stEditDisplay->updateDefaultSub();
        }

        updateTextPosition();
    }
}

// Horizontal alignment toolbox value changed
void MainWindow::on_hAlignBox_activated(const QString &value) {

    if ( mTextPosChangedBySoft == false ) {

        if ( ui->subTable->isNewEntry( ui->waveForm->currentPositonMs() ) ) {
            qApp->setProperty("prop_Halign", value);
            ui->stEditDisplay->updateDefaultSub();
        }

        updateTextPosition();
    }
}

// Horizontal position toolbox value changed
void MainWindow::on_hPosSpinBox_valueChanged(const QString &value) {

    if ( mTextPosChangedBySoft == false ) {

        if ( ui->subTable->isNewEntry( ui->waveForm->currentPositonMs() ) ) {
            qApp->setProperty("prop_Hposition_percent", QString::number(ui->hPosSpinBox->value(), 'f', 1));
            ui->stEditDisplay->updateDefaultSub();
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

        QString font_color_str = textFont.fontColor();

        qApp->setProperty("prop_FontColor_rgba", font_color_str);

        if ( font_color_str == qApp->property("prop_Color1") ) {
            ui->fontColor1Button->setChecked(true);
        }
        else if ( font_color_str == qApp->property("prop_Color2") ) {
            ui->fontColor2Button->setChecked(true);
        }
        else if ( font_color_str == qApp->property("prop_Color3") ) {
            ui->fontColor3Button->setChecked(true);
        }
        else if ( font_color_str == qApp->property("prop_Color4") ) {
            ui->fontColor4Button->setChecked(true);
        }
        else if ( font_color_str == qApp->property("prop_Color5") ) {
            ui->fontColor5Button->setChecked(true);
        }
        else if ( font_color_str == qApp->property("prop_Color6") ) {
            ui->fontColor6Button->setChecked(true);
        }
        else {
            ui->fontColor7Button->setChecked(true);
            qApp->setProperty("prop_Color7", font_color_str);
            mColor7 = MyAttributesConverter::stringToColor(font_color_str);
            MyAttributesConverter::setColorToButton(ui->fontColor7Button, mColor7);
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

        if ( textFont.fontBorderEffect().contains("yes", Qt::CaseInsensitive) == true ) {
            ui->fontBorderCheckBox->setChecked(true);
        }
        else {
            ui->fontBorderCheckBox->setChecked(false);
        }

        if ( textFont.fontShadowEffect().contains("yes", Qt::CaseInsensitive) == true ) {
            ui->fontShadowCheckBox->setChecked(true);
        }
        else {
            ui->fontShadowCheckBox->setChecked(false);
        }

        if ( textFont.fontBackgroundEffect().contains("yes", Qt::CaseInsensitive) == true ) {
            ui->fontBackgroundCheckBox->setChecked(true);
        }
        else {
            ui->fontBackgroundCheckBox->setChecked(false);
        }


        mTextBorderColor = MyAttributesConverter::stringToColor( textFont.fontBorderEffectColor() );
        mTextShadowColor = MyAttributesConverter::stringToColor( textFont.fontShadowEffectColor() );
        mTextBackgroundColor = MyAttributesConverter::stringToColor( textFont.fontBackgroundEffectColor() );

        MyAttributesConverter::setColorToButton(ui->fontBorderColor, mTextBorderColor);
        MyAttributesConverter::setColorToButton(ui->fontShadowColor, mTextShadowColor);
        MyAttributesConverter::setColorToButton(ui->fontBackgroundColor, mTextBackgroundColor);
    }

    mTextFontChangedBySoft = false;
}

// Get the font parameters from the tool boxes
TextFont MainWindow::getFontToolBox() {

    TextFont text_font;
    QString font_color_str;

    if ( ui->fontLabel->isEnabled() == true )  {

        QFont font = ui->fontIdComboBox->currentFont();
        text_font.setFontId( font.family() );

        text_font.setFontSize(ui->fontSizeSpinBox->cleanText());

        if ( ui->fontColor1Button->isChecked() ) {
            font_color_str = MyAttributesConverter::colorToString(mColor1);
        }
        else if ( ui->fontColor2Button->isChecked() ) {
            font_color_str = MyAttributesConverter::colorToString(mColor2);
        }
        else if ( ui->fontColor3Button->isChecked() ) {
            font_color_str = MyAttributesConverter::colorToString(mColor3);
        }
        else if ( ui->fontColor4Button->isChecked() ) {
            font_color_str = MyAttributesConverter::colorToString(mColor4);
        }
        else if ( ui->fontColor5Button->isChecked() ) {
            font_color_str = MyAttributesConverter::colorToString(mColor5);
        }
        else if ( ui->fontColor6Button->isChecked() ) {
            font_color_str = MyAttributesConverter::colorToString(mColor6);
        }
        else if ( ui->fontColor7Button->isChecked() ) {
            font_color_str = MyAttributesConverter::colorToString(mColor7);
        }

        text_font.setFontColor(font_color_str);

        if ( ui->subTable->isNewEntry( ui->waveForm->currentPositonMs() ) ) {
            qApp->setProperty("prop_FontColor_rgba", font_color_str);
            ui->stEditDisplay->updateDefaultSub();
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

        if ( ui->fontBorderCheckBox->isChecked() ) {
            text_font.setFontBorderEffect("yes");
        }
        else {
            text_font.setFontBorderEffect("no");
        }

        if ( ui->fontShadowCheckBox->isChecked() ) {
            text_font.setFontShadowEffect("yes");
        }
        else {
            text_font.setFontShadowEffect("no");
        }

        if ( ui->fontBackgroundCheckBox->isChecked() ) {
            text_font.setFontBackgroundEffect("yes");
        }
        else {
            text_font.setFontBackgroundEffect("no");
        }
        text_font.setFontBorderEffectColor( MyAttributesConverter::colorToString(mTextBorderColor) );
        text_font.setFontShadowEffectColor( MyAttributesConverter::colorToString(mTextShadowColor) );
        text_font.setFontBackgroundEffectColor( MyAttributesConverter::colorToString(mTextBackgroundColor) );
    }

    return text_font;
}

// Change the "font" of the current "edit line" from the font toolboxes parameters
void MainWindow::updateTextFont() {

    mTextFontChangedByUser = true;

    // Retrieve font parameters from tool boxes
    TextFont new_font = this->getFontToolBox();

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

        if ( line_nbr >= 0 ) {

            // Replace the font for "line_nbr" in current subtitle
            current_subtitle_datas.text()[line_nbr].setFont(new_font);
        }

        // Push the new datas in the table
        ui->subTable->updateDatas(current_subtitle_datas);

        // Save the database current state in history
        this->saveToHistory("Change subtitle font");
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
            ui->stEditDisplay->updateDefaultSub();
        }

        // Update the text edit font
        updateTextFont();
    }
}

// Font color selected (the 7 colors boxes are exclusive, if one is checked, the others are unchecked)
void MainWindow::on_fontColor1Button_toggled(bool checked) {

    if ( ( mTextFontChangedBySoft == false ) && ( checked == true ) ) {
        updateTextFont();
        ui->fontColor1Button->clearFocus();
    }
}

void MainWindow::on_fontColor2Button_toggled(bool checked) {

    if ( ( mTextFontChangedBySoft == false ) && ( checked == true ) ) {
        updateTextFont();
        ui->fontColor2Button->clearFocus();
    }
}

void MainWindow::on_fontColor3Button_toggled(bool checked) {

    if ( ( mTextFontChangedBySoft == false ) && ( checked == true ) ) {
        updateTextFont();
        ui->fontColor3Button->clearFocus();
    }
}

void MainWindow::on_fontColor4Button_toggled(bool checked) {

    if ( ( mTextFontChangedBySoft == false ) && ( checked == true ) ) {
        updateTextFont();
        ui->fontColor4Button->clearFocus();
    }
}

void MainWindow::on_fontColor5Button_toggled(bool checked) {

    if ( ( mTextFontChangedBySoft == false ) && ( checked == true ) ) {
        updateTextFont();
        ui->fontColor5Button->clearFocus();
    }
}

void MainWindow::on_fontColor6Button_toggled(bool checked) {

    if ( ( mTextFontChangedBySoft == false ) && ( checked == true ) ) {
        updateTextFont();
        ui->fontColor6Button->clearFocus();
    }
}

void MainWindow::on_fontColor7Button_toggled(bool checked) {

    if ( ( mTextFontChangedBySoft == false ) && ( checked == true ) ) {
        updateTextFont();
        ui->fontColor7Button->clearFocus();
    }
}

// Font color custom menu : change the color
void MainWindow::on_fontColor1Button_customContextMenuRequested(const QPoint &pos) {

    // Set context menu position
    QPoint global_pos = ui->fontColor1Button->mapToGlobal(pos);

    QMenu custom_menu;
    custom_menu.addAction("Change color ...");

    QAction* selected_item = custom_menu.exec(global_pos);

    if ( selected_item ) {
        if ( selected_item->text() == "Change color ..." ) {

            // Open a dialog abling user to chose a color
            QColor init_color = mColor1;
            QColor font_color = QColorDialog::getColor(init_color, this, tr("Select color"), QColorDialog::ShowAlphaChannel);

            if ( font_color.isValid() ) {

                // Save the color
                mColor1 = font_color;
                qApp->setProperty("prop_Color1", MyAttributesConverter::colorToString(font_color));

                // Change the color of the PushButton
                MyAttributesConverter::setColorToButton(ui->fontColor1Button, font_color);

                // Update the text font in the database and in the text edit
                if ( ui->fontColor1Button->isChecked() ) {
                    updateTextFont();
                }
            }
        }
    }
    ui->fontColor1Button->clearFocus();
}

void MainWindow::on_fontColor2Button_customContextMenuRequested(const QPoint &pos) {

    // Set context menu position
    QPoint global_pos = ui->fontColor2Button->mapToGlobal(pos);

    QMenu custom_menu;
    custom_menu.addAction("Change color ...");

    QAction* selected_item = custom_menu.exec(global_pos);

    if ( selected_item ) {
        if ( selected_item->text() == "Change color ..." ) {

            // Open a dialog abling user to chose a color
            QColor init_color = mColor2;
            QColor font_color = QColorDialog::getColor(init_color, this, tr("Select color"), QColorDialog::ShowAlphaChannel);

            if ( font_color.isValid() ) {

                // Save the color
                mColor2 = font_color;
                qApp->setProperty("prop_Color2", MyAttributesConverter::colorToString(font_color));

                // Change the color of the PushButton
                MyAttributesConverter::setColorToButton(ui->fontColor2Button, font_color);

                // Update the text font in the database and in the text edit
                if ( ui->fontColor2Button->isChecked() ) {
                    updateTextFont();
                }
            }
        }
    }
    ui->fontColor2Button->clearFocus();
}

void MainWindow::on_fontColor3Button_customContextMenuRequested(const QPoint &pos) {

    // Set context menu position
    QPoint global_pos = ui->fontColor3Button->mapToGlobal(pos);

    QMenu custom_menu;
    custom_menu.addAction("Change color ...");

    QAction* selected_item = custom_menu.exec(global_pos);

    if ( selected_item ) {
        if ( selected_item->text() == "Change color ..." ) {

            // Open a dialog abling user to chose a color
            QColor init_color = mColor3;
            QColor font_color = QColorDialog::getColor(init_color, this, tr("Select color"), QColorDialog::ShowAlphaChannel);

            if ( font_color.isValid() ) {

                // Save the color
                mColor3 = font_color;
                qApp->setProperty("prop_Color3", MyAttributesConverter::colorToString(font_color));

                // Change the color of the PushButton
                MyAttributesConverter::setColorToButton(ui->fontColor3Button, font_color);

                // Update the text font in the database and in the text edit
                if ( ui->fontColor3Button->isChecked() ) {
                    updateTextFont();
                }
            }
        }
    }
    ui->fontColor3Button->clearFocus();
}

void MainWindow::on_fontColor4Button_customContextMenuRequested(const QPoint &pos) {

    // Set context menu position
    QPoint global_pos = ui->fontColor4Button->mapToGlobal(pos);

    QMenu custom_menu;
    custom_menu.addAction("Change color ...");

    QAction* selected_item = custom_menu.exec(global_pos);

    if ( selected_item ) {
        if ( selected_item->text() == "Change color ..." ) {

            // Open a dialog abling user to chose a color
            QColor init_color = mColor4;
            QColor font_color = QColorDialog::getColor(init_color, this, tr("Select color"), QColorDialog::ShowAlphaChannel);

            if ( font_color.isValid() ) {

                // Save the color
                mColor4 = font_color;
                qApp->setProperty("prop_Color4", MyAttributesConverter::colorToString(font_color));

                // Change the color of the PushButton
                MyAttributesConverter::setColorToButton(ui->fontColor4Button, font_color);

                // Update the text font in the database and in the text edit
                if ( ui->fontColor4Button->isChecked() ) {
                    updateTextFont();
                }
            }
        }
    }
    ui->fontColor4Button->clearFocus();
}

void MainWindow::on_fontColor5Button_customContextMenuRequested(const QPoint &pos) {

    // Set context menu position
    QPoint global_pos = ui->fontColor5Button->mapToGlobal(pos);

    QMenu custom_menu;
    custom_menu.addAction("Change color ...");

    QAction* selected_item = custom_menu.exec(global_pos);

    if ( selected_item ) {
        if ( selected_item->text() == "Change color ..." ) {

            // Open a dialog abling user to chose a color
            QColor init_color = mColor5;
            QColor font_color = QColorDialog::getColor(init_color, this, tr("Select color"), QColorDialog::ShowAlphaChannel);

            if ( font_color.isValid() ) {

                // Save the color
                mColor5 = font_color;
                qApp->setProperty("prop_Color5", MyAttributesConverter::colorToString(font_color));

                // Change the color of the PushButton
                MyAttributesConverter::setColorToButton(ui->fontColor5Button, font_color);

                // Update the text font in the database and in the text edit
                if ( ui->fontColor5Button->isChecked() ) {
                    updateTextFont();
                }
            }
        }
    }
    ui->fontColor5Button->clearFocus();
}

void MainWindow::on_fontColor6Button_customContextMenuRequested(const QPoint &pos) {

    // Set context menu position
    QPoint global_pos = ui->fontColor6Button->mapToGlobal(pos);

    QMenu custom_menu;
    custom_menu.addAction("Change color ...");

    QAction* selected_item = custom_menu.exec(global_pos);

    if ( selected_item ) {
        if ( selected_item->text() == "Change color ..." ) {

            // Open a dialog abling user to chose a color
            QColor init_color = mColor6;
            QColor font_color = QColorDialog::getColor(init_color, this, tr("Select color"), QColorDialog::ShowAlphaChannel);

            if ( font_color.isValid() ) {

                // Save the color
                mColor6 = font_color;
                qApp->setProperty("prop_Color6", MyAttributesConverter::colorToString(font_color));

                // Change the color of the PushButton
                MyAttributesConverter::setColorToButton(ui->fontColor6Button, font_color);

                // Update the text font in the database and in the text edit
                if ( ui->fontColor6Button->isChecked() ) {
                    updateTextFont();
                }
            }
        }
    }
    ui->fontColor6Button->clearFocus();
}


// On font border on/off
void MainWindow::on_fontBorderCheckBox_toggled(bool checked) {

    if ( mTextFontChangedBySoft == false ) {

        // If there are no subtitle for the current position,
        // save this parameter as default parameter
        if ( ui->subTable->isNewEntry( ui->waveForm->currentPositonMs() ) ) {

            if ( checked ) {
                qApp->setProperty("prop_FontBorder", "yes");
            }
            else {
                qApp->setProperty("prop_FontBorder", "no");
            }

            ui->stEditDisplay->updateDefaultSub();
        }

        // Update the text font in the database and in the text edit
        updateTextFont();
    }
}

// On font shadow on/off
void MainWindow::on_fontShadowCheckBox_toggled(bool checked) {

    if ( mTextFontChangedBySoft == false ) {

        // If there are no subtitle for the current position,
        // save this parameter as default parameter
        if ( ui->subTable->isNewEntry( ui->waveForm->currentPositonMs() ) ) {

            if ( checked ) {
                qApp->setProperty("prop_FontShadow", "yes");
            }
            else {
                qApp->setProperty("prop_FontShadow", "no");
            }

            ui->stEditDisplay->updateDefaultSub();
        }

        // Update the text font in the database and in the text edit
        updateTextFont();
    }

}

// On font background on/off
void MainWindow::on_fontBackgroundCheckBox_toggled(bool checked) {

    if ( mTextFontChangedBySoft == false ) {

        // If there are no subtitle for the current position,
        // save this parameter as default parameter
        if ( ui->subTable->isNewEntry( ui->waveForm->currentPositonMs() ) ) {

            if ( checked ) {
                qApp->setProperty("prop_FontBackground", "yes");
            }
            else {
                qApp->setProperty("prop_FontBackground", "no");
            }

            ui->stEditDisplay->updateDefaultSub();
        }

        // Update the text font in the database and in the text edit
        updateTextFont();
    }
}

// On border color clicked
void MainWindow::on_fontBorderColor_clicked() {

    if ( mTextFontChangedBySoft == false ) {

        // Open a dialog abling user to chose a color
        QColor init_color = MyAttributesConverter::stringToColor( qApp->property("prop_FontBorderColor").toString() );
        QColor font_border_color = QColorDialog::getColor(init_color, this, tr("Select border effect color"), QColorDialog::ShowAlphaChannel);

        if ( font_border_color.isValid() ) {

            // Save the color
            mTextBorderColor = font_border_color;

            // If there are no subtitle for the current position,
            // save this parameter as default parameter
            if ( ui->subTable->isNewEntry( ui->waveForm->currentPositonMs() ) ) {
                qApp->setProperty("prop_FontBorderColor", MyAttributesConverter::colorToString(font_border_color));
                ui->stEditDisplay->updateDefaultSub();
            }

            // Change the color of the PushButton
            MyAttributesConverter::setColorToButton(ui->fontBorderColor, font_border_color);

            // Update the text font in the database and in the text edit
            updateTextFont();
        }
    }
}

// On shadow color clicked
void MainWindow::on_fontShadowColor_clicked() {

    if ( mTextFontChangedBySoft == false ) {

        // Open a dialog abling user to chose a color
        QColor init_color = MyAttributesConverter::stringToColor( qApp->property("prop_FontShadowColor").toString() );
        QColor font_shadow_color = QColorDialog::getColor(init_color, this, tr("Select shadow effect color"), QColorDialog::ShowAlphaChannel);

        if ( font_shadow_color.isValid() ) {

            // Save the color
            mTextShadowColor = font_shadow_color;

            // If there are no subtitle for the current position,
            // save this parameter as default parameter
            if ( ui->subTable->isNewEntry( ui->waveForm->currentPositonMs() ) ) {
                qApp->setProperty("prop_FontShadowColor", MyAttributesConverter::colorToString(font_shadow_color));
                ui->stEditDisplay->updateDefaultSub();
            }

            // Change the color of the PushButton
            MyAttributesConverter::setColorToButton(ui->fontShadowColor, font_shadow_color);

            // Update the text font in the database and in the text edit
            updateTextFont();
        }
    }
}

// On background color clicked
void MainWindow::on_fontBackgroundColor_clicked() {

    if ( mTextFontChangedBySoft == false ) {

        // Open a dialog abling user to chose a color
        QColor init_color = MyAttributesConverter::stringToColor( qApp->property("prop_FontBackgroundColor").toString() );
        QColor font_background_color = QColorDialog::getColor(init_color, this, tr("Select backgroung effect color"), QColorDialog::ShowAlphaChannel);

        if ( font_background_color.isValid() ) {

            // Save the color
            mTextBackgroundColor = font_background_color;

            // If there are no subtitle for the current position,
            // save this parameter as default parameter
            if ( ui->subTable->isNewEntry( ui->waveForm->currentPositonMs() ) ) {
                qApp->setProperty("prop_FontBackgroundColor", MyAttributesConverter::colorToString(font_background_color));
                ui->stEditDisplay->updateDefaultSub();
            }

            // Change the color of the PushButton
            MyAttributesConverter::setColorToButton(ui->fontBackgroundColor, font_background_color);

            // Update the text font in the database and in the text edit
            updateTextFont();
        }
    }
}

// Italic value changed
void MainWindow::on_fontItalicButton_toggled(bool checked) {

    if ( mTextFontChangedBySoft == false ) {

        // If there are no subtitle for the current position,
        // save this parameter as default parameter
        if ( ui->subTable->isNewEntry( ui->waveForm->currentPositonMs() ) ) {

            if ( checked ) {
                qApp->setProperty("prop_FontItalic", "yes");
            }
            else {
                qApp->setProperty("prop_FontItalic", "no");
            }

            ui->stEditDisplay->updateDefaultSub();
        }

        // Update the text font in the database and in the text edit
        updateTextFont();
    }
}

// Underlined value changed
void MainWindow::on_fontUnderlinedButton_toggled(bool checked) {

    if ( mTextFontChangedBySoft == false ) {

        // If there are no subtitle for the current position,
        // save this parameter as default parameter
        if ( ui->subTable->isNewEntry( ui->waveForm->currentPositonMs() ) ) {

            if ( checked ) {
                qApp->setProperty("prop_FontUnderlined", "yes");
            }
            else {
                qApp->setProperty("prop_FontUnderlined", "no");
            }

            ui->stEditDisplay->updateDefaultSub();
        }

        // Update the text font in the database and in the text edit
        updateTextFont();
    }
}

// Font name changed
void MainWindow::on_fontIdComboBox_currentFontChanged(const QFont &f) {

    if ( mTextFontChangedBySoft == false ) {

        // If there are no subtitle for the current position,
        // save this parameter as default parameter
        if ( ui->subTable->isNewEntry( ui->waveForm->currentPositonMs() ) ) {

            qApp->setProperty("prop_FontName", ui->fontIdComboBox->currentText());
            ui->stEditDisplay->updateDefaultSub();
        }

        // Update the text font in the database and in the text edit
        updateTextFont();
    }
}

void MainWindow::on_firstNumSpinBox_customContextMenuRequested(const QPoint &pos) {

    QPoint global_pos = ui->firstNumSpinBox->mapToGlobal(pos);

    QMenu custom_menu;
    custom_menu.addAction("Set from current");
    // ...

    QAction* selected_item = custom_menu.exec(global_pos);
    if ( selected_item ) {

        if ( selected_item->text() == "Set from current" ) {

            qint32 current_subtitle_index = ui->subTable->currentIndex();

            if (current_subtitle_index >= 0 ) {
                ui->firstNumSpinBox->setValue(current_subtitle_index);

                MySubtitles current_subtitle = ui->subTable->getSubInfos(current_subtitle_index);
                ui->firstStartTimeEdit->setTime( QTime::fromString(current_subtitle.startTime(), "hh:mm:ss.zzz") );
                ui->firstNewStartTimeEdit->setTime( QTime::fromString(current_subtitle.startTime(), "hh:mm:ss.zzz") );
            }
        }
    }
    else {
        // nothing was chosen
    }
}

void MainWindow::on_lastNumSpinBox_customContextMenuRequested(const QPoint &pos) {

    QPoint global_pos = ui->lastNumSpinBox->mapToGlobal(pos);

    QMenu custom_menu;
    custom_menu.addAction("Set from current");
    // ...

    QAction* selected_item = custom_menu.exec(global_pos);
    if ( selected_item ) {

        if ( selected_item->text() == "Set from current" ) {

            qint32 current_subtitle_index = ui->subTable->currentIndex();

            if (current_subtitle_index >= 0 ) {

                ui->lastNumSpinBox->setValue(current_subtitle_index);

                MySubtitles current_subtitle = ui->subTable->getSubInfos(current_subtitle_index);
                ui->lastStartTimeEdit->setTime( QTime::fromString(current_subtitle.startTime(), "hh:mm:ss.zzz") );
                ui->lastNewStartTimeEdit->setTime( QTime::fromString(current_subtitle.startTime(), "hh:mm:ss.zzz") );
            }
        }
    }
    else {
        // nothing was chosen
    }
}

void MainWindow::on_firstNewStartTimeEdit_customContextMenuRequested(const QPoint &pos) {

    QTime time_base(0, 0, 0, 0);

    QPoint global_pos = ui->firstNewStartTimeEdit->mapToGlobal(pos);

    QMenu custom_menu;
    custom_menu.addAction("Set from current");
    // ...

    QAction* selected_item = custom_menu.exec(global_pos);
    if ( selected_item ) {

        if ( selected_item->text() == "Set from current" ) {

            ui->firstNewStartTimeEdit->setTime( time_base.addMSecs(ui->waveForm->currentPositonMs()) );
        }
    }
    else {
        // nothing was chosen
    }
}

void MainWindow::on_lastNewStartTimeEdit_customContextMenuRequested(const QPoint &pos) {

    QTime time_base(0, 0, 0, 0);

    QPoint global_pos = ui->lastNewStartTimeEdit->mapToGlobal(pos);

    QMenu custom_menu;
    custom_menu.addAction("Set from current");
    // ...

    QAction* selected_item = custom_menu.exec(global_pos);
    if ( selected_item ) {

        if ( selected_item->text() == "Set from current" ) {

            ui->lastNewStartTimeEdit->setTime( time_base.addMSecs(ui->waveForm->currentPositonMs()) );
        }
    }
    else {
        // nothing was chosen
    }
}

// Rescale the subtitles timecodes
void MainWindow::on_syncApplyPushButton_clicked() {

    QTime time_base(0, 0, 0, 0);
    qint64 first_source_time_ms;
    qint64 first_dest_time_ms;
    qint64 last_source_time_ms;
    qint64 last_dest_time_ms;
    qint32 from_index, to_index;

    MySubtitles current_subtitle;
    qint64 current_sub_start_time_ms;
    qint64 current_sub_end_time_ms;
    qreal temp_time_ms;
    qint64 new_start_time_ms;
    qint64 new_end_time_ms;


    // Retrieve the timecodes set by the users
    first_source_time_ms = qAbs( ui->firstStartTimeEdit->time().msecsTo(time_base) );
    first_dest_time_ms = qAbs( ui->firstNewStartTimeEdit->time().msecsTo(time_base) );
    last_source_time_ms = qAbs( ui->lastStartTimeEdit->time().msecsTo(time_base) );
    last_dest_time_ms = qAbs( ui->lastNewStartTimeEdit->time().msecsTo(time_base) );

    // Compute the rescale factor
    qreal rescale_factor = (qreal)( (last_dest_time_ms - last_source_time_ms) - (first_dest_time_ms - first_source_time_ms) ) / (qreal)(last_source_time_ms - first_source_time_ms);

    // Get the current subtiles list from the database
    QList<MySubtitles> sub_list = ui->subTable->saveSubtitles();

    if ( ui->firstNumSpinBox->value() >= ui->lastNumSpinBox->value() ) {
        // Error
        QMessageBox::warning(this, "Rescale", "Rescale failed\nThe first point is superior to the last point");
        return;
    }

    if ( ( first_source_time_ms == first_dest_time_ms ) && ( last_source_time_ms == last_dest_time_ms ) ) {
        // Nothing to change
        return;
    }

    // Apply on all or just selection
    if ( ui->syncApplyAllCheckBox->isChecked() ) {
        from_index = 0;
        to_index = sub_list.count() - 1;
    }
    else {
        from_index = ui->firstNumSpinBox->value();
        to_index = ui->lastNumSpinBox->value();
    }

    // Loop on subtitles list
    for ( qint32 i = from_index; i <= to_index; i++ ) {

        // Compute new timecodes
        current_subtitle = sub_list.at(i);
        current_sub_start_time_ms = MyAttributesConverter::timeStrHMStoMs( current_subtitle.startTime() );
        current_sub_end_time_ms = MyAttributesConverter::timeStrHMStoMs( current_subtitle.endTime() );

        temp_time_ms = (qreal)current_sub_start_time_ms + ( (qreal)(current_sub_start_time_ms - first_source_time_ms) * rescale_factor ) + (qreal)(first_dest_time_ms - first_source_time_ms);
        new_start_time_ms = MyAttributesConverter::roundToFrame(temp_time_ms, qApp->property("prop_FrameRate_fps").toInt());

        temp_time_ms = (qreal)current_sub_end_time_ms + ( (qreal)(current_sub_end_time_ms - first_source_time_ms) * rescale_factor ) + (qreal)(first_dest_time_ms - first_source_time_ms);
        new_end_time_ms = MyAttributesConverter::roundToFrame(temp_time_ms, qApp->property("prop_FrameRate_fps").toInt());

        // If it's the first subtitle, check if it's new timecode is positive and not erase the previous subtitle
        if ( i == from_index ) {

            if ( from_index > 0 ) {

                qint64 previous_end_time_ms = MyAttributesConverter::timeStrHMStoMs( sub_list[i -1].endTime() );

                if ( new_start_time_ms <= previous_end_time_ms ) {
                    // Error
                    QMessageBox::warning(this, "Rescale", "Rescale failed\nThe first subtitle new timecode conflict with the previous subtitle");
                    return;
                }
            }
            else if ( i == 0 ) {

                if ( new_start_time_ms < 0 ) {
                    // Error
                    QMessageBox::warning(this, "Rescale", "Rescale failed\nThe first subtitle new start time is negative");
                    return;
                }
            }
        } // If it's the last subtitle, check if it's new timecode not erase the next subtitle
        else if ( i == to_index ) {

            if ( ( i + 1 ) < sub_list.count() ) {

                qint64 next_start_time_ms = MyAttributesConverter::timeStrHMStoMs( sub_list[i + 1].startTime() );

                if ( new_end_time_ms >= next_start_time_ms ) {
                    // Error
                    QMessageBox::warning(this, "Rescale", "Rescale failed\nThe last subtitle timecode conflict with the next subtitle  ");
                    return;
                }
            }
        }

        // Set modifications in the list
        current_subtitle.setStartTime( time_base.addMSecs(new_start_time_ms).toString("hh:mm:ss.zzz") );
        current_subtitle.setEndTime( time_base.addMSecs(new_end_time_ms).toString("hh:mm:ss.zzz") );
        sub_list.replace(i, current_subtitle );
    }

    // Load the new subtitles list in the database
    ui->subTable->loadSubtitles(sub_list);

    // Save the database current state in history
    this->saveToHistory("Rescale");

    // Remove all and draw subtitles zones in the waveform
    ui->waveForm->removeAllSubtitlesZones();
    ui->waveForm->drawSubtitlesZone(sub_list, ui->subTable->currentIndex());
    ui->waveForm->changeZoneColor(ui->subTable->selectedIndex(), ui->subTable->currentIndex());
}

// Apply the current text line position to all selected subtitles corresponding line
void MainWindow::on_applyPosSelButton_clicked() {

    MySubtitles current_sub;
    qint32 index;
    qint16 nbr_of_lines;
    TextFont current_text_font;
    bool data_changed = false;

            // Retrieve the number of the line wich change of position
    qint16 line_focussed = ui->stEditDisplay->lastFocused();

    // Retrieve the indexes of the selected subtitles
    QList<qint32> selected_indexes = ui->subTable->selectedIndex();

    // Retrieve position parameters from tool boxes
    TextLine new_text_line = this->getPosToolBox();

    // For each subtitle selected
    QList<qint32>::iterator it;
    for ( it = selected_indexes.begin(); it != selected_indexes.end(); ++it ) {

        index = *it;

        current_sub = ui->subTable->getSubInfos(index);
        nbr_of_lines = current_sub.text().count();

        // If the line changed exist for this subtitle
        if ( nbr_of_lines > line_focussed ) {

            // Retrieve the current subtitle line "line_nbr" font
            current_text_font = current_sub.text()[line_focussed].Font();

            // Push the font properties in the new line
            new_text_line.setFont(current_text_font);

            // Replace the line in the subtitle container
            current_sub.text()[line_focussed] = new_text_line;

            // Push the new datas in the table
            ui->subTable->updateDatas(current_sub, index);

            data_changed = true;
        }
    }

    if ( data_changed ) {
        // Save the database current state in history
        this->saveToHistory("Change selection position");
    }
}

// Apply the current text line font to all selected subtitles corresponding line
void MainWindow::on_applyFontSelButton_clicked() {

    MySubtitles current_sub;
    qint32 index;
    qint16 nbr_of_lines;
    bool data_changed = false;


    // Retrieve the number of the line wich change of position
    qint16 line_focussed = ui->stEditDisplay->lastFocused();

    // Retrieve the indexes of the selected subtitles
    QList<qint32> selected_indexes = ui->subTable->selectedIndex();

    // Retrieve the subtitles list from the databases
    QList<MySubtitles> sub_list = ui->subTable->saveSubtitles();

    // For each subtitle selected
    QList<qint32>::iterator it;
    for ( it = selected_indexes.begin(); it != selected_indexes.end(); ++it ) {

        index = *it;

        current_sub = ui->subTable->getSubInfos(index);
        nbr_of_lines = current_sub.text().count();

        // If the line changed exist for this subtitle
        if ( nbr_of_lines > line_focussed ) {

            // Replace the font for "line_nbr" in current subtitle
            current_sub.text()[line_focussed].setFont( this->getFontToolBox() );

            // Push the new datas in the table
            ui->subTable->updateDatas(current_sub, index);

            data_changed = true;
        }
    }

    if ( data_changed ) {
        // Save the database current state in history
        this->saveToHistory("Change selection font");
    }
}

// Auto duration settings changed
void MainWindow::on_durationAutoCheckBox_clicked(bool checked) {

    // Set this settings to the current subtitle in the datbase
    ui->subTable->setDurationAuto(ui->subTable->currentIndex(), checked);
}

// Manage the selected subtitles context menu
void MainWindow::on_subTable_customContextMenuRequested(const QPoint &pos) {

    // Set context menu position
    QPoint global_pos = ui->subTable->mapToGlobal(pos);
    global_pos.ry() = global_pos.y() + ui->subTable->horizontalHeader()->height();

    // Build the menu
    QMenu custom_menu;
    custom_menu.addAction("Delete");
    custom_menu.addSeparator();
    custom_menu.addAction("Italic");
    custom_menu.actions().last()->setCheckable(true);
    custom_menu.addAction("Underline");
    custom_menu.actions().last()->setCheckable(true);
    custom_menu.addAction("Border");
    custom_menu.actions().last()->setCheckable(true);
    custom_menu.addAction("Shadow");
    custom_menu.actions().last()->setCheckable(true);
    custom_menu.addAction("Background");
    custom_menu.actions().last()->setCheckable(true);
    custom_menu.addAction("Text color...");
    custom_menu.addAction("Border color...");
    custom_menu.addAction("Shadow color...");
    custom_menu.addAction("Background color...");
    custom_menu.addAction("Font...");
    // ...

    MySubtitles current_sub;
    QList<TextLine> text_lines;
    bool italic_found = false;
    bool underlined_found = false;
    bool border_found = false;
    bool shadow_found = false;
    bool background_found = false;
    bool data_changed = false;

    // Retrieve the indexes of the selected subtitles
    QList<qint32> selected_indexes = ui->subTable->selectedIndex();

    // Check if there is at least one subtitle line with "Italic" or "Underlined" attribute
    QList<qint32>::iterator it;
    for ( it = selected_indexes.begin(); it != selected_indexes.end(); ++it ) {

        current_sub = ui->subTable->getSubInfos(*it);
        text_lines = current_sub.text();

        // If "Italic" or "Underlined" attribute is found, check the corresponding checkbox
        for ( qint16 i = 0; i < text_lines.count(); i++ ) {

            if ( text_lines[i].Font().fontItalic() == "yes" ) {
                custom_menu.actions().at(2)->setChecked(true);
                italic_found = true;
            }

            if ( text_lines[i].Font().fontUnderlined() == "yes" ) {
                custom_menu.actions().at(3)->setChecked(true);
                underlined_found = true;
            }

            if ( text_lines[i].Font().fontBorderEffect() == "yes" ) {
                custom_menu.actions().at(4)->setChecked(true);
                border_found = true;
            }

            if ( text_lines[i].Font().fontShadowEffect() == "yes" ) {
                custom_menu.actions().at(5)->setChecked(true);
                shadow_found = true;
            }

            if ( text_lines[i].Font().fontBackgroundEffect() == "yes" ) {
                custom_menu.actions().at(6)->setChecked(true);
                background_found = true;
            }

        }

        if ( ( italic_found ) && ( underlined_found) && ( border_found ) && ( shadow_found ) && ( background_found ) ) {
            break;
        }
    }

    // Execute the menu and retrieve the selected action
    QAction* selected_item = custom_menu.exec(global_pos);

    if ( selected_item ) {

        // "Delete" : remove selected subtitles
        if ( selected_item->text() == "Delete" ) {

            this->removeSubtitles();
            data_changed = false;
        }
        // "Italic"
        else if ( selected_item->text() == "Italic" ) {

            QString is_italic;

            // If at least one subtitle line has "Italic" attribute,
            // unset "Italic" to all selected subtitles
            // If not found, set "Italic" to all selected subtitles

            if ( italic_found ) {
                is_italic = "no";
            }
            else {
                is_italic = "yes";
            }

            for ( it = selected_indexes.begin(); it != selected_indexes.end(); ++it ) {

                current_sub = ui->subTable->getSubInfos(*it);

                for ( qint16 i = 0; i < current_sub.text().count(); i++ ) {

                    current_sub.text()[i].Font().setFontItalic(is_italic);
                }

                // Push the new datas in the table
                ui->subTable->updateDatas(current_sub, *it);
            }

            data_changed = true;
        }
        // "Underline"
        else if ( selected_item->text() == "Underline" ) {

            QString is_underlined;

            // If at least one subtitle line has "Underline" attribute,
            // unset "Italic" to all selected subtitles
            // If not found, set "Underline" to all selected subtitles

            if ( underlined_found ) {
                is_underlined = "no";
            }
            else {
                is_underlined = "yes";
            }

            for ( it = selected_indexes.begin(); it != selected_indexes.end(); ++it ) {

                current_sub = ui->subTable->getSubInfos(*it);

                for ( qint16 i = 0; i < current_sub.text().count(); i++ ) {
                    current_sub.text()[i].Font().setFontUnderlined(is_underlined);
                }

                // Push the new datas in the database
                ui->subTable->updateDatas(current_sub, *it);
            }

            data_changed = true;
        }
        // "Border"
        else if ( selected_item->text() == "Border" ) {

            QString is_border;

            // If at least one subtitle line has "Border" attribute,
            // unset "Border" to all selected subtitles
            // If not found, set "Border" to all selected subtitles

            if ( border_found ) {
                is_border = "no";
            }
            else {
                is_border = "yes";
            }

            for ( it = selected_indexes.begin(); it != selected_indexes.end(); ++it ) {

                current_sub = ui->subTable->getSubInfos(*it);

                for ( qint16 i = 0; i < current_sub.text().count(); i++ ) {

                    current_sub.text()[i].Font().setFontBorderEffect(is_border);
                }

                // Push the new datas in the table
                ui->subTable->updateDatas(current_sub, *it);
            }

            data_changed = true;
        }
        // "Shadow"
        else if ( selected_item->text() == "Shadow" ) {

            QString is_shadow;

            // If at least one subtitle line has "Shadow" attribute,
            // unset "Shadow" to all selected subtitles
            // If not found, set "Shadow" to all selected subtitles

            if ( shadow_found ) {
                is_shadow = "no";
            }
            else {
                is_shadow = "yes";
            }

            for ( it = selected_indexes.begin(); it != selected_indexes.end(); ++it ) {

                current_sub = ui->subTable->getSubInfos(*it);

                for ( qint16 i = 0; i < current_sub.text().count(); i++ ) {

                    current_sub.text()[i].Font().setFontShadowEffect(is_shadow);
                }

                // Push the new datas in the table
                ui->subTable->updateDatas(current_sub, *it);
            }

            data_changed = true;
        }
        // "Background"
        else if ( selected_item->text() == "Background" ) {

            QString is_background;

            // If at least one subtitle line has "Background" attribute,
            // unset "Background" to all selected subtitles
            // If not found, set "Background" to all selected subtitles

            if ( background_found ) {
                is_background = "no";
            }
            else {
                is_background = "yes";
            }

            for ( it = selected_indexes.begin(); it != selected_indexes.end(); ++it ) {

                current_sub = ui->subTable->getSubInfos(*it);

                for ( qint16 i = 0; i < current_sub.text().count(); i++ ) {

                    current_sub.text()[i].Font().setFontBackgroundEffect(is_background);
                }

                // Push the new datas in the table
                ui->subTable->updateDatas(current_sub, *it);
            }

            data_changed = true;
        }
        // "Text color"
        else if ( selected_item->text() == "Text color..." ) {

            // Open a color chooser dialog
            QColor font_color = QColorDialog::getColor(QColor("FFFFFFFF"), this, tr("Select text color"), QColorDialog::ShowAlphaChannel);

            // If valid color, set to all selected subtitles
            if ( font_color.isValid() ) {

                QString font_color_str = MyAttributesConverter::colorToString(font_color);

                for ( it = selected_indexes.begin(); it != selected_indexes.end(); ++it ) {

                    current_sub = ui->subTable->getSubInfos(*it);

                    for ( qint16 i = 0; i < current_sub.text().count(); i++ ) {
                        current_sub.text()[i].Font().setFontColor(font_color_str);
                    }

                    // Push the new datas in the database
                    ui->subTable->updateDatas(current_sub, *it);
                }

                data_changed = true;
            }
            else {
                data_changed = false;
            }
        }
        // "Border color"
        else if ( selected_item->text() == "Border color..." ) {

            // Open a color chooser dialog
            QColor border_color = QColorDialog::getColor(QColor("FF000000"), this, tr("Select border effect color"), QColorDialog::ShowAlphaChannel);

            // If valid color, set to all selected subtitles
            if ( border_color.isValid() ) {

                QString border_color_str = MyAttributesConverter::colorToString(border_color);

                for ( it = selected_indexes.begin(); it != selected_indexes.end(); ++it ) {

                    current_sub = ui->subTable->getSubInfos(*it);

                    for ( qint16 i = 0; i < current_sub.text().count(); i++ ) {
                        current_sub.text()[i].Font().setFontBorderEffectColor(border_color_str);
                    }

                    // Push the new datas in the database
                    ui->subTable->updateDatas(current_sub, *it);
                }

                data_changed = true;
            }
            else {
                data_changed = false;
            }
        }
        // "Shadow color"
        else if ( selected_item->text() == "Shadow color..." ) {

            // Open a color chooser dialog
            QColor shadow_color = QColorDialog::getColor(QColor("FF000000"), this, tr("Select shadow effect color"), QColorDialog::ShowAlphaChannel);

            // If valid color, set to all selected subtitles
            if ( shadow_color.isValid() ) {

                QString shadow_color_str = MyAttributesConverter::colorToString(shadow_color);

                for ( it = selected_indexes.begin(); it != selected_indexes.end(); ++it ) {

                    current_sub = ui->subTable->getSubInfos(*it);

                    for ( qint16 i = 0; i < current_sub.text().count(); i++ ) {
                        current_sub.text()[i].Font().setFontShadowEffectColor(shadow_color_str);
                    }

                    // Push the new datas in the database
                    ui->subTable->updateDatas(current_sub, *it);
                }

                data_changed = true;
            }
            else {
                data_changed = false;
            }
        }
        // "Background color"
        else if ( selected_item->text() == "Background color..." ) {

            // Open a color chooser dialog
            QColor background_color = QColorDialog::getColor(QColor("FF000000"), this, tr("Select background effect color"), QColorDialog::ShowAlphaChannel);

            // If valid color, set to all selected subtitles
            if ( background_color.isValid() ) {

                QString background_color_str = MyAttributesConverter::colorToString(background_color);

                for ( it = selected_indexes.begin(); it != selected_indexes.end(); ++it ) {

                    current_sub = ui->subTable->getSubInfos(*it);

                    for ( qint16 i = 0; i < current_sub.text().count(); i++ ) {
                        current_sub.text()[i].Font().setFontBackgroundEffectColor(background_color_str);
                    }

                    // Push the new datas in the database
                    ui->subTable->updateDatas(current_sub, *it);
                }

                data_changed = true;
            }
            else {
                data_changed = false;
            }
        }
        // "Font"
        else if ( selected_item->text() == "Font..." ) {

            bool ok;

            // Open a font chooser dialog
            QFont font = QFontDialog::getFont(&ok, QFont("Arial", 42), this);

            // If valid font, set font name and size to all selected subtitles
            if ( ok ) {

                for ( it = selected_indexes.begin(); it != selected_indexes.end(); ++it ) {

                    current_sub = ui->subTable->getSubInfos(*it);

                    for ( qint16 i = 0; i < current_sub.text().count(); i++ ) {
                        current_sub.text()[i].Font().setFontId(font.family());
                        current_sub.text()[i].Font().setFontSize(QString::number(font.pointSize()));
                    }

                    // Push the new datas in the database
                    ui->subTable->updateDatas(current_sub, *it);
                }
                data_changed = true;
            }
            else {
                data_changed = false;
            }
        }

        // There are change to load in the database
        if ( data_changed == true ) {

            ui->stEditDisplay->setText(ui->subTable->getSubInfos(ui->subTable->currentIndex()));

            // Save the database current state in history
            this->saveToHistory("Change selection " +selected_item->text());
        }
    }
    else {
        // nothing was chosen
    }
}
