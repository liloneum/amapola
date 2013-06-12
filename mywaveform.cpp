#include "mywaveform.h"
#include "ui_mywaveform.h"
#include <Qt>
#include <QAudioFormat>
#include <QAudioDecoder>
#include <QFile>
#include <QWheelEvent>
#include <QAudioDeviceInfo>
#include <QTime>
#include <qwt_plot_curve.h>
#include <qwt_scale_engine.h>
#include <qwt_plot_marker.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_textlabel.h>

#define SAMPLE_PER_SEC 1000
#define SAMPLE_SIZE_BYTES 2
#define BYTE_IN_BITS 8
#define SEC_TO_MSEC 1000
#define ZOOM_FACTOR 2
#define TIME_SHIFT_FACTOR 0.1
#define MIN_TIME_SCALE_MS 5000
#define MAX_TIME_SCALE_MS 60000

// Class to convert the time label from "milliseconds" to "hh:mm:ss"
class TimeScaleDraw: public QwtScaleDraw
{
public:
    TimeScaleDraw( const QTime &base ):
        baseTime( base )
    {
    }
    virtual QwtText label( double v ) const
    {
        QTime upTime = baseTime.addMSecs(static_cast<int>( v ) );
        return upTime.toString();
    }
private:
    QTime baseTime;
};

// MyWaveForm class constructor
MyWaveForm::MyWaveForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyWaveForm)
{
    QTime t(0, 0, 0, 0);

    ui->setupUi(this);

    // Create file, decoder and stream
    mpFile = new QFile(this);
    mpDecoder = new QAudioDecoder(this);
    mpStream = new QDataStream(mpFile);
    mpStream->setByteOrder(QDataStream::LittleEndian);

    // Init the data vectors
    mAmplitudeVector.resize(1);
    mAmplitudeVector.fill(0.0);
    mTimeVectorMs.resize(1);
    mTimeVectorMs.fill(0.0);

    // Init the Min and Max time to plot
    mMinPlotTimeMs = 0;
    mMaxPlotTimeMs = 0;

    // Create the waveform curve to plot
    mpWaveFormCurve = new QwtPlotCurve("WaveForm");

    // Init the ploting object - Y axis : hide, auto-scale off, set min/max (signed 16 bits range)
    ui->waveFormPlot->enableAxis(QwtPlot::yLeft,false);
    ui->waveFormPlot->setAxisAutoScale(QwtPlot::yLeft, false);
    ui->waveFormPlot->setAxisScale(QwtPlot::yLeft, -32767.0, 32767, 0);

    // Init the ploting object - X axis : auto-scale on, remove the end gap, label draw in hh:mm:ss
    ui->waveFormPlot->axisScaleEngine(QwtPlot::xBottom)->setAttribute(QwtScaleEngine::Floating,true);
//    ui->waveFormPlot->setAxisAutoScale(QwtPlot::xBottom, false);
    ui->waveFormPlot->axisWidget(QwtPlot::xBottom)->setMinBorderDist(0, 0);
    ui->waveFormPlot->setAxisScaleDraw( QwtPlot::xBottom, new TimeScaleDraw(t));

    // Init the ploting object : no auto-replot
    ui->waveFormPlot->setAutoReplot(false);

    // Add an event filter on the waveform plot object
    ui->waveFormPlot->installEventFilter(this);

    // Create and init the position marker
    QPen pen(Qt::darkCyan, 1);
    mpPositionMarker = new QwtPlotMarker("PositionMarker");
    mpPositionMarker->setLineStyle(QwtPlotMarker::VLine);
    mpPositionMarker->setLinePen(pen);

    // Create text label to display "Loading..."
    mpLoadingTextItem = new QwtPlotTextLabel();

    // Init SIGNAL / SLOT connections
    connect(mpDecoder, SIGNAL(bufferReady()), this, SLOT(readBuffer()));
    connect(mpDecoder, SIGNAL(finished()), this, SLOT(decodingFinished()));
}

MyWaveForm::~MyWaveForm()
{
    delete ui;
}

