#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <myvideoplayer.h>

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
    void updateTextEdit(QString stText);
    void resizeEvent(QResizeEvent* event);
    void updateStEditSize();

private:
    Ui::MainWindow* ui;
};

#endif // MAINWINDOW_H
