#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "myfilereader.h"
#include "myfilewriter.h"
#include "mysubtitlefileparser.h"
#include "SubtitlesParsers/SubRip/subripparser.h"
#include "SubtitlesParsers/DcSubtitle/dcsubparser.h"
#include <QFileDialog>
#include <QColorDialog>
#include <QString>
#include <QKeyEvent>
#include <QMessageBox>

#define TEXT_HALIGN_DEFAULT_VALUE "center"
#define TEXT_HPOSITION_DEFAULT_VALUE 0.0
#define TEXT_VALIGN_DEFAULT_VALUE "bottom"
#define TEXT_VPOSITION_DEFAULT_VALUE 8.0

#define SEC_TO_MSEC 1000
#define FRAME_PER_SEC 25
#define SUB_MIN_INTERVAL_FRAME 5


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
    this->installEventFilter(this);

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

    // Init the SIGNAL / SLOT connections
    connect(ui->videoPlayer, SIGNAL(durationChanged(qint64)),this, SLOT(updateStEditSize()));
    connect(ui->videoPlayer, SIGNAL(positionChanged(qint64)), this, SLOT(videoPositionChanged(qint64)));

    connect(ui->waveForm, SIGNAL(markerPositionChanged(qint64)), this, SLOT(waveformMarerkPosChanged(qint64)));

    connect(ui->stEditDisplay, SIGNAL(cursorPositionChanged()), this, SLOT(updateSubTable()));
    connect(ui->stEditDisplay, SIGNAL(subDatasChanged(MySubtitles)), ui->subTable, SLOT(updateDatas(MySubtitles)));
    connect(ui->stEditDisplay, SIGNAL(textLineFocusChanged(TextFont, TextLine)), this, SLOT(updateToolBox(TextFont, TextLine)));

    connect(ui->subTable, SIGNAL(itemSelectionChanged(qint64)), this, SLOT(currentItemChanged(qint64)));
    connect(ui->subTable, SIGNAL(newTextToDisplay(MySubtitles)), this, SLOT(updateTextEdit(MySubtitles)));
}

MainWindow::~MainWindow() {
    delete ui;
}


