#include "waveform.h"
#include "ui_waveform.h"
#include <Qt>
#include <QAudioFormat>
#include <QAudioDecoder>
#include <QFile>
#include <QWheelEvent>
#include <QAudioDeviceInfo>
#include <QTime>
#include <subtitles.h>
#include <attributesconverter.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_engine.h>
#include <qwt_plot_marker.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_textlabel.h>
#include <qwt_plot_zoneitem.h>
#include <qwt_plot_grid.h>
#include <QTimeEdit>
#include <QLabel>

// Number of audio samples per second displayed.
// If there are too much samples, the navigation may be not fluid
// If there are not enought samples, the wave displayed will be not precise
// 1000 samples per second is fine for Amapola application
#define SAMPLES_PER_SEC 1000
// Sample size in bytes (here it's 16 bits -> 2 Bytes)
#define SAMPLE_SIZE_BYTES 2
// Number of bits in one bytes
#define BYTE_IN_BITS 8
// Convert second in milliseconds
#define SEC_TO_MSEC 1000
// The zoom factor. Mean that one step of zoom in (zoom-out) will show 2 time less (more) of samples
#define ZOOM_FACTOR 2
// Time shift factor. Shift the waveform of 0.1 * time displayed
#define TIME_SHIFT_FACTOR 0.1
// Minimum time displayed
#define MIN_TIME_SCALE_MS 5000
// Maximum time displayed
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

// This widget manage an audio waveform displaying, with zoom and shift.
// Display a position marker at the input time,
// allow media control by sending output time of the position clicked.
WaveForm::WaveForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WaveForm)
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

    // Init the current position
    mCurrentPositonMs = 0;

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
    ui->waveFormPlot->canvas()->setMouseTracking(true);
    ui->waveFormPlot->installEventFilter(this);
    ui->waveFormPlot->axisWidget(QwtPlot::xBottom)->installEventFilter(this);

    // Draw a sexy frmae
    ui->waveFormPlot->canvas()->setStyleSheet("background: transparent");
    ui->waveFormPlot->setFrameShape(QFrame::StyledPanel);
    ui->waveFormPlot->setFrameShadow(QFrame::Sunken);
    ui->waveFormPlot->setLineWidth(0);
    ui->waveFormPlot->setMidLineWidth(0);

    // Create and init the position marker
    QPen pen(Qt::darkCyan, 1);
    mpPositionMarker = new QwtPlotMarker("PositionMarker");
    mpPositionMarker->setLineStyle(QwtPlotMarker::VLine);
    mpPositionMarker->setLinePen(pen);

    // Create and init the real time position marker
    QPen pen2(Qt::darkCyan, 1, Qt::DashDotLine);
    mpRTMarker = new QwtPlotMarker("RTMarker");
    mpRTMarker->setLineStyle(QwtPlotMarker::VLine);
    mpRTMarker->setLinePen(pen2);

    // Create and init the real time display widget
    mpRtTime = new QTimeEdit(ui->waveFormPlot);
    mpRtTime->setDisplayFormat("HH:mm:ss.zzz");
    mpRtTime->setButtonSymbols(QAbstractSpinBox::NoButtons);
    mpRtTime->setReadOnly(true);
    mpRtTime->setFrame(false);
    mpRtTime->setStyleSheet("background: transparent");
    mpRtTime->hide();

    mpRtFrames = new QLabel(ui->waveFormPlot);
    mpRtFrames->setStyleSheet("background: transparent");
    mpRtFrames->setFixedWidth(250);
    mpRtFrames->hide();

    // Create and init the grid
    mpGrid = new QwtPlotGrid();
    mpGrid->enableY(false);
    mpGrid->enableXMin(true);
    mpGrid->setPen(QColor(Qt::gray), 0.0, Qt::DotLine);
    mpGrid->setMinorPen(QColor(Qt::lightGray), 0.0, Qt::DotLine);

    mpZoneItemList.clear();

    // Create text label to display "Loading..."
    mpLoadingTextItem = new QwtPlotTextLabel();

    // Init SIGNAL / SLOT connections
    connect(mpDecoder, SIGNAL(bufferReady()), this, SLOT(readBuffer()));
    connect(mpDecoder, SIGNAL(finished()), this, SLOT(decodingFinished()));
}

WaveForm::~WaveForm()
{
    delete ui;
}