void MyWaveForm::openFile(QString waveform_file_name, QString video_file_name) {

    mpFile->setFileName(waveform_file_name);

    // Erase previous waveform
    mpWaveFormCurve->detach();
    mpPositionMarker->detach();
    mpLoadingTextItem->detach();

    // If a ".wf" file doesn't exist, decode and save audios data
    if ( !mpFile->exists() ) {

        QAudioFormat desired_format;
        desired_format.setChannelCount(1);
        desired_format.setCodec("audio/pcm");
        desired_format.setSampleType(QAudioFormat::SignedInt);
        desired_format.setSampleRate(SAMPLE_PER_SEC);
        desired_format.setSampleSize(SAMPLE_SIZE_BYTES * BYTE_IN_BITS);
        desired_format.setByteOrder(QAudioFormat::LittleEndian);

        mpDecoder->setAudioFormat(desired_format);
        mpDecoder->setSourceFilename(video_file_name);

        if ( !mpFile->open(QIODevice::WriteOnly) ) {
            // Error msg
        }

        mpDecoder->start();

        // Display "Loading..." while decoding audio data
        QwtText title( "Loading... ");
        title.setRenderFlags( Qt::AlignHCenter | Qt::AlignVCenter );

        QFont font;
        font.setBold( true );
        title.setFont( font );
        mpLoadingTextItem->setText( title );
        mpLoadingTextItem->attach(ui->waveFormPlot);

        ui->waveFormPlot->replot();
    }
    else { // If ".wf" exit, retrive audios data from it directly
        initWaveForm();
    }
}

// readBuffer is called when a decoder audio-buffer is ready
// Write the audio-buffer data in ".wf" file
void MyWaveForm::readBuffer() {

    QAudioBuffer audio_buffer;
    audio_buffer = mpDecoder->read();
//    qint64 duration = audio_buffer.duration();
//    bool valid_buffer = audio_buffer.isValid();

    if (audio_buffer.isValid()) {

        int buffer_size = audio_buffer.byteCount();
        const qint16* data = audio_buffer.constData<qint16>();

        for (int i=0; i < buffer_size / SAMPLE_SIZE_BYTES; i++) {
            *mpStream << data[i];
        }
    }
}

void MyWaveForm::decodingFinished() {

     mpDecoder->stop();
     mpFile->close();
     initWaveForm();
}

void MyWaveForm::initWaveForm() {

    qint32 size_of_file;
    qreal time_accuracy_ms;

    if (!mpFile->open(QIODevice::ReadOnly)) {
        // Error msg
    }

    size_of_file = mpFile->size();
    time_accuracy_ms = (qreal)SEC_TO_MSEC / (qreal)SAMPLE_PER_SEC;
    mAmplitudeVector.resize(size_of_file / SAMPLE_SIZE_BYTES);
    mTimeVectorMs.resize(size_of_file / SAMPLE_SIZE_BYTES);

    for (qint32 i = 0; i < mTimeVectorMs.size(); ++i) {
        qint16 buffer;
        *mpStream >> buffer;
        mAmplitudeVector[i] = (double)buffer;
        mTimeVectorMs[i] = time_accuracy_ms * i;
    }

    mpFile->close();

    mMinPlotTimeMs = 0;
    mMaxPlotTimeMs = (qint32)( (qreal)MAX_TIME_SCALE_MS * ( (qreal)SAMPLE_PER_SEC / (qreal)SEC_TO_MSEC) );
    if ( mMaxPlotTimeMs > (qint32)( mTimeVectorMs[mTimeVectorMs.size() - 1] ) ) {
        mMaxPlotTimeMs = (qint32)( mTimeVectorMs[mTimeVectorMs.size() - 1] );
    }

//    mpWaveFormCurve->setSamples(mTimeVectorMs, mAmplitudeVector);
//    mpWaveFormCurve->attach(ui->waveFormPlot);
//    ui->waveFormPlot->replot();

    mpWaveFormCurve->attach(ui->waveFormPlot);
    mpPositionMarker->attach(ui->waveFormPlot);
    mpLoadingTextItem->detach();
    plotWaveForm();

}

void MyWaveForm::plotWaveForm() {

    qint32 vector_size;
    QVector<double> amplitude_vector;
    QVector<double> time_vector;

    vector_size = (qint32)( (qreal)(mMaxPlotTimeMs - mMinPlotTimeMs) * ( (qreal)SAMPLE_PER_SEC / (qreal)SEC_TO_MSEC ) ) + 1;

    amplitude_vector.resize(vector_size);
    time_vector.resize(vector_size);

    for ( qint32 i = mMinPlotTimeMs; i <= mMaxPlotTimeMs; i++ ) {
        amplitude_vector[i - mMinPlotTimeMs] = mAmplitudeVector[i];
        time_vector[i - mMinPlotTimeMs] = mTimeVectorMs[i];
    }

    mpWaveFormCurve->setSamples(time_vector, amplitude_vector);
//    ui->waveFormPlot->setAxisScale(QwtPlot::xBottom, (double)mMinPlotTimeMs, (double)mMaxPlotTimeMs, 0);
    ui->waveFormPlot->replot();
}

