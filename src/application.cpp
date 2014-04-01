#include "application.h"
#include "mainwindow.h"

MainWindow* Application::mpMainWindow = NULL;

Application::Application(int argc, char* argv[]):
    QApplication(argc, argv)
{
    mpMainWindow = new MainWindow();
    mpMainWindow->show();
}

Application::~Application() {

}

MainWindow* Application::getMainWindow() {

    return mpMainWindow;
}
