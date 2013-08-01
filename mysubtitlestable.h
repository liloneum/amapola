#ifndef MYSUBTITLESTABLE_H
#define MYSUBTITLESTABLE_H

#include <QTableWidget>
#include "mysubtitles.h"

class MySubtitlesTable : public QTableWidget
{
    Q_OBJECT

public:
    explicit MySubtitlesTable(QWidget *parent = 0);
    //~MySubtitlesTable();

signals:
    void itemSelectionChanged(qint32);
    void newTextToDisplay(MySubtitles);
    void currentSubChanged(qint32);

public slots:
    bool insertNewSub(MySubtitles newSubtitle, qint64 positionMs = -1, bool shiftNextSub = false);
    bool insertNewSubAfterCurrent(MySubtitles newSubtitle);
    void deleteCurrentSub();
    void initStTable(qint32 numberOfRow);
    void loadSubtitles(QList<MySubtitles> subtitlesList);
    QList<MySubtitles> saveSubtitles();
    bool isNewEntry();
    void updateText(QList<TextLine> textLines, qint64 positionMs = -1);
    void updateDatas(MySubtitles subtitle, qint64 positionMs = -1);
    bool setEndTime(qint64 positionMs);
    bool setStartTime(qint64 positionMs);
    QString errorMsg();

private slots:
    void updateSelectedItem();
    void videoDurationChanged(qint64 videoDurationMs);
    void updateStDisplay(qint64 videoPositionMs);
    void updateItem(QTableWidgetItem* item);
    QString updateStTime(QTableWidgetItem* time_item);
    MySubtitles fillSubInfos(qint32 stIndex);
    void addRows(qint32 numberOfRow, qint32 fromRowNbr);


private:
    // Lookup table to retrieive faster a subtitle number in function of time in millisecond
    QVector<qint32> mPositionMsToStIndex;

    // Position time of the video in millisecond
    qint64 mVideoPositionMs;

    // Previous subtitle displayed index.
    qint32 mPreviousIndex;

    // Total number of subtitles
    qint32 mStCount;

    bool mSubLoadding;
    bool mSelectedBySoft;
    bool mSelectedByUser;

    qint32 mCurrentIndex;

    QString mErrorMsg;
};

#endif // MYSUBTITLESTABLE_H
