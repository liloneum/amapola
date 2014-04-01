#include "subexportdialog.h"
#include "ui_subexportdialog.h"
#include "subtitlefileparser.h"
#include "SubtitlesParsers/SubRip/subripparser.h"
#include "SubtitlesParsers/DcSubtitle/interop/dcsubinteropparser.h"
#include "SubtitlesParsers/DcSubtitle/smpte/dcsubsmpteparser.h"
#include "SubtitlesParsers/EBU/ebuparser.h"
#include "SubtitlesParsers/ScenaristSub/scenaristsubparser.h"
#include "SubtitlesParsers/BDN/bdnparser.h"
#include "subtitles.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QUuid>
#include "filereader.h"
#include <QtDebug>

SubExportDialog::SubExportDialog(QList<Subtitles> subList, QList<qint32>selectedSub, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SubExportDialog)
{
    ui->setupUi(this);

    mSubList = subList;
    mSelectedIndex = selectedSub;

    // Init EBu parameters
    mStlFileName = "";
    mProgTitleVO = "";
    mProgTitleTR = "";
    mEpTitleVO = "";
    mEpTitleTR = "";
    mTranslatorName = "";
    mTranslatorContact = "";
    mSubListRef = "";
    mCreationDate = "";
    mRevisionNbr = 1;
    mStartOfProg = "";
    mCountryOrigin = "";
    mPublisher = "";
    mEditorName = "";
    mEditorContact = "";

    for ( quint16 i = 0; i < 128; i++ ) {

        QString language = EbuParser::LANGUAGES_CODES[i];

        if ( ( language != "" ) && ( language != "Reserved" ) ) {
            ui->languageComboBox->addItem(EbuParser::LANGUAGES_CODES[i], i);
        }
    }

    ui->languageComboBox->model()->sort(0);
    ui->languageComboBox->setCurrentIndex(ui->languageComboBox->findText("French"));
    on_languageComboBox_currentIndexChanged(ui->languageComboBox->currentIndex());

    // Init BDN parameters
    ui->frameRateComboBox->setItemData(0, 23.976);
    ui->frameRateComboBox->setItemData(1, 24);
    ui->frameRateComboBox->setItemData(2, 25);
    ui->frameRateComboBox->setItemData(3, 29.97);
    ui->frameRateComboBox->setItemData(4, 50);
    ui->frameRateComboBox->setItemData(5, 59.94);

    ui->sizeFormatcomboBox->setItemData(0, QSize(720,480));
    ui->sizeFormatcomboBox->setItemData(1, QSize(720,480));
    ui->sizeFormatcomboBox->setItemData(2, QSize(720,576));
    ui->sizeFormatcomboBox->setItemData(3, QSize(1280,720));
    ui->sizeFormatcomboBox->setItemData(4, QSize(1920,1080));
    ui->sizeFormatcomboBox->setItemData(5, QSize(1920,1080));

    connect(ui->exportPushButton, SIGNAL(clicked()), this, SLOT(exportDatas()), Qt::QueuedConnection);

}

SubExportDialog::~SubExportDialog() {
    delete ui;
}

