#ifndef MYFILEREADER_H
#define MYFILEREADER_H

#include <QtGlobal>
#include <QVector>


class MyFileReader
{
public:
    MyFileReader(const QString fileName, QString textCodec);
    bool readFile(const QString fileName, QString textCodec);
    QVector<QString> lines();

//private slots:

private:

    QVector<QString> mLines;
    qint32 mNumberOfLines;
};


#endif // MYFILEREADER_H
