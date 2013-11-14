#ifndef SUBEXPORTDIALOG_H
#define SUBEXPORTDIALOG_H

#include <QDialog>
#include "mysubtitles.h"

namespace Ui {
class SubExportDialog;
}

class SubExportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SubExportDialog(QList<MySubtitles> subList, QList<qint32>selectedSub, QWidget *parent = 0);
    ~SubExportDialog();
    QString version();
    QString subId();
    QString movieTitle();
    QString comment();
    QString reelNumber();
    QString language();
    QString fontPath();
    QString editRate();
    QString timeCodeRate();
    QString startTime();
    QString preferredEffect();
    bool hasHtmlTags();

private slots:
    void on_subNormList_currentTextChanged(const QString &currentText);
    void exportDatas();
    void on_cancelPushButton_clicked();
    void on_versionLineEdit_editingFinished();
    void on_subIDLineEdit_editingFinished();
    void on_movieTitleLineEdit_editingFinished();
    void on_commentLineEdit_editingFinished();
    void on_reelNumSpinBox_editingFinished();
    void on_languageLineEdit_editingFinished();
    void on_fontLineEdit_editingFinished();
    void on_editRateSpinBox_editingFinished();
    void on_timeCodeRateSpinBox_editingFinished();
    void on_startTimeEdit_editingFinished();
    void on_borderRadioButton_toggled(bool checked);
    void on_htmlYesRadioButton_toggled(bool checked);
    void on_genIDPushButton_clicked();
    void on_fontPushButton_clicked();

private:
    Ui::SubExportDialog *ui;

    QList<MySubtitles> mSubList;
    QList<qint32> mSelectedIndex;
    QString mVersion;
    QString mSubUUID;
    QString mMovieTitle;
    QString mComment;
    QString mReelNumber;
    QString mLanguage;
    QString mFontPath;
    QString mEditRate;
    QString mTimeCodeRate;
    QString mStartTime;
    QString mPreferredEffect;
    bool mHtmlTags;
};

#endif // SUBEXPORTDIALOG_H