void SubExportDialog::on_subNormList_currentTextChanged(const QString &currentText) {

    if ( currentText == "SubRip (*.srt)" ) {

        ui->stackedWidget->setCurrentIndex(0);

        ui->versionLabel->setEnabled(false);
        ui->versionLineEdit->setEnabled(false);
        ui->movieTilteLabel->setEnabled(false);
        ui->movieTitleLineEdit->setEnabled(false);
        ui->commentLabel->setEnabled(false);
        ui->commentLineEdit->setEnabled(false);
        ui->languageLabel->setEnabled(false);
        ui->languageLineEdit->setEnabled(false);
        ui->subIDLabel->setEnabled(false);
        ui->subIDLineEdit->setEnabled(false);
        ui->genIDPushButton->setEnabled(false);
        ui->reelNumLabel->setEnabled(false);
        ui->reelNumSpinBox->setEnabled(false);
        ui->fontLabel->setEnabled(false);
        ui->fontLineEdit->setEnabled(false);
        ui->fontPushButton->setEnabled(false);
        ui->editRateLabel->setEnabled(false);
        ui->editRateSpinBox->setEnabled(false);
        ui->timeCodeRateLabel->setEnabled(false);
        ui->timeCodeRateSpinBox->setEnabled(false);
        ui->startTimeLabel->setEnabled(false);
        ui->startTimeEdit->setEnabled(false);
        ui->fadeInLabel->setEnabled(false);
        ui->fadeInTimeEdit->setEnabled(false);
        ui->fadeOutLabel->setEnabled(false);
        ui->fadeOutTimeEdit->setEnabled(false);
        ui->effectGroupBox->setEnabled(false);

        ui->effectGroupBox_2->setEnabled(true);
        on_htmlYesRadioButton_toggled(ui->htmlYesRadioButton->isChecked());

        ui->exportedLabel->setEnabled(true);
        ui->exportedGroupBox->setEnabled(true);

    }
    else if ( currentText == "DCSub interop (*.xml)") {

        ui->stackedWidget->setCurrentIndex(0);

        ui->commentLabel->setEnabled(false);
        ui->commentLineEdit->setEnabled(false);
        ui->editRateLabel->setEnabled(false);
        ui->editRateSpinBox->setEnabled(false);
        ui->timeCodeRateLabel->setEnabled(false);
        ui->timeCodeRateSpinBox->setEnabled(false);
        ui->effectGroupBox_2->setEnabled(false);
        ui->startTimeLabel->setEnabled(false);
        ui->startTimeEdit->setEnabled(false);


        ui->versionLabel->setEnabled(true);
        ui->versionLineEdit->setEnabled(true);
        ui->versionLineEdit->setText("1.1");
        on_versionLineEdit_editingFinished();

        ui->subIDLabel->setEnabled(true);
        ui->subIDLineEdit->setEnabled(true);
        ui->genIDPushButton->setEnabled(true);
        on_subIDLineEdit_editingFinished();

        ui->reelNumLabel->setEnabled(true);
        ui->reelNumSpinBox->setEnabled(true);
        on_reelNumSpinBox_editingFinished();

        ui->fontLabel->setEnabled(true);
        ui->fontLineEdit->setEnabled(true);
        ui->fontPushButton->setEnabled(true);
        on_fontLineEdit_editingFinished();

        ui->movieTilteLabel->setEnabled(true);
        ui->movieTitleLineEdit->setEnabled(true);
        on_movieTitleLineEdit_editingFinished();

        ui->languageLabel->setEnabled(true);
        ui->languageLineEdit->setEnabled(true);
        on_languageLineEdit_editingFinished();

        ui->fadeInLabel->setEnabled(true);
        ui->fadeInTimeEdit->setEnabled(true);
        ui->fadeOutLabel->setEnabled(true);
        ui->fadeOutTimeEdit->setEnabled(true);

        ui->effectGroupBox->setEnabled(true);
        on_borderRadioButton_toggled(ui->borderRadioButton->isChecked());

        ui->exportedLabel->setEnabled(true);
        ui->exportedGroupBox->setEnabled(true);
    }
    else if ( currentText == "DCSub smpte (*.xml)") {

        ui->stackedWidget->setCurrentIndex(0);

        ui->versionLabel->setEnabled(false);
        ui->versionLineEdit->setEnabled(false);
        ui->effectGroupBox_2->setEnabled(false);


        ui->commentLabel->setEnabled(true);
        ui->commentLineEdit->setEnabled(true);
        on_commentLineEdit_editingFinished();

        ui->editRateLabel->setEnabled(true);
        ui->editRateSpinBox->setEnabled(true);
        on_editRateSpinBox_editingFinished();

        ui->timeCodeRateLabel->setEnabled(true);
        ui->timeCodeRateSpinBox->setEnabled(true);
        on_timeCodeRateSpinBox_editingFinished();

        ui->subIDLabel->setEnabled(true);
        ui->subIDLineEdit->setEnabled(true);
        ui->genIDPushButton->setEnabled(true);
        on_subIDLineEdit_editingFinished();

        ui->reelNumLabel->setEnabled(true);
        ui->reelNumSpinBox->setEnabled(true);
        on_reelNumSpinBox_editingFinished();

        ui->fontLabel->setEnabled(true);
        ui->fontLineEdit->setEnabled(true);
        ui->fontPushButton->setEnabled(true);
        on_fontLineEdit_editingFinished();

        ui->movieTilteLabel->setEnabled(true);
        ui->movieTitleLineEdit->setEnabled(true);
        on_movieTitleLineEdit_editingFinished();

        ui->languageLabel->setEnabled(true);
        ui->languageLineEdit->setEnabled(true);
        on_languageLineEdit_editingFinished();

        ui->startTimeLabel->setEnabled(true);
        ui->startTimeEdit->setEnabled(true);
        on_startTimeEdit_editingFinished();

        ui->fadeInLabel->setEnabled(true);
        ui->fadeInTimeEdit->setEnabled(true);
        on_fadeInTimeEdit_editingFinished();
        ui->fadeOutLabel->setEnabled(true);
        ui->fadeOutTimeEdit->setEnabled(true);
        on_fadeOutTimeEdit_editingFinished();

        ui->effectGroupBox->setEnabled(true);
        on_borderRadioButton_toggled(ui->borderRadioButton->isChecked());

        ui->exportedLabel->setEnabled(true);
        ui->exportedGroupBox->setEnabled(true);

    }
    else if ( currentText == "EBU-N19 (*.stl)" ) {

        ui->stackedWidget->setCurrentIndex(1);

        on_dfcSpinBox_editingFinished();
        on_mnrSpinBox_editingFinished();
        on_dscComboBox_currentTextChanged(ui->dscComboBox->currentText());
        on_cctComboBox_currentTextChanged(ui->cctComboBox->currentText());
        on_doubleHeightcheckBox_toggled(ui->doubleHeightcheckBox->isChecked());
        on_languageComboBox_currentIndexChanged(ui->languageComboBox->currentIndex());

    }
    else if ( currentText == "Scenarist Sub (*.sst)" ) {

        ui->stackedWidget->setCurrentIndex(2);

        on_imageSizeComboBox_currentTextChanged(ui->imageSizeComboBox->currentText());
        on_tapeTypeComboBox_currentIndexChanged(ui->tapeTypeComboBox->currentText());
        on_baseTimeEdit_editingFinished();
        on_languageLineEdit2_editingFinished() ;
    }
    else if ( currentText == "BDN (*.xml)" ) {

        ui->stackedWidget->setCurrentIndex(3);

        on_movieTitleLineEdit2_editingFinished();
        on_languageLineEdit3_editingFinished();
        on_sizeFormatcomboBox_currentIndexChanged(ui->sizeFormatcomboBox->currentText());
        on_frameRateComboBox_currentIndexChanged(ui->frameRateComboBox->currentIndex());
        on_dropFrameComboBox_currentIndexChanged(ui->dropFrameComboBox->currentText());
        on_contentInTimeEdit_editingFinished();
    }
}

