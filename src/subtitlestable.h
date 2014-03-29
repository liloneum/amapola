#ifndef SUBTITLESTABLE_H
#define SUBTITLESTABLE_H

#include <QTableWidget>
#include "subtitles.h"

// This widget manage a subtitles table.
// It's a database to stock text, timecode, position, font.
// Display only text and timecode.
class MySubtitlesTable : public QTableWidget
{
    Q_OBJECT

public:
    explicit MySubtitlesTable(QWidget *parent = 0);

    enum columnRole {
        SUB_NUM_COL = 0,
        SUB_START_TIME_COL = 1,
        SUB_END_TIME_COL = 2,
        SUB_DURATION_COL = 3,
        SUB_CHARNUM_COL = 4,
        SUB_CHAR_PER_SEC_COL = 5,
        SUB_TEXT_COL = 6
    };

    //~MySubtitlesTable();

signals:
    void itemSelectionChanged(qint64);
    void newTextToDisplay(MySubtitles);
    void currentSubChanged(qint32);
    void endTimeCodeChanged(qint32, qint64);

public slots:
    bool insertNewSub(MySubtitles &newSubtitle, qint64 positionMs);
    bool insertNewSubAfterCurrent(MySubtitles &newSubtitle);
    void deleteCurrentSub();
    void initStTable(qint32 numberOfRow);
    void loadSubtitles(QList<MySubtitles> subtitlesList, bool keepSelection = true);
    QList<MySubtitles> saveSubtitles();
    bool isNewEntry(qint64 positionMs);
    void updateText(QList<TextLine> textLines);
    void updateDatas(MySubtitles subtitle, qint32 index = -1);
    bool setEndTime(qint64 positionMs, qint32 stIndex, bool force = false, qint32 intervalMs = 0);
    bool setStartTime(qint64 positionMs, qint32 stIndex, bool force = false, qint32 intervalMs = 0);
    void updateStDisplay(qint64 positionMs);
    qint32 currentIndex();
    QList<qint32> selectedIndex();
    qint32 subtitlesCount();
    MySubtitles getSubInfos(qint32 stIndex);
    QString errorMsg();
    void setDurationAuto(qint32 index, bool state);

private slots:
    void updateSelectedItem();
    void updateItem(QTableWidgetItem* item);
    QString updateStTime(QTableWidgetItem* time_item);
    void addRows(qint32 numberOfRow, qint32 fromRowNbr);

private:
    // Lookup table to retrieive faster a subtitle number in function of time in millisecond
    QVector<qint32> mPositionMsToStIndex;

    // The current subtitles list
    QList<MySubtitles> mSubtitlesList;

    // Previous subtitle displayed index.
    qint32 mPreviousIndex;

    // Save the current item index.
    qint32 mCurrentIndex;

    // All subtitle selected
    QList<qint32> mSelectedIndex;

    // Total number of subtitles
    qint32 mStCount;

    // Subtitles are loading in the table flag. Avoid all other operation on the table
    bool mSubLoadding;

    // An item was selected by software
    bool mSelectedBySoft;

    // An item was selected by user
    bool mSelectedByUser;

    // New subtitle is inserting
    bool mInsertingSub;

    // Error message
    QString mErrorMsg;
};

#endif // SUBTITLESTABLE_H
