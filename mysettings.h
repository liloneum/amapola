#ifndef MYSETTINGS_H
#define MYSETTINGS_H

#include <QWidget>

namespace Ui {
class MySettings;
}

class MySettings : public QWidget
{
    Q_OBJECT
    
public:
    explicit MySettings(QWidget *parent = 0);
    ~MySettings();
    
private slots:
    void on_displayTimeMinSpinBox_valueChanged(int value);

    void on_intervalMinSpinBox_valueChanged(int value);


    void on_frameRateComboBox_currentIndexChanged(int index);

    void on_frameRateSpinBox_valueChanged(double value);

private:
    Ui::MySettings *ui;
};

#endif // MYSETTINGS_H
