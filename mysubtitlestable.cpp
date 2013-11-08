#include "mysubtitlestable.h"
#include "mysubtitles.h"
#include <QTime>
#include <QTextDocumentFragment>
#include "mysettings.h"
#include <QCoreApplication>
#include "myattributesconverter.h"

// Temp : Default font
#define FONT_ID_DEFAULT_VALUE "Arial"
#define FONT_COLOR_DEFAULT_VALUE "FFFFFFFF"
#define FONT_SHADOW_EFFECT_DEFAULT_VALUE "yes"
#define FONT_SHADOW_EFFECT_COLOR_DEFAULT_VALUE "FF000000"
#define FONT_BORDER_EFFECT_DEFAULT_VALUE "yes"
#define FONT_BORDER_EFFECT_COLOR_DEFAULT_VALUE "FF000000"
#define FONT_ITALIC_DEFAULT_VALUE "no"
#define FONT_SCRIPT_DEFAULT_VALUE "normal"
#define FONT_SIZE_DEFAULT_VALUE "42"
#define FONT_UNDERLINED_DEFAULT_VALUE "no"

// Temp : Default position
#define TEXT1_DIRECTION_DEFAULT_VALUE "horizontal"
#define TEXT1_HALIGN_DEFAULT_VALUE "center"
#define TEXT1_HPOSITION_DEFAULT_VALUE "0"
#define TEXT1_VALIGN_DEFAULT_VALUE "bottom"
#define TEXT1_VPOSITION_DEFAULT_VALUE "8"

// Columns numbers define
#define SUB_NUM_COL 0
#define SUB_START_TIME_COL 1
#define SUB_END_TIME_COL 2
#define SUB_DURATION_COL 3
#define SUB_CHARNUM_COL 4
#define SUB_CHAR_PER_SEC_COL 5
#define SUB_TEXT_COL 6


// Convert second in milliseconds
#define SEC_TO_MSEC 1000

// This widget manage a subtitles table.
// It's a database to stock text, timecode, position, font.
// Display only text and timecode.
MySubtitlesTable::MySubtitlesTable(QWidget *parent) :
    QTableWidget(parent)
{
    // Init the lookup table
    mPositionMsToStIndex.reserve(14400000);
    mPositionMsToStIndex.resize(3600000);
    mPositionMsToStIndex.fill(-1);

    // Init the subtitles list
    mSubtitlesList.clear();

    // Init the variables
    mPreviousIndex = -1;
    mStCount = 0;
    mCurrentIndex = 0;

    // Init flags
    mSubLoadding = false;
    mSelectedBySoft = false;
    mSelectedByUser = false;
    mInsertingSub = false;

    // Init error message
    mErrorMsg = "";

    // Init the connections
    connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(updateSelectedItem()));
    //connect(this, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(updateItem(QTableWidgetItem*)));

}

// Init the table with 100 rows, reset subtitle counter to 0, disable the table
void MySubtitlesTable::initStTable (qint32 numberOfRow) {

    this->setEnabled(false);

    mStCount = 0;

    this->setRowCount(0);

    // Resize columns to contents
    for ( int col = 0; col < 6; col++ ) {
        this->resizeColumnToContents(col);
    }

    if ( numberOfRow == 0 ) {
        numberOfRow = 100;
    }

    this->addRows(numberOfRow, 0);

    mPositionMsToStIndex.resize(3600000);
    mPositionMsToStIndex.fill(-1);

    this->setEnabled(true);
}

// Add new rows to the table. "numberOfRow" define the number of rows to add.
// "fromRowNbr" define where insert this new row.
void MySubtitlesTable::addRows(qint32 numberOfRow, qint32 fromRowNbr) {

    qint32 row_count = this->rowCount();
    row_count = row_count + numberOfRow;
    setRowCount(row_count);

    for ( qint32 i = 0; i < this->columnCount(); i++ ) {
        for ( qint32 j = fromRowNbr; j < (fromRowNbr + numberOfRow); j++) {

            QTableWidgetItem* new_item = new QTableWidgetItem;

            // The start time items are init with "New Entry" string to able sorting
            if ( ( i == SUB_NUM_COL ) ||
                 ( i == SUB_CHARNUM_COL ) ||
                 ( i == SUB_CHAR_PER_SEC_COL ) ) {
                new_item->setText("");
                new_item->setTextAlignment(Qt::AlignCenter);
            }
            else if ( i == SUB_START_TIME_COL ) {
                new_item->setText("o");
                new_item->setTextAlignment(Qt::AlignCenter);
            }
            else if ( i == SUB_TEXT_COL ){
                new_item->setText("");
                new_item->setTextAlignment(Qt::AlignLeft);
                this->hideRow(j);
            }
            else {
                new_item->setText("");
                new_item->setTextAlignment(Qt::AlignCenter);
            }

            this->setItem(j, i, new_item);
        }
    }
}



