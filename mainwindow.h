#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <myvideoplayer.h>
#include "mysubtitles.h"
#include "mysettings.h"

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
    void saveProject(QString fileName);
    void on_actionOpen_video_triggered();
    void openVideo(QString fileName);
    void on_actionSave_as_triggered();
    void on_actionSave_triggered();
    void onCtrlLeftClickEvent(qint64 positionMs);
    void onCtrlRightClickEvent(qint64 positionMs);
    void updateSubTableText();
    void updateSubTableDatas(MySubtitles subtitleDatas);
    void videoPositionChanged(qint64 positionMs);
    void currentSelectionChanged(qint64 positionMs);
    void waveformMarerkPosChanged(qint64 positionMs);
    void updateVideoPosition(qint64 positionMs);
    void currentSubChanged(MySubtitles subtitle);
    bool eventFilter(QObject* watched, QEvent* event);
    void resizeEvent(QResizeEvent* event);
    void updateStEditSize();
    void updateMarginsDraw();
    void displayErrorMsg(QString);
    void displayInfo(QString info);
    void eraseInfo();
    bool changeSubStartTime(qint64 &positionMs, qint32 refIndex = -1, QList<MySubtitles> subList = QList<MySubtitles>(), bool multiChange = true, bool applyChange = true, bool movePrevious = true);
    bool changeSubEndTime(qint64 &positionMs, qint32 refIndex = -1, QList<MySubtitles> subList = QList<MySubtitles>(), bool multiChange = true, bool applyChange = true, bool moveNext = true);
    void shiftSubtitles(qint64 positionMs, qint32 index = -1);
    void removeSubtitles();
    void updateFrameRate(qreal frameRate);
    void saveToHistory(QString changeReason);
    void clearHistory();
    bool undo();
    bool redo();
    void on_actionImport_Subtitles_triggered();
    void on_settingsButton_clicked();
    void on_actionExport_Subtitles_triggered();

    // Tool bar
    void resolutionComboBox_currentIndexChanged(int index);
    void updateResoltionBox(QSizeF videoResolution);
    void frameRateComboBox_currentIndexChanged(int index);
    void updateFrameRateBox(qreal frameRate);


    //***************************Tool Box ***************************//

    void updateToolBox();

    void updatePosToolBox(TextLine textLine);
    TextLine getPosToolBox();
    void updateTextPosition();

    void on_vPosSpinBox_valueChanged(const QString &value);
    void on_hPosSpinBox_valueChanged(const QString &value);
    void on_vAlignBox_activated(const QString &value);
    void on_hAlignBox_activated(const QString &value);


    void updateFontToolBox(TextFont textFont);
    TextFont getFontToolBox();
    void updateTextFont();

    void on_fontSizeSpinBox_valueChanged(const QString &value);
    void on_fontColor1Button_toggled(bool checked);
    void on_fontColor2Button_toggled(bool checked);
    void on_fontColor3Button_toggled(bool checked);
    void on_fontColor4Button_toggled(bool checked);
    void on_fontColor5Button_toggled(bool checked);
    void on_fontColor6Button_toggled(bool checked);
    void on_fontColor7Button_toggled(bool checked);
    void on_fontColor1Button_customContextMenuRequested(const QPoint &pos);
    void on_fontColor2Button_customContextMenuRequested(const QPoint &pos);
    void on_fontColor3Button_customContextMenuRequested(const QPoint &pos);
    void on_fontColor4Button_customContextMenuRequested(const QPoint &pos);
    void on_fontColor5Button_customContextMenuRequested(const QPoint &pos);
    void on_fontColor6Button_customContextMenuRequested(const QPoint &pos);
    void on_fontItalicButton_toggled(bool checked);
    void on_fontUnderlinedButton_toggled(bool checked);
    void on_fontIdComboBox_currentFontChanged(const QFont &f);
    void on_firstNumSpinBox_customContextMenuRequested(const QPoint &pos);
    void on_lastNumSpinBox_customContextMenuRequested(const QPoint &pos);
    void on_syncApplyPushButton_clicked();
    void on_firstNewStartTimeEdit_customContextMenuRequested(const QPoint &pos);
    void on_lastNewStartTimeEdit_customContextMenuRequested(const QPoint &pos);
    void on_applyPosSelButton_clicked();
    void on_applyFontSelButton_clicked();
    void on_subTable_customContextMenuRequested(const QPoint &pos);
    void on_durationAutoCheckBox_clicked(bool checked);
    void on_fontBorderCheckBox_toggled(bool checked);
    void on_fontShadowCheckBox_toggled(bool checked);
    void on_fontBorderColor_clicked();
    void on_fontShadowColor_clicked();
    void on_fontBackgroundCheckBox_toggled(bool checked);
    void on_fontBackgroundColor_clicked();


private:
    Ui::MainWindow* ui;

    // Flag if the toolbox was changed by user or by software
    // To only treat the user changment
    bool mTextPosChangedBySoft;
    bool mTextPosChangedByUser;
    bool mTextFontChangedBySoft;
    bool mTextFontChangedByUser;

    // Tool Bar
    QComboBox* mFrameRateComboBox;
    bool mFrameRateChangedBySoft;
    QComboBox* mResolutionComboBox;

    // Flag to avoid infinite inter call between video-player and waveform
    bool mVideoPositionChanged;
    bool mMarkerPosChanged;

    QColor mTextBorderColor;
    QColor mTextShadowColor;
    QColor mTextBackgroundColor;
    QColor mColor1;
    QColor mColor2;
    QColor mColor3;
    QColor mColor4;
    QColor mColor5;
    QColor mColor6;
    QColor mColor7;

    // History (undo/redo) varaiables
    QList<QString> mHistoryReasons;
    QList< QList<MySubtitles> > mSubListHistory;
    QList<MyProperties> mPropertyHistory;
    qint32 mHistoryCurrentIndex;

    QTimer* mpDisplayInfoTimer;
};

#endif // MAINWINDOW_H
