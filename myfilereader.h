#ifndef MYFILEREADER_H
#define MYFILEREADER_H

#include <QtGlobal>
#include <QVector>
#include <QString>


class MyFileReader
{
public:
    MyFileReader(const QString fileName, QString textCodec);
    bool readFile(const QString fileName, QString textCodec);
    QVector<QString> lines();
    QString getFileName();

//private slots:

private:

    QVector<QString> mLines;
    qint32 mNumberOfLines;
    QString mFileName;
};


#endif // MYFILEREADER_H