// Update the font, position, text informations in the table with the given subtitles container
void MySubtitlesTable::loadSubtitles(QList<MySubtitles> subtitlesList, bool keepSelection) {

    QTableWidgetItem* start_time_item;
    QTableWidgetItem* end_time_item;
    QTableWidgetItem* text_item;

    if ( this->isEnabled() == true ) {

        // Save the selected row
        QList<QTableWidgetSelectionRange>  selected_ranges = this->selectedRanges();

        // First init the table with a number of rows equal to the number of subtitles + 20
        initStTable(subtitlesList.size() + 20);

        // Indicate that subtitles are loading to avoid all other operations
        mSubLoadding = true;

        mSubtitlesList.clear();

        for ( qint32 i = 0; i < subtitlesList.size(); i++ ) {

            start_time_item = this->item(mStCount, SUB_START_TIME_COL);
            start_time_item->setText( subtitlesList[i].startTime() );

            if ( this->updateStTime(start_time_item) != "" ) {
                //Errorr
                continue;
            }

            end_time_item = this->item(mStCount, SUB_END_TIME_COL);
            end_time_item->setText( subtitlesList[i].endTime() );

            if ( this->updateStTime(end_time_item) != "" ) {
                //Errorr
                continue;
            }

            this->showRow(mStCount);

            // If subtitle is valid, load it in the database
            mSubtitlesList.append( subtitlesList[i] ) ;

            text_item = this->item(mStCount, SUB_TEXT_COL);

            QString text = "";
            QString char_number_str = "";
            QString char_per_sec_str = "";
            qreal char_per_sec = 0.0;
            bool max_cher_exceeded= false;

            QList<TextLine> text_list = mSubtitlesList[i].text();

            for ( qint32 j = 0; j < text_list.size(); j++ ) {

                // If it's the second line, add a "line return" caractere
                if ( !text.isEmpty() ) {
                    text += "\n";
                    char_number_str += "\n";
                }

                TextLine text_line = text_list.at(j);
                QString plain_text = MyAttributesConverter::htmlToPlainText( text_line.Line() );

                // Calculate characters number per second
                char_per_sec += ( (qreal)plain_text.count() * (qreal)SEC_TO_MSEC ) / (qreal)MyAttributesConverter::timeStrHMStoMs( this->item(mStCount, SUB_DURATION_COL)->text() );
                // Count characters number for the line
                char_number_str += QString::number( plain_text.count() );

                text += plain_text;

                // Check if characters number per line is superior to specified
                if ( plain_text.count() > qApp->property("prop_MaxCharPerLine").toInt() ) {
                    max_cher_exceeded = true;
                }
            }

            // Update font and position parameters
            this->updateDatas(subtitlesList[i], i);

            // Convert characters per second in string
            char_per_sec_str += QString::number(char_per_sec, 'f', 1);

            // Dispaly in red if characters number is superior to specified
            if ( char_per_sec > qApp->property("prop_MaxCharPerSec").toReal() ) {
                this->item(mStCount, SUB_CHAR_PER_SEC_COL)->setForeground(QBrush(Qt::red));
            }
            else {
                this->item(mStCount, SUB_CHAR_PER_SEC_COL)->setForeground(QBrush(Qt::black));
            }

            if ( max_cher_exceeded ) {
                this->item(mStCount, SUB_CHARNUM_COL)->setForeground(QBrush(Qt::red));
            }
            else {
                this->item(mStCount, SUB_CHARNUM_COL)->setForeground(QBrush(Qt::black));
            }

            this->item(mStCount, SUB_CHARNUM_COL)->setText(char_number_str);
            this->item(mStCount, SUB_CHAR_PER_SEC_COL)->setText(char_per_sec_str);
            text_item->setText(text);

            // Increment the subtitles counter
            mStCount ++;
        }

        this->resizeRowsToContents();

        this->setEnabled(true);

        mPreviousIndex = -1;

        // Keep the same selection as before changment loading
        if ( keepSelection == true ) {

            // Change the selection mode to "Multiselection" to avoid "Shift" or "Ctrl" key modifiers conflict
            this->setSelectionMode(QAbstractItemView::MultiSelection);

            // If the current index always exist, keep it. Else set to last subtitle
            if ( mCurrentIndex < mStCount ) {
                // Nothing to do
            }
            else {

                mCurrentIndex = mStCount - 1;
                this->selectRow(mCurrentIndex);

                mSubLoadding = false;

                return;

            }

            // Reselect the row
            if ( !selected_ranges.isEmpty() ) {
                QList<QTableWidgetSelectionRange>::iterator it;
                for ( it = selected_ranges.begin(); it != selected_ranges.end(); ++it ) {

                    this->setRangeSelected(*it, true);
                }
            }

            this->setSelectionMode(QAbstractItemView::ExtendedSelection);
        }
        else {
            // Reset selection to 0
            mCurrentIndex = 0;

            if ( mStCount > 0 ) {
                this->setSelectionMode(QAbstractItemView::SingleSelection);
                this->selectRow(mCurrentIndex);
                this->setSelectionMode(QAbstractItemView::ExtendedSelection);
            }
        }

        mSubLoadding = false;
    }
}

// Get a container with all subtitles information from the table
QList<MySubtitles> MySubtitlesTable::saveSubtitles() {

    return mSubtitlesList;
}