void SubExportDialog::exportDatas() {

    QString sub_norm = ui->subNormList->currentItem()->text();

    QString file_name = "";
    bool raw_data_flag = false;

    if ( sub_norm == "EBU-N19 (*.stl)" ) {
        file_name = mStlFileName;
        raw_data_flag = true;
    }

    if ( file_name.isEmpty() ) {

        // Create or open file to write datas
        file_name = QFileDialog::getSaveFileName(this, tr("Open Subtitles"),QDir::homePath() + "/Videos",
                                                 tr(sub_norm.toUtf8()));
    }

    if ( file_name.isEmpty() ) {
        return;
    }

    // Choose the good parser
    SubtitleFileParser* parser;
    QString file_extension = "";

    if ( sub_norm ==  "SubRip (*.srt)" ) {
        parser = new SubRipParser();
        file_extension = ".srt";
    }
    else if ( sub_norm ==  "DCSub interop (*.xml)" ) {
        parser = new DcSubInteropParser();
        file_extension = ".xml";
    }
    else if ( sub_norm == "DCSub smpte (*.xml)" ) {
        parser = new DcSubSmpteParser();
        file_extension = ".xml";
    }
    else if ( sub_norm == "EBU-N19 (*.stl)" ) {
        parser = new EbuParser();
        file_extension = ".stl";
    }
    else if ( sub_norm == "Scenarist Sub (*.sst)") {
        parser = new ScenaristSubParser();
        file_extension = ".sst";
    }
    else if ( sub_norm == "BDN (*.xml)") {
        parser = new BDNparser();
        file_extension = ".xml";
    }
    else {
        // Nothing to do
    }

    // Add the extension if it miss
    if ( !file_name.contains(file_extension) ) {

        if ( file_name.lastIndexOf('.') >= 0 ) {
            file_name.remove(file_name.lastIndexOf('.', file_name.size()));
        }

        file_name.append(file_extension);
    }


    if ( ui->exportedSelectedRadioButton->isChecked() ) {

        QList<Subtitles> temp_list;

        QList<qint32>::iterator it;
        for ( it = mSelectedIndex.begin(); it != mSelectedIndex.end(); ++it ) {

            temp_list.append(mSubList.at(*it));
        }

        mSubList = temp_list;
    }

    FileWriter file_writer(file_name, "UTF-8");

    // If there are datas, write it to asked format
    if ( !mSubList.isEmpty() ) {

        if ( parser->save(file_writer, mSubList, this) ) {

            if ( file_writer.toFile(raw_data_flag) == false ) {
                QString error_msg = file_writer.errorMsg();
                QMessageBox::warning(this, tr("Export subtitles"), error_msg);
            }
        }
        else {
            QMessageBox::warning(this, tr("Export subtitles"), tr("Exporting aborded"));
            return;
        }
    }
    else {

        QMessageBox::warning(this, tr("Export subtitles"), tr("The subtitle list is empty"));
    }

    QMessageBox::information(this, tr("Export subtitles"), tr("Subtitles successfully exported"));
    return;
}

