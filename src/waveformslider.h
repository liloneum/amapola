#ifndef WAVEFORMSLIDER_H
#define WAVEFORMSLIDER_H

#include <QWidget>
#include <waveform.h>

namespace Ui {
class WaveFormSlider;
}

class QFile;
class QwtPlot;
class QwtPlotCurve;
class QwtPlotMarker;
class QwtPlotZoneItem;
class QTimeEdit;

class WaveFormSlider : public QWidget
{
    Q_OBJECT

public:
    explicit WaveFormSlider(QWidget *parent = 0);
    ~WaveFormSlider();

signals:
    void sliderPositionChanged(qint64);

public slots:
    void openFile(QString waveform_file_name, qint64 mediaDurationMs);
    void updatePostionSlider(qint64 positionMs);
    qint64 currentPositonMs();
    qint64 posMsFromPosPx(int xPos);
    void setRtMarkerPos(int xPos);

private slots:
    void initWaveForm(qint64 durationMs);
    bool eventFilter(QObject* watched, QEvent* event);
    void setSliderPosFromClick(int xPos);

private:
    Ui::WaveForm *ui;

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

    // Real time position Marker object
    QwtPlotMarker* mpRTMarker;

    // Display the time in function of mouse position
    QTimeEdit* mpRtTime;

    // Current media duration in millisecond
    qint32 mMediaDurationMs;

    // Current position bar
    QwtPlotZoneItem* mpPosSlider;

    // The current position in millisecond
    qint64 mCurrentPositonMs;
};

#endif // WAVEFORMSLIDER_H
