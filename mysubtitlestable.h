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
    void newTextToDisplay(QString);

public slots:
    void updateStTable(QString stText);
    void initStTable(qint32 numberOfRow);
    void loadSubtitles(QList<MySubtitles> subtitlesList);
    QList<MySubtitles> saveSubtitles();

private slots:
    void updateSelectedItem();
    void videoDurationChanged(qint64 videoDurationMs);
    void updateStDisplay(qint64 videoPositionMs);
    void updateItem(QTableWidgetItem* item);
    bool updateStTime(QTableWidgetItem* time_item);

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
};

#endif // MYSUBTITLESTABLE_H
