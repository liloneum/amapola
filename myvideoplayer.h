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
    QMediaPlayer* mpPlayer;
    Ui::MyVideoPlayer *ui;
    QGraphicsScene* mpVideoScene;
    QGraphicsVideoItem* mpVideoItem;
    QTime* mpCurrentTimeHMS;
    QTime* mpDurationTimeHMS;
    QGraphicsScene* mpTimeGraphicsScene;
    QGraphicsTextItem* mpTimeTextItem;
    qint32 mTimeSliderMaxRange;
    qint16 mPlayerPositionNotifyIntervalMs;
    bool mTimeSliderPositionChangedByGui;
    qint64 mVideoDuration;
};

#endif // MYVIDEOPLAYER_H
