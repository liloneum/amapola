#include "myvideoplayer.h"
#include "ui_myvideoplayer.h"
#include <QMediaPlayer>
//#include <QVideoWidget>
#include <QGraphicsVideoItem>
#include <QFileDialog>
#include <QTime>
#include <QMouseEvent>

MyVideoPlayer::MyVideoPlayer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyVideoPlayer)
{
    ui->setupUi(this);

    // Init video scene, add video item, set scene to videoview
    mpVideoScene = new QGraphicsScene(this);
    mpVideoItem = new QGraphicsVideoItem;
    ui->videoView->setScene(mpVideoScene);
    mpVideoScene->addItem(mpVideoItem);
    //ui->videoView->sizePolicy().setHeightForWidth(true);

    // Init mediaplayer, set mediaplayer output to video item
    mpPlayer = new QMediaPlayer(this, QMediaPlayer::StreamPlayback);
    mpPlayer->setVideoOutput(mpVideoItem);
    // Set the player position notify interval to 1 frame (1000ms/nbr of frame)
    mPlayerPositionNotifyInterval = 1000/24;
    mpPlayer->setNotifyInterval(mPlayerPositionNotifyInterval);

    // Init timescto 00:00:00.000
    mpDurationTime = new QTime(0, 0, 0, 0);
    mpCurrentTime = new QTime(0, 0, 0, 0);

    mpTimeGraphicsScene = new QGraphicsScene;
    mpTimeTextItem = new QGraphicsTextItem;
    ui->timeView->setStyleSheet("background: transparent");

    // Init SIGNAL / SLOT connections
    connect(mpPlayer, SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged(qint64)));
    connect(mpPlayer, SIGNAL(durationChanged(qint64)), this, SLOT(durationChanged()));
    connect(mpPlayer, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(mediaStateChanged(QMediaPlayer::State)));

    ui->playButton->setEnabled(false);
    ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));

    ui->timeSlider->installEventFilter(this);
    mTimeSliderPositionChangedByGui = false;
}

MyVideoPlayer::~MyVideoPlayer()
{
    delete ui;
}

void MyVideoPlayer::on_playButton_clicked() {

    switch(mpPlayer->state()) {
    case QMediaPlayer::PlayingState:
        mpPlayer->pause();
        break;
    default:
        mpPlayer->play();
        break;
    }
}

void MyVideoPlayer::mediaStateChanged(QMediaPlayer::State state)
{
    switch(state) {
    case QMediaPlayer::PlayingState:
        ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        break;
    default:
        ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        break;
    }
}

QString MyVideoPlayer::openFile() {

    QString file_name = QFileDialog::getOpenFileName(this, tr("Open Movie"),QDir::homePath());

    if (!file_name.isEmpty()) {
        mpPlayer->setMedia(QUrl::fromLocalFile(file_name));
        mpPlayer->play();
        ui->playButton->setEnabled(true);
    }
    return file_name;
}

void MyVideoPlayer::durationChanged() {

    qint64 video_duration = mpPlayer->duration();

    //QGraphicsTextItem *label = new QGraphicsTextItem("\\" + tempTime.toString("hh:mm:ss.zzz"), videoItem);
    //label->moveBy(100, 100);

    mTimeSliderMaxRange = video_duration / mPlayerPositionNotifyInterval;
    ui->timeSlider->setRange(0, mTimeSliderMaxRange);

    MyVideoPlayer::UpdateTime();
}

void MyVideoPlayer::positionChanged(qint64 playerPosition) {

    int slider_position;

        if (mTimeSliderPositionChangedByGui == false) {
            slider_position = (int) (playerPosition / mPlayerPositionNotifyInterval);
            ui->timeSlider->setSliderPosition(slider_position);
        }
        else {
            mTimeSliderPositionChangedByGui = false;
        }

    MyVideoPlayer::UpdateTime();
}

void MyVideoPlayer::on_timeSlider_sliderPressed()
{
    mpPlayer->pause();
}

void MyVideoPlayer::on_timeSlider_sliderMoved(int sliderPosition)
{
//    qint64 player_position;

//    if(mpPlayer->state() != QMediaPlayer::PlayingState) {
//        player_position = sliderPosition * mPlayerPositionNotifyInterval;
//        mpPlayer->setPosition(player_position);
//    }

//    MyVideoPlayer::UpdateTime();
}

void MyVideoPlayer::UpdateTime() {

    QTime temp_time;

    // Update video current time
    temp_time.setHMS(0, 0, 0, 0);
    temp_time = temp_time.addMSecs(mpPlayer->position());
    mpCurrentTime->setHMS(temp_time.hour(), temp_time.minute(), temp_time.second(), temp_time.msec());
    // Update video duration time
    temp_time.setHMS(0, 0, 0, 0);
    temp_time = temp_time.addMSecs(mpPlayer->duration());
    mpDurationTime->setHMS(temp_time.hour(), temp_time.minute(), temp_time.second(), temp_time.msec());
    // Display
    mpTimeTextItem->~QGraphicsTextItem();
    mpTimeTextItem = mpTimeGraphicsScene->addText(mpCurrentTime->toString("hh:mm:ss.zzz") +" \\ " +mpDurationTime->toString("hh:mm:ss.zzz"));
    ui->timeView->setScene(mpTimeGraphicsScene);
}

void MyVideoPlayer::on_timeSlider_sliderReleased() {

    mpPlayer->play();
}

bool MyVideoPlayer::eventFilter(QObject* watched, QEvent* event) {

    bool status = QWidget::eventFilter(watched, event);

    if (watched == ui->timeSlider) {
        if (event->type() == QEvent::MouseButtonRelease ) {
            QMouseEvent* mouse_event = static_cast<QMouseEvent*>(event);
//            int min = ui->timeSlider->minimum();
//            int max = ui->timeSlider->maximum();
//            int pos = mouse_event->x();
//            int span = ui->timeSlider->width();
//            int value = QStyle::sliderValueFromPosition(ui->timeSlider->minimum(), ui->timeSlider->maximum(), mouse_event->x(), ui->timeSlider->width());
            ui->timeSlider->setValue(QStyle::sliderValueFromPosition(ui->timeSlider->minimum(), ui->timeSlider->maximum(), mouse_event->x(), ui->timeSlider->width()));
        }
        else if (event->type() == QEvent::MouseButtonPress ) {
            return true;
        }
    }
    return status;
}


void MyVideoPlayer::resizeEvent(QResizeEvent* event) {

    mpVideoItem->setSize(QSizeF(ui->videoView->size()));
    QWidget::resizeEvent(event);
}

void MyVideoPlayer::on_timeSlider_valueChanged(int sliderPosition)
{
    qint64 player_position;

    mTimeSliderPositionChangedByGui = true;
    player_position = sliderPosition * mPlayerPositionNotifyInterval;
    mpPlayer->setPosition(player_position);
}


void MyVideoPlayer::on_timeSlider_actionTriggered(int action)
{
    //int a = action;
}
