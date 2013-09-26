#ifndef MYWAVEFORM_H
#define MYWAVEFORM_H

#include <QWidget>
#include <mysubtitles.h>

namespace Ui {
class MyWaveForm;
}

class MyVideoPlayer;
class QAudioDecoder;
class QFile;
class QwtPlotCurve;
class QwtPlotMarker;
class QwtPlotTextLabel;
class QwtPlotZoneItem;
class QwtPlotGrid;

// This widget manage an audio waveform displaying, with zoom and shift.
// Display a position marker at the input time,
// allow media control by sending output time of the position clicked.
class MyWaveForm : public QWidget
{
    Q_OBJECT
    
public:
    explicit MyWaveForm(QWidget *parent = 0);
    ~MyWaveForm();

signals:
    void markerPositionChanged(qint64);
    void ctrlLeftClickEvent(qint64);
    void ctrlRightClickEvent(qint64);
    void shiftLeftClickEvent(qint64);

public slots:
    void openFile(QString waveform_file_name, QString video_file_name);
    void updatePostionMarker(qint64 positionMs);
    void drawSubtitlesZone(QList<MySubtitles> subtitlesList, qint32 subtitleIndex);
    void changeZoneColor(qint32 subtitleIndex);
    void removeSubtitleZone(qint32 subtitleIndex);
    void removeAllSubtitlesZones();
    void changeZoneStartTime(qint32 subtitleIndex, qint64 startTimeMs);
    void changeZoneEndTime(qint32 subtitleIndex, qint64 endTimeMs);
    qint64 currentPositonMs();
    qint64 posMsFromPosPx(int xPos);

private slots:
    void readBuffer();
    void decodingFinished();
    void initWaveForm();
    bool eventFilter(QObject* watched, QEvent* event);
    void plotWaveForm();
    bool computeZoom(qint16 step, qint16 xPos);
    bool timeScaleShift(qint16 step);
    void setMarkerPosFromClick(int xPos);


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

    //
    QList<QwtPlotZoneItem*> mpZoneItemList;

    QwtPlotGrid* mpGrid;

    // The current position in millisecond
    qint64 mCurrentPositonMs;



};

#endif // MYWAVEFORM_H
