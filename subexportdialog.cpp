#include "subexportdialog.h"
#include "ui_subexportdialog.h"
#include "mysubtitlefileparser.h"
#include "SubtitlesParsers/SubRip/subripparser.h"
#include "SubtitlesParsers/DcSubtitle/interop/dcsubinteropparser.h"
#include "SubtitlesParsers/DcSubtitle/smpte/dcsubsmpteparser.h"
#include "mysubtitles.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QUuid>

SubExportDialog::SubExportDialog(QList<MySubtitles> subList, QList<qint32>selectedSub, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SubExportDialog)
{
    ui->setupUi(this);

    mSubList = subList;
    mSelectedIndex = selectedSub;

    on_versionLineEdit_editingFinished();
    on_subIDLineEdit_editingFinished();
    on_movieTitleLineEdit_editingFinished();
    on_commentLineEdit_editingFinished();
    on_reelNumSpinBox_editingFinished();
    on_languageLineEdit_editingFinished();
    on_fontLineEdit_editingFinished();
    on_editRateSpinBox_editingFinished();
    on_timeCodeRateSpinBox_editingFinished();
    on_startTimeEdit_editingFinished();
    on_borderRadioButton_toggled(ui->borderRadioButton->isChecked());
    on_htmlYesRadioButton_toggled(ui->htmlYesRadioButton->isChecked());

    connect(ui->exportPushButton, SIGNAL(clicked()), this, SLOT(exportDatas()), Qt::QueuedConnection);

}

SubExportDialog::~SubExportDialog() {
    delete ui;
}

void SubExportDialog::on_subNormList_currentTextChanged(const QString &currentText) {

    if ( currentText == "SubRip (*.srt)" ) {

        ui->versionLabel->setEnabled(false);
        ui->versionLineEdit->setEnabled(false);
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
        ui->effectGroupBox->setEnabled(false);


        ui->movieTilteLabel->setEnabled(true);
        ui->movieTitleLineEdit->setEnabled(true);
        ui->commentLabel->setEnabled(true);
        ui->commentLineEdit->setEnabled(true);
        ui->languageLabel->setEnabled(true);
        ui->languageLineEdit->setEnabled(true);
        ui->effectGroupBox_2->setEnabled(true);
        ui->exportedLabel->setEnabled(true);
        ui->exportedGroupBox->setEnabled(true);

    }
    else if ( currentText == "DCSub interop (*.xml)") {

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

        ui->reelNumLabel->setEnabled(true);
        ui->reelNumSpinBox->setEnabled(true);

        ui->fontLabel->setEnabled(true);
        ui->fontLineEdit->setEnabled(true);
        ui->fontPushButton->setEnabled(true);

        ui->movieTilteLabel->setEnabled(true);
        ui->movieTitleLineEdit->setEnabled(true);

        ui->languageLabel->setEnabled(true);
        ui->languageLineEdit->setEnabled(true);

        ui->effectGroupBox->setEnabled(true);
        ui->exportedLabel->setEnabled(true);
        ui->exportedGroupBox->setEnabled(true);

    }
    else if ( currentText == "DCSub smpte (*.xml)") {


        ui->versionLabel->setEnabled(false);
        ui->versionLineEdit->setEnabled(false);
        ui->effectGroupBox_2->setEnabled(false);


        ui->commentLabel->setEnabled(true);
        ui->commentLineEdit->setEnabled(true);
        ui->editRateLabel->setEnabled(true);
        ui->editRateSpinBox->setEnabled(true);
        ui->timeCodeRateLabel->setEnabled(true);
        ui->timeCodeRateSpinBox->setEnabled(true);
        ui->subIDLabel->setEnabled(true);
        ui->subIDLineEdit->setEnabled(true);
        ui->genIDPushButton->setEnabled(true);
        ui->reelNumLabel->setEnabled(true);
        ui->reelNumSpinBox->setEnabled(true);
        ui->fontLabel->setEnabled(true);
        ui->fontLineEdit->setEnabled(true);
        ui->fontPushButton->setEnabled(true);
        ui->movieTilteLabel->setEnabled(true);
        ui->movieTitleLineEdit->setEnabled(true);
        ui->languageLabel->setEnabled(true);
        ui->languageLineEdit->setEnabled(true);
        ui->startTimeLabel->setEnabled(true);
        ui->startTimeEdit->setEnabled(true);
        ui->effectGroupBox->setEnabled(true);
        ui->exportedLabel->setEnabled(true);
        ui->exportedGroupBox->setEnabled(true);

    }
}

void SubExportDialog::exportDatas() {

    QString sub_norm = ui->subNormList->currentItem()->text();

    // Create or open file to write datas
    QString file_name = QFileDialog::getSaveFileName(this, tr("Open Subtitles"),QDir::homePath() + "/Videos",
                                                     tr(sub_norm.toUtf8()));



    // Choose the good parser
    MySubtitleFileParser* parser;
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
    else {
        // Nothing to do
    }

    // Add the extension if it miss
    if ( !file_name.contains(file_extension) ) {
        file_name.append(file_extension);
    }


    if ( ui->exportedSelectedRadioButton->isChecked() ) {

        QList<MySubtitles> temp_list;

        QList<qint32>::iterator it;
        for ( it = mSelectedIndex.begin(); it != mSelectedIndex.end(); ++it ) {

            temp_list.append(mSubList.at(*it));
        }

        mSubList = temp_list;
    }

    MyFileWriter file_writer(file_name, "UTF-8");

    // If there are datas, write it to asked format
   if ( !mSubList.isEmpty() ) {

       parser->save(file_writer, mSubList, this);

       if ( file_writer.toFile() == false ) {
           QString error_msg = file_writer.errorMsg();
           QMessageBox::warning(this, "Export subtitles", error_msg);
           this->done(QDialog::Rejected);
       }

       this->done(QDialog::Accepted);
   }
   else {
       this->done(QDialog::Rejected);
   }
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


void SubExportDialog::on_cancelPushButton_clicked() {

    this->done(QDialog::Rejected);
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

QString SubExportDialog::preferredEffect() {

    return mPreferredEffect;
}

bool SubExportDialog::hasHtmlTags() {

    return mHtmlTags;
}
