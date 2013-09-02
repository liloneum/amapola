#include "mysubtitlestable.h"
#include <QTime>

#define SUB_NUM_COL 0
#define SUB_START_TIME_COL 1
#define SUB_END_TIME_COL 2
#define SUB_DURATION_COL 3
#define SUB_TEXT_COL 4

MySubtitlesTable::MySubtitlesTable(QWidget *parent) :
    QTableWidget(parent)
{
    mPositionMsToStIndex.resize(1);
    mPositionMsToStIndex.fill(-1);

    mVideoPositionMs = 0;
    mPreviousIndex = 0;

    mStCount = 0;

    connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(updateSelectedItem()));
    connect(this, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(updateItem(QTableWidgetItem*)));

}


void MySubtitlesTable::initStTable () {

    this->setEnabled(false);

    for ( qint32 i = 0; i < this->columnCount(); i++ ) {
        for ( qint32 j = 0; j < this->rowCount(); j++) {

            QTableWidgetItem* new_item = new QTableWidgetItem;

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

void MySubtitlesTable::updateStTable(QString stText) {

    QTableWidgetItem* start_time_item;
    QTableWidgetItem* end_time_item;
    QTime start_time_HMS;
    QTime end_time_HMS;
    qint32 start_time_ms;
    qint32 end_time_ms;
    QTime time_base(0, 0, 0, 0);

    qint32 row_index = mPositionMsToStIndex[mVideoPositionMs];

    if (row_index != -1) {
        this->item(row_index, SUB_TEXT_COL)->setText(stText);
    }
    else { //Create a new subtitle entry

        if ( stText != "") {

            start_time_ms = mVideoPositionMs;
            end_time_ms = mVideoPositionMs + 1000;

            if ( mPositionMsToStIndex[start_time_ms] == -1) {

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

void MySubtitlesTable::updateItem(QTableWidgetItem *item) {

    if ( ( item->column() == SUB_START_TIME_COL ) || ( item->column() == SUB_END_TIME_COL ) ) {

        if ( ( item->text() != "") && ( item->text() != "New Entry" ) ) {

            if ( !updateStTime(item) ) {

                this->editItem(item);
            }
        }
    }
}

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

    row_index = time_item->row();

    if ( time_item->column() == SUB_START_TIME_COL ) {

        start_time_item = time_item;
        end_time_item = this->item(row_index, SUB_END_TIME_COL);

        start_time_HMS = QTime::fromString(start_time_item->text(), "hh:mm:ss.zzz");

        if ( start_time_HMS.isValid() ) {

            end_time_HMS = QTime::fromString(end_time_item->text(), "hh:mm:ss.zzz");

            if ( end_time_HMS.isValid() ) {
                compute_duration = true;
            }
        }
        else {
            return false;
        }
    }
    else if ( time_item->column() == SUB_END_TIME_COL ) {

        end_time_item = time_item;
        start_time_item = this->item(row_index, SUB_START_TIME_COL);

        end_time_HMS = QTime::fromString(end_time_item->text(), "hh:mm:ss.zzz");

        if ( end_time_HMS.isValid() ) {

            start_time_HMS = QTime::fromString(start_time_item->text(), "hh:mm:ss.zzz");

            if ( start_time_HMS.isValid() ) {
                compute_duration = true;
            }
        }
        else {
            return false;
        }
    }

    if ( compute_duration ) {

        qint32 first_index;
        qint32 last_index;
        start_time_ms = qAbs( start_time_HMS.msecsTo(time_base) );
        end_time_ms = qAbs( end_time_HMS.msecsTo(time_base) );

        duration_time_ms = end_time_ms - start_time_ms;
        duration_time_HMS = time_base.addMSecs(duration_time_ms);

        duration_time_item = this->item(row_index, SUB_DURATION_COL);
        duration_time_item->setText(duration_time_HMS.toString("hh:mm:ss.zzz"));

        // If it's new entry
        if ( this->item(row_index, SUB_NUM_COL)->text() == "" ) {

            mStCount ++;
            this->sortItems(SUB_START_TIME_COL, Qt::AscendingOrder);

            new_row_index = 0;

            for ( qint32 i = start_time_ms; i >= 0; i--) {
                if ( mPositionMsToStIndex[i] != -1) {
                    new_row_index = mPositionMsToStIndex[i] + 1;
                    break;
                }
            }

            for ( qint32 i = start_time_ms; i < mPositionMsToStIndex.size(); i++ ) {

                if ( ( i >= start_time_ms ) && ( i <= end_time_ms) ) {
                     mPositionMsToStIndex[i] = new_row_index;
                }
                else if ( mPositionMsToStIndex[i] != -1 ) {
                    mPositionMsToStIndex[i] = mPositionMsToStIndex[i] + 1;
                }
            }

            for ( qint32 i = new_row_index; i < mStCount; i++ ) {
                this->item(i, SUB_NUM_COL)->setText(QString::number(i + 1));
            }
        }
        else { // Resize entry

            if ( time_item->column() == SUB_START_TIME_COL ) {
                first_index = mPositionMsToStIndex.indexOf(row_index, 0);
                if ( ( first_index > start_time_ms ) || ( first_index < 0 ) ) {
                    first_index = start_time_ms;
                }
                last_index = end_time_ms;
            }
            else if ( time_item->column() == SUB_END_TIME_COL ) {
                last_index = mPositionMsToStIndex.lastIndexOf(row_index, -1);
                if ( last_index < end_time_ms ) {
                    last_index = end_time_ms;
                }
                first_index = start_time_ms;
            }

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
    return true;
}


void MySubtitlesTable::updateSelectedItem() {

    qint32 start_time_ms;
    QTime start_time_HMS;
    QTime time_base(0, 0, 0, 0);

    QList<QTableWidgetItem*> selected_items = this->selectedItems();
    this->scrollToItem(selected_items.first(), QAbstractItemView::PositionAtCenter);

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

    mVideoPositionMs = videoPositionMs;

    qint32 st_index = mPositionMsToStIndex[videoPositionMs];

    if ( st_index != mPreviousIndex ) {
        if ( st_index != -1 ) {
            QString st_text = this->item(st_index, SUB_TEXT_COL)->text();
            emit newTextToDisplay(st_text);
        }
        else {
            emit newTextToDisplay("");
        }
    }
    mPreviousIndex = st_index;
}