void SubExportDialog::on_genIDPushButton_clicked() {

    QString sub_uuid = QUuid::createUuid().toString().remove('{').remove('}');
    ui->subIDLineEdit->setText(sub_uuid);
    on_subIDLineEdit_editingFinished();
}

void SubExportDialog::on_fontPushButton_clicked() {

    // Create or open file to write datas
    QString file_name = QFileDialog::getOpenFileName(this, tr("Open font file"),QDir::homePath() );

    ui->fontLineEdit->setText(file_name);
    on_fontLineEdit_editingFinished();
}


void SubExportDialog::on_closePushButton_clicked() {

    this->done(QDialog::Accepted);
}

void SubExportDialog::on_versionLineEdit_editingFinished() {

    mVersion = ui->versionLineEdit->text();
}

void SubExportDialog::on_subIDLineEdit_editingFinished() {

    mSubUUID = ui->subIDLineEdit->text();
}

void SubExportDialog::on_movieTitleLineEdit_editingFinished() {

    mMovieTitle = ui->movieTitleLineEdit->text();
}

void SubExportDialog::on_commentLineEdit_editingFinished() {

    mComment = ui->commentLineEdit->text();
}

void SubExportDialog::on_reelNumSpinBox_editingFinished() {

    mReelNumber = ui->reelNumSpinBox->text();
}

void SubExportDialog::on_languageLineEdit_editingFinished() {

    mLanguage = ui->languageLineEdit->text();
}

void SubExportDialog::on_fontLineEdit_editingFinished() {

    mFontPath = ui->fontLineEdit->text();
}

void SubExportDialog::on_editRateSpinBox_editingFinished() {

    mEditRate = ui->editRateSpinBox->text();
}

