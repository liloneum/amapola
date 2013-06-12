#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->videoPlayer, SIGNAL(positionChanged(qint64)), ui->waveForm, SLOT(updatePostionMarker(qint64)));
    connect(ui->waveForm, SIGNAL(positionChanged(qint64)), ui->videoPlayer, SLOT(setPosition(qint64)));
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    QString video_file_name = ui->videoPlayer->openFile();

    if ( !video_file_name.isEmpty() ) {

        QString wf_file_name = video_file_name;
        wf_file_name.truncate(wf_file_name.lastIndexOf("."));
        wf_file_name.append(".wf");

        ui->waveForm->openFile(wf_file_name, video_file_name);
    }
}


void MainWindow::on_actionQuit_triggered()
{
    close();
}
