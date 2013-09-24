#include "myvideoplayer.h"
#include "ui_myvideoplayer.h"
#include <QMediaPlayer>
#include <QGraphicsVideoItem>
#include <QFileDialog>
#include <QTime>
#include <QMouseEvent>

// Convert second in milliseconds
#define SEC_TO_MSEC 1000
// Number of frame per second
#define FRAME_PS_SEC 25

// This widget manage a media player with a position slider, a clock and a play/pause button
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
    mPlayerPositionNotifyIntervalMs = SEC_TO_MSEC / FRAME_PS_SEC;
    mpPlayer->setNotifyInterval(mPlayerPositionNotifyIntervalMs);

    // Init timescto 00:00:00.000
    mpDurationTimeHMS = new QTime(0, 0, 0, 0);
    mpCurrentTimeHMS = new QTime(0, 0, 0, 0);

    // Init graphics scene to display the player clock
    mpTimeGraphicsScene = new QGraphicsScene;
    mpTimeTextItem = new QGraphicsTextItem;
    ui->timeView->setStyleSheet("background: transparent");

    // Init SIGNAL / SLOT connections
    connect(mpPlayer, SIGNAL(positionChanged(qint64)), this, SLOT(updateSliderPosition(qint64)));
    connect(mpPlayer, SIGNAL(durationChanged(qint64)), this, SLOT(updateDuration()));
    connect(mpPlayer, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(updatePlayerState(QMediaPlayer::State)));

    ui->playButton->setEnabled(false);
    ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));

    ui->timeSlider->installEventFilter(this);
    mTimeSliderPositionChangedByGui = false;
}

MyVideoPlayer::~MyVideoPlayer()
{
    delete ui;
}

// Swith the player state to "play/pause" when play/pause button clicked
void MyVideoPlayer::on_playButton_clicked() {

    switch ( mpPlayer->state() ) {
    case QMediaPlayer::PlayingState:
        mpPlayer->pause();
        break;
    default:
        mpPlayer->play();
        break;
    }
}

// Update the play/pause icon displayed in function of the player state
void MyVideoPlayer::updatePlayerState(QMediaPlayer::State state)
{
    switch ( state ) {
    case QMediaPlayer::PlayingState:
        ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        break;
    default:
        ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        break;
    }
}

// Open a media file
QString MyVideoPlayer::openFile() {

    QString file_name = QFileDialog::getOpenFileName(this, tr("Open Movie"),QDir::homePath() +"/Videos",
                                                     tr("Video Files (*.mp4 *.wmv *.avi)"));

    if ( !file_name.isEmpty() ) {
        mpPlayer->setMedia(QUrl::fromLocalFile(file_name));
        //mpPlayer->play();
        ui->playButton->setEnabled(true);
    }
    return file_name;
}

// Update de duration of the current media
void MyVideoPlayer::updateDuration() {

    mVideoDuration = mpPlayer->duration();

    // Send the new duration to other module
    emit durationChanged(mVideoDuration);

    // Compute the slider range in function of the video duration and the player position notification interval time
    mTimeSliderMaxRange = (qint32)( mVideoDuration / (qint64)mPlayerPositionNotifyIntervalMs );
    ui->timeSlider->setRange(0, mTimeSliderMaxRange);

    MyVideoPlayer::updateTime();
}

// Update the position of the slider in function of the player position in millisecond
void MyVideoPlayer::updateSliderPosition(qint64 playerPositionMs) {

    qint32 slider_position;

    if ( playerPositionMs <= mVideoDuration ) {
        emit positionChanged(playerPositionMs);

        // If the player position was changed by software, move the slider
        if ( mTimeSliderPositionChangedByGui == false ) {
            slider_position = (qint32)(playerPositionMs / mPlayerPositionNotifyIntervalMs);
            ui->timeSlider->setSliderPosition(slider_position);
        } // If the user moved the cursor, do nothing
        else {
            mTimeSliderPositionChangedByGui = false;
        }

        MyVideoPlayer::updateTime();
    }
}