void SubExportDialog::on_timeCodeRateSpinBox_editingFinished() {

    mTimeCodeRate = ui->timeCodeRateSpinBox->text();
}

void SubExportDialog::on_startTimeEdit_editingFinished() {

    mStartTime = ui->startTimeEdit->text();
}

void SubExportDialog::on_fadeInTimeEdit_editingFinished() {

    mFadeInTime = ui->fadeInTimeEdit->text();
}

void SubExportDialog::on_fadeOutTimeEdit_editingFinished() {

    mFadeOutTime = ui->fadeOutTimeEdit->text();
}


void SubExportDialog::on_borderRadioButton_toggled(bool checked) {

    if ( checked == true ) {
        mPreferredEffect = "border";
    }
    else {
        mPreferredEffect = "shadow";
    }
}

void SubExportDialog::on_htmlYesRadioButton_toggled(bool checked) {

    mHtmlTags = checked;
}

// Ebu
void SubExportDialog::on_filePushButton_clicked() {

    // Create or open file to write datas
    mStlFileName = QFileDialog::getOpenFileName(this,
                                                tr("Open stl file"),
                                                QDir::homePath(),
                                                tr("EBU-N19 (*.stl)"));

    if ( !mStlFileName.isEmpty() ) {

        FileReader file_reader(mStlFileName,"");

        if ( file_reader.readRawData(mStlFileName) == false ) {
            QString error_msg = file_reader.errorMsg();
            QMessageBox::warning(this, "Open EBU-N19 file", error_msg);
            return;
        }

        EbuParser* parser = new EbuParser;

        parser->readGsiBlock(file_reader, this);
    }

    ui->fileLineEdit->setText(mStlFileName);
}

void SubExportDialog::on_dfcSpinBox_editingFinished() {

    mDiskFormatCode = QString::number(ui->dfcSpinBox->value());
}

void SubExportDialog::on_mnrSpinBox_editingFinished() {

    mMaxRow = ui->mnrSpinBox->text();
}

void SubExportDialog::on_doubleHeightcheckBox_toggled(bool checked) {

    mDoubleHeight = checked;
}

void SubExportDialog::on_dscComboBox_currentTextChanged(const QString &arg1) {

    mDisplayMode = arg1;
}

void SubExportDialog::on_cctComboBox_currentTextChanged(const QString &arg1) {

    mCharCodeTable = arg1;
}

void SubExportDialog::on_languageComboBox_currentIndexChanged(int index) {

    mLanguageCode = ui->languageComboBox->itemData(index).toInt();
}

void SubExportDialog::on_optLineEdit_editingFinished() {

    mProgTitleVO = ui->optLineEdit->text();
}

void SubExportDialog::on_tptLineEdit_editingFinished() {

    mProgTitleTR = ui->tptLineEdit->text();
}

void SubExportDialog::on_oetLineEdit_editingFinished() {

    mEpTitleVO = ui->oetLineEdit->text();
}

void SubExportDialog::on_tetLineEdit_editingFinished() {

    mEpTitleTR = ui->tetLineEdit->text();
}

void SubExportDialog::on_tnLineEdit_editingFinished() {

    mTranslatorName = ui->tnLineEdit->text();
}

void SubExportDialog::on_tcdLineEdit_editingFinished() {

    mTranslatorContact = ui->tcdLineEdit->text();
}

void SubExportDialog::on_slrLineEdit_editingFinished() {

    mSubListRef = ui->slrLineEdit->text();
}

// Scenarist
void SubExportDialog::on_imageSizeComboBox_currentTextChanged(const QString &arg1) {

    mImageSize = arg1;
}

void SubExportDialog::on_tapeTypeComboBox_currentIndexChanged(const QString &arg1) {

    if ( arg1 == "drop") {
        mTapeTypeDrop = true;
    }
    else {
        mTapeTypeDrop = false;
    }
}

void SubExportDialog::on_baseTimeEdit_editingFinished() {

    mStartTime = ui->baseTimeEdit->text();
}

