#include "myvideoplayer.h"
#include "ui_myvideoplayer.h"
#include <QMediaPlayer>
#include <QGraphicsVideoItem>
#include <QFileDialog>
#include <QTime>

// Convert second in milliseconds
#define SEC_TO_MSEC 1000

// This widget manage a media player with a position slider, a clock and a play/pause button
MyVideoPlayer::MyVideoPlayer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyVideoPlayer)
{
    ui->setupUi(this);

    // Init video scene, add video item, set scene to videoview
    mpVideoScene = new QGraphicsScene(this);
    mpVideoItem = new QGraphicsVideoItem;
    ui->videoView->setStyleSheet("background: black");
    ui->videoView->setScene(mpVideoScene);
    mpVideoScene->addItem(mpVideoItem);

    // Init mediaplayer, set mediaplayer output to video item
    mpPlayer = new QMediaPlayer(this);
    mpPlayer->setVideoOutput(mpVideoItem);

    // Init the media player position changed notifications interval
    mPlayerPositionNotifyIntervalMs = 1000;

    // Player clock background transparent
    ui->currentTimeHMS->setStyleSheet("background: transparent");
    ui->durationTimeHMS->setStyleSheet("background: transparent");

    // Init SIGNAL / SLOT connections
    connect(mpPlayer, SIGNAL(positionChanged(qint64)), this, SLOT(updateSliderPosition(qint64)));
    connect(mpPlayer, SIGNAL(durationChanged(qint64)), this, SLOT(updateDuration()));
    connect(mpPlayer, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(updatePlayerState(QMediaPlayer::State)));

    connect(ui->timeSlider, SIGNAL(sliderPositionChanged(qint64)), this, SLOT(sliderMoved(qint64)));

    connect(mpVideoItem, SIGNAL(nativeSizeChanged(QSizeF)), this, SLOT(checkVideoNativeSize(QSizeF)));

    // Init the Play / Pause button
    ui->playButton->setEnabled(false);
    ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));

    // Init the video duration
    mVideoDuration = 0;

    // Flag to check if the slider was moved by user
    mTimeSliderPositionChangedByGui = false;

    // Flag to check if waveform data are redy
    mWaveFormReady = false;
    mMediaChanged = false;
    // Init a empty waveform slider
    mWaveFormFileName = "";
    ui->timeSlider->openFile("", mVideoDuration);
}

MyVideoPlayer::~MyVideoPlayer()
{
    delete ui;
}