void WaveForm::openFile(QString waveform_file_name, QString video_file_name) {

    if ( video_file_name.isEmpty() ) {
        mpFile->setFileName("");
        initWaveForm();
        return;
    }

    mpFile->setFileName(waveform_file_name);

    // Erase previous waveform
    mpWaveFormCurve->detach();
    mpPositionMarker->detach();
    mpRTMarker->detach();
    mpLoadingTextItem->detach();

    // If a ".awf" file doesn't exist, decode and save audio data
    if ( !mpFile->exists() ) {

        QAudioFormat desired_format;
        desired_format.setChannelCount(1);
        desired_format.setCodec("audio/pcm");
        desired_format.setSampleType(QAudioFormat::SignedInt);
        desired_format.setSampleRate(SAMPLES_PER_SEC);
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
        font.setBold(true);
        title.setFont(font);
        mpLoadingTextItem->setText( title );
        mpLoadingTextItem->attach(ui->waveFormPlot);

        ui->waveFormPlot->replot();
    }
    else { // If ".awf" exit, retrive audios data from it directly
        initWaveForm();
    }
}

// readBuffer is called when a decoded audio-buffer is ready
// Write the audio-buffer data in ".awf" file
void WaveForm::readBuffer() {

    QAudioBuffer audio_buffer;
    audio_buffer = mpDecoder->read();

    if (audio_buffer.isValid()) {

        int buffer_size = audio_buffer.byteCount();
        const qint16* data = audio_buffer.constData<qint16>();

        for (int i=0; i < buffer_size / SAMPLE_SIZE_BYTES; i++) {
            *mpStream << data[i];
        }
    }
}

void WaveForm::decodingFinished() {

     mpDecoder->stop();
     mpFile->close();
     initWaveForm();
}

void WaveForm::initWaveForm() {

    qint32 size_of_file;
    qreal time_accuracy_ms;

    time_accuracy_ms = (qreal)SEC_TO_MSEC / (qreal)SAMPLES_PER_SEC;

    mAmplitudeVector.resize(72000000 * time_accuracy_ms);
    mTimeVectorMs.resize(72000000 * time_accuracy_ms);

    mAmplitudeVector.fill(0);

    if (!mpFile->open(QIODevice::ReadOnly)) {

        // No wave form to load. Genrate 4 hours of empty waveform
        for (qint32 i = 0; i < mTimeVectorMs.size(); i++) {
            mTimeVectorMs[i] = time_accuracy_ms * i;
        }

        mMinPlotTimeMs = 0;
        mMaxPlotTimeMs = (qint32)MAX_TIME_SCALE_MS;
    }
    else {

        qint32 highest_amplitude = 0;

        size_of_file = mpFile->size();

        // Copy the data from ".awf" file to a vector
        for (qint32 i = 0; i < (size_of_file / SAMPLE_SIZE_BYTES); ++i) {
            qint16 buffer;
            *mpStream >> buffer;
            mAmplitudeVector[i] = (double)buffer;

            if ( qAbs(buffer) > highest_amplitude ) {
                highest_amplitude = qAbs(buffer);
            }

            mTimeVectorMs[i] = time_accuracy_ms * (qreal)i;
            mMediaDurationMs = time_accuracy_ms * (qreal)i;
        }

        mpFile->close();

        ui->waveFormPlot->setAxisScale(QwtPlot::yLeft, -(double)highest_amplitude, (double)highest_amplitude, 0);

        emit waveFormFileReady(mpFile->fileName());

        // Set scope of wave-form to the maximum defined, with origin at 0
        mMinPlotTimeMs = 0;
        if ( mMediaDurationMs < MAX_TIME_SCALE_MS ) {
            mMaxPlotTimeMs = mMediaDurationMs;
        }
        else {
            mMaxPlotTimeMs = (qint32)MAX_TIME_SCALE_MS;
        }
    }

    mLastTimeMs = (qint32)mTimeVectorMs.last();

    // Plot the wave-form and the marker
    mpWaveFormCurve->attach(ui->waveFormPlot);
    mpPositionMarker->attach(ui->waveFormPlot);
    mpGrid->attach(ui->waveFormPlot);
    mpLoadingTextItem->detach();
    plotWaveForm();

}

