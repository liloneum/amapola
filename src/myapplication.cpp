#include "myapplication.h"
#include "mainwindow.h"

MainWindow* MyApplication::mpMainWindow = NULL;

MyApplication::MyApplication(int argc, char* argv[]):
    QApplication(argc, argv)
{
    mpMainWindow = new MainWindow();
    mpMainWindow->show();
}

MyApplication::~MyApplication() {

}

MainWindow* MyApplication::getMainWindow() {

    return mpMainWindow;
}