bool MainWindow::eventFilter(QObject *watched, QEvent *event) {

    // Catch shortcut key events
    if ( event->type() == QEvent::KeyPress ) {

        QKeyEvent* key_event = static_cast<QKeyEvent*>(event);

        if ( key_event->key() == Qt::Key_F1 ) {

            // F1 set current subtitle start time
            if ( ui->subTable->setStartTime( ui->videoPlayer->playerPosition()) == false ) {
                QString error_msg = ui->subTable->errorMsg();
                QMessageBox::warning(this, "Set start time", error_msg);
            }
        }
        else if ( key_event->key() == Qt::Key_F2 ) {

            // F2 set current subtitle end time
            if ( ui->subTable->setEndTime( ui->videoPlayer->playerPosition()) == false ) {
                QString error_msg = ui->subTable->errorMsg();
                QMessageBox::warning(this, "Set end time", error_msg);
            }
        }
        else if ( key_event->key() == Qt::Key_F3 ) {

            qint64 end_time_ms = ui->videoPlayer->playerPosition();

            // F3 set current subtitle end time + add new subtitle entry
            if ( ui->subTable->setEndTime(end_time_ms) == false ) {
                QString error_msg = ui->subTable->errorMsg();
                QMessageBox::warning(this, "Set end time", error_msg);
            }
            else {

                qint64 start_time_ms = end_time_ms + ( ( (qreal)SEC_TO_MSEC / (qreal)FRAME_PER_SEC ) * (qreal)SUB_MIN_INTERVAL_FRAME );

                if ( ui->subTable->insertNewSub( ui->stEditDisplay->getDefaultSub(), start_time_ms  ) == false ) {
                    QString error_msg = ui->subTable->errorMsg();
                    QMessageBox::warning(this, "Insert subtitle", error_msg);
                }
            }
        }
        else if ( key_event->key() == Qt::Key_F4 ) {

            // F4 add new subtitle entry after current subtitle
            if ( ui->subTable->insertNewSubAfterCurrent( ui->stEditDisplay->getDefaultSub() ) == false ) {
                QString error_msg = ui->subTable->errorMsg();
                QMessageBox::warning(this, "Insert subtitle", error_msg);
            }
        }
        else if ( key_event->key() == Qt::Key_Backspace ) {

            // Ctrl + BackSpace remove current subtitle
            Qt::KeyboardModifiers event_keybord_modifier = key_event->modifiers();

            if ( event_keybord_modifier == Qt::ControlModifier ) {

                ui->subTable->deleteCurrentSub();
            }
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
void MainWindow::updateSubTable() {

    // There are no subtitle for the current time. Try to add new subtitle entry
    if ( ui->subTable->isNewEntry() ) {
        if ( ui->subTable->insertNewSub( ui->stEditDisplay->subtitleData()) == false ) {

            QString error_msg = ui->subTable->errorMsg();
            QMessageBox::warning(this, "Insert subtitle", error_msg);
            MySubtitles empty_subtitle;
            updateTextEdit(empty_subtitle);
        }
    } // There are a subtitle for the current time. Update the text in the database
    else {
        ui->subTable->updateText( ui->stEditDisplay->text());
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

// Inteface to resize the edit zone in function of the current video size
void MainWindow::updateStEditSize() {

    QSizeF video_item_size;
    QSizeF video_item_native_size;
    qint32 video_height;

    // Resize the width of subtitles edit zone in function of video width
    video_item_size = ui->videoPlayer->videoItemSize();

    video_item_native_size = ui->videoPlayer->videoItemNativeSize();

    if ( video_item_native_size.isValid() ) {
        qreal video_ratio = video_item_native_size.width() / video_item_native_size.height();

        video_height = (qint32) qRound( video_item_size.width() / video_ratio );
    }
    else {
        video_height = video_item_size.height();
    }

    // Set the edit region size and position mapped on video.
    QSize video_current_size(video_item_size.width(), video_height + 1);
    ui->stEditDisplay->setFixedSize(video_current_size);
    qint32 video_top_pos = (qint32)( qRound( video_item_size.height() - (qreal)video_height ) / 2.0 ) + 9;
    ui->stEditDisplay->move(9, video_top_pos);

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



// ******************************** Tool Box ***************************************************************//

void MainWindow::updateToolBox(TextFont textFont, TextLine textLine) {

    this->updatePosToolBox(textLine);

    this->updateFontToolBox(textFont);
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

    TextLine text_line;

    text_line.setTextHAlign( ui->hAlignBox->currentText() );

    QString hPos_str = QString::number(ui->hPosSpinBox->value(), 'f', 1);
    text_line.setTextHPosition( hPos_str );

    text_line.setTextVAlign( ui->vAlignBox->currentText() );

    QString vPos_str = QString::number(ui->vPosSpinBox->value(), 'f', 1);
    text_line.setTextVPosition( vPos_str );

    ui->stEditDisplay->updateTextPosition(text_line);

    mTextPosChangedByUser = false;
}

// Vertical alignment toolbox value changed
void MainWindow::on_vAlignBox_activated(const QString &value) {

    qApp->setProperty("prop_Valign", value);

    // Tool box changed by user
    if ( mTextPosChangedBySoft == false ) {
        // Update the text edit position
        updateTextPosition();
    }
}

// Vertical position toolbox value changed
void MainWindow::on_vPosSpinBox_valueChanged(const QString &value) {

    qApp->setProperty("prop_Vposition_percent", value);

    if ( mTextPosChangedBySoft == false ) {
        updateTextPosition();
    }
}

// Horizontal alignment toolbox value changed
void MainWindow::on_hAlignBox_activated(const QString &value) {

    qApp->setProperty("prop_Halign", value);

    if ( mTextPosChangedBySoft == false ) {
        updateTextPosition();
    }
}

// Horizontal position toolbox value changed
void MainWindow::on_hPosSpinBox_valueChanged(const QString &value) {

    qApp->setProperty("prop_Hposition_percent", value);

    if ( mTextPosChangedBySoft == false ) {
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

// Change the "font" of the current "edit line" from the font toolboxes parameters
void MainWindow::updateTextFont(bool customColorClicked) {

    mTextFontChangedByUser = true;

    TextFont text_font;

    if ( ui->fontLabel->isEnabled() == true )  {

        QFont font = ui->fontIdComboBox->currentFont();
        text_font.setFontId( font.family() );

        text_font.setFontSize(ui->fontSizeSpinBox->cleanText());

        ui->fontColorOtherText->setEnabled(false);

        if ( ui->fontColorRButton->isChecked() ) {
            text_font.setFontColor("FFFF0000");
            qApp->setProperty("prop_FontColor_rgba", "FFFF0000");
        }
        else if ( ui->fontColorGButton->isChecked() ) {
            text_font.setFontColor("FF00FF00");
            qApp->setProperty("prop_FontColor_rgba", "FF00FF00");
        }
        else if ( ui->fontColorBButton->isChecked() ) {
            text_font.setFontColor("FF0000FF");
            qApp->setProperty("prop_FontColor_rgba", "FF0000FF");
        }
        else if ( ui->fontColorYButton->isChecked() ) {
            text_font.setFontColor("FFFFFF00");
            qApp->setProperty("prop_FontColor_rgba", "FFFFFF00");
        }
        else if ( ui->fontColorBlButton->isChecked() ) {
            text_font.setFontColor("FF000000");
            qApp->setProperty("prop_FontColor_rgba", "FF000000");
        }
        else if ( ui->fontColorWButton->isChecked() ) {
            text_font.setFontColor("FFFFFFFF");
            qApp->setProperty("prop_FontColor_rgba", "FFFFFFFF");
        }
        else if ( ui->fontColorOtherButton->isChecked() ) {

            QString font_color_str;

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
            text_font.setFontColor(font_color_str);
            qApp->setProperty("prop_FontColor_rgba", font_color_str);
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

    ui->stEditDisplay->updateTextFont(text_font);

    mTextFontChangedByUser = false;
}

// Font size toolbox changed
void MainWindow::on_fontSizeSpinBox_valueChanged(const QString &value) {

    qApp->setProperty("prop_FontSize_pt", value);

    // Tool box changed by user
    if ( mTextFontChangedBySoft == false ) {
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

    if ( checked ) {
        qApp->setProperty("prop_FontItalic", "yes");
    }
    else {
        qApp->setProperty("prop_FontItalic", "no");
    }

    if ( mTextFontChangedBySoft == false ) {
        updateTextFont(false);
    }
}

// Underlined value changed
void MainWindow::on_fontUnderlinedButton_toggled(bool checked) {

    if ( checked ) {
        qApp->setProperty("prop_FontUnderlined", "yes");
    }
    else {
        qApp->setProperty("prop_FontUnderlined", "no");
    }

    if ( mTextFontChangedBySoft == false ) {
        updateTextFont(false);
    }
}

// Font name changed
void MainWindow::on_fontIdComboBox_currentFontChanged(const QFont &f) {

    qApp->setProperty("prop_FontName", f.family());

    if ( mTextFontChangedBySoft == false ) {
        updateTextFont(false);
    }
}
