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


#define TEXT_HALIGN_DEFAULT_VALUE "center"
#define TEXT_HPOSITION_DEFAULT_VALUE 0.0
#define TEXT_VALIGN_DEFAULT_VALUE "bottom"
#define TEXT_VPOSITION_DEFAULT_VALUE 8.0

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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
    connect(ui->stEditDisplay2, SIGNAL(subDatasChanged(MySubtitles)), this, SLOT(updatePosToolBox(MySubtitles)));
    connect(ui->stEditDisplay2, SIGNAL(textLineFocusChanged(TextFont)), this, SLOT(updateFontToolBox(TextFont)));
    connect(ui->subTable, SIGNAL(itemSelectionChanged(qint32)), this, SLOT(updateVideoPosition(qint32)));
    connect(ui->subTable, SIGNAL(newTextToDisplay(MySubtitles)), this, SLOT(updateTextEdit(MySubtitles)));

    connect(ui->videoPlayer, SIGNAL(durationChanged(qint64)),this, SLOT(updateStEditSize()));
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::updateSubTable() {

    if ( ui->subTable->isNewEntry() ) {
        ui->subTable->insertNewSub( ui->stEditDisplay2->subtitleData() );
    }
    else {
        ui->subTable->updateText( ui->stEditDisplay2->text() );
    }
}

void MainWindow::updateVideoPosition(qint32 positionMs) {

    ui->videoPlayer->setPosition(positionMs);
    //ui->stEditDisplay2->setFocus();
}

