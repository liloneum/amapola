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
    Ui::MyWaveForm *ui;
    QAudioDecoder* mpDecoder;
    QDataStream* mpStream;
    QFile* mpFile;
    quint16 mZoomValue;
    qint32 mMinPlotTimeMs;
    qint32 mMaxPlotTimeMs;
    QVector<double> mAmplitudeVector;
    QVector<double> mTimeVectorMs;
    QwtPlotCurve* mpWaveFormCurve;
    QwtPlotMarker* mpPositionMarker;
    QwtPlotTextLabel* mpLoadingTextItem;
};

#endif // MYWAVEFORM_H