void SubExportDialog::on_languageLineEdit2_editingFinished() {

    mLanguage = ui->languageLineEdit2->text();
}

// BDN
void SubExportDialog::on_movieTitleLineEdit2_editingFinished() {

    mMovieTitle = ui->movieTitleLineEdit2->text();
}

void SubExportDialog::on_languageLineEdit3_editingFinished() {

    mLanguage = ui->languageLineEdit3->text();
}

void SubExportDialog::on_sizeFormatcomboBox_currentIndexChanged(const QString &arg1) {

    mImageSize = arg1;
}

void SubExportDialog::on_frameRateComboBox_currentIndexChanged(int index) {

    mFrameRate = ui->frameRateComboBox->itemData(index).toDouble();
}

void SubExportDialog::on_dropFrameComboBox_currentIndexChanged(const QString &arg1) {

    if ( arg1 == "drop") {
        mTapeTypeDrop = true;
    }
    else {
        mTapeTypeDrop = false;
    }
}

void SubExportDialog::on_contentInTimeEdit_editingFinished() {

    mStartTime = ui->contentInTimeEdit->text();
}



// ACESSORS
QString SubExportDialog::version() {
    return mVersion;
}

QString SubExportDialog::subId() {

    return mSubUUID;
}

QString SubExportDialog::movieTitle() {

    return mMovieTitle;
}

QString SubExportDialog::comment() {

    return mComment;
}

QString SubExportDialog::reelNumber() {

    return mReelNumber;
}

QString SubExportDialog::language() {

    return mLanguage;
}

QString SubExportDialog::fontPath() {

    return mFontPath;
}

QString SubExportDialog::editRate() {

    return mEditRate;
}

QString SubExportDialog::timeCodeRate() {

    return mTimeCodeRate;
}

QString SubExportDialog::startTime() {

    return mStartTime;
}

QString SubExportDialog::fadeInTime() {

    return mFadeInTime;
}

QString SubExportDialog::fadeOutTime() {

    return mFadeOutTime;
}

QString SubExportDialog::preferredEffect() {

    return mPreferredEffect;
}

bool SubExportDialog::hasHtmlTags() {

    return mHtmlTags;
}

// Ebu
QString SubExportDialog::maxRow() {

    return mMaxRow;
}

bool SubExportDialog::isDoubleHeight() {

    return mDoubleHeight;
}

QString SubExportDialog::stlFileName() {

    return mStlFileName;
}

QString SubExportDialog::displayMode() {

    return mDisplayMode;
}

QString SubExportDialog::charCodeTable() {

    return mCharCodeTable;
}

quint8 SubExportDialog::languageCode() {

    return mLanguageCode;
}

QString SubExportDialog::progTitleVO() {

    return mProgTitleVO;
}

QString SubExportDialog::progTitleTR() {

    return mProgTitleTR;
}

QString SubExportDialog::epTitleVO() {

    return mEpTitleVO;
}

QString SubExportDialog::epTitleTR() {

    return mEpTitleTR;
}

QString SubExportDialog::translatorName() {

    return mTranslatorName;
}

QString SubExportDialog::translatorContact() {

    return mTranslatorContact;
}

QString SubExportDialog::subListRef() {

    return mSubListRef;
}

QString SubExportDialog::creationDate() {

    return mCreationDate;
}

quint16 SubExportDialog::revisionNbr() {

    return mRevisionNbr;
}

QString SubExportDialog::startOfProg() {

    return mStartOfProg;
}

QString SubExportDialog::countryOrigin() {

    return mCountryOrigin;
}

QString SubExportDialog::publisher() {

    return mPublisher;
}

QString SubExportDialog::editorName() {

    return mEditorName;
}

QString SubExportDialog::editorContact() {

    return mEditorContact;
}

QString SubExportDialog::diskFormatCode() {

    return mDiskFormatCode;
}

//Scenarist
QString SubExportDialog::imageSize() {

    return mImageSize;
}

