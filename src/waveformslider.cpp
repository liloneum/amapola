#include "waveformslider.h"
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
#include <qwt_curve_fitter.h>

// Number of audio samples displayed.
// If there are too much samples, the navigation may be not fluid
// If there are not enought samples, the wave displayed will be not precise
// 60000 samples per second is fine for Amapola application
#define MAX_SAMPLES 60000
// Sample size in bytes (here it's 16 bits -> 2 Bytes)
#define SAMPLE_SIZE_BYTES 2

MyWaveFormSlider::MyWaveFormSlider(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyWaveForm)
{
    ui->setupUi(this);

    // Create file, decoder and stream
    mpFile = new QFile(this);
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
    mpWaveFormCurve->setPaintAttribute( QwtPlotCurve::FilterPoints, true );

    // Init the ploting object - Y axis : hide, auto-scale off, set min/max (signed 16 bits range)
    ui->waveFormPlot->enableAxis(QwtPlot::yLeft,false);
    ui->waveFormPlot->setAxisAutoScale(QwtPlot::yLeft, false);
    ui->waveFormPlot->setAxisScale(QwtPlot::yLeft, -32767.0, 32767, 0);

    // Init the ploting object - X axis : auto-scale on, remove the end gap, label draw in hh:mm:ss
    ui->waveFormPlot->axisScaleEngine(QwtPlot::xBottom)->setAttribute(QwtScaleEngine::Floating,true);
    ui->waveFormPlot->axisWidget(QwtPlot::xBottom)->setMinBorderDist(0, 0);
    ui->waveFormPlot->enableAxis(QwtPlot::xBottom,false);

    // Init the ploting object : no auto-replot
    ui->waveFormPlot->setAutoReplot(false);

    // Add an event filter on the waveform plot object
    ui->waveFormPlot->canvas()->setMouseTracking(true);
    ui->waveFormPlot->installEventFilter(this);

    // Set cursor "arrow"
    ui->waveFormPlot->canvas()->setCursor(Qt::ArrowCursor);

    // Background transparent
    ui->waveFormPlot->canvas()->setStyleSheet("background: transparent");
//    ui->waveFormPlot->setFrameShape(QFrame::StyledPanel);
//    ui->waveFormPlot->setFrameShadow(QFrame::Plain);
//    ui->waveFormPlot->setLineWidth(0);
//    ui->waveFormPlot->setMidLineWidth(0);

    // Create and init the real time position marker
    QPen pen2(Qt::red, 1);
    mpRTMarker = new QwtPlotMarker("RTMarker");
    mpRTMarker->setLineStyle(QwtPlotMarker::VLine);
    mpRTMarker->setLinePen(pen2);

    // Create and init the real time display widget
    mpRtTime = new QTimeEdit(this->parentWidget());
    mpRtTime->setDisplayFormat("HH:mm");
    mpRtTime->setButtonSymbols(QAbstractSpinBox::NoButtons);
    QFont rt_time_font = mpRtTime->font();
    rt_time_font.setPointSize(8);
    mpRtTime->setFont(rt_time_font);
    mpRtTime->setFixedWidth(37);
    mpRtTime->setFixedHeight(10);
    mpRtTime->setReadOnly(true);
    mpRtTime->setFrame(false);
    mpRtTime->setStyleSheet("background: transparent");
    mpRtTime->hide();

    // Create and init the slider
    bool ok;
    QString slider_color_str = "FFF07746";
    QColor slider_color_zone(slider_color_str.toUInt(&ok,16));
    QColor slider_color_border = slider_color_zone;
    slider_color_zone.setAlpha(100);

    mpPosSlider = new QwtPlotZoneItem;
    mpPosSlider->setOrientation(Qt::Vertical);
    mpPosSlider->setInterval(0, 0);
    mpPosSlider->setPen(QPen(slider_color_border));
    mpPosSlider->setBrush(QBrush(slider_color_zone));
}

MyWaveFormSlider::~MyWaveFormSlider()
{
    delete ui;
}


void MyWaveFormSlider::openFile(QString waveform_file_name, qint64 mediaDurationMs) {

    mpFile->setFileName("");

    // Erase previous waveform
    mpWaveFormCurve->detach();
    mpPosSlider->detach();

    if ( waveform_file_name.isEmpty() ) {
        initWaveForm(mediaDurationMs);
        return;
    }

    mpFile->setFileName(waveform_file_name);

    initWaveForm(mediaDurationMs);
}

