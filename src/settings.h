#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>

namespace Ui {
class Settings;
}

class Properties {

public:
    Properties();
    void setFrameRate(qreal frameRate);
    qreal frameRate();
private:
    qreal mFrameRate;
};

class Settings : public QWidget
{
    Q_OBJECT
    
public:
    explicit Settings(QWidget *parent = 0);
    ~Settings();
    
signals:
    void marginChanged();
    void frameRateChanged(qreal);

public slots:
    void setFrameRate(qreal frameRate);
    Properties getCurrentProp();
    void updateDisplayAll();

private slots:
    void on_displayTimeMinSpinBox_valueChanged(int value);
    void on_intervalMinSpinBox_valueChanged(int value);
    void on_frameRateComboBox_currentIndexChanged(int index);
    void on_frameRateSpinBox_editingFinished();
    void on_maxCharPerLineSpinBox_valueChanged(int value);
    void on_maxCharPerSecDoubleSpinBox_valueChanged(double value);
    void on_leftMarginSpinBox_valueChanged(double arg1);
    void on_rightMarginSpinBox_valueChanged(double arg1);
    void on_topMarginSpinBox_valueChanged(double arg1);
    void on_bottomMarginSpinBox_valueChanged(double arg1);

private:
    Ui::Settings *ui;
    Properties mCurrentProperties;
    bool mSettingsChangedBySoft;
};

#endif // SETTINGS_H