// Compute the new sub "start time" at time corresponding to X frames from the current sub "end time"
bool MySubtitlesTable::insertNewSubAfterCurrent(MySubtitles &newSubtitle) {

    QTableWidgetItem* end_time_item;
    QTime time_base(0, 0, 0, 0);
    QTime end_time_HMS;
    qint32 end_time_ms;
    qint64 position_ms;

    if ( mStCount == 0 ) {
        position_ms = 0;
    }
    else {
        end_time_item = this->item(mCurrentIndex,SUB_END_TIME_COL);
        end_time_HMS = QTime::fromString(end_time_item->text(), "hh:mm:ss.zzz");
        end_time_ms = qAbs( end_time_HMS.msecsTo(time_base) );

        position_ms = end_time_ms + ( ( (qreal)SEC_TO_MSEC / qApp->property("prop_FrameRate_fps").toReal() ) * qApp->property("prop_SubMinInterval_frame").toReal() );
    }
    return insertNewSub(newSubtitle, position_ms);
}

// Insert a subtitle in the table
bool MySubtitlesTable::insertNewSub(MySubtitles &newSubtitle, qint64 positionMs) {

    QTableWidgetItem* start_time_item;
    QTableWidgetItem* end_time_item;
    QTime start_time_HMS;
    QTime end_time_HMS;
    qint32 start_time_ms;
    qint32 end_time_ms;
    QTime time_base(0, 0, 0, 0);

    // Retrieve index for the given time
    start_time_ms = positionMs;

    end_time_ms = start_time_ms + qApp->property("prop_SubMinDuration_ms").toInt();

    // Scale the end time function of the frame rate
    end_time_ms = MyAttributesConverter::roundToFrame(end_time_ms, qApp->property("prop_FrameRate_fps").toReal());

    if ( ( mPositionMsToStIndex[start_time_ms] == -1 ) && ( mPositionMsToStIndex[end_time_ms] == -1 ) ) {

        // Only 10 minutes (60000ms) remaining in the lookup table. Add 30 minutes.
        if ( end_time_ms > mPositionMsToStIndex.size() - 600000 ) {
            QVector<qint32> temp_vector;
            temp_vector.resize(1800000);
            temp_vector.fill(-1);
            mPositionMsToStIndex += temp_vector;
        }

        // Retrieive the first empty item, set the start/end time and text
        start_time_HMS = time_base.addMSecs(start_time_ms);
        start_time_item = this->item(mStCount, SUB_START_TIME_COL);
        start_time_item->setText(start_time_HMS.toString("hh:mm:ss.zzz"));

        QString status_msg = this->updateStTime(start_time_item);
        if ( status_msg !="" ) {
            mErrorMsg = status_msg;
            return false;
        }

        end_time_HMS = time_base.addMSecs(end_time_ms);
        end_time_item = this->item(mStCount, SUB_END_TIME_COL);
        end_time_item->setText(end_time_HMS.toString("hh:mm:ss.zzz"));

        status_msg = this->updateStTime(start_time_item);
        if ( status_msg !="" ) {
            mErrorMsg = status_msg;
            return false;
        }

        this->showRow(mCurrentIndex);

        // Add new rows to the table when empty rows remaining are less than 10
        if ( ( this->rowCount() - mStCount ) < 10 ) {

           this->addRows(100, this->rowCount());
        }

        // Insert the new subtitle in the subtitles list
        mSubtitlesList.insert(mCurrentIndex, newSubtitle);

        mInsertingSub = true;
        this->updateText(newSubtitle.text());
        this->updateDatas(newSubtitle);
        mInsertingSub = false;

    }
    else {
        mErrorMsg = "Not enought space to insert subtile";
        return false;
    }

    newSubtitle.setStartTime(this->item(mCurrentIndex, SUB_START_TIME_COL)->text());
    newSubtitle.setEndTime(this->item(mCurrentIndex, SUB_END_TIME_COL)->text());
    mSubtitlesList[mCurrentIndex].setStartTime(this->item(mCurrentIndex, SUB_START_TIME_COL)->text());
    mSubtitlesList[mCurrentIndex].setEndTime(this->item(mCurrentIndex, SUB_END_TIME_COL)->text());

    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->selectRow(mCurrentIndex);
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);

    return true;
}

// Remove the current sub
void MySubtitlesTable::deleteCurrentSub() {

    qint32 start_time_ms;

    start_time_ms = mPositionMsToStIndex.indexOf(mCurrentIndex);

    if ( start_time_ms >= 0 ) {

        // Remove from subtitles list
        mSubtitlesList.removeAt(mCurrentIndex);

        // Remove the index in the lookup table
        for ( qint32 i = start_time_ms; i < mPositionMsToStIndex.size(); i++ ) {

            if ( mPositionMsToStIndex[i] == mCurrentIndex ) {
                mPositionMsToStIndex[i] = -1;
            }
            else if ( mPositionMsToStIndex[i] != -1 ) {
                mPositionMsToStIndex[i] = mPositionMsToStIndex[i] - 1;
            }
        }

        // Re-compute all subtitles index and display it
        for ( qint32 i = mCurrentIndex; i < mStCount; i++ ) {
            this->item(i, SUB_NUM_COL)->setText(QString::number(i));
        }

        mStCount = mStCount - 1;
        mPreviousIndex = -1;

        this->clearSelection();

        this->removeRow(mCurrentIndex);

        this->setSelectionMode(QAbstractItemView::SingleSelection);
        if ( mCurrentIndex < mStCount ) {
            this->selectRow(mCurrentIndex);
        }
        else if ( mStCount > 0 ) {
            this->selectRow(mStCount - 1);
        }
        this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    }
}

