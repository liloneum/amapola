#include "mysettings.h"
#include "ui_mysettings.h"

MySettings::MySettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MySettings)
{
    ui->setupUi(this);

    // Use QApplication properties to save the settings.
    // This properties are accessible from all components (video-player, waveform, subtable ...)
    qApp->setProperty("prop_SubMinInterval_frame", ui->intervalMinSpinBox->value());
    qApp->setProperty("prop_SubMinDuration_ms", ui->displayTimeMinSpinBox->value());
    qApp->setProperty("prop_FrameRate_fps", ui->frameRateComboBox->currentText().toDouble());
}

MySettings::~MySettings()
{
    delete ui;
}

void MySettings::on_displayTimeMinSpinBox_valueChanged(int value) {

    qApp->setProperty("prop_SubMinDuration_ms", value);
}

void MySettings::on_intervalMinSpinBox_valueChanged(int value) {

    qApp->setProperty("prop_SubMinInterval_frame", value);
}


void MySettings::on_frameRateComboBox_currentIndexChanged(int index) {

    qreal frame_rate;

    ui->frameRateSpinBox->setEnabled(false);

    switch (index) {
    case 0:
        frame_rate = 23.976;
        break;
    case 1:
        frame_rate = 24;
        break;
    case 2:
        frame_rate = 25;
        break;
    case 3:
        frame_rate = 29.97;
        break;
    case 4:
        frame_rate = 30;
        break;
    case 5:
        ui->frameRateSpinBox->setEnabled(true);
        frame_rate = ui->frameRateSpinBox->value();
        break;
    default:
        frame_rate = 25;
        break;
    }

    qApp->setProperty("prop_FrameRate_fps", frame_rate);
}

void MySettings::on_frameRateSpinBox_valueChanged(double value) {

    qApp->setProperty("prop_FrameRate_fps", value);

}