bool MyWaveForm::eventFilter(QObject* watched, QEvent* event) {

    if ( watched == ui->waveFormPlot ) {
        if ( event->type() == QEvent::Wheel ) {

            QWheelEvent* wheel_event = static_cast<QWheelEvent*>(event);

            Qt::KeyboardModifiers event_keybord_modifier = wheel_event->modifiers();

            if ( event_keybord_modifier & ( Qt::ControlModifier | Qt::ShiftModifier ) ) {

                QPoint num_degrees = wheel_event->angleDelta() / 8;

                if( !num_degrees.isNull() ) {
                    QPoint numSteps = num_degrees / 15;
                    if ( event_keybord_modifier == Qt::ControlModifier ) {
                        if ( !computeZoom(numSteps.y(), wheel_event->x()) ) {
                            return true;
                        }
                    }
                    else if ( event_keybord_modifier == Qt::ShiftModifier ) {
                        if ( !timeScaleShift(numSteps.y()) ) {
                            return true;
                        }
                    }
                    plotWaveForm();
                }
            }
            return true;
        }
        else if( event->type() == QEvent::MouseButtonPress ) {

            QMouseEvent* mouse_event = static_cast<QMouseEvent*>(event);

            setPosition(mouse_event->x());
            return true;
        }
    }
    return QWidget::eventFilter(watched, event);
}

bool MyWaveForm::computeZoom(qint16 step, qint16 xPos) {

    quint32 scale_size_ms;
    quint32 new_scale_size_ms;
    quint32 x_pos_ms;
    int start_scale_dist_from_border, end_scale_dist_from_border;
    int plot_width_px;

    scale_size_ms = mMaxPlotTimeMs - mMinPlotTimeMs;

    if ( step > 0 ) {
        if ( ( scale_size_ms == (quint32)( ( qreal)MAX_TIME_SCALE_MS * ( (qreal)SAMPLE_PER_SEC / (qreal)SEC_TO_MSEC ) ) ) ||
             (scale_size_ms == (quint32)( mTimeVectorMs[mTimeVectorMs.size() - 1] ) ) ) {
            return false;
        }
        new_scale_size_ms = scale_size_ms * (step * ZOOM_FACTOR);
    }
    else if ( step < 0 ) {
        if ( scale_size_ms == (quint32)( (qreal)MIN_TIME_SCALE_MS * ( (qreal)SAMPLE_PER_SEC / (qreal)SEC_TO_MSEC ) ) ) {
            return false;
        }
        new_scale_size_ms = (quint32)( (qreal)scale_size_ms / (qreal)( qAbs(step) * ZOOM_FACTOR) );
    }

    ui->waveFormPlot->axisWidget(QwtPlot::xBottom)->getBorderDistHint(start_scale_dist_from_border, end_scale_dist_from_border);

    xPos = xPos - start_scale_dist_from_border - ui->waveFormPlot->axisWidget(QwtPlot::xBottom)->x();
    if ( xPos < 0 ) {
        xPos = 0;
    }

    plot_width_px = ui->waveFormPlot->axisWidget(QwtPlot::xBottom)->size().width() - start_scale_dist_from_border - end_scale_dist_from_border;

    x_pos_ms = mMinPlotTimeMs + (qint32)( ( (qreal)scale_size_ms / (qreal)plot_width_px ) * (qreal)xPos );

    new_scale_size_ms = qBound((quint32)( (qreal)MIN_TIME_SCALE_MS * ( (qreal)SAMPLE_PER_SEC / (qreal)SEC_TO_MSEC ) ),
                               new_scale_size_ms,
                               (quint32)( (qreal)MAX_TIME_SCALE_MS * ( (qreal)SAMPLE_PER_SEC / (qreal)SEC_TO_MSEC ) ) );

    mMinPlotTimeMs = x_pos_ms - (qint32)( (qreal)( x_pos_ms - mMinPlotTimeMs ) * ( (qreal)new_scale_size_ms / (qreal)scale_size_ms ) );
    if ( mMinPlotTimeMs < 0 ) {
        mMinPlotTimeMs = 0;
    }
    mMaxPlotTimeMs = mMinPlotTimeMs + new_scale_size_ms;
    if ( mMaxPlotTimeMs >  (qint32)( mTimeVectorMs[mTimeVectorMs.size() - 1] ) ) {
        mMaxPlotTimeMs = (qint32)mTimeVectorMs[mTimeVectorMs.size() - 1];
    }
    return true;
}