// Plot the wave-form with in function of the current scope
void WaveForm::plotWaveForm() {

    qint32 vector_size;
    QVector<double> amplitude_vector;
    QVector<double> time_vector;

    vector_size = (qint32)( (qreal)(mMaxPlotTimeMs - mMinPlotTimeMs) * ( (qreal)SAMPLES_PER_SEC / (qreal)SEC_TO_MSEC ) ) + 1;

    amplitude_vector.resize(vector_size);
    time_vector.resize(vector_size);

    qint32 min_index = (qint32)( (qreal)mMinPlotTimeMs * ( (qreal)SAMPLES_PER_SEC / (qreal)SEC_TO_MSEC ) );
    qint32 max_index = (qint32)( (qreal)mMaxPlotTimeMs * ( (qreal)SAMPLES_PER_SEC / (qreal)SEC_TO_MSEC ) );

    for ( qint32 i = min_index; i <= max_index; i++ ) {
        amplitude_vector[i - min_index] = mAmplitudeVector[i];
        time_vector[i - min_index] = mTimeVectorMs[i];
    }

    mpWaveFormCurve->setSamples(time_vector, amplitude_vector);
//    ui->waveFormPlot->setAxisScale(QwtPlot::xBottom, (double)mMinPlotTimeMs, (double)mMaxPlotTimeMs, 0);
    ui->waveFormPlot->replot();
}

bool WaveForm::eventFilter(QObject* watched, QEvent* event) {

    // Manage mouse click and wheel event for the wave-form plot object
    if ( watched == ui->waveFormPlot ) {
        if ( event->type() == QEvent::Wheel ) {

            QWheelEvent* wheel_event = static_cast<QWheelEvent*>(event);

            Qt::KeyboardModifiers event_keybord_modifier = wheel_event->modifiers();

            // Ctrl key + Mouse wheel -> zoom in/out the scope
            // Mouse wheel -> shift the scope

            // See the Qt doc to convert the wheel angle delta to number of steps
            QPoint num_degrees = wheel_event->angleDelta() / 8;

            if( !num_degrees.isNull() ) {
                QPoint numSteps = num_degrees / 15;
                if ( event_keybord_modifier == Qt::ControlModifier ) {
                    if ( !computeZoom(numSteps.y(), wheel_event->x()) ) {
                        return true;
                    }
                }
                else if ( event_keybord_modifier == Qt::NoModifier ) {
                    if ( !timeScaleShift(numSteps.y()) ) {
                        return true;
                    }
                    this->setRtMarkerPos(wheel_event->x());
                }
                // Replot wave form if the scope was modified
                plotWaveForm();
            }
            return true;
        }
        else if( event->type() == QEvent::MouseButtonPress ) {

            // Set the marker position to position clicked
            QMouseEvent* mouse_event = static_cast<QMouseEvent*>(event);

            if ( mouse_event->modifiers() == Qt::ControlModifier ) {

                // Ctrl + left click
                if ( mouse_event->button() == Qt::LeftButton ) {
                    emit ctrlLeftClickEvent( this->posMsFromPosPx( mouse_event->x() ) );
                    return true;
                } // Ctrl + right click
                else if ( mouse_event->button() == Qt::RightButton ) {
                    emit ctrlRightClickEvent( this->posMsFromPosPx( mouse_event->x() ) );
                    return true;
                }
            } // Shift + left clik
            else if ( mouse_event->modifiers() == Qt::ShiftModifier ) {

                if ( mouse_event->button() == Qt::LeftButton ) {
                    emit shiftLeftClickEvent( this->posMsFromPosPx( mouse_event->x() ) );
                    return true;
                }
            } // Only click (right, mid or left)
            else {
                // Move the marker to the mouse position
                setMarkerPosFromClick(mouse_event->x());
                return true;
            }
        }
        else if( event->type() == QEvent::MouseMove ) {

            // Set real time marker position under mouse
            QMouseEvent* mouse_event = static_cast<QMouseEvent*>(event);
            this->setRtMarkerPos(mouse_event->x());
        }
        else if ( event->type() == QEvent::Enter ) {
            mpRTMarker->attach(ui->waveFormPlot);
            mpRtTime->show();
            mpRtFrames->show();
        }
        else if ( event->type() == QEvent::Leave ) {
            mpRTMarker->detach();
            ui->waveFormPlot->replot();
            mpRtTime->hide();
            mpRtFrames->hide();
        }
    }
    else if ( watched == ui->waveFormPlot->axisWidget(QwtPlot::xBottom) ) {
        ui->waveFormPlot->canvas()->setFixedHeight( ui->waveFormPlot->axisWidget(QwtPlot::xBottom)->y() + 2);
    }
    return QWidget::eventFilter(watched, event);
}

