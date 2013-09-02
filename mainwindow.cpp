#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->stEditDisplay->setStyleSheet("background: transparent; color: yellow");
    ui->stEditDisplay->setFocus(Qt::OtherFocusReason);

    connect(ui->videoPlayer, SIGNAL(positionChanged(qint64)), ui->waveForm, SLOT(updatePostionMarker(qint64)));
    connect(ui->waveForm, SIGNAL(positionChanged(qint64)), ui->videoPlayer, SLOT(setPosition(qint64)));

    connect(ui->videoPlayer, SIGNAL(durationChanged(qint64)),ui->subTable, SLOT(videoDurationChanged(qint64)));
    connect(ui->videoPlayer, SIGNAL(positionChanged(qint64)), ui->subTable, SLOT(updateStDisplay(qint64)));
    connect(ui->stEditDisplay, SIGNAL(cursorPositionChanged()), this, SLOT(updateSubTable()));
    connect(ui->subTable, SIGNAL(itemSelectionChanged(qint32)), this, SLOT(updateVideoPosition(qint32)));
    connect(ui->subTable, SIGNAL(newTextToDisplay(QString)), this, SLOT(updateTextEdit(QString)));

    connect(ui->videoPlayer, SIGNAL(durationChanged(qint64)),this, SLOT(updateStEditSize()));
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::updateSubTable() {

    QString st_text;

    st_text = ui->stEditDisplay->toPlainText();
    ui->subTable->updateStTable(st_text);
}

void MainWindow::updateVideoPosition(qint32 positionMs) {

    ui->videoPlayer->setPosition(positionMs);
    ui->stEditDisplay->setFocus(Qt::OtherFocusReason);
    ui->stEditDisplay->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
}

void MainWindow::updateTextEdit(QString stText) {

    ui->stEditDisplay->setPlainText(stText);
    ui->stEditDisplay->setFocus(Qt::OtherFocusReason);
    ui->stEditDisplay->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
}

void MainWindow::on_actionOpen_triggered()
{
    QString video_file_name = ui->videoPlayer->openFile();

    if ( !video_file_name.isEmpty() ) {

        QString wf_file_name = video_file_name;
        wf_file_name.truncate(wf_file_name.lastIndexOf("."));
        wf_file_name.append(".wf");

        ui->waveForm->openFile(wf_file_name, video_file_name);

        ui->subTable->initStTable();

    }
}

void MainWindow::updateStEditSize() {

    QSizeF video_item_size;
    QSizeF video_item_native_size;
    qint32 row_height;
    qint32 text_edit_height;

    // Resize the height of subtitles edit zone in function of font size
    QFontMetrics font_metrics(ui->stEditDisplay->font());
    row_height = font_metrics.lineSpacing();
    text_edit_height = (2 * row_height) + 4;
    ui->stEditDisplay->setFixedHeight(text_edit_height);

    // Resize the width of subtitles edit zone in function of video width
    video_item_size = ui->videoPlayer->videoItemSize();
    ui->stEditDisplay->setFixedWidth(video_item_size.width());

    video_item_native_size = ui->videoPlayer->videoItemNativeSize();
    qreal video_ratio = video_item_native_size.width() / video_item_native_size.height();

    qint32 video_height = (qint32) ( video_item_size.width() / video_ratio );

    qint32 video_bottom_pos = (qint32)( ( ( video_item_size.height() - (qreal)video_height ) / 2.0 ) + (qreal)video_height );

    ui->stEditDisplay->move(0, video_bottom_pos - text_edit_height - 1);
}

void MainWindow::resizeEvent(QResizeEvent* event) {

    updateStEditSize();
    QMainWindow::resizeEvent(event);
}


void MainWindow::on_actionQuit_triggered()
{
    close();
}