// Swith the player state to "play/pause" when play/pause button clicked
void MyVideoPlayer::on_playButton_clicked() {

    QString player_state_infos;

    switch ( mpPlayer->state() ) {
    case QMediaPlayer::PlayingState:
        mpPlayer->pause();
        player_state_infos = "Pause";
        break;
    default:
        mpPlayer->play();
        player_state_infos = "Play";
        break;
    }

    emit playerStateInfos(player_state_infos);
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

// Change video playback rate (x1.0 to x4.0)
bool MyVideoPlayer::changePlaybackRate(bool moreSpeed) {

    bool status = false;
    QString player_state_infos = "";
    qreal current_playback_rate = mpPlayer->playbackRate();

    if ( moreSpeed == true ) {
        if ( current_playback_rate <= 3.81 ) {
            mpPlayer->setPlaybackRate(current_playback_rate + 0.2);
            player_state_infos = "x " +QString::number(current_playback_rate + 0.2, 'f', 1);
            status = true;
        }
    }
    else {
        if ( current_playback_rate >= 1.19 ) {
            mpPlayer->setPlaybackRate(current_playback_rate - 0.2);
            player_state_infos = "x " +QString::number(current_playback_rate - 0.2,'f', 1);
            status = true;
        }
    }

    if ( !player_state_infos.isEmpty() ) {
        emit playerStateInfos(player_state_infos);
    }

    return status;
}

// Open a media file
QString MyVideoPlayer::openFile(QString fileName) {

    if ( fileName == "" ) {

        fileName = QFileDialog::getOpenFileName(this, tr("Open Movie"),QDir::homePath() +"/Videos",
                                                     tr("Video Files (*.mp4 *.wmv *.avi)"));
    }

    if ( !fileName.isEmpty() ) {

        // Set the player position notify interval to 1 frame (1000ms/nbr of frame)
        mPlayerPositionNotifyIntervalMs = qRound( (qreal)SEC_TO_MSEC / qApp->property("prop_FrameRate_fps").toReal() );
        mpPlayer->setNotifyInterval(mPlayerPositionNotifyIntervalMs);

        mpPlayer->setMedia(QUrl::fromLocalFile(fileName));
        mpPlayer->play();
        mpPlayer->pause();
        ui->playButton->setEnabled(true);
    }

    return fileName;
}

// Unload all media
void MyVideoPlayer::unloadMedia() {

    mpPlayer->stop();
    mpPlayer->setMedia(NULL);
    ui->timeSlider->openFile("", 0);
    ui->playButton->setEnabled(false);
}

// Load tha waveform in the slider
void MyVideoPlayer::loadWaveForm(QString waveFormFileName) {

    // The video is loaded. Load the waveform
    if ( mMediaChanged == true ) {
        ui->timeSlider->openFile(waveFormFileName, mVideoDuration);
        mMediaChanged = false;
    }
    else {
        // If video not loaded yet, save the file name for later
        mWaveFormFileName = waveFormFileName;
        mWaveFormReady = true;
    }
}

// Update de duration of the current media
// New duration signal mean that media is loaded
void MyVideoPlayer::updateDuration() {

    mVideoDuration = mpPlayer->duration();

    // Waveform data are ready, load it
    if ( mWaveFormReady == true ) {
        ui->timeSlider->openFile(mWaveFormFileName, mVideoDuration);
        mWaveFormReady = false;
        mWaveFormFileName = "";
    } // Not ready yet, load an empty waveform
    else {
        ui->timeSlider->openFile("", mVideoDuration);
        mMediaChanged = true;
    }

    // Send the new duration to other module
    emit durationChanged(mVideoDuration);

    this->updateTime(ui->timeSlider->currentPositonMs());
}

// Update the position of the slider in function of the player position in millisecond
void MyVideoPlayer::updateSliderPosition(qint64 playerPositionMs) {

    if ( playerPositionMs <= mVideoDuration ) {

        emit positionChanged(playerPositionMs);

        // If the player position was changed by software, move the slider
        if ( mTimeSliderPositionChangedByGui == false ) {
            ui->timeSlider->updatePostionSlider(playerPositionMs);
        } // If the user moved the cursor, do nothing
        else {
            mTimeSliderPositionChangedByGui = false;
        }

        this->updateTime(ui->timeSlider->currentPositonMs());
    }
}

// Update and display the player clock
void MyVideoPlayer::updateTime(qint64 positionMs) {

    QTime time_base(0, 0, 0, 0);

    ui->currentTimeHMS->setTime( time_base.addMSecs(positionMs) );
    ui->durationTimeHMS->setTime( time_base.addMSecs(mVideoDuration) );
}

void MyVideoPlayer::checkVideoNativeSize(QSizeF videoNativeSize) {

    if ( ( videoNativeSize.isValid() ) && ( videoNativeSize.toSize() != QSize(0,0) ) ) {
        emit nativeSizeChanged(videoNativeSize);
    }
}

void MyVideoPlayer::resizeEvent(QResizeEvent* event) {

    mpVideoScene->setSceneRect(0.0, 0.0, ui->videoView->size().width(), ui->videoView->size().height() );
    mpVideoItem->setSize( QSizeF(ui->videoView->size()) - ( QSizeF(ui->videoView->frameWidth(), ui->videoView->frameWidth()) * 2 ) );
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
void MyVideoPlayer::sliderMoved(qint64 positionMs) {

    // Slider was moved by user
    mTimeSliderPositionChangedByGui = true;
    this->setPosition(positionMs);
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

