#include "mysubtitlestable.h"
#include "mysubtitles.h"
#include <QTime>
#include <QTextDocumentFragment>
#include "mysettings.h"
#include <QCoreApplication>

// Columns numbers define
#define SUB_NUM_COL 0
#define SUB_START_TIME_COL 1
#define SUB_END_TIME_COL 2
#define SUB_DURATION_COL 3
#define SUB_TEXT_COL 4

#define SUB_NBR_OF_LINES_COL 5

#define SUB_LINE1_COL 6
#define SUB_VPOSITION1_COL 7
#define SUB_VALIGN1_COL 8
#define SUB_HPOSITION1_COL 9
#define SUB_HALIGN1_COL 10
#define SUB_DIRECTION1_COL 11
#define SUB_FONT_ID1_COL 12
#define SUB_FONT_COLOR1_COL 13
#define SUB_FONT_EFFECT1_COL 14
#define SUB_FONT_EFFECTCOLOR1_COL 15
#define SUB_FONT_ITALIC1_COL 16
#define SUB_FONT_UNDERLINED1_COL 17
#define SUB_FONT_SCRIPT1_COL 18
#define SUB_FONT_SIZE1_COL 19

#define SUB_LINE2_COL 20
#define SUB_VPOSITION2_COL 21
#define SUB_VALIGN2_COL 22
#define SUB_HPOSITION2_COL 23
#define SUB_HALIGN2_COL 24
#define SUB_DIRECTION2_COL 25
#define SUB_FONT_ID2_COL 26
#define SUB_FONT_COLOR2_COL 27
#define SUB_FONT_EFFECT2_COL 28
#define SUB_FONT_EFFECTCOLOR2_COL 29
#define SUB_FONT_ITALIC2_COL 30
#define SUB_FONT_UNDERLINED2_COL 31
#define SUB_FONT_SCRIPT2_COL 32
#define SUB_FONT_SIZE2_COL 33

// Subtitle default duration in millisecond
#define SUB_DEFAULT_DURATION_MS 1000

// Convert second in milliseconds
#define SEC_TO_MSEC 1000
// Number of frame per seconde
#define FRAME_PER_SEC 25
// Minimum interval between two subtitles (in frame)
#define SUB_MIN_INTERVAL_FRAME 5

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

    // Init the variables
    mCurrentPositionMs = 0;
    mPreviousIndex = 0;
    mStCount = 0;
    mCurrentIndex = 0;

    // Init flags
    mSubLoadding = false;
    mSelectedBySoft = false;
    mSelectedByUser = false;

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
            if ( i == SUB_START_TIME_COL ) {
                new_item->setText("New Entry");
                new_item->setTextAlignment(Qt::AlignCenter);
            }
            else if ( i == SUB_TEXT_COL ){
                new_item->setText("");
                new_item->setTextAlignment(Qt::AlignLeft);
            }
            else if ( i == SUB_NBR_OF_LINES_COL ) {
                new_item->setText("0");
                new_item->setTextAlignment(Qt::AlignCenter);
            }
            else {
                new_item->setText("");
                new_item->setTextAlignment(Qt::AlignCenter);
            }
            this->setItem(j, i, new_item);

            // Hide columns useless for the user
            if ( i >= SUB_NBR_OF_LINES_COL ) {
                this->setColumnHidden(i, true);
            }
        }
    }
}

