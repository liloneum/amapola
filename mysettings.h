#ifndef MYSETTINGS_H
#define MYSETTINGS_H

#include <QWidget>

namespace Ui {
class MySettings;
}

class MyProperties {

public:
    MyProperties();
    void setFrameRate(qreal frameRate);
    qreal frameRate();
private:
    qreal mFrameRate;
};

class MySettings : public QWidget
{
    Q_OBJECT
    
public:
    explicit MySettings(QWidget *parent = 0);
    ~MySettings();
    
signals:
    void frameRateChanged(qreal);

private slots:
    void on_displayTimeMinSpinBox_valueChanged(int value);
    void on_intervalMinSpinBox_valueChanged(int value);
    void on_frameRateComboBox_currentIndexChanged(int index);
    void on_frameRateSpinBox_editingFinished();

public slots:
    void setFrameRate(qreal frameRate);
    MyProperties getCurrentProp();

private:
    Ui::MySettings *ui;
    MyProperties mCurrentProperties;
    bool mSettingsChangedBySoft;
};

#endif // MYSETTINGS_H
