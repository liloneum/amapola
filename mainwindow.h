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
    bool eventFilter(QObject* watched, QEvent* event);
    void resizeEvent(QResizeEvent* event);
    void updateStEditSize();
    void displayErrorMsg(QString);

    void on_actionImport_Subtitles_triggered();

    void on_settingsButton_clicked();

    void on_actionExport_Subtitles_triggered();


    //***************************Tool Box ***************************//

    void updateToolBox(TextFont textFont, TextLine textLine);

    void updatePosToolBox(TextLine textLine);
    void updateTextPosition();

    void on_vPosSpinBox_valueChanged(const QString &arg1);

    void on_hPosSpinBox_valueChanged(const QString &arg1);

    void on_vAlignBox_activated(const QString &arg1);

    void on_hAlignBox_activated(const QString &arg1);


    void updateFontToolBox(TextFont textFont);
    void updateTextFont(bool customColorClicked);

    void on_fontSizeSpinBox_valueChanged(const QString &arg1);

    void on_fontColorRButton_toggled(bool checked);

    void on_fontColorGButton_toggled(bool checked);

    void on_fontColorBButton_toggled(bool checked);

    void on_fontColorYButton_toggled(bool checked);

    void on_fontColorBlButton_toggled(bool checked);

    void on_fontColorWButton_toggled(bool checked);

    void on_fontItalicButton_toggled(bool checked);

    void on_fontUnderlinedButton_toggled(bool checked);

    void on_fontIdComboBox_currentFontChanged(const QFont &f);

    void on_fontColorOtherButton_clicked();

private:
    Ui::MainWindow* ui;
    bool mTextPosChangedBySoft;
    bool mTextPosChangedByUser;
    bool mTextFontChangedBySoft;
    bool mTextFontChangedByUser;
};

#endif // MAINWINDOW_H