void MainWindow::updateTextEdit(MySubtitles subtitle) {

    ui->stEditDisplay2->setText(subtitle);
    this->updatePosToolBox(subtitle);
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
// ******************************** Tool Box ***************************************************************//

void MainWindow::updatePosToolBox(MySubtitles subtitle) {

    if ( mTextPosChangedByUser != true ) {

        mTextPosChangedBySoft = true;

        QList<TextLine> TextLines = subtitle.text();
        TextLine text;

        if ( TextLines.count() == 2 ) {

            if ( ui->lineDownLabel->isEnabled() == false )  {
                ui->lineDownLabel->setEnabled(true);
                ui->hAlignDownBox->setEnabled(true);
                ui->hAlignDownLabel->setEnabled(true);
                ui->hPosDownSpinBox->setEnabled(true);
                ui->hPosDownLabel->setEnabled(true);
                ui->vAlignDownBox->setEnabled(true);
                ui->vAlignDownLabel->setEnabled(true);
                ui->vPosDownSpinBox->setEnabled(true);
                ui->vPosDownLabel->setEnabled(true);
            }

            if ( ui->lineUpLabel->isEnabled() == false )  {
                ui->lineUpLabel->setEnabled(true);
                ui->hAlignUpBox->setEnabled(true);
                ui->hAlignUpLabel->setEnabled(true);
                ui->hPosUpSpinBox->setEnabled(true);
                ui->hPosUpLabel->setEnabled(true);
                ui->vAlignUpBox->setEnabled(true);
                ui->vAlignUpLabel->setEnabled(true);
                ui->vPosUpSpinBox->setEnabled(true);
                ui->vPosUpLabel->setEnabled(true);
            }

            text = TextLines.first();

            ui->hAlignUpBox->setCurrentText( text.textHAlign() );
            ui->hPosUpSpinBox->setValue( text.textHPosition().toDouble() );
            ui->vAlignUpBox->setCurrentText( text.textVAlign() );
            ui->vPosUpSpinBox->setValue( text.textVPosition().toDouble() );

            text = TextLines.last();

            ui->hAlignDownBox->setCurrentText( text.textHAlign() );
            ui->hPosDownSpinBox->setValue( text.textHPosition().toDouble() );
            ui->vAlignDownBox->setCurrentText(text.textVAlign() );
            ui->vPosDownSpinBox->setValue( text.textVPosition().toDouble() );
        }
        else if ( TextLines.count() == 1 ) {

            if ( ui->lineDownLabel->isEnabled() == false )  {
                ui->lineDownLabel->setEnabled(true);
                ui->hAlignDownBox->setEnabled(true);
                ui->hAlignDownLabel->setEnabled(true);
                ui->hPosDownSpinBox->setEnabled(true);
                ui->hPosDownLabel->setEnabled(true);
                ui->vAlignDownBox->setEnabled(true);
                ui->vAlignDownLabel->setEnabled(true);
                ui->vPosDownSpinBox->setEnabled(true);
                ui->vPosDownLabel->setEnabled(true);
            }

            if ( ui->lineUpLabel->isEnabled() == true )  {
                ui->lineUpLabel->setEnabled(false);
                ui->hAlignUpBox->setEnabled(false);
                ui->hAlignUpLabel->setEnabled(false);
                ui->hPosUpSpinBox->setEnabled(false);
                ui->hPosUpLabel->setEnabled(false);
                ui->vAlignUpBox->setEnabled(false);
                ui->vAlignUpLabel->setEnabled(false);
                ui->vPosUpSpinBox->setEnabled(false);
                ui->vPosUpLabel->setEnabled(false);
            }

            text = TextLines.first();

            ui->hAlignDownBox->setCurrentText( text.textHAlign() );
            ui->hPosDownSpinBox->setValue( text.textHPosition().toDouble() );
            ui->vAlignDownBox->setCurrentText( text.textVAlign() );
            ui->vPosDownSpinBox->setValue( text.textVPosition().toDouble() );
        }
        else if ( TextLines.count() == 0 ) {

            if ( ui->lineDownLabel->isEnabled() == false )  {
                ui->lineDownLabel->setEnabled(true);
                ui->hAlignDownBox->setEnabled(true);
                ui->hAlignDownLabel->setEnabled(true);
                ui->hPosDownSpinBox->setEnabled(true);
                ui->hPosDownLabel->setEnabled(true);
                ui->vAlignDownBox->setEnabled(true);
                ui->vAlignDownLabel->setEnabled(true);
                ui->vPosDownSpinBox->setEnabled(true);
                ui->vPosDownLabel->setEnabled(true);
            }

            if ( ui->lineUpLabel->isEnabled() == true )  {
                ui->lineUpLabel->setEnabled(false);
                ui->hAlignUpBox->setEnabled(false);
                ui->hAlignUpLabel->setEnabled(false);
                ui->hPosUpSpinBox->setEnabled(false);
                ui->hPosUpLabel->setEnabled(false);
                ui->vAlignUpBox->setEnabled(false);
                ui->vAlignUpLabel->setEnabled(false);
                ui->vPosUpSpinBox->setEnabled(false);
                ui->vPosUpLabel->setEnabled(false);
            }

            ui->hAlignDownBox->setCurrentText( TEXT_HALIGN_DEFAULT_VALUE );
            ui->hPosDownSpinBox->setValue( TEXT_HPOSITION_DEFAULT_VALUE );
            ui->vAlignDownBox->setCurrentText( TEXT_VALIGN_DEFAULT_VALUE );
            ui->vPosDownSpinBox->setValue( TEXT_VPOSITION_DEFAULT_VALUE );
        }
        mTextPosChangedBySoft = false;
    }
}

void MainWindow::updateTextPosition() {

    mTextPosChangedByUser = true;

    QList<TextLine> text_lines;
    TextLine text_line;

    if ( ui->lineDownLabel->isEnabled() == true )  {

        if ( ui->lineUpLabel->isEnabled() == true )  {

            text_line.setTextHAlign( ui->hAlignUpBox->currentText() );

            QString hPos_str = QString::number(ui->hPosUpSpinBox->value(), 'f', 1);
            text_line.setTextHPosition( hPos_str );

            text_line.setTextVAlign( ui->vAlignUpBox->currentText() );

            QString vPos_str = QString::number(ui->vPosUpSpinBox->value(), 'f', 1);
            text_line.setTextVPosition( vPos_str );

            text_lines.append(text_line);
        }

        text_line.setTextHAlign( ui->hAlignDownBox->currentText() );

        QString hPos_str = QString::number(ui->hPosDownSpinBox->value(), 'f', 1);
        text_line.setTextHPosition( hPos_str );

        text_line.setTextVAlign( ui->vAlignDownBox->currentText() );

        QString vPos_str = QString::number(ui->vPosDownSpinBox->value(), 'f', 1);
        text_line.setTextVPosition( vPos_str );

        text_lines.append(text_line);
    }

    ui->stEditDisplay2->updateTextPosition(text_lines);

    mTextPosChangedByUser = false;
}

void MainWindow::on_vAlignDownBox_activated(const QString &arg1) {

    if ( mTextPosChangedBySoft == false ) {
        updateTextPosition();
    }
}

void MainWindow::on_vPosDownSpinBox_valueChanged(const QString &arg1) {

    if ( mTextPosChangedBySoft == false ) {
        updateTextPosition();
    }
}

void MainWindow::on_hAlignDownBox_activated(const QString &arg1) {

    if ( mTextPosChangedBySoft == false ) {
        updateTextPosition();
    }
}

void MainWindow::on_hPosDownSpinBox_valueChanged(const QString &arg1) {

    if ( mTextPosChangedBySoft == false ) {
        updateTextPosition();
    }
}

void MainWindow::on_vAlignUpBox_activated(const QString &arg1) {

    if ( mTextPosChangedBySoft == false ) {
        updateTextPosition();
    }
}

void MainWindow::on_vPosUpSpinBox_valueChanged(const QString &arg1) {

    if ( mTextPosChangedBySoft == false ) {
        updateTextPosition();
    }
}

void MainWindow::on_hAlignUpBox_activated(const QString &arg1) {

    if ( mTextPosChangedBySoft == false ) {
        updateTextPosition();
    }
}

void MainWindow::on_hPosUpSpinBox_valueChanged(const QString &arg1) {

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
