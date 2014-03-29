#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <mainwindow.h>

class MyApplication : public QApplication {

public:
    MyApplication(int argc, char* argv[]);
    static MainWindow* getMainWindow();
    ~MyApplication();

protected:
    static MainWindow* mpMainWindow;
};

#endif // APPLICATION_H
