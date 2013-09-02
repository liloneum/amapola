#include "mysubtitlestable.h"
#include "mysubtitles.h"
#include <QTime>

#define SUB_NUM_COL 0
#define SUB_START_TIME_COL 1
#define SUB_END_TIME_COL 2
#define SUB_DURATION_COL 3
#define SUB_TEXT_COL 4
#define SUB_DEFAULT_DURATION_MS 1000

MySubtitlesTable::MySubtitlesTable(QWidget *parent) :
    QTableWidget(parent)
{
    // Init the vector
    mPositionMsToStIndex.resize(1);
    mPositionMsToStIndex.fill(-1);

    // Init the variables
    mVideoPositionMs = 0;
    mPreviousIndex = 0;

    mStCount = 0;

    mSubLoadding = false;

    // Init the connections
    connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(updateSelectedItem()));
    connect(this, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(updateItem(QTableWidgetItem*)));

}

// Init all the table items with an empty string
void MySubtitlesTable::initStTable (qint32 numberOfRow) {

    this->setEnabled(false);

    mStCount = 0;

    if ( numberOfRow == 0 ) {
        this->setRowCount(100);
    }
    else {
        this->setRowCount(numberOfRow);
    }

    for ( qint32 i = 0; i < this->columnCount(); i++ ) {
        for ( qint32 j = 0; j < this->rowCount(); j++) {

            QTableWidgetItem* new_item = new QTableWidgetItem;

            // The start time items are init with "New Entry" string to able sorting
            if ( i == SUB_START_TIME_COL ) {
                new_item->setText("New Entry");
                new_item->setTextAlignment(Qt::AlignCenter);
            }
            else if (i == SUB_TEXT_COL ){
                new_item->setText("");
                new_item->setTextAlignment(Qt::AlignLeft);
            }
            else {
                new_item->setText("");
                new_item->setTextAlignment(Qt::AlignCenter);
            }
            this->setItem(j, i, new_item);
        }
    }
}

void MySubtitlesTable::loadSubtitles(QList<MySubtitles> subtitlesList) {

    QTableWidgetItem* start_time_item;
    QTableWidgetItem* end_time_item;
    QTableWidgetItem* text_item;

    if ( this->isEnabled() == true ) {

        initStTable(subtitlesList.size() + 20);

        mSubLoadding = true;

        for ( qint32 i = 0; i < subtitlesList.size(); i++ ) {

            start_time_item = this->item(mStCount, SUB_START_TIME_COL);
            start_time_item->setText( subtitlesList[i].startTime() );

            end_time_item = this->item(mStCount, SUB_END_TIME_COL);
            end_time_item->setText( subtitlesList[i].endTime() );

            text_item = this->item(mStCount, SUB_TEXT_COL);
            text_item->setText( subtitlesList[i].text() );

            mStCount ++;
        }
        mSubLoadding = false;
        this->setEnabled(true);

    }
}

// Insert or modify a subtitle in the table
void MySubtitlesTable::updateStTable(QString stText) {

    QTableWidgetItem* start_time_item;
    QTableWidgetItem* end_time_item;
    QTime start_time_HMS;
    QTime end_time_HMS;
    qint32 start_time_ms;
    qint32 end_time_ms;
    QTime time_base(0, 0, 0, 0);

    qint32 row_index = mPositionMsToStIndex[mVideoPositionMs];

    // The subtitle exist, update its text
    if (row_index != -1) {
        this->item(row_index, SUB_TEXT_COL)->setText(stText);
    }
    else { //Create a new subtitle entry

        if ( stText != "") {

            start_time_ms = mVideoPositionMs;
            end_time_ms = mVideoPositionMs + SUB_DEFAULT_DURATION_MS;

            if ( mPositionMsToStIndex[start_time_ms] == -1) {

                // Retrieive the first empty item, set the start/end time and text
                start_time_HMS = time_base.addMSecs(start_time_ms);
                start_time_item = this->item(mStCount, SUB_START_TIME_COL);
                start_time_item->setText(start_time_HMS.toString("hh:mm:ss.zzz"));

                end_time_HMS = time_base.addMSecs(end_time_ms);
                end_time_item = this->item(mStCount, SUB_END_TIME_COL);
                end_time_item->setText(end_time_HMS.toString("hh:mm:ss.zzz"));

                this->item(mPositionMsToStIndex[start_time_ms], SUB_TEXT_COL)->setText(stText);
            }
        }
    }
}

// Re-direct the items modifications
void MySubtitlesTable::updateItem(QTableWidgetItem *item) {

    if ( ( item->column() == SUB_START_TIME_COL ) || ( item->column() == SUB_END_TIME_COL ) ) {

        // Bypass during the table init
        if ( ( item->text() != "") && ( item->text() != "New Entry" ) ) {

            if ( !updateStTime(item) ) {

                this->editItem(item);
            }
        }
    }
}

// Manage time change of an start/end time item
bool MySubtitlesTable::updateStTime(QTableWidgetItem* time_item) {

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
            return false;
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
            return false;
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
    return true;
}

void MySubtitlesTable::updateSelectedItem() {

    qint32 start_time_ms;
    QTime start_time_HMS;
    QTime time_base(0, 0, 0, 0);

    // Center scroll area at the selected item line
    QList<QTableWidgetItem*> selected_items = this->selectedItems();
    this->scrollToItem(selected_items.first(), QAbstractItemView::PositionAtCenter);

    // If the select item start time is valid, emit a signal sending this start time
    QString start_time_str = selected_items.at(SUB_START_TIME_COL)->text();

    start_time_HMS = QTime::fromString(start_time_str, "hh:mm:ss.zzz");

    if ( start_time_HMS.isValid() ) {
        start_time_ms = qAbs( start_time_HMS.msecsTo(time_base) );
        emit itemSelectionChanged(start_time_ms);
    }
}

void MySubtitlesTable::videoDurationChanged(qint64 videoDurationMs) {

    if ( this->isEnabled() == false) {

        this->setEnabled(true);
        mPositionMsToStIndex.resize(videoDurationMs + 1);
        mPositionMsToStIndex.fill(-1);
    }
}

void MySubtitlesTable::updateStDisplay(qint64 videoPositionMs) {

    // Save the current player position
    mVideoPositionMs = videoPositionMs;

    qint32 st_index = mPositionMsToStIndex[videoPositionMs];

    // Check in the lookup table if there is an valid subtitle index for this position
    if ( st_index != mPreviousIndex ) {
        if ( st_index != -1 ) {
            // Send a signal to display the subtilte
            QString st_text = this->item(st_index, SUB_TEXT_COL)->text();
            emit newTextToDisplay(st_text);
        }
        else {
            emit newTextToDisplay("");
        }
    }
    mPreviousIndex = st_index;
}