// Rescale the waveform in function of the zoom (in or out) and the mouse position
bool WaveForm::computeZoom(qint16 step, qint16 xPos) {

    quint32 scale_size_ms;
    quint32 new_scale_size_ms;
    quint32 x_pos_ms;
    int start_scale_dist_from_border, end_scale_dist_from_border;
    int plot_width_px;

    // Current scope size in millisecond
    scale_size_ms = mMaxPlotTimeMs - mMinPlotTimeMs;

    // If zoom in/out isn't ever at min/max possible, compute the new scope size in millisecond.
    if ( step > 0 ) { // Zoom out
        if ( ( scale_size_ms == (quint32)MAX_TIME_SCALE_MS )  ||
             (scale_size_ms == (quint32)mTimeVectorMs.last() ) ) {
            return false;
        }
        new_scale_size_ms = scale_size_ms * (step * ZOOM_FACTOR);
    }
    else if ( step < 0 ) { // Zoom in
        if ( scale_size_ms == (quint32)MIN_TIME_SCALE_MS ) {
            return false;
        }
        new_scale_size_ms = (quint32)( (qreal)scale_size_ms / (qreal)( qAbs(step) * ZOOM_FACTOR) );
    }

    // Bounded by the min/max(or video duration) defined
    new_scale_size_ms = qBound( (quint32)MIN_TIME_SCALE_MS, new_scale_size_ms, (quint32)MAX_TIME_SCALE_MS );

    ui->waveFormPlot->axisWidget(QwtPlot::xBottom)->getBorderDistHint(start_scale_dist_from_border, end_scale_dist_from_border);

    // Calculate the mouse position relative to the horizontale scale
    xPos = xPos - start_scale_dist_from_border - ui->waveFormPlot->axisWidget(QwtPlot::xBottom)->x();
    if ( xPos < 0 ) {
        xPos = 0;
    }

    // Calculate plot surface width in pixels
    plot_width_px = ui->waveFormPlot->axisWidget(QwtPlot::xBottom)->size().width() - start_scale_dist_from_border - end_scale_dist_from_border;

    // Convert mouse position in pixel to a position in millisecond
    x_pos_ms = mMinPlotTimeMs + (qint32)( ( (qreal)scale_size_ms / (qreal)plot_width_px ) * (qreal)xPos );

    // Compute lower and higher values of the scope
    mMinPlotTimeMs = x_pos_ms - (qint32)( (qreal)( x_pos_ms - mMinPlotTimeMs ) * ( (qreal)new_scale_size_ms / (qreal)scale_size_ms ) );
    if ( mMinPlotTimeMs < 0 ) {
        mMinPlotTimeMs = 0;
    }
    mMaxPlotTimeMs = mMinPlotTimeMs + new_scale_size_ms;
    if ( mMaxPlotTimeMs >  mLastTimeMs ) {
        mMaxPlotTimeMs = mLastTimeMs;
    }
    return true;
}

// Shift the waveform the number of step, left (step<0) or right(step>0)
bool WaveForm::timeScaleShift(qint16 step) {

    quint32 scale_size_ms;
    qint32 time_shift_ms;

    // Current scope size in millisecond
    scale_size_ms = mMaxPlotTimeMs - mMinPlotTimeMs;

    // Compute scope shift in millisecond
    time_shift_ms = (qint32)( (qreal)scale_size_ms * (qreal)TIME_SHIFT_FACTOR * (qreal)( qAbs(step) ) );

    // Right shift
    if ( (step > 0) && ( mMinPlotTimeMs != 0 ) ) {

        mMinPlotTimeMs -= time_shift_ms;

        if ( mMinPlotTimeMs < 0 ) {
            mMinPlotTimeMs = 0;
        }

        mMaxPlotTimeMs = mMinPlotTimeMs + scale_size_ms;
    } // Left shift
    else if ( (step < 0) &&
              (mMaxPlotTimeMs != mLastTimeMs ) ) {

        mMaxPlotTimeMs += time_shift_ms;

        if( mMaxPlotTimeMs > mLastTimeMs ) {
            mMaxPlotTimeMs = mLastTimeMs;
        }

        mMinPlotTimeMs = mMaxPlotTimeMs - scale_size_ms;
    }
    else {
        return false;
    }
    return true;
}

// Compute new position in fonction of mouse click position
void WaveForm::setMarkerPosFromClick(int xPos) {

    mCurrentPositonMs = this->posMsFromPosPx(xPos);

    // Replot the marker
    mpPositionMarker->setXValue((double)mCurrentPositonMs);
    ui->waveFormPlot->replot();

    emit markerPositionChanged(mCurrentPositonMs);
}