bool MySubtitlesTable::isNewEntry(qint64 positionMs) {

    qint32 row_index;

    row_index = mPositionMsToStIndex[positionMs];

    // The subtitle exist
    if (row_index != -1) {
        return false;
    }
    else { // The subtitle not exist
        return true;
    }
}

// Update the text in the table
void MySubtitlesTable::updateText(QList<TextLine> textLines) {

    qint32 row_index;
    QString text = "";
    QString char_number_line_str = "";
    QString char_per_sec_str = "";
    TextLine new_text_line;
    qint16 char_number_total = 0;
    qreal char_per_sec = 0.0;
    qint32 sub_duration_ms;
    QString plain_text;
    bool max_cher_exceeded = false;

    // Retrieve the index for this given time
    row_index = mCurrentIndex;

    sub_duration_ms = MyAttributesConverter::timeStrHMStoMs( this->item(row_index, SUB_DURATION_COL)->text() );

    // If a line was added or removed, replace all current subtitle lines in the subtitles list
    if ( mSubtitlesList[mCurrentIndex].text().count() != textLines.count() ) {
        mSubtitlesList[mCurrentIndex].text().clear();
        mSubtitlesList[mCurrentIndex].setText(textLines);
    }

    for ( qint32 i = 0; i < textLines.count(); i++ ) {

        // If it's the second line, add a "line return" caractere
        if ( i > 0 ) {
            text += "\n";
            char_number_line_str += "\n";
        }

        new_text_line = textLines.at(i);
        plain_text = MyAttributesConverter::htmlToPlainText(new_text_line.Line());

        // Set the line in the subtitles list
        mSubtitlesList[mCurrentIndex].text()[i].setLine(new_text_line.Line());

        char_number_total += plain_text.count();
        // Calculate characters number per second
        char_per_sec += ( ( (qreal)plain_text.count() * (qreal)SEC_TO_MSEC ) / (qreal)sub_duration_ms );
        // Count characters number for the line
        char_number_line_str += QString::number( plain_text.count() );

        text += plain_text;

        if ( plain_text.count() > qApp->property("prop_MaxCharPerLine").toInt() ) {
            max_cher_exceeded = true;
        }
    }

    // Change subtitle duration in function of the number of characters
    // and the maximum number of char / sec specified
    qint32 new_sub_duration_ms = qRound( ( (qreal)char_number_total / qApp->property("prop_MaxCharPerSec").toReal() ) * (qreal)SEC_TO_MSEC );

    if ( mSubtitlesList[mCurrentIndex].isDurationAuto() ) {

        // Case of auto duration recovery
        // Set new duration equal to the minimum duration specified
        if ( ( new_sub_duration_ms < qApp->property("prop_SubMinDuration_ms").toInt() ) &&
             ( new_sub_duration_ms < sub_duration_ms ) ) {

            new_sub_duration_ms = qApp->property("prop_SubMinDuration_ms").toInt();
        }

        // Change duration only if the new duration is superior to the minimum duration specified
        if ( new_sub_duration_ms >= qApp->property("prop_SubMinDuration_ms").toInt() ) {

            qint32 new_end_time_ms = MyAttributesConverter::timeStrHMStoMs( this->item(row_index, SUB_START_TIME_COL)->text() ) + new_sub_duration_ms;

            // Scale the positon in function of the framerate
            new_end_time_ms = MyAttributesConverter::roundToFrame(new_end_time_ms, qApp->property("prop_FrameRate_fps").toReal());

            if ( ( mPositionMsToStIndex[new_end_time_ms] == -1 ) || ( mPositionMsToStIndex[new_end_time_ms] == row_index ) ) {

                // Compute the minimum interval between two subtitles
                qint32 interval_ms = ( (qreal)SEC_TO_MSEC / qApp->property("prop_FrameRate_fps").toReal() ) * qApp->property("prop_SubMinInterval_frame").toReal();

                if ( this->setEndTime(new_end_time_ms, row_index, true, interval_ms ) ) {

                    // Calculate characters number per second
                    char_per_sec = ( ( (qreal)char_number_total * (qreal)SEC_TO_MSEC ) / (qreal)new_sub_duration_ms );

                    // Char / sec ok -> write in black
                    this->item(row_index, SUB_CHAR_PER_SEC_COL)->setForeground(QBrush(Qt::black));
                }
            }
        }
    }

    // Dispaly in red if characters number is superior to specified
    if ( char_per_sec > qApp->property("prop_MaxCharPerSec").toReal() ) {
        this->item(row_index, SUB_CHAR_PER_SEC_COL)->setForeground(QBrush(Qt::red));
    }
    else {
        this->item(row_index, SUB_CHAR_PER_SEC_COL)->setForeground(QBrush(Qt::black));
    }

    // At lest one line has more characters than allowed
    if ( max_cher_exceeded ) {
        this->item(row_index, SUB_CHARNUM_COL)->setForeground(QBrush(Qt::red));
    }
    else {
        this->item(row_index, SUB_CHARNUM_COL)->setForeground(QBrush(Qt::black));
    }

    // Convert characters per second in string
    char_per_sec_str += QString::number(char_per_sec, 'f', 1);

    this->item(row_index, SUB_CHARNUM_COL)->setText(char_number_line_str);
    this->item(row_index, SUB_CHAR_PER_SEC_COL)->setText(char_per_sec_str);
    this->item(row_index, SUB_TEXT_COL)->setText(text);

    this->resizeRowToContents(row_index);
}

