#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "myfilereader.h"
#include "myfilewriter.h"
#include "SubtitlesParsers/SubRip/subripparser.h"
#include "SubtitlesParsers/DcSubtitle/dcsub.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //ui->stEditDisplay->setStyleSheet("background: transparent; color: yellow");

    connect(ui->videoPlayer, SIGNAL(positionChanged(qint64)), ui->waveForm, SLOT(updatePostionMarker(qint64)));
    connect(ui->waveForm, SIGNAL(positionChanged(qint64)), ui->videoPlayer, SLOT(setPosition(qint64)));

    connect(ui->videoPlayer, SIGNAL(durationChanged(qint64)),ui->subTable, SLOT(videoDurationChanged(qint64)));
    connect(ui->videoPlayer, SIGNAL(positionChanged(qint64)), ui->subTable, SLOT(updateStDisplay(qint64)));
    connect(ui->stEditDisplay2, SIGNAL(cursorPositionChanged()), this, SLOT(updateSubTable()));
    connect(ui->subTable, SIGNAL(itemSelectionChanged(qint32)), this, SLOT(updateVideoPosition(qint32)));
    connect(ui->subTable, SIGNAL(newTextToDisplay(MySubtitles)), this, SLOT(updateTextEdit(MySubtitles)));

    connect(ui->videoPlayer, SIGNAL(durationChanged(qint64)),this, SLOT(updateStEditSize()));
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::updateSubTable() {

    if ( ui->subTable->isNewEntry() ) {
        ui->subTable->updateStTable( ui->stEditDisplay2->subtitleData() );
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

    QString file_name = QFileDialog::getOpenFileName(this, tr("Open Subtitles"),QDir::homePath());
    DcSub* parser = new DcSub();
    QList<MySubtitles> subtitles_list = parser->open(file_name);

    if ( !subtitles_list.isEmpty() ) {

        ui->subTable->loadSubtitles(subtitles_list);
    }


}

void MainWindow::on_actionExport_Subtitles_triggered() {

    QString file_name = QFileDialog::getSaveFileName(this, tr("Save Subtitles"),QDir::homePath());
    MyFileWriter file_writer(file_name, "UTF-8");
    SubRipParser* parser = new SubRipParser();

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
