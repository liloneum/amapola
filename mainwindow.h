#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <myvideoplayer.h>
#include "mysubtitles.h"

namespace Ui {
class MainWindow;
}

class QTableWidgetItem;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionOpen_triggered();
    void on_actionQuit_triggered();
    void updateSubTable();
    void updateVideoPosition(qint32 positionMs);
    void updateTextEdit(MySubtitles subtitle);
    void resizeEvent(QResizeEvent* event);
    void updateStEditSize();

    void on_actionImport_Subtitles_triggered();

    void on_settingsButton_clicked();

    void on_actionExport_Subtitles_triggered();

private:
    Ui::MainWindow* ui;
};

#endif // MAINWINDOW_H