bool MyWaveForm::timeScaleShift(qint16 step) {

    quint32 scale_size_ms;
    qint32 time_shift_ms;

    scale_size_ms = mMaxPlotTimeMs - mMinPlotTimeMs;
    time_shift_ms = (qint32)( (qreal)scale_size_ms * (qreal)TIME_SHIFT_FACTOR * (qreal)( qAbs(step) ) );

    if ( (step > 0) && ( mMinPlotTimeMs != 0 ) ) {

        mMinPlotTimeMs -= time_shift_ms;

        if ( mMinPlotTimeMs < 0 ) {
            mMinPlotTimeMs = 0;
        }

        mMaxPlotTimeMs = mMinPlotTimeMs + scale_size_ms;
    }
    else if ( (step < 0) &&
              (mMaxPlotTimeMs != (qint32)( mTimeVectorMs[mTimeVectorMs.size() - 1] ) ) ) {

        mMaxPlotTimeMs += time_shift_ms;

        if( mMaxPlotTimeMs > (qint32)( mTimeVectorMs[mTimeVectorMs.size() - 1] ) ) {
            mMaxPlotTimeMs = (qint32)mTimeVectorMs[mTimeVectorMs.size() - 1];
        }

        mMinPlotTimeMs = mMaxPlotTimeMs - scale_size_ms;
    }
    else {
        return false;
    }
    return true;
}

void MyWaveForm::setPosition(int xPos) {

    qint32 scale_size_ms;
    qint32 x_pos_ms;
    qint32 position_ms;
    int start_scale_dist_from_border, end_scale_dist_from_border;
    int plot_width_px;

    ui->waveFormPlot->axisWidget(QwtPlot::xBottom)->getBorderDistHint(start_scale_dist_from_border, end_scale_dist_from_border);

    xPos = xPos - start_scale_dist_from_border - ui->waveFormPlot->axisWidget(QwtPlot::xBottom)->x();
    if ( xPos < 0 ) {
        xPos = 0;
    }
    plot_width_px = ui->waveFormPlot->axisWidget(QwtPlot::xBottom)->size().width() - start_scale_dist_from_border - end_scale_dist_from_border;
    //int plot_width = ui->waveFormPlot->canvas()->size().width();
    scale_size_ms = mMaxPlotTimeMs - mMinPlotTimeMs;
    x_pos_ms = mMinPlotTimeMs + (qint32)( ( (qreal)scale_size_ms / (qreal)plot_width_px ) * (qreal)xPos );

    position_ms = (qint32)mTimeVectorMs[x_pos_ms];

    emit positionChanged(position_ms);
}

void MyWaveForm::updatePostionMarker(qint64 playerPositionMs) {

    qint32 scale_size_ms;

    if ( playerPositionMs > (qint32)( mTimeVectorMs[mTimeVectorMs.size() - 1] ) ) {
        return;
    }

    if ( ( playerPositionMs > mMaxPlotTimeMs ) || ( playerPositionMs < mMinPlotTimeMs ) ) {

        scale_size_ms = mMaxPlotTimeMs - mMinPlotTimeMs;
        mMinPlotTimeMs =  (qint32)( (qreal)playerPositionMs * ( (qreal)SAMPLE_PER_SEC / (qreal)SEC_TO_MSEC ) );
        mMaxPlotTimeMs = mMinPlotTimeMs + scale_size_ms;

        if ( mMaxPlotTimeMs > (qint32)( mTimeVectorMs[mTimeVectorMs.size() - 1] ) ) {
            mMaxPlotTimeMs = (qint32)mTimeVectorMs[mTimeVectorMs.size() - 1];
            mMinPlotTimeMs = mMaxPlotTimeMs - scale_size_ms;
        }

        plotWaveForm();
    }
    mpPositionMarker->setXValue((double)playerPositionMs);
    ui->waveFormPlot->replot();
}
