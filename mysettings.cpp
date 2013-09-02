#include "mysettings.h"
#include "ui_mysettings.h"

MySettings::MySettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MySettings)
{
    ui->setupUi(this);
}

MySettings::~MySettings()
{
    delete ui;
}
