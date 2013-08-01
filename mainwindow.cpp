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
    ui->setupUi(this);

    for ( qint32 i = 5; i < ui->subTable->columnCount(); i++ ) {
        ui->subTable->setColumnHidden(i, true);
    }

    this->installEventFilter(this);

    mTextPosChangedBySoft = false;
    mTextPosChangedByUser = false;
    mTextFontChangedBySoft = false;
    mTextFontChangedByUser = false;

    //ui->stEditDisplay->setStyleSheet("background: transparent; color: yellow");

    connect(ui->videoPlayer, SIGNAL(positionChanged(qint64)), ui->waveForm, SLOT(updatePostionMarker(qint64)));
    connect(ui->waveForm, SIGNAL(positionChanged(qint64)), ui->videoPlayer, SLOT(setPosition(qint64)));

    connect(ui->videoPlayer, SIGNAL(durationChanged(qint64)),ui->subTable, SLOT(videoDurationChanged(qint64)));
    connect(ui->videoPlayer, SIGNAL(positionChanged(qint64)), ui->subTable, SLOT(updateStDisplay(qint64)));
    connect(ui->stEditDisplay2, SIGNAL(cursorPositionChanged()), this, SLOT(updateSubTable()));
    connect(ui->stEditDisplay2, SIGNAL(subDatasChanged(MySubtitles)), ui->subTable, SLOT(updateDatas(MySubtitles)));
    connect(ui->stEditDisplay2, SIGNAL(textLineFocusChanged(TextFont, TextLine)), this, SLOT(updateToolBox(TextFont, TextLine)));
    connect(ui->subTable, SIGNAL(itemSelectionChanged(qint32)), this, SLOT(updateVideoPosition(qint32)));
    connect(ui->subTable, SIGNAL(newTextToDisplay(MySubtitles)), this, SLOT(updateTextEdit(MySubtitles)));

    connect(ui->videoPlayer, SIGNAL(durationChanged(qint64)),this, SLOT(updateStEditSize()));
}