void WaveForm::setRtMarkerPos(int xPos) {

    QTime time_base(0, 0, 0, 0);

    qint64 position_ms = this->posMsFromPosPx(xPos);

    mpRTMarker->setXValue((double)position_ms);
    ui->waveFormPlot->replot();

    mpRtTime->setTime( time_base.addMSecs(position_ms) );
    mpRtTime->move(xPos, 10);

    qint32 frame_nbr = AttributesConverter::timeMsToFrames( position_ms, qApp->property("prop_FrameRate_fps").toReal() );

    mpRtFrames->setText(" Frame n° "+QString::number(frame_nbr));
    mpRtFrames->move(xPos, 25);
}

// Retrieve the position in millisecond from the mouse horizontal position (X coordonates)
qint64 WaveForm::posMsFromPosPx(int xPos) {

    qint32 scale_size_ms;
    qint32 position_ms;
    int start_scale_dist_from_border, end_scale_dist_from_border;
    int plot_width_px;
    ui->waveFormPlot->axisWidget(QwtPlot::xBottom)->getBorderDistHint(start_scale_dist_from_border, end_scale_dist_from_border);

    // Calculate the mouse position relative to the horizontale scale
    xPos = xPos - start_scale_dist_from_border - ui->waveFormPlot->axisWidget(QwtPlot::xBottom)->x();
    if ( xPos < 0 ) {
        xPos = 0;
    }
    // Calculate plot surface width in pixels
    plot_width_px = ui->waveFormPlot->axisWidget(QwtPlot::xBottom)->size().width() - start_scale_dist_from_border - end_scale_dist_from_border;
    // Current scope size in millisecond
    scale_size_ms = mMaxPlotTimeMs - mMinPlotTimeMs;
    // Convert mouse position in pixel to a position in millisecond
    position_ms = mMinPlotTimeMs + (qint32)( ( (qreal)scale_size_ms / (qreal)plot_width_px ) * (qreal)xPos );

    // Scale the positon in function of the framerate
    return AttributesConverter::roundToFrame(position_ms, qApp->property("prop_FrameRate_fps").toReal());
}

// Draw the marker in function of the player current position
void WaveForm::updatePostionMarker(qint64 positionMs) {

    qint32 scale_size_ms;

    positionMs = AttributesConverter::roundToFrame(positionMs, qApp->property("prop_FrameRate_fps").toReal());

    if ( positionMs > mLastTimeMs ) {
        return;
    }

    mCurrentPositonMs = positionMs;

    // Shift the scope when the marker arrive at end of viewed scope
    if ( ( positionMs > mMaxPlotTimeMs ) || ( positionMs < mMinPlotTimeMs ) ) {

        scale_size_ms = mMaxPlotTimeMs - mMinPlotTimeMs;
        mMinPlotTimeMs =  (qint32)positionMs;
        mMaxPlotTimeMs = mMinPlotTimeMs + scale_size_ms;

        // Limit the time shift at end of media
        if ( mMaxPlotTimeMs > mLastTimeMs ) {
            mMaxPlotTimeMs = mLastTimeMs;
            mMinPlotTimeMs = mMaxPlotTimeMs - scale_size_ms;
        }
        // Replot the wave-form
        plotWaveForm();
    }
    // Replot the marker
    mpPositionMarker->setXValue((double)positionMs);
    ui->waveFormPlot->replot();

    emit markerPositionChanged(positionMs);
}

qint64 WaveForm::currentPositonMs() {

    return mCurrentPositonMs;
}

// Draw one or more subtitles zones
void WaveForm::drawSubtitlesZone(QList<Subtitles> subtitlesList, qint32 subtitleIndex) {

    qint32 start_time_ms;
    qint32 end_time_ms;
    QTime time_base(0, 0, 0, 0);
    Subtitles subtitle;

    // Set the "non-active zone" color
    QString color_str = "FF96CA2D";
    bool ok;
    QColor color_zone(color_str.toUInt(&ok,16));
    QColor color_border = color_zone;
    color_border.setAlpha(50);

    QList<Subtitles>::iterator it;
    for ( it = subtitlesList.begin(); it != subtitlesList.end(); ++it ) {

        subtitle = *it;
        start_time_ms = qAbs( QTime::fromString(subtitle.startTime(), "hh:mm:ss.zzz").msecsTo(time_base) );
        end_time_ms = qAbs( QTime::fromString(subtitle.endTime(), "hh:mm:ss.zzz").msecsTo(time_base) );

        // Create zone item and draw it from "start time" to "end time"
        QwtPlotZoneItem* zone_item;
        zone_item = new QwtPlotZoneItem;
        zone_item->setOrientation(Qt::Vertical);
        zone_item->setInterval(start_time_ms, end_time_ms);
        zone_item->setPen(QPen(color_zone));
        zone_item->setBrush(QBrush(color_border));

        zone_item->attach(ui->waveFormPlot);

        // Save the item memory adresse in a list at the same as the subtitle index
        // Allow to retrieve the zone item corresponding to a subtitle to modify it later
        mpZoneItemList.insert(subtitleIndex, zone_item);
        subtitleIndex++;
    }

    ui->waveFormPlot->replot();
}