// Update the font, position informations in the table with the given subtitle data
void MySubtitlesTable::updateDatas(MySubtitles subtitle, qint32 index) {

    // Retrieve the current index
    if ( index < 0 ) {
        index = mCurrentIndex;
    }

    if ( index != -1) {

        TextLine text_line;
        TextFont text_font;
        TextLine new_text_line;

        for ( qint32 i = 0; i < subtitle.text().size(); i++) {

            text_line = mSubtitlesList[index].text()[i];
            text_font = text_line.Font();

            new_text_line = subtitle.text().at(i);

            // Check if line parameters is set. If not, set a default parameter
            if ( new_text_line.textVPosition().isEmpty() ) {

                QString v_position_str = TEXT1_VPOSITION_DEFAULT_VALUE;
                qreal v_position = (qreal)v_position_str.toFloat();
                v_position = v_position * (qreal)( subtitle.text().count() - i);

                text_line.setTextVPosition(QString::number(v_position, 'f', 1));
            }
            else {
                text_line.setTextVPosition(new_text_line.textVPosition());
            }

            if ( new_text_line.textVAlign().isEmpty() ) {

                text_line.setTextVAlign(TEXT1_VALIGN_DEFAULT_VALUE);
            }
            else {
                text_line.setTextVAlign(new_text_line.textVAlign());
            }

            if ( new_text_line.textHPosition().isEmpty() ) {

                text_line.setTextHPosition(TEXT1_HPOSITION_DEFAULT_VALUE);
            }
            else {
                text_line.setTextHPosition(new_text_line.textHPosition());
            }

            if ( new_text_line.textHAlign().isEmpty() ) {

                text_line.setTextHAlign(TEXT1_HALIGN_DEFAULT_VALUE);
            }
            else {
                text_line.setTextHAlign(new_text_line.textHAlign());
            }

            if ( new_text_line.textDirection().isEmpty() ) {

                text_line.setTextDirection(TEXT1_DIRECTION_DEFAULT_VALUE);
            }
            else {
                text_line.setTextDirection(new_text_line.textDirection());
            }

            if ( new_text_line.Font().fontId().isEmpty() ) {

                text_font.setFontId(FONT_ID_DEFAULT_VALUE);
            }
            else {
                text_font.setFontId(new_text_line.Font().fontId());
            }

            if ( new_text_line.Font().fontColor().isEmpty() ) {

                text_font.setFontColor(FONT_COLOR_DEFAULT_VALUE);
            }
            else {
                text_font.setFontColor(new_text_line.Font().fontColor());
            }

            if ( new_text_line.Font().fontShadowEffect().isEmpty() ) {

                text_font.setFontShadowEffect(FONT_SHADOW_EFFECT_DEFAULT_VALUE);
            }
            else {
                text_font.setFontShadowEffect(new_text_line.Font().fontShadowEffect());
            }

            if ( new_text_line.Font().fontShadowEffectColor().isEmpty() ) {

                text_font.setFontShadowEffectColor(FONT_SHADOW_EFFECT_COLOR_DEFAULT_VALUE);
            }
            else {
                text_font.setFontShadowEffectColor(new_text_line.Font().fontShadowEffectColor());
            }

            if ( new_text_line.Font().fontBorderEffect().isEmpty() ) {

                text_font.setFontBorderEffect(FONT_BORDER_EFFECT_DEFAULT_VALUE);
            }
            else {
                text_font.setFontBorderEffect(new_text_line.Font().fontBorderEffect());
            }

            if ( new_text_line.Font().fontBorderEffectColor().isEmpty() ) {

                text_font.setFontBorderEffectColor(FONT_BORDER_EFFECT_COLOR_DEFAULT_VALUE);
            }
            else {
                text_font.setFontBorderEffectColor(new_text_line.Font().fontBorderEffectColor());
            }

            if ( new_text_line.Font().fontItalic().isEmpty() ) {

                text_font.setFontItalic(FONT_ITALIC_DEFAULT_VALUE);
            }
            else {
                text_font.setFontItalic(new_text_line.Font().fontItalic());
            }

            if ( new_text_line.Font().fontScript().isEmpty() ) {

                text_font.setFontScript(FONT_SCRIPT_DEFAULT_VALUE);
            }
            else {
                text_font.setFontScript(new_text_line.Font().fontScript());
            }

            if ( new_text_line.Font().fontSize().isEmpty() ) {

                text_font.setFontSize(FONT_SIZE_DEFAULT_VALUE);
            }
            else {
                text_font.setFontSize(new_text_line.Font().fontSize());
            }

            if ( new_text_line.Font().fontUnderlined().isEmpty() ) {

                text_font.setFontUnderlined(FONT_UNDERLINED_DEFAULT_VALUE);
            }
            else {
                text_font.setFontUnderlined(new_text_line.Font().fontUnderlined());
            }

            text_line.setFont(text_font);
            mSubtitlesList[index].text()[i] = text_line;
        }
    }
}