// Update the font, position, text informations in the table with the given subtitles container
void MySubtitlesTable::loadSubtitles(QList<MySubtitles> subtitlesList) {

    QTableWidgetItem* start_time_item;
    QTableWidgetItem* end_time_item;
    QTableWidgetItem* text_item;

    QTableWidgetItem* line1_item;
    QTableWidgetItem* vposition1_item;
    QTableWidgetItem* valign1_item;
    QTableWidgetItem* hposition1_item;
    QTableWidgetItem* halign1_item;
    QTableWidgetItem* direction1_item;
    QTableWidgetItem* font_id1_item;
    QTableWidgetItem* font_color1_item;
    QTableWidgetItem* font_effect1_item;
    QTableWidgetItem* font_effectcolor1_item;
    QTableWidgetItem* font_italic1_item;
    QTableWidgetItem* font_underlined1_item;
    QTableWidgetItem* font_script1_item;
    QTableWidgetItem* font_size1_item;
    QTableWidgetItem* line2_item;
    QTableWidgetItem* vposition2_item;
    QTableWidgetItem* valign2_item;
    QTableWidgetItem* hposition2_item;
    QTableWidgetItem* halign2_item;
    QTableWidgetItem* direction2_item;
    QTableWidgetItem* font_id2_item;
    QTableWidgetItem* font_color2_item;
    QTableWidgetItem* font_effect2_item;
    QTableWidgetItem* font_effectcolor2_item;
    QTableWidgetItem* font_italic2_item;
    QTableWidgetItem* font_underlined2_item;
    QTableWidgetItem* font_script2_item;
    QTableWidgetItem* font_size2_item;

    if ( this->isEnabled() == true ) {

        // First init the table with a number of rows equal to the number of subtitles + 20
        initStTable(subtitlesList.size() + 20);

        // Indicate that subtitles are loading to avoid all other operations
        mSubLoadding = true;

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

            text_item = this->item(mStCount, SUB_TEXT_COL);

            QString text = "";
            QList<TextLine> text_list = subtitlesList[i].text();
            for ( qint32 j = 0; j < text_list.size(); j++ ) {

                // If it's the second line, add a "line return" caractere
                if ( !text.isEmpty() ) {
                    text += "\n";
                }
                TextLine text_line = text_list.at(j);
                text += text_line.Line();

                // Line 1
                if ( j == 0 ) {
                    this->item(mStCount, SUB_NBR_OF_LINES_COL)->setText("1");
                    line1_item  = this->item(mStCount, SUB_LINE1_COL);
                    line1_item->setText( text_line.Line() );

                    vposition1_item  = this->item(mStCount, SUB_VPOSITION1_COL);
                    if ( text_line.textVPosition().isEmpty() ) {
                        //TO DO
                    }
                    else {
                        vposition1_item->setText( text_line.textVPosition() );
                    }

                    valign1_item  = this->item(mStCount, SUB_VALIGN1_COL);
                    valign1_item->setText( text_line.textVAlign() );
                    hposition1_item  = this->item(mStCount, SUB_HPOSITION1_COL);
                    hposition1_item->setText( text_line.textHPosition() );
                    halign1_item  = this->item(mStCount, SUB_HALIGN1_COL);
                    halign1_item->setText( text_line.textHAlign() );
                    direction1_item  = this->item(mStCount, SUB_DIRECTION1_COL);
                    direction1_item->setText( text_line.textDirection() );
                    font_id1_item  = this->item(mStCount, SUB_FONT_ID1_COL);
                    font_id1_item->setText( text_line.Font().fontId() );
                    font_color1_item  = this->item(mStCount, SUB_FONT_COLOR1_COL);
                    font_color1_item->setText( text_line.Font().fontColor() );
                    font_effect1_item  = this->item(mStCount, SUB_FONT_EFFECT1_COL);
                    font_effect1_item->setText( text_line.Font().fontEffect() );
                    font_effectcolor1_item  = this->item(mStCount, SUB_FONT_EFFECTCOLOR1_COL);
                    font_effectcolor1_item->setText( text_line.Font().fontEffectColor() );
                    font_italic1_item  = this->item(mStCount, SUB_FONT_ITALIC1_COL);
                    font_italic1_item->setText( text_line.Font().fontItalic() );
                    font_underlined1_item  = this->item(mStCount, SUB_FONT_UNDERLINED1_COL);
                    font_underlined1_item->setText( text_line.Font().fontUnderlined() );
                    font_script1_item  = this->item(mStCount, SUB_FONT_SCRIPT1_COL);
                    font_script1_item->setText( text_line.Font().fontScript() );
                    font_size1_item  = this->item(mStCount, SUB_FONT_SIZE1_COL);
                    font_size1_item->setText( text_line.Font().fontSize() );
                } // Line 2
                else if ( j == 1 ) {
                    this->item(mStCount, SUB_NBR_OF_LINES_COL)->setText("2");
                    line2_item  = this->item(mStCount, SUB_LINE2_COL);
                    line2_item->setText( text_line.Line() );
                    vposition2_item  = this->item(mStCount, SUB_VPOSITION2_COL);
                    vposition2_item->setText( text_line.textVPosition() );
                    valign2_item  = this->item(mStCount, SUB_VALIGN2_COL);
                    valign2_item->setText( text_line.textVAlign() );
                    hposition2_item  = this->item(mStCount, SUB_HPOSITION2_COL);
                    hposition2_item->setText( text_line.textHPosition() );
                    halign2_item  = this->item(mStCount, SUB_HALIGN2_COL);
                    halign2_item->setText( text_line.textHAlign() );
                    direction2_item  = this->item(mStCount, SUB_DIRECTION2_COL);
                    direction2_item->setText( text_line.textDirection() );
                    font_id2_item  = this->item(mStCount, SUB_FONT_ID2_COL);
                    font_id2_item->setText( text_line.Font().fontId() );
                    font_color2_item  = this->item(mStCount, SUB_FONT_COLOR2_COL);
                    font_color2_item->setText( text_line.Font().fontColor() );
                    font_effect2_item  = this->item(mStCount, SUB_FONT_EFFECT2_COL);
                    font_effect2_item->setText( text_line.Font().fontEffect() );
                    font_effectcolor2_item  = this->item(mStCount, SUB_FONT_EFFECTCOLOR2_COL);
                    font_effectcolor2_item->setText( text_line.Font().fontEffectColor() );
                    font_italic2_item  = this->item(mStCount, SUB_FONT_ITALIC2_COL);
                    font_italic2_item->setText( text_line.Font().fontItalic() );
                    font_underlined2_item  = this->item(mStCount, SUB_FONT_UNDERLINED2_COL);
                    font_underlined2_item->setText( text_line.Font().fontUnderlined() );
                    font_script2_item  = this->item(mStCount, SUB_FONT_SCRIPT2_COL);
                    font_script2_item->setText( text_line.Font().fontScript() );
                    font_size2_item  = this->item(mStCount, SUB_FONT_SIZE2_COL);
                    font_size2_item->setText( text_line.Font().fontSize() );
                }
            }

            text_item->setText(text);

            // Increment the subtitles counter
            mStCount ++;
        }
        mSubLoadding = false;
        this->setEnabled(true);

        mPreviousIndex = -1;
        mCurrentIndex = 0;
        this->selectRow(mCurrentIndex);

    }
}

