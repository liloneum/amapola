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

public slots:
    QString openFile();

private slots:
    void on_playButton_clicked();
    void mediaStateChanged(QMediaPlayer::State state);
    void on_timeSlider_sliderReleased();
    void positionChanged(qint64 playerPosition);
    void durationChanged();
    void on_timeSlider_sliderPressed();
    void on_timeSlider_sliderMoved(int sliderPosition);
    void resizeEvent(QResizeEvent* event);
    bool eventFilter(QObject* watched, QEvent* event);
    void UpdateTime();

    void on_timeSlider_valueChanged(int value);

    void on_timeSlider_actionTriggered(int action);
    
private:
    Ui::MyVideoPlayer *ui;
    QMediaPlayer* mpPlayer;
    QGraphicsScene* mpVideoScene;
    QGraphicsVideoItem* mpVideoItem;
    //QVideoWidget *video;
    QTime* mpCurrentTime;
    QTime* mpDurationTime;
    QGraphicsScene* mpTimeGraphicsScene;
    QGraphicsTextItem* mpTimeTextItem;
    int mTimeSliderMaxRange;
    int mPlayerPositionNotifyInterval;
    bool mTimeSliderPositionChangedByGui;
};

#endif // MYVIDEOPLAYER_H