// Re-direct the items modifications
void MySubtitlesTable::updateItem(QTableWidgetItem *item) {

    if ( ( item->column() == SUB_START_TIME_COL ) || ( item->column() == SUB_END_TIME_COL ) ) {

        // Bypass during the table init
        if ( ( item->text() != "") && ( item->text() != "New Entry" ) ) {

            if ( updateStTime(item) != "" ) {

                //this->editItem(item);
            }
        }
    }
}

// Set "end time" to provided index
// force : if new end time > next sub start time, force new endtime = next sub start time - intervalMs
// intervalMs : interval min between two subtitle
bool MySubtitlesTable::setEndTime(qint64 positionMs, qint32 stIndex, bool force, qint32 intervalMs) {

    qint64 start_time_ms;
    qint64 end_time_ms;
    qint64 next_start_time_ms;
    QTime time_base(0, 0, 0, 0);
    QTableWidgetItem* end_time_item;

    start_time_ms = MyAttributesConverter::timeStrHMStoMs( this->item(stIndex, SUB_START_TIME_COL)->text() );
    end_time_ms = positionMs;

    // Check if a new end time entry is > to the current item start time
    if ( end_time_ms > start_time_ms ) {

        // If next sub exist
        if ( (stIndex + 1) < mStCount ) {

            next_start_time_ms = MyAttributesConverter::timeStrHMStoMs( this->item(stIndex + 1, SUB_START_TIME_COL)->text() );

            // Check if next sub start time is superior to the new end time
             if ( ( next_start_time_ms - intervalMs ) <= end_time_ms ) {

                 if ( force == true ) {
                     end_time_ms = next_start_time_ms - intervalMs;
                 }
                 else {
                    mErrorMsg = "Subtitle " +QString::number(stIndex) +" end time > subtitle " +QString::number(stIndex + 1) +" start time";
                    return false;
                 }
            }
        }

        end_time_item = this->item(stIndex, SUB_END_TIME_COL);
        end_time_item->setText( time_base.addMSecs(end_time_ms).toString("hh:mm:ss.zzz") );

        // Try to update the current item end time
        QString status_msg = this->updateStTime(end_time_item);
        if ( status_msg != "" ) {
            mErrorMsg = status_msg;
            return false;
        }
        else if ( !mInsertingSub ) {
            emit endTimeCodeChanged(stIndex, end_time_ms);
        }
    }
    else {
        mErrorMsg = "Subtitle " +QString::number(stIndex) +" end time < subtitle " +QString::number(stIndex) +" start time";
        return false;
    }

    mSubtitlesList[stIndex].setEndTime( time_base.addMSecs(end_time_ms).toString("hh:mm:ss.zzz") );

    return true;
}

// Set "start time" to provided index
// force : if new start time < previous sub end time, force new start time = previous sub end time + intervalMs
// intervalMs : interval min between two subtitle
bool MySubtitlesTable::setStartTime(qint64 positionMs, qint32 stIndex, bool force, qint32 intervalMs) {

    QTime start_time_HMS;
    QTime end_time_HMS;
    QTime previous_end_time_HMS;
    QTime time_base(0, 0, 0, 0);
    QTableWidgetItem* start_time_item;


    start_time_HMS = time_base.addMSecs(positionMs);
    end_time_HMS = QTime::fromString(this->item(stIndex, SUB_END_TIME_COL)->text(), "hh:mm:ss.zzz");

    // Check if a new start time entry is < to the current item end time
    if ( end_time_HMS > start_time_HMS ) {

        // Check if previous sub exist
        if ( (stIndex - 1) > 0 ) {

            previous_end_time_HMS = QTime::fromString(this->item(stIndex - 1, SUB_END_TIME_COL)->text(), "hh:mm:ss.zzz");

            // Check if previous sub end time is inferior to the new start time
            if ( previous_end_time_HMS.addMSecs(intervalMs) >= start_time_HMS ) {

                if ( force == true ) {

                    start_time_HMS = previous_end_time_HMS.addMSecs(intervalMs);

                }
                else {
                    mErrorMsg = "Subtitle " +QString::number(stIndex) +" start time < subtitle " +QString::number(stIndex - 1) +" end time";
                    return false;
                }
            }
        }

        start_time_item = this->item(stIndex, SUB_START_TIME_COL);
        start_time_item->setText(start_time_HMS.toString("hh:mm:ss.zzz"));

        // Try to update the current item start time
        QString status_msg = this->updateStTime(start_time_item);
        if ( status_msg != "" ) {
            mErrorMsg = status_msg;
            return false;
        }
    }
    else {
        mErrorMsg = "Subtitle " +QString::number(stIndex) +" start time > subtitle " +QString::number(stIndex) +" end time";
        return false;
    }

    mSubtitlesList[stIndex].setStartTime( start_time_HMS.toString("hh:mm:ss.zzz") );

    return true;
}

