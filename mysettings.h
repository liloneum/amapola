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
    
private:
    Ui::MySettings *ui;
};

#endif // MYSETTINGS_H
