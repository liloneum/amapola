#ifndef MYFILEREADER_H
#define MYFILEREADER_H

#include <QtGlobal>
#include <QVector>
#include <QString>
#include <QStringList>

// Class to read text datas in a specified file "fileName" with specified format "textCodec"
class MyFileReader
{
public:
    MyFileReader(const QString fileName, QString textCodec);
    bool readFile(const QString fileName, QString textCodec);
    bool readRawData(const QString fileName);
    QStringList lines();
    QList<quint8> *data();
    QString getFileName();
    QString errorMsg();

//private slots:

private:

    // List of string lines
    QStringList mLines;
    // Data of the file read
    QList<quint8> mData;
    // Number of lines read
    qint32 mNumberOfLines;
    // File name read
    QString mFileName;
    // Error msg
    QString mErrorMsg;
};


#endif // MYFILEREADER_H