void MyWaveFormSlider::initWaveForm(qint64 durationMs) {

    qint32 size_of_file;

    // If no waveform data available
    if (!mpFile->open(QIODevice::ReadOnly)) {

        // If a media duration is not set, no media loaded
        if ( durationMs <= 0 ) {
            // Disable the slider
            this->setEnabled(false);
            return;
        } // A video is loaded, but data not ready yet
        else {

            this->setEnabled(true);

            // No wave form to load. Genarate an empty waveform
            mAmplitudeVector.resize(2);
            mTimeVectorMs.resize(2);

            mAmplitudeVector.fill(0);

            mTimeVectorMs[0] = 0;
            mTimeVectorMs[1] = durationMs;
        }
    } // Data ready, genrate the waveform
    else {

        qint32 highest_amplitude = 0;

        this->setEnabled(true);

        size_of_file = mpFile->size();

        // To improve performance, 60000 samples max
        qint32 samples_nbr;

        if ( durationMs > MAX_SAMPLES ) {
            samples_nbr = MAX_SAMPLES;
        } // If tha duration is inferior to 60000ms, we can work with 1 samples per millisecond
        else {
            samples_nbr = durationMs;
        }

        mAmplitudeVector.resize( samples_nbr );
        mTimeVectorMs.resize( samples_nbr );

        // Calculate the interval in millsecond between 2 samples
        qreal step_size = (qreal)durationMs / (qreal)samples_nbr;

        qint32 j = 0;
        // Copy the data from ".wf" file to a vector
        for ( qint32 i = 0; i < durationMs; i++ ) {

            qint16 buffer;
            *mpStream >> buffer;

            if ( i == qRound( (qreal)j * step_size) ) {

                if ( i < (size_of_file / SAMPLE_SIZE_BYTES) ) {

                    mAmplitudeVector[j] = (double)buffer;

                    if ( qAbs(buffer) > highest_amplitude ) {
                        highest_amplitude = qAbs(buffer);
                    }
                }
                else {
                    mAmplitudeVector[j] = 0;
                }
                mTimeVectorMs[j] = i;
                j++;
            }
        }

        mpFile->close();

        ui->waveFormPlot->setAxisScale(QwtPlot::yLeft, -(double)highest_amplitude, (double)highest_amplitude, 0);
    }

    mMediaDurationMs = (qint32)mTimeVectorMs.last();

    // Set scope of wave-form to the maximum defined, with origin at 0
    mMinPlotTimeMs = 0;
    mMaxPlotTimeMs = mMediaDurationMs;

    ui->waveFormPlot->setAxisScale(QwtPlot::xBottom, mMinPlotTimeMs, mMaxPlotTimeMs, 1);

    // Plot the wave-form
    mpWaveFormCurve->setSamples(mTimeVectorMs, mAmplitudeVector);
    mpWaveFormCurve->attach(ui->waveFormPlot);
    mpPosSlider->attach(ui->waveFormPlot);
    ui->waveFormPlot->replot();

}

bool MyWaveFormSlider::eventFilter(QObject* watched, QEvent* event) {

    // Manage mouse click and wheel event for the wave-form plot object
    if ( watched == ui->waveFormPlot ) {

        if ( this->isEnabled() ) {

            if( event->type() == QEvent::MouseButtonPress ) {

                // Set the marker position to position clicked
                QMouseEvent* mouse_event = static_cast<QMouseEvent*>(event);

                // Move the marker to the mouse position
                this->setSliderPosFromClick(mouse_event->x());
                return true;
            }
            else if( event->type() == QEvent::MouseMove ) {

                // Set real time marker position under mouse
                QMouseEvent* mouse_event = static_cast<QMouseEvent*>(event);
                this->setRtMarkerPos(mouse_event->x());
            }
            else if ( event->type() == QEvent::Enter ) {
                mpRTMarker->attach(ui->waveFormPlot);
                mpRtTime->show();
            }
            else if ( event->type() == QEvent::Leave ) {
                mpRTMarker->detach();
                ui->waveFormPlot->replot();
                mpRtTime->hide();
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

// Draw a marker under the mouse in real time.
// With time "hh:mm"
void MyWaveFormSlider::setRtMarkerPos(int xPos) {

    QTime time_base(0, 0, 0, 0);

    qint64 position_ms = this->posMsFromPosPx(xPos);

    mpRTMarker->setXValue((double)position_ms);
    ui->waveFormPlot->replot();

    mpRtTime->setTime( time_base.addMSecs(position_ms) );
    mpRtTime->move( this->x() + ui->waveFormPlot->x() + xPos - ( mpRtTime->width() / 2 ), this->y() + ui->waveFormPlot->y() - 10);
}

// Retrieve the position in millisecond from the mouse horizontal position (X coordonates)
qint64 MyWaveFormSlider::posMsFromPosPx(int xPos) {

    qint32 scale_size_ms;
    qint32 position_ms;
    int start_scale_dist_from_border, end_scale_dist_from_border;
    int plot_width_px;

    ui->waveFormPlot->axisWidget(QwtPlot::xBottom)->getBorderDistHint(start_scale_dist_from_border, end_scale_dist_from_border);

    // Calculate the mouse position relative to the horizontale scale
    xPos = xPos - ui->waveFormPlot->canvas()->x() - start_scale_dist_from_border - 3;

    if ( xPos < 0 ) {
        xPos = 0;
    }
    // Calculate plot surface width in pixels
    plot_width_px = ui->waveFormPlot->canvas()->width() - start_scale_dist_from_border - 9;

    // Current scope size in millisecond
    scale_size_ms = mMaxPlotTimeMs - mMinPlotTimeMs;

    // Convert mouse position in pixel to a position in millisecond
    position_ms = mMinPlotTimeMs + (qint32)( ( (qreal)scale_size_ms / (qreal)plot_width_px ) * (qreal)xPos );

    // Scale the positon in function of the framerate
    return MyAttributesConverter::roundToFrame(position_ms, qApp->property("prop_FrameRate_fps").toReal());
}

// Compute new position in fonction of mouse click position
void MyWaveFormSlider::setSliderPosFromClick(int xPos) {

    mCurrentPositonMs = this->posMsFromPosPx(xPos);

    // Replot the slider
    mpPosSlider->setInterval(0, mCurrentPositonMs);
    ui->waveFormPlot->replot();

    emit sliderPositionChanged(mCurrentPositonMs);
}

// Draw the slider in function of the player current position
void MyWaveFormSlider::updatePostionSlider(qint64 positionMs) {

    positionMs = MyAttributesConverter::roundToFrame(positionMs, qApp->property("prop_FrameRate_fps").toReal());

    if ( positionMs > mMediaDurationMs ) {
        return;
    }

    mCurrentPositonMs = positionMs;

    // Replot the Slider
    mpPosSlider->setInterval(0, positionMs);
    ui->waveFormPlot->replot();
}

qint64 MyWaveFormSlider::currentPositonMs() {

    return mCurrentPositonMs;
}
