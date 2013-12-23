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

    // SubRip, DcSub
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

    // Ebu
    QString maxRow();
    bool isDoubleHeight();
    QString diskFormatCode();
    QString stlFileName();
    QString displayMode();
    QString charCodeTable();
    quint8 languageCode();
    QString progTitleVO();
    QString progTitleTR();
    QString epTitleVO();
    QString epTitleTR();
    QString translatorName();
    QString translatorContact();
    QString subListRef();
    QString creationDate();
    quint16 revisionNbr();
    QString startOfProg();
    QString countryOrigin();
    QString publisher();
    QString editorName();
    QString editorContact();

    // Scenarist
    QString imageSize();
    bool tapeTypeDrope();

    // BDN
    qreal frameRate();

    // Ebu
    void setDiskFormatCode(QString diskFormatCode);
    void setDisplayMode(QString displayMode);
    void setCharCodeTable(QString charCodeTable);
    void setLanguageCode(QString languageCode);
    void setProgTitleVO(QString progTitle);
    void setProgTitleTR(QString progTitle);
    void setEpTitleVO(QString epTitle);
    void setEpTitleTR(QString epTitle);
    void setTranslatorName(QString translatorName);
    void setTranslatorContact(QString translatorContact);
    void setSubListRef(QString subListRef);
    void setCreationDate(QString creationDate);
    void setRevisionNbr(QString revisionNbr);
    void setMaxRow(QString maxRow);
    void setStartOfProg(QString startOfProg);
    void setCountryOrigin(QString countryOrigin);
    void setPublisher(QString publisher);
    void setEditorsName(QString editorName);
    void setEditorsContact(QString editorContact);

private slots:

    // SubRip, DCSub
    void on_subNormList_currentTextChanged(const QString &currentText);
    void exportDatas();
    void on_closePushButton_clicked();
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

    // Ebu
    void on_dfcSpinBox_editingFinished();
    void on_mnrSpinBox_editingFinished();
    void on_dscComboBox_currentTextChanged(const QString &arg1);
    void on_cctComboBox_currentTextChanged(const QString &arg1);
    void on_languageComboBox_currentIndexChanged(int index);
    void on_optLineEdit_editingFinished();
    void on_tptLineEdit_editingFinished();
    void on_oetLineEdit_editingFinished();
    void on_tetLineEdit_editingFinished();
    void on_tnLineEdit_editingFinished();
    void on_tcdLineEdit_editingFinished();
    void on_slrLineEdit_editingFinished();
    void on_filePushButton_clicked();
    void on_doubleHeightcheckBox_toggled(bool checked);

    // Scenarist
    void on_imageSizeComboBox_currentTextChanged(const QString &arg1);
    void on_baseTimeEdit_editingFinished();
    void on_languageLineEdit2_editingFinished();
    void on_tapeTypeComboBox_currentIndexChanged(const QString &arg1);

    void on_movieTitleLineEdit2_editingFinished();

    void on_languageLineEdit3_editingFinished();

    void on_frameRateComboBox_currentIndexChanged(int index);

    void on_dropFrameComboBox_currentIndexChanged(const QString &arg1);

    void on_contentInTimeEdit_editingFinished();

    void on_sizeFormatcomboBox_currentIndexChanged(const QString &arg1);

private:
    Ui::SubExportDialog *ui;

    // SubRip, DCSub
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

    // Ebu
    QString mStlFileName;
    QString mDiskFormatCode;
    QString mMaxRow;
    bool mDoubleHeight;
    QString mDisplayMode;
    QString mCharCodeTable;
    quint8 mLanguageCode;
    QString mProgTitleVO;
    QString mProgTitleTR;
    QString mEpTitleVO;
    QString mEpTitleTR;
    QString mTranslatorName;
    QString mTranslatorContact;
    QString mSubListRef;
    QString mCreationDate;
    quint16 mRevisionNbr;
    QString mStartOfProg;
    QString mCountryOrigin;
    QString mPublisher;
    QString mEditorName;
    QString mEditorContact;

    // Scenarist
    QString mImageSize;
    bool mTapeTypeDrop;

    // BDN
    qreal mFrameRate;
    bool mFrameDrop;
};

#endif // SUBEXPORTDIALOG_H