// Get a container with all subtitles information from the table
QList<MySubtitles> MySubtitlesTable::saveSubtitles() {

    QList<MySubtitles> subtitles_list;

    for ( qint32 i = 0; i < mStCount; i++) {

        subtitles_list.append(this->getSubInfos(i));
    }
    return subtitles_list;
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
bool MySubtitlesTable::insertNewSub(MySubtitles &newSubtitle, qint64 positionMs, bool shiftNextSub) {

    QTableWidgetItem* start_time_item;
    QTableWidgetItem* end_time_item;
    QTime start_time_HMS;
    QTime end_time_HMS;
    qint32 start_time_ms;
    qint32 end_time_ms;
    QTime time_base(0, 0, 0, 0);

    // Insert new sub at the current video position
    if ( positionMs == -1 ) {
        start_time_ms = mCurrentPositionMs;
    } // If a time position is given, insert new sub at this given time
    else {
        start_time_ms = positionMs;
    }

    end_time_ms = start_time_ms + qApp->property("prop_SubMinDuration_ms").toInt();

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

        // Add new rows to the table when empty rows remaining are less than 10
        if ( ( this->rowCount() - mStCount ) < 10 ) {

           this->addRows(100, this->rowCount());
        }

        this->updateText(newSubtitle.text(), start_time_ms);
        this->updateDatas(newSubtitle, start_time_ms);

        this->selectRow(mCurrentIndex);
    }
    else if ( shiftNextSub == true ) {

    }
    else {
        mErrorMsg = "Not enought space to insert subtile";
        return false;
    }

    newSubtitle.setStartTime(start_time_HMS.toString("hh:mm:ss.zzz"));
    newSubtitle.setEndTime(end_time_HMS.toString("hh:mm:ss.zzz"));

    return true;
}