// Manage time change of an start/end time item
QString MySubtitlesTable::updateStTime(QTableWidgetItem* time_item) {

    QTableWidgetItem* start_time_item;
    QTableWidgetItem* end_time_item;
    QTableWidgetItem* duration_time_item;
    QTime time_base(0, 0, 0, 0);
    QTime start_time_HMS;
    QTime end_time_HMS;
    QTime duration_time_HMS;
    qint32 start_time_ms;
    qint32 end_time_ms;
    qint32 duration_time_ms;
    qint32 row_index;
    qint32 new_row_index;

    bool compute_duration = false;

    // Retrieive the modified row index
    row_index = time_item->row();

    if ( time_item->column() == SUB_START_TIME_COL ) {

        start_time_item = time_item;
        end_time_item = this->item(row_index, SUB_END_TIME_COL);

        start_time_HMS = QTime::fromString(start_time_item->text(), "hh:mm:ss.zzz");

        // Check if the start time entry is valid
        if ( start_time_HMS.isValid() ) {

            end_time_HMS = QTime::fromString(end_time_item->text(), "hh:mm:ss.zzz");

            // Check if the end time entry is valid
            if ( end_time_HMS.isValid() ) {
                // Time is valid, continue to manage the new entry
                compute_duration = true;
            }
        }
        else { // The new start entry is not a valid entry
            return "The new start entry is not a valid entry - hh:mm:ss.zzz";
        }
    }
    else if ( time_item->column() == SUB_END_TIME_COL ) {

        end_time_item = time_item;
        start_time_item = this->item(row_index, SUB_START_TIME_COL);

        end_time_HMS = QTime::fromString(end_time_item->text(), "hh:mm:ss.zzz");

        // Check if the end time entry is valid
        if ( end_time_HMS.isValid() ) {

            start_time_HMS = QTime::fromString(start_time_item->text(), "hh:mm:ss.zzz");

            // Check if the start time entry is valid
            if ( start_time_HMS.isValid() ) {
                // Time is valid, continue to manage the new entry
                compute_duration = true;
            }
        }
        else { // The new end entry is not a valid entry
            return "The new end entry is not a valid entry - hh:mm:ss.zzz";
        }
    }

    if ( compute_duration ) {

        qint32 first_index;
        qint32 last_index;

        // Compute the duration
        start_time_ms = qAbs( start_time_HMS.msecsTo(time_base) );
        end_time_ms = qAbs( end_time_HMS.msecsTo(time_base) );

        duration_time_ms = end_time_ms - start_time_ms;
        duration_time_HMS = time_base.addMSecs(duration_time_ms);

        duration_time_item = this->item(row_index, SUB_DURATION_COL);
        duration_time_item->setText(duration_time_HMS.toString("hh:mm:ss.zzz"));

        // When loading a subtitle file, it's not necessary to sort items
        if ( mSubLoadding == true ) {

            // No more place in the lookup table. Add 30 minutes (1800000 ms).
            if ( end_time_ms >= mPositionMsToStIndex.size() ) {
                QVector<qint32> temp_vector;
                temp_vector.resize(1800000);
                temp_vector.fill(-1);
                mPositionMsToStIndex += temp_vector;
            }

            // Just insert the new index in the lookup table
            for ( qint32 i = start_time_ms; i <= end_time_ms; i++ ) {

                mPositionMsToStIndex[i] = mStCount;
            }

            // Display the new subtitle index
            this->item(mStCount, SUB_NUM_COL)->setText(QString::number(mStCount + 1));
        }
        else { // A subtitle is incerted or modified by user

            // If it's new entry
            if ( this->item(row_index, SUB_NUM_COL)->text() == "" ) {

                // Increment the subtitles items counter
                mStCount ++;
                // Sort the st table to add the new subtitle at good place
                this->sortItems(SUB_START_TIME_COL, Qt::AscendingOrder);

                new_row_index = 0;

                // Retrieive the item index situated before the start time of the new item
                for ( qint32 i = start_time_ms; i >= 0; i--) {
                    if ( mPositionMsToStIndex[i] != -1) {
                        new_row_index = mPositionMsToStIndex[i] + 1;
                        break;
                    }
                }

                // Insert the new index in the lookup table
                for ( qint32 i = start_time_ms; i < mPositionMsToStIndex.size(); i++ ) {

                    if ( ( i >= start_time_ms ) && ( i <= end_time_ms) ) {
                        mPositionMsToStIndex[i] = new_row_index;
                    }
                    else if ( mPositionMsToStIndex[i] != -1 ) {
                        mPositionMsToStIndex[i] = mPositionMsToStIndex[i] + 1;
                    }
                }

                // Re-compute all subtitles index and display it
                for ( qint32 i = new_row_index; i < mStCount; i++ ) {
                    this->item(i, SUB_NUM_COL)->setText(QString::number(i + 1));
                }

                mCurrentIndex = new_row_index;
            }
            else { // Resize entry

                // If start time was modified
                if ( time_item->column() == SUB_START_TIME_COL ) {

                    // In lookup table, retrieve first occurence of the modified item
                    first_index = mPositionMsToStIndex.indexOf(row_index, 0);

                    // If this first reference is after the new start time, new subtitle duration is longer.
                    // Just need add new index reference for the new time.
                    // Else, new subtitle duration is shorter. Need to erase index reference for the old time.
                    if ( ( first_index > start_time_ms ) || ( first_index < 0 ) ) {
                        first_index = start_time_ms;
                    }
                    last_index = end_time_ms;
                } //If end time was modified
                else if ( time_item->column() == SUB_END_TIME_COL ) {

                    // In lookup table, retrieve last occurence of the modified item
                    last_index = mPositionMsToStIndex.lastIndexOf(row_index, -1);

                    // If this last reference is before the new end time, new subtitle duration is longer.
                    // Just need add new index reference for the new time.
                    // Else, new subtitle duration is shorter. Need to erase index reference for the old time.
                    if ( last_index < end_time_ms ) {
                        last_index = end_time_ms;
                    }
                    first_index = start_time_ms;
                }

                // Add or erase index of the lookup table
                for ( qint32 i = first_index; i <= last_index; i++ ) {

                    if ( ( i >= start_time_ms ) && ( i <= end_time_ms) ) {
                        mPositionMsToStIndex[i] = row_index;
                    }
                    else {
                        mPositionMsToStIndex[i] = -1;
                    }
                }
            }
        }
    }
    return "";
}