MainWindow::~MainWindow() {
    delete ui;
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event) {

    if ( event->type() == QEvent::KeyPress ) {

        QKeyEvent* key_event = static_cast<QKeyEvent*>(event);

        if ( key_event->key() == Qt::Key_F1 ) {

            if ( ui->subTable->setStartTime( ui->videoPlayer->playerPosition()) == false ) {
                QString error_msg = ui->subTable->errorMsg();
                QMessageBox::warning(this, "Set start time", error_msg);
            }
        }
        else if ( key_event->key() == Qt::Key_F2 ) {

            if ( ui->subTable->setEndTime( ui->videoPlayer->playerPosition()) == false ) {
                QString error_msg = ui->subTable->errorMsg();
                QMessageBox::warning(this, "Set end time", error_msg);
            }
        }
        else if ( key_event->key() == Qt::Key_F3 ) {

            qint64 end_time_ms = ui->videoPlayer->playerPosition();

            if ( ui->subTable->setEndTime(end_time_ms) == false ) {
                QString error_msg = ui->subTable->errorMsg();
                QMessageBox::warning(this, "Set end time", error_msg);
            }
            else {

                qint64 start_time_ms = end_time_ms + ( ( (qreal)SEC_TO_MSEC / (qreal)FRAME_PER_SEC ) * (qreal)SUB_MIN_INTERVAL_FRAME );

                if ( ui->subTable->insertNewSub( ui->stEditDisplay2->getDefaultSub(), start_time_ms  ) == false ) {
                    QString error_msg = ui->subTable->errorMsg();
                    QMessageBox::warning(this, "Insert subtitle", error_msg);
                }
            }
        }
        else if ( key_event->key() == Qt::Key_F4 ) {

            if ( ui->subTable->insertNewSubAfterCurrent( ui->stEditDisplay2->getDefaultSub() ) == false ) {
                QString error_msg = ui->subTable->errorMsg();
                QMessageBox::warning(this, "Insert subtitle", error_msg);
            }
        }
        else if ( key_event->key() == Qt::Key_Backspace ) {

            Qt::KeyboardModifiers event_keybord_modifier = key_event->modifiers();

            if ( event_keybord_modifier == Qt::ControlModifier ) {

                ui->subTable->deleteCurrentSub();
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::updateSubTable() {

    if ( ui->subTable->isNewEntry() ) {
        if ( ui->subTable->insertNewSub( ui->stEditDisplay2->subtitleData()) == false ) {
            QString error_msg = ui->subTable->errorMsg();
            QMessageBox::warning(this, "Insert subtitle", error_msg);
            MySubtitles empty_subtitle;
            updateTextEdit(empty_subtitle);
        }
    }
    else {
        ui->subTable->updateText( ui->stEditDisplay2->text());
    }
}

void MainWindow::updateVideoPosition(qint32 positionMs) {

    ui->videoPlayer->setPosition(positionMs);
    //ui->stEditDisplay2->setFocus();
}

void MainWindow::updateTextEdit(MySubtitles subtitle) {

    ui->stEditDisplay2->setText(subtitle);
}

void MainWindow::on_actionOpen_triggered()
{
    QString video_file_name = ui->videoPlayer->openFile();

    if ( !video_file_name.isEmpty() ) {

        QString wf_file_name = video_file_name;
        wf_file_name.truncate(wf_file_name.lastIndexOf("."));
        wf_file_name.append(".wf");

        ui->waveForm->openFile(wf_file_name, video_file_name);

        ui->subTable->initStTable(0);

    }
}

void MainWindow::updateStEditSize() {

    QSizeF video_item_size;
    QSizeF video_item_native_size;

    // Resize the width of subtitles edit zone in function of video width
    video_item_size = ui->videoPlayer->videoItemSize();

    video_item_native_size = ui->videoPlayer->videoItemNativeSize();
    qreal video_ratio = video_item_native_size.width() / video_item_native_size.height();

    qint32 video_height = (qint32) qRound( video_item_size.width() / video_ratio );

    // Set the edit region size and position mapped on video.
    QSize video_current_size(video_item_size.width(), video_height + 1);
    ui->stEditDisplay2->setFixedSize(video_current_size);
    qint32 video_top_pos = (qint32)( qRound( video_item_size.height() - (qreal)video_height ) / 2.0 ) + 9;
    ui->stEditDisplay2->move(9, video_top_pos);

//    ui->textEdit->setStyleSheet("background: transparent; color: yellow");
//    ui->textEdit->setFixedSize(ui->stEditDisplay2->size());
//    ui->textEdit->move(0, 0);

}

void MainWindow::resizeEvent(QResizeEvent* event) {

    updateStEditSize();
    QMainWindow::resizeEvent(event);
}


void MainWindow::on_actionQuit_triggered()
{
    close();
}

void MainWindow::on_actionImport_Subtitles_triggered()
{
//    QString file_name = QFileDialog::getOpenFileName(this, tr("Open Subtitles"),QDir::homePath());
//    MyFileReader file_reader(file_name,"UTF-8");
//    SubRipParser* parser = new SubRipParser();
//    QList<MySubtitles> subtitles_list = parser->open(file_reader);

    QString selected_filter;

    QString file_name = QFileDialog::getOpenFileName(this, tr("Open Subtitles"),QDir::homePath() + "/Videos",
                                                     tr("SubRip (*.srt);;DCSubtitle (*.xml);;All Files(*)"),
                                                     &selected_filter);


    MyFileReader file_reader(file_name, "UTF-8");

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

    //DcSubParser* parser = new DcSubParser();
    QList<MySubtitles> subtitles_list = parser->open(file_reader);

    if ( !subtitles_list.isEmpty() ) {

        ui->subTable->loadSubtitles(subtitles_list);
    }
}

void MainWindow::on_actionExport_Subtitles_triggered() {

    QString selected_filter;

    QString file_name = QFileDialog::getSaveFileName(this, tr("Open Subtitles"),QDir::homePath() + "/Videos",
                                                     tr("SubRip (*.srt);;DCSubtitle (*.xml);;All Files(*)"),
                                                     &selected_filter);


    MyFileWriter file_writer(file_name, "UTF-8");
    //SubRipParser* parser = new SubRipParser();

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

    //DcSubParser* parser = new DcSubParser();

    QList<MySubtitles> subtitles_list = ui->subTable->saveSubtitles();

   if ( !subtitles_list.isEmpty() ) {

       parser->save(file_writer, subtitles_list);
       file_writer.toFile();
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

void MainWindow::updatePosToolBox(TextLine textLine) {

    if ( mTextPosChangedByUser != true ) {

        mTextPosChangedBySoft = true;

        if ( ui->lineLabel->isEnabled() == false )  {
            ui->lineLabel->setEnabled(true);
            ui->hAlignBox->setEnabled(true);
            ui->hAlignLabel->setEnabled(true);
            ui->hPosSpinBox->setEnabled(true);
            ui->hPosLabel->setEnabled(true);
            ui->vAlignBox->setEnabled(true);
            ui->vAlignLabel->setEnabled(true);
            ui->vPosSpinBox->setEnabled(true);
            ui->vPosLabel->setEnabled(true);
        }

        ui->hAlignBox->setCurrentText( textLine.textHAlign() );
        ui->hPosSpinBox->setValue( textLine.textHPosition().toDouble() );
        ui->vAlignBox->setCurrentText( textLine.textVAlign() );
        ui->vPosSpinBox->setValue( textLine.textVPosition().toDouble() );

    }
    mTextPosChangedBySoft = false;
}

void MainWindow::updateTextPosition() {

    mTextPosChangedByUser = true;

    TextLine text_line;

    text_line.setTextHAlign( ui->hAlignBox->currentText() );

    QString hPos_str = QString::number(ui->hPosSpinBox->value(), 'f', 1);
    text_line.setTextHPosition( hPos_str );

    text_line.setTextVAlign( ui->vAlignBox->currentText() );

    QString vPos_str = QString::number(ui->vPosSpinBox->value(), 'f', 1);
    text_line.setTextVPosition( vPos_str );

    ui->stEditDisplay2->updateTextPosition(text_line);

    mTextPosChangedByUser = false;
}

void MainWindow::on_vAlignBox_activated(const QString &arg1) {

    if ( mTextPosChangedBySoft == false ) {
        updateTextPosition();
    }
}

void MainWindow::on_vPosSpinBox_valueChanged(const QString &arg1) {

    if ( mTextPosChangedBySoft == false ) {
        updateTextPosition();
    }
}

void MainWindow::on_hAlignBox_activated(const QString &arg1) {

    if ( mTextPosChangedBySoft == false ) {
        updateTextPosition();
    }
}

void MainWindow::on_hPosSpinBox_valueChanged(const QString &arg1) {

    if ( mTextPosChangedBySoft == false ) {
        updateTextPosition();
    }
}


void MainWindow::updateFontToolBox(TextFont textFont) {

    if ( mTextFontChangedByUser != true ) {

        mTextFontChangedBySoft = true;

        if ( ui->fontLabel->isEnabled() == false )  {

            ui->fontLabel->setEnabled(true);
            ui->fontIdComboBox->setEnabled(true);
            ui->fontSizeSpinBox->setEnabled(true);
            ui->fontColorLabel->setEnabled(true);
            ui->fontColorRButton->setEnabled(true);
            ui->fontColorGButton->setEnabled(true);
            ui->fontColorBButton->setEnabled(true);
            ui->fontColorYButton->setEnabled(true);
            ui->fontColorBlButton->setEnabled(true);
            ui->fontColorWButton->setEnabled(true);
            ui->fontColorOtherButton->setEnabled(true);
            ui->fontStyleLabel->setEnabled(true);
            ui->fontItalicButton->setEnabled(true);
            ui->fontUnderlinedButton->setEnabled(true);
        }

        QFont font(textFont.fontId());
        ui->fontIdComboBox->setCurrentFont(font);
        ui->fontSizeSpinBox->setValue(textFont.fontSize().toInt());

        QString font_color = textFont.fontColor();

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
            ui->fontColorOtherText->setEnabled("true");
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

void MainWindow::updateTextFont(bool customColorClicked) {

    mTextFontChangedByUser = true;

    TextFont text_font;

    if ( ui->fontLabel->isEnabled() == true )  {

        QFont font = ui->fontIdComboBox->currentFont();
        QString font_familly = font.family();
        text_font.setFontId( font.family() );

        text_font.setFontSize(ui->fontSizeSpinBox->cleanText());

        ui->fontColorOtherText->setEnabled(false);

        if ( ui->fontColorRButton->isChecked() ) {
            text_font.setFontColor("FFFF0000");
        }
        else if ( ui->fontColorGButton->isChecked() ) {
            text_font.setFontColor("FF00FF00");
        }
        else if ( ui->fontColorBButton->isChecked() ) {
            text_font.setFontColor("FF0000FF");
        }
        else if ( ui->fontColorYButton->isChecked() ) {
            text_font.setFontColor("FFFFFF00");
        }
        else if ( ui->fontColorBlButton->isChecked() ) {
            text_font.setFontColor("FF000000");
        }
        else if ( ui->fontColorWButton->isChecked() ) {
            text_font.setFontColor("FFFFFFFF");
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

    ui->stEditDisplay2->updateTextFont(text_font);

    mTextFontChangedByUser = false;
}


void MainWindow::on_fontSizeSpinBox_valueChanged(const QString &arg1) {

    if ( mTextFontChangedBySoft == false ) {
        updateTextFont(false);
    }
}

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

void MainWindow::on_fontItalicButton_toggled(bool checked) {

    if ( mTextFontChangedBySoft == false ) {
        updateTextFont(false);
    }
}

void MainWindow::on_fontUnderlinedButton_toggled(bool checked) {

    if ( mTextFontChangedBySoft == false ) {
        updateTextFont(false);
    }
}

void MainWindow::on_fontIdComboBox_currentFontChanged(const QFont &f) {

    if ( mTextFontChangedBySoft == false ) {
        updateTextFont(false);
    }
}

void MainWindow::on_fontColorOtherButton_clicked() {

    if ( mTextFontChangedBySoft == false ) {
        updateTextFont(true);
    }
}
