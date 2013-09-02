#ifndef MYVIDEOPLAYER_H
#define MYVIDEOPLAYER_H

#include <QWidget>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QGraphicsTextItem>
#define FRAME_RATE 25

namespace Ui {
class MyVideoPlayer;
}

class QMediaPlayer;
//class QVideoWidget;
class QGraphicsVideoItem;
class QTime;
class QGraphicsScene;

class MyVideoPlayer : public QWidget
{
    Q_OBJECT

    
public:
    explicit MyVideoPlayer(QWidget *parent = 0);
    ~MyVideoPlayer();
    QSizeF videoItemSize();
    QSizeF videoItemNativeSize();

signals:
    void positionChanged(qint64);
    void durationChanged(qint64);

public slots:
    QString openFile();
    void setPosition(qint64 videoPlayerPositionMs);

private slots:
    void on_playButton_clicked();
    void updatePlayerState(QMediaPlayer::State state);
    void on_timeSlider_sliderReleased();
    void updateSliderPosition(qint64 playerPositionMs);
    void updateDuration();
    void on_timeSlider_sliderPressed();
    void on_timeSlider_sliderMoved(int sliderPosition);
    void resizeEvent(QResizeEvent* event);
    bool eventFilter(QObject* watched, QEvent* event);
    void updateTime();

    void on_timeSlider_valueChanged(int value);

    
private:
    // Qt designer ui reference
    Ui::MyVideoPlayer *ui;

    // Media player object
    QMediaPlayer* mpPlayer;

    // Graphics scene to display video item
    QGraphicsScene* mpVideoScene;

    // Graphics item to draw video
    QGraphicsVideoItem* mpVideoItem;

    // Time object which contain the current time with "hh:mm:ss.zzz" format
    QTime* mpCurrentTimeHMS;

    // Time object which contain the video duration time with "hh:mm:ss.zzz" format
    QTime* mpDurationTimeHMS;

    // Graphics scene to display time item
    QGraphicsScene* mpTimeGraphicsScene;

    // Graphics item to draw time
    QGraphicsTextItem* mpTimeTextItem;

    // Maximum range of position slider
    qint32 mTimeSliderMaxRange;

    // Player position changed notify interval
    qint16 mPlayerPositionNotifyIntervalMs;

    // Flag to check if the slider was moved by user
    bool mTimeSliderPositionChangedByGui;

    // Video duration time in millisecond
    qint64 mVideoDuration;
};

#endif // MYVIDEOPLAYER_H