void MyVideoPlayer::on_timeSlider_sliderPressed()
{
    mpPlayer->pause();
}

void MyVideoPlayer::on_timeSlider_sliderMoved(int sliderPosition)
{
//    qint64 player_position;

//    if(mpPlayer->state() != QMediaPlayer::PlayingState) {
//        player_position = sliderPosition * mPlayerPositionNotifyIntervalMs;
//        mpPlayer->setPosition(player_position);
//    }

//    MyVideoPlayer::updateTime();
}

// Update and display the player clock
void MyVideoPlayer::updateTime() {

    QTime temp_time;

    // Update video current time
    temp_time.setHMS(0, 0, 0, 0);
    temp_time = temp_time.addMSecs(mpPlayer->position());
    mpCurrentTimeHMS->setHMS(temp_time.hour(), temp_time.minute(), temp_time.second(), temp_time.msec());
    // Update video duration time
    temp_time.setHMS(0, 0, 0, 0);
    temp_time = temp_time.addMSecs(mpPlayer->duration());
    mpDurationTimeHMS->setHMS(temp_time.hour(), temp_time.minute(), temp_time.second(), temp_time.msec());
    // Display
    mpTimeTextItem->~QGraphicsTextItem();
    mpTimeTextItem = mpTimeGraphicsScene->addText(mpCurrentTimeHMS->toString("hh:mm:ss.zzz") +" \\ " +mpDurationTimeHMS->toString("hh:mm:ss.zzz"));
    ui->timeView->setScene(mpTimeGraphicsScene);
}

void MyVideoPlayer::on_timeSlider_sliderReleased() {

    mpPlayer->play();
}

bool MyVideoPlayer::eventFilter(QObject* watched, QEvent* event) {

    bool status = QWidget::eventFilter(watched, event);

    // Move the slider at mouse released position
    if ( watched == ui->timeSlider ) {
        if ( event->type() == QEvent::MouseButtonRelease ) {
            QMouseEvent* mouse_event = static_cast<QMouseEvent*>(event);
            ui->timeSlider->setValue(QStyle::sliderValueFromPosition(ui->timeSlider->minimum(),
                                                                     ui->timeSlider->maximum(),
                                                                     mouse_event->x(),
                                                                     ui->timeSlider->width()));
        }
        else if ( event->type() == QEvent::MouseButtonPress ) {
            return true;
        }
    }
    return status;
}

void MyVideoPlayer::resizeEvent(QResizeEvent* event) {

    mpVideoScene->setSceneRect(0.0, 0.0, ui->videoView->size().width(), ui->videoView->size().height() );
    mpVideoItem->setSize(QSizeF(ui->videoView->size()));
    QWidget::resizeEvent(event);
}

// Get the video item size
QSizeF MyVideoPlayer::videoItemSize() {

    return mpVideoItem->size();
}

// Get the video item native size (the native size of the media)
QSizeF MyVideoPlayer::videoItemNativeSize() {

    return mpVideoItem->nativeSize();
}

// Update the player position in function of slider position
void MyVideoPlayer::on_timeSlider_valueChanged(int sliderPosition)
{
    qint64 player_position_ms;

    // Slider was moved by user
    mTimeSliderPositionChangedByGui = true;
    player_position_ms = sliderPosition * mPlayerPositionNotifyIntervalMs;
    setPosition(player_position_ms);
}

// Interface to set the player position
void MyVideoPlayer::setPosition(qint64 videoPlayerPositionMs) {


    if ( mpPlayer->isVideoAvailable() ) {
        mpPlayer->setPosition(videoPlayerPositionMs);
    }
}

// get the player position
qint64 MyVideoPlayer::playerPosition() {
    return mpPlayer->position();
}

bool MyVideoPlayer::videoAvailable() {
    return mpPlayer->isVideoAvailable();
}

