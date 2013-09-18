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
    QStringList lines();
    QString getFileName();
    QString errorMsg();

//private slots:

private:

    // List of string lines
    QStringList mLines;
    // Number of lines read
    qint32 mNumberOfLines;
    // File name read
    QString mFileName;
    // Error msg
    QString mErrorMsg;
};


#endif // MYFILEREADER_H
