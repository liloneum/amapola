#ifndef MYSUBTITLESTABLE_H
#define MYSUBTITLESTABLE_H

#include <QTableWidget>

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
    void initStTable();

private slots:
    void updateSelectedItem();
    void videoDurationChanged(qint64 videoDurationMs);
    void updateStDisplay(qint64 videoPositionMs);
    void updateItem(QTableWidgetItem* item);
    bool updateStTime(QTableWidgetItem* time_item);

private:
    QVector<qint32> mPositionMsToStIndex;
    qint64 mVideoPositionMs;
    qint32 mPreviousIndex;
    qint32 mStCount;
};

#endif // MYSUBTITLESTABLE_H