// Change the color of a given zone and reset all others zones colors
// to the "non-active zone" color
// The subtitle index is the same as its zone item
void WaveForm::changeZoneColor(QList<qint32> selectedIndex, qint32 currentIndex) {

    QString color_non_active_str = "FF96CA2D";
    QString color_active_str = "FFF07746";
    qint32 i = 0;

    // Set the "non-active zone" color
    bool ok;
    QColor color_non_active_border(color_non_active_str.toUInt(&ok,16));
    QColor color_non_active_zone = color_non_active_border;
    color_non_active_zone.setAlpha(50);

    // Set the "active zone" color to given zone index
    QColor color_active_border(color_active_str.toUInt(&ok,16));
    QColor color_active_zone = color_active_border;
    QColor color_active_zone2 = color_active_border;
    color_active_zone.setAlpha(20);
    color_active_zone2.setAlpha(60);

    QwtPlotZoneItem* zone_item;

    // Reset all zones colors to the "non-active zone" color
    QList<QwtPlotZoneItem*>::iterator it;
    for ( it = mpZoneItemList.begin(); it != mpZoneItemList.end(); ++it ) {

        zone_item = *it.operator->();

        if ( selectedIndex.contains(i) ) {
            if ( i == currentIndex ) {
                zone_item->setBrush(QBrush(color_active_zone2));
            }
            else {
                zone_item->setBrush(QBrush(color_active_zone));
            }
            zone_item->setPen(QPen(color_active_border));
        }
        else {
            zone_item->setPen(QPen(color_non_active_border));
            zone_item->setBrush(QBrush(color_non_active_zone));
        }

        i++;
    }

    ui->waveFormPlot->replot();
}

// Redraw zone item corresponding to the given index with a new start time
void WaveForm::changeZoneStartTime(qint32 subtitleIndex, qint64 startTimeMs) {

    if ( mpZoneItemList.size() > subtitleIndex ) {

        qint64 end_time_ms = mpZoneItemList.at(subtitleIndex)->interval().maxValue();
        mpZoneItemList.at(subtitleIndex)->setInterval(startTimeMs, end_time_ms);

        ui->waveFormPlot->replot();
    }
}

// Redraw zone item corresponding to the given index with a new end time
void WaveForm::changeZoneEndTime(qint32 subtitleIndex, qint64 endTimeMs) {

    if ( mpZoneItemList.size() > subtitleIndex ) {

        qint64 start_time_ms = mpZoneItemList.at(subtitleIndex)->interval().minValue();
        mpZoneItemList.at(subtitleIndex)->setInterval(start_time_ms, endTimeMs);

        ui->waveFormPlot->replot();
    }
}

// Remove zone item corresponding to the given index
void WaveForm::removeSubtitleZone(qint32 subtitleIndex) {

    if ( mpZoneItemList.size() > subtitleIndex ) {
        mpZoneItemList.at(subtitleIndex)->detach();
        mpZoneItemList.removeAt(subtitleIndex);

        ui->waveFormPlot->replot();
    }
}

// Remove all zones items
void WaveForm::removeAllSubtitlesZones() {

    if ( !mpZoneItemList.isEmpty() ) {

        QwtPlotZoneItem* zone_item;

        QList<QwtPlotZoneItem*>::iterator it;
        for ( it = mpZoneItemList.begin(); it != mpZoneItemList.end(); ++it ) {
            zone_item = *it.operator->();
            zone_item->detach();
        }

        mpZoneItemList.clear();

        ui->waveFormPlot->replot();
    }
}

QwtPlot* WaveForm::qwtPlot() {

    return ui ->waveFormPlot;
}