bool SubExportDialog::tapeTypeDrope() {

    return mTapeTypeDrop;
}

// BDN

qreal SubExportDialog::frameRate() {

    return mFrameRate;
}

// MUTATOR

// Ebu
void SubExportDialog::setDiskFormatCode(QString diskFormatCode) {

    mDiskFormatCode = diskFormatCode;
    ui->dfcSpinBox->setValue(diskFormatCode.toInt());
}

void SubExportDialog::setDisplayMode(QString displayMode) {

    if ( displayMode == "0" ) {
        mDisplayMode = "Open subtitling";
    }
    else if ( displayMode == "1" ) {
        mDisplayMode = "Level-1 teletext";
    }
    else if ( displayMode == "2" ) {
        mDisplayMode = "Level-2 teletext";
    }
    else {
        mDisplayMode = "Undefined";
    }

    ui->dscComboBox->setCurrentIndex(ui->dscComboBox->findText(mDisplayMode));
}

void SubExportDialog::setCharCodeTable(QString charCodeTable) {

    if ( charCodeTable == "01" ) {
        mCharCodeTable = "Cyrillic";
    }
    else if ( charCodeTable == "02" ) {
        mCharCodeTable = "Arabic";
    }
    else if ( charCodeTable == "03" ) {
        mCharCodeTable = "Greek";
    }
    else if ( charCodeTable == "04" ) {
        mCharCodeTable = "Hebrew";
    }
    else  {
        mCharCodeTable = "Latin";
    }

    ui->cctComboBox->setCurrentIndex(ui->cctComboBox->findText(mCharCodeTable));
}

void SubExportDialog::setLanguageCode(QString languageCode) {

    bool ok;
    mLanguageCode = languageCode.toInt(&ok, 16);

    ui->languageComboBox->setCurrentIndex(ui->languageComboBox->findData(mLanguageCode));
}

void SubExportDialog::setProgTitleVO(QString progTitle) {

    mProgTitleVO = progTitle;
    ui->optLineEdit->setText(mProgTitleVO);
}

void SubExportDialog::setProgTitleTR(QString progTitle) {

    mProgTitleTR = progTitle;
    ui->tptLineEdit->setText(mProgTitleTR);
}

void SubExportDialog::setEpTitleVO(QString epTitle) {

    mEpTitleVO = epTitle;
    ui->oetLineEdit->setText(mEpTitleVO);
}

void SubExportDialog::setEpTitleTR(QString epTitle) {

    mEpTitleTR = epTitle;
    ui->tetLineEdit->setText(mEpTitleTR);
}

void SubExportDialog::setTranslatorName(QString translatorName) {

    mTranslatorName = translatorName;
    ui->tnLineEdit->setText(mTranslatorName);
}

void SubExportDialog::setTranslatorContact(QString translatorContact) {

    mTranslatorContact = translatorContact;
    ui->tcdLineEdit->setText(mTranslatorContact);
}

void SubExportDialog::setSubListRef(QString subListRef) {

    mSubListRef = subListRef;
    ui->slrLineEdit->setText(mSubListRef);
}

void SubExportDialog::setCreationDate(QString creationDate) {

    mCreationDate = creationDate;
}

void SubExportDialog::setRevisionNbr(QString revisionNbr) {

    mRevisionNbr = revisionNbr.toInt() + 1;
}

void SubExportDialog::setMaxRow(QString maxRow) {

    mMaxRow = maxRow;
    ui->mnrSpinBox->setValue(maxRow.toInt());
}

void SubExportDialog::setStartOfProg(QString startOfProg) {

    mStartOfProg = startOfProg;
}

void SubExportDialog::setCountryOrigin(QString countryOrigin) {

    mCountryOrigin = countryOrigin;
}

void SubExportDialog::setPublisher(QString publisher) {

    mPublisher = publisher;
}

void SubExportDialog::setEditorsName(QString editorName) {

    mEditorName = editorName;
}

void SubExportDialog::setEditorsContact(QString editorContact) {

    mEditorContact = editorContact;
}
