#ifndef MYWAVEFORM_H
#define MYWAVEFORM_H

#include <QWidget>

namespace Ui {
class MyWaveForm;
}

class MyVideoPlayer;
class QAudioDecoder;
class QFile;
class QwtPlotCurve;
class QwtPlotMarker;
class QwtPlotTextLabel;

class MyWaveForm : public QWidget
{
    Q_OBJECT
    
public:
    explicit MyWaveForm(QWidget *parent = 0);
    ~MyWaveForm();

signals:
    void positionChanged(qint64);

public slots:
    void openFile(QString waveform_file_name, QString video_file_name);

private slots:
    void readBuffer();
    void decodingFinished();
    void initWaveForm();
    bool eventFilter(QObject* watched, QEvent* event);
    void plotWaveForm();
    bool computeZoom(qint16 step, qint16 xPos);
    bool timeScaleShift(qint16 step);
    void setPosition(int xPos);
    void updatePostionMarker(qint64 playerPositionMs);

private:
    // Qt designer ui reference
    Ui::MyWaveForm *ui;

    // Audio decoder object
    QAudioDecoder* mpDecoder;

    // Stream to read/write file
    QDataStream* mpStream;

    // File object
    QFile* mpFile;

    // Lower time in millisecond to display
    qint32 mMinPlotTimeMs;

    // Higher time in millisecond to display
    qint32 mMaxPlotTimeMs;

    // Vector wich contain amplitude data
    QVector<double> mAmplitudeVector;

    // Vector wich contain time data in millisecond
    QVector<double> mTimeVectorMs;

    // Wave-form curve
    QwtPlotCurve* mpWaveFormCurve;

    // Marker object
    QwtPlotMarker* mpPositionMarker;

    // Text label to display "loading..." message
    QwtPlotTextLabel* mpLoadingTextItem;

    // Current media duration in millisecond
    qint32 mMediaDurationMs;
};

#endif // MYWAVEFORM_H
