#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <mainwindow.h>

class Application : public QApplication {

public:
    Application(int &argc, char* argv[]);
    static MainWindow* getMainWindow();
    ~Application();

protected:
    static MainWindow* mpMainWindow;
};

#endif // APPLICATION_H