// Remove the current sub
void MySubtitlesTable::deleteCurrentSub() {

    qint32 start_time_ms;

    start_time_ms = mPositionMsToStIndex.indexOf(mCurrentIndex);

    if ( start_time_ms >= 0 ) {
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
        this->selectRow(mCurrentIndex);
    }
}

bool MySubtitlesTable::isNewEntry() {

    qint32 row_index;

    row_index = mPositionMsToStIndex[mCurrentPositionMs];

    // The subtitle exist
    if (row_index != -1) {
        return false;
    }
    else { // The subtitle not exist
        return true;
    }
}

// Update the text in the table
void MySubtitlesTable::updateText(QList<TextLine> textLines, qint64 positionMs) {

    qint32 row_index;
    QString plain_text;
    TextLine text_line;

    // Update the text for the index at the current video position
    if ( positionMs == -1 ) {
        row_index = mPositionMsToStIndex[mCurrentPositionMs];
    } // If a time position is given, update the text for the index at this given time
    else {
        row_index = mPositionMsToStIndex[positionMs];
    }

    for ( qint32 i = 0; i < textLines.count(); i++ ) {

        text_line = textLines.at(i);

        if ( i == 0 ) {

            this->item(row_index, SUB_NBR_OF_LINES_COL)->setText("1");
            this->item(row_index, SUB_LINE1_COL)->setText(text_line.Line());
            plain_text = text_line.Line();

            if ( textLines.count() == 1 ) {
                this->item(row_index, SUB_LINE2_COL)->setText("");
            }
        }
        else if ( i == 1 ) {

            this->item(row_index, SUB_NBR_OF_LINES_COL)->setText("2");
            this->item(row_index, SUB_LINE2_COL)->setText(text_line.Line());
            plain_text += "\n";
            plain_text += text_line.Line();
        }
    }
    this->item(row_index, SUB_TEXT_COL)->setText(plain_text);
}