// Send time information when the user select a new item in the table
void MySubtitlesTable::updateSelectedItem() {

    qint32 row_index;
    qint32 start_time_ms;
    QTime start_time_HMS;
    QTime time_base(0, 0, 0, 0);

    // Center scroll area at the selected item line
    QList<QTableWidgetItem*> selected_items = this->selectedItems();

    mSelectedIndex.clear();

    if ( !selected_items.isEmpty() ) {

        row_index = selected_items.first()->row();

        // Save the selected row
        QList<QTableWidgetItem*> ::iterator it;
        for ( it = selected_items.begin(); it != selected_items.end(); it+=7 ) {

            QTableWidgetItem* item = *it;
            if ( item->row() < mStCount ) {
                mSelectedIndex.append(item->row());

                if ( item->row() == mCurrentIndex ) {
                    row_index = item->row();
                }
            }
            else {
                break;
            }
        }

        // If the row was selected by user, send the new time information
        if ( mSelectedBySoft == false ) {

            mSelectedByUser = true;

            this->scrollToItem( this->item(row_index, SUB_NUM_COL), QAbstractItemView::PositionAtCenter);

            // If the selected item start time is valid
            QString start_time_str = this->item(row_index, SUB_START_TIME_COL)->text();

            start_time_HMS = QTime::fromString(start_time_str, "hh:mm:ss.zzz");

            if ( start_time_HMS.isValid() ) {

                // Emit a signal sending this start time
                start_time_ms = qAbs( start_time_HMS.msecsTo(time_base) );
                emit itemSelectionChanged(start_time_ms);
            }
        } // If the row was selected by software, just reset the flags
        else {
            mSelectedByUser = false;
            mSelectedBySoft = false;
        }
    }
}


// Check if display need to be refresh for a given time.
// Send the subtitle information if refresh is needed
void MySubtitlesTable::updateStDisplay(qint64 positionMs) {

    MySubtitles subtitle;

    // Position doesn't exist in the lookup table. Resize lookup table to positionMs + 10 minutes (600000 ms)
    if ( positionMs >= mPositionMsToStIndex.size() ) {
        QVector<qint32> temp_vector;
        temp_vector.resize(positionMs - mPositionMsToStIndex.size() + 600000);
        temp_vector.fill(-1);
        mPositionMsToStIndex += temp_vector;
    }

    qint32 st_index = mPositionMsToStIndex[positionMs];

    // Check in the lookup table if there is a valid subtitle index for this position
    if ( st_index != mPreviousIndex ) {
        if ( st_index != -1 ) {
            // Retrieve the subtitle infos
            subtitle = getSubInfos(st_index);
            // Change the current index
            mCurrentIndex = st_index;
            // If update was foced by software, highlight the row corresponding to the index
            if ( mSelectedByUser == false ) {
                mSelectedBySoft = true;

                this->setSelectionMode(QAbstractItemView::SingleSelection);
                this->selectRow(mCurrentIndex);
                this->setSelectionMode(QAbstractItemView::ExtendedSelection);
            }
        }

        // Send a signal to display the subtilte or nothing if it's empty
        emit newTextToDisplay(subtitle);
    }

    mSelectedByUser = false;
    mSelectedBySoft = false;
    mPreviousIndex = st_index;
}

// Get the sustitles data for a given index in the table
MySubtitles MySubtitlesTable::getSubInfos(qint32 stIndex) {

    MySubtitles new_subtitle;

    if ( stIndex >= 0 ) {
        return mSubtitlesList[stIndex];
    }

    // Return a MySubtitles container
    return new_subtitle;
}

void MySubtitlesTable::setDurationAuto(qint32 index, bool state) {

    if ( ( index >= 0 ) && (index < mStCount) ) {

       mSubtitlesList[index].setDurationAuto(state);
    }
}

QList<qint32> MySubtitlesTable::selectedIndex() {
    return mSelectedIndex;
}

qint32 MySubtitlesTable::currentIndex() {
    return mCurrentIndex;
}

qint32 MySubtitlesTable::subtitlesCount() {
    return mStCount;
}

QString MySubtitlesTable::errorMsg() {
    return mErrorMsg;
}
