#ifndef MYSUBTITLESTABLE_H
#define MYSUBTITLESTABLE_H

#include <QTableWidget>
#include "mysubtitles.h"

// This widget manage a subtitles table.
// It's a database to stock text, timecode, position, font.
// Display only text and timecode.
class MySubtitlesTable : public QTableWidget
{
    Q_OBJECT

public:
    explicit MySubtitlesTable(QWidget *parent = 0);
    //~MySubtitlesTable();

signals:
    void itemSelectionChanged(qint64);
    void newTextToDisplay(MySubtitles);
    void currentSubChanged(qint32);

public slots:
    bool insertNewSub(MySubtitles &newSubtitle, qint64 positionMs = -1, bool shiftNextSub = false);
    bool insertNewSubAfterCurrent(MySubtitles &newSubtitle);
    void deleteCurrentSub();
    void initStTable(qint32 numberOfRow);
    void loadSubtitles(QList<MySubtitles> subtitlesList);
    QList<MySubtitles> saveSubtitles();
    bool isNewEntry();
    void updateText(QList<TextLine> textLines, qint64 positionMs = -1);
    void updateDatas(MySubtitles subtitle, qint64 positionMs = -1);
    bool setEndTime(qint64 positionMs, qint32 stIndex);
    bool setStartTime(qint64 positionMs, qint32 stIndex);
    void updateStDisplay(qint64 positionMs);
    qint32 currentIndex();
    qint32 subtitlesCount();
    MySubtitles getSubInfos(qint32 stIndex);
    QString errorMsg();

private slots:
    void updateSelectedItem();
    void updateItem(QTableWidgetItem* item);
    QString updateStTime(QTableWidgetItem* time_item);
    void addRows(qint32 numberOfRow, qint32 fromRowNbr);


private:
    // Lookup table to retrieive faster a subtitle number in function of time in millisecond
    QVector<qint32> mPositionMsToStIndex;

    // Position time of the video in millisecond
    qint64 mCurrentPositionMs;

    // Previous subtitle displayed index.
    qint32 mPreviousIndex;

    // Total number of subtitles
    qint32 mStCount;

    // Subtitles are loading in the table flag. Avoid all other operation on the table
    bool mSubLoadding;

    // An item was selected by software
    bool mSelectedBySoft;

    // An item was selected by user
    bool mSelectedByUser;

    // Save the current item index.
    qint32 mCurrentIndex;

    // Error message
    QString mErrorMsg;
};

#endif // MYSUBTITLESTABLE_H