// Update the font, position informations in the table with the given subtitle data
void MySubtitlesTable::updateDatas(MySubtitles subtitle, qint64 positionMs) {

    qint32 index;

    QTableWidgetItem* vposition1_item;
    QTableWidgetItem* valign1_item;
    QTableWidgetItem* hposition1_item;
    QTableWidgetItem* halign1_item;
    QTableWidgetItem* direction1_item;
    QTableWidgetItem* font_id1_item;
    QTableWidgetItem* font_color1_item;
    QTableWidgetItem* font_effect1_item;
    QTableWidgetItem* font_effectcolor1_item;
    QTableWidgetItem* font_italic1_item;
    QTableWidgetItem* font_underlined1_item;
    QTableWidgetItem* font_script1_item;
    QTableWidgetItem* font_size1_item;
    QTableWidgetItem* vposition2_item;
    QTableWidgetItem* valign2_item;
    QTableWidgetItem* hposition2_item;
    QTableWidgetItem* halign2_item;
    QTableWidgetItem* direction2_item;
    QTableWidgetItem* font_id2_item;
    QTableWidgetItem* font_color2_item;
    QTableWidgetItem* font_effect2_item;
    QTableWidgetItem* font_effectcolor2_item;
    QTableWidgetItem* font_italic2_item;
    QTableWidgetItem* font_underlined2_item;
    QTableWidgetItem* font_script2_item;
    QTableWidgetItem* font_size2_item;

    // Update the informations for the index at the current video position
    if ( positionMs == -1 ) {
        index = mPositionMsToStIndex[mCurrentPositionMs];
    } // If a time position is given, update the informations for the index at this given time
    else {
        index = mPositionMsToStIndex[positionMs];
    }

    if ( index != -1) {

        for ( qint32 i = 0; i < subtitle.text().size(); i++) {

            TextLine text_line = subtitle.text().at(i);

            // Update line 1 informations
            if ( i == 0 ) {

                // Reset the line 2 informations
                for ( qint32 j = SUB_VPOSITION2_COL; j <= SUB_FONT_SIZE2_COL; j++ ) {
                    this->item(index, j)->setText("");
                }

                vposition1_item  = this->item(index, SUB_VPOSITION1_COL);
                vposition1_item->setText( text_line.textVPosition() );
                valign1_item  = this->item(index, SUB_VALIGN1_COL);
                valign1_item->setText( text_line.textVAlign() );
                hposition1_item  = this->item(index, SUB_HPOSITION1_COL);
                hposition1_item->setText( text_line.textHPosition() );
                halign1_item  = this->item(index, SUB_HALIGN1_COL);
                halign1_item->setText( text_line.textHAlign() );
                direction1_item  = this->item(index, SUB_DIRECTION1_COL);
                direction1_item->setText( text_line.textDirection() );
                font_id1_item  = this->item(index, SUB_FONT_ID1_COL);
                font_id1_item->setText( text_line.Font().fontId() );
                font_color1_item  = this->item(index, SUB_FONT_COLOR1_COL);
                font_color1_item->setText( text_line.Font().fontColor() );
                font_effect1_item  = this->item(index, SUB_FONT_EFFECT1_COL);
                font_effect1_item->setText( text_line.Font().fontEffect() );
                font_effectcolor1_item  = this->item(index, SUB_FONT_EFFECTCOLOR1_COL);
                font_effectcolor1_item->setText( text_line.Font().fontEffectColor() );
                font_italic1_item  = this->item(index, SUB_FONT_ITALIC1_COL);
                font_italic1_item->setText( text_line.Font().fontItalic() );
                font_underlined1_item  = this->item(index, SUB_FONT_UNDERLINED1_COL);
                font_underlined1_item->setText( text_line.Font().fontUnderlined() );
                font_script1_item  = this->item(index, SUB_FONT_SCRIPT1_COL);
                font_script1_item->setText( text_line.Font().fontScript() );
                font_size1_item  = this->item(index, SUB_FONT_SIZE1_COL);
                font_size1_item->setText( text_line.Font().fontSize() );
            }
            // Update line 2 informations
            else if ( i == 1 ) {
                vposition2_item  = this->item(index, SUB_VPOSITION2_COL);
                vposition2_item->setText( text_line.textVPosition() );
                valign2_item  = this->item(index, SUB_VALIGN2_COL);
                valign2_item->setText( text_line.textVAlign() );
                hposition2_item  = this->item(index, SUB_HPOSITION2_COL);
                hposition2_item->setText( text_line.textHPosition() );
                halign2_item  = this->item(index, SUB_HALIGN2_COL);
                halign2_item->setText( text_line.textHAlign() );
                direction2_item  = this->item(index, SUB_DIRECTION2_COL);
                direction2_item->setText( text_line.textDirection() );
                font_id2_item  = this->item(index, SUB_FONT_ID2_COL);
                font_id2_item->setText( text_line.Font().fontId() );
                font_color2_item  = this->item(index, SUB_FONT_COLOR2_COL);
                font_color2_item->setText( text_line.Font().fontColor() );
                font_effect2_item  = this->item(index, SUB_FONT_EFFECT2_COL);
                font_effect2_item->setText( text_line.Font().fontEffect() );
                font_effectcolor2_item  = this->item(index, SUB_FONT_EFFECTCOLOR2_COL);
                font_effectcolor2_item->setText( text_line.Font().fontEffectColor() );
                font_italic2_item  = this->item(index, SUB_FONT_ITALIC2_COL);
                font_italic2_item->setText( text_line.Font().fontItalic() );
                font_underlined2_item  = this->item(index, SUB_FONT_UNDERLINED2_COL);
                font_underlined2_item->setText( text_line.Font().fontUnderlined() );
                font_script2_item  = this->item(index, SUB_FONT_SCRIPT2_COL);
                font_script2_item->setText( text_line.Font().fontScript() );
                font_size2_item  = this->item(index, SUB_FONT_SIZE2_COL);
                font_size2_item->setText( text_line.Font().fontSize() );
            }
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

// Set "end time" to current item
bool MySubtitlesTable::setEndTime(qint64 positionMs, qint32 stIndex) {

    QTime start_time_HMS;
    QTime end_time_HMS;
    QTime time_base(0, 0, 0, 0);
    QTableWidgetItem* end_time_item;

    start_time_HMS = QTime::fromString(this->item(stIndex, SUB_START_TIME_COL)->text(), "hh:mm:ss.zzz");
    end_time_HMS = time_base.addMSecs(positionMs);

    // Check if a new end time entry is > to the current item start time
    if ( end_time_HMS > start_time_HMS ) {
        end_time_item = this->item(stIndex, SUB_END_TIME_COL);
        end_time_item->setText(end_time_HMS.toString("hh:mm:ss.zzz"));
        // Try to update the current item end time
        QString status_msg = this->updateStTime(end_time_item);
        if ( status_msg != "" ) {
            mErrorMsg = status_msg;
            return false;
        }
    }
    else {
        mErrorMsg = "end time < start time";
        return false;
    }

    return true;
}

// Set "start time" to current item
bool MySubtitlesTable::setStartTime(qint64 positionMs, qint32 stIndex) {

    QTime start_time_HMS;
    QTime end_time_HMS;
    QTime time_base(0, 0, 0, 0);
    QTableWidgetItem* start_time_item;


    start_time_HMS = time_base.addMSecs(positionMs);
    end_time_HMS = QTime::fromString(this->item(stIndex, SUB_END_TIME_COL)->text(), "hh:mm:ss.zzz");
    // Check if a new start time entry is < to the current item end time
    if ( end_time_HMS > start_time_HMS ) {
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
        mErrorMsg = "end time < start time";
        return false;
    }

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

    qint32 start_time_ms;
    QTime start_time_HMS;
    QTime time_base(0, 0, 0, 0);

    // Center scroll area at the selected item line
    QList<QTableWidgetItem*> selected_items = this->selectedItems();

    if ( !selected_items.isEmpty() ) {
        this->scrollToItem(selected_items.first(), QAbstractItemView::PositionAtCenter);

        // If the row was selected by user, send the new time information
        if ( mSelectedBySoft == false ) {

            mSelectedByUser = true;

            // If the selected item start time is valid
            QString start_time_str = selected_items.at(SUB_START_TIME_COL)->text();

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

    // Save the current player position
    mCurrentPositionMs = positionMs;

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
            // If time was changed by software, highlight the row corresponding to the index
            if ( mSelectedByUser == false ) {
                mSelectedBySoft = true;
                this->selectRow(mCurrentIndex);
            }
            else {
                mSelectedByUser = false;
            }
        }

        // Send a signal to display the subtilte or nothing if it's empty
        emit newTextToDisplay(subtitle);
    }
    mPreviousIndex = st_index;
}

// Get the sustitles data for a given index in the table
MySubtitles MySubtitlesTable::getSubInfos(qint32 stIndex) {

    MySubtitles new_subtitle;

    if ( stIndex >= 0 ) {

        // Get start time and end time
        if ( !this->item(stIndex, SUB_START_TIME_COL)->text().isEmpty() ) {
            new_subtitle.setStartTime( this->item(stIndex, SUB_START_TIME_COL)->text() );
        }

        if ( !this->item(stIndex, SUB_END_TIME_COL)->text().isEmpty() ) {
            new_subtitle.setEndTime( this->item(stIndex, SUB_END_TIME_COL)->text() );
        }

        // Check the number of lines
        qint8 nbr_of_lines = this->item(stIndex, SUB_NBR_OF_LINES_COL)->text().toInt();

        if ( nbr_of_lines >= 1 ) {

            // Retrieive line 1 font, position and text information
            TextLine sub_line1;
            TextFont sub_font1;

            sub_font1.setFontId( this->item(stIndex, SUB_FONT_ID1_COL)->text() );
            sub_font1.setFontColor( this->item(stIndex, SUB_FONT_COLOR1_COL)->text() );
            sub_font1.setFontEffect( this->item(stIndex, SUB_FONT_EFFECT1_COL)->text() );
            sub_font1.setFontEffectColor( this->item(stIndex, SUB_FONT_EFFECTCOLOR1_COL)->text() );
            sub_font1.setFontItalic( this->item(stIndex, SUB_FONT_ITALIC1_COL)->text() );
            sub_font1.setFontUnderlined( this->item(stIndex, SUB_FONT_UNDERLINED1_COL)->text() );
            sub_font1.setFontScript( this->item(stIndex, SUB_FONT_SCRIPT1_COL)->text() );
            sub_font1.setFontSize( this->item(stIndex, SUB_FONT_SIZE1_COL)->text() );

            sub_line1.setLine( this->item(stIndex, SUB_LINE1_COL)->text() );

            sub_line1.setTextDirection( this->item(stIndex, SUB_DIRECTION1_COL)->text() );
            sub_line1.setTextHAlign( this->item(stIndex, SUB_HALIGN1_COL)->text() );
            sub_line1.setTextHPosition( this->item(stIndex, SUB_HPOSITION1_COL)->text() );
            sub_line1.setTextVAlign( this->item(stIndex, SUB_VALIGN1_COL)->text() );
            sub_line1.setTextVPosition( this->item(stIndex, SUB_VPOSITION1_COL)->text() );

            new_subtitle.setText(sub_line1, sub_font1);
        }

        if ( nbr_of_lines >= 2 ) {

            // Retrieive line 2 font, position and text information
            TextLine sub_line2;
            TextFont sub_font2;

            sub_font2.setFontId( this->item(stIndex, SUB_FONT_ID2_COL)->text() );
            sub_font2.setFontColor( this->item(stIndex, SUB_FONT_COLOR2_COL)->text() );
            sub_font2.setFontEffect( this->item(stIndex, SUB_FONT_EFFECT2_COL)->text() );
            sub_font2.setFontEffectColor( this->item(stIndex, SUB_FONT_EFFECTCOLOR2_COL)->text() );
            sub_font2.setFontItalic( this->item(stIndex, SUB_FONT_ITALIC2_COL)->text() );
            sub_font2.setFontUnderlined( this->item(stIndex, SUB_FONT_UNDERLINED2_COL)->text() );
            sub_font2.setFontScript( this->item(stIndex, SUB_FONT_SCRIPT2_COL)->text() );
            sub_font2.setFontSize( this->item(stIndex, SUB_FONT_SIZE2_COL)->text() );

            sub_line2.setLine( this->item(stIndex, SUB_LINE2_COL)->text() );

            sub_line2.setTextDirection( this->item(stIndex, SUB_DIRECTION2_COL)->text() );
            sub_line2.setTextHAlign( this->item(stIndex, SUB_HALIGN2_COL)->text() );
            sub_line2.setTextHPosition( this->item(stIndex, SUB_HPOSITION2_COL)->text() );
            sub_line2.setTextVAlign( this->item(stIndex, SUB_VALIGN2_COL)->text() );
            sub_line2.setTextVPosition( this->item(stIndex, SUB_VPOSITION2_COL)->text() );

            new_subtitle.setText(sub_line2, sub_font2);
        }
    }
    // Return a MySubtitles container
    return new_subtitle;
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
