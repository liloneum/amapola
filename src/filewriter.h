#ifndef FILEWRITER_H
#define FILEWRITER_H

#include <QString>
#include <QList>

// Class to write text datas in a specified file "fileName" with specified format "textCodec"
class FileWriter
{
public:
    FileWriter(QString fileName, QString textCodec);
    void writeText(QString buffer);
    void writeRawData(QList<quint8> buffer);
    bool toFile(bool rawData = false);
    QString errorMsg();
    QString fileName();

private:
    // The string data buffer
    QString mStringData;
    // The raw data buffer
    QList<quint8> mRawData;
    // Name of the text encoding
    QString mTextCodec;
    // Name of the out file
    QString mFileName;
    // Error msg
    QString mErrorMsg;
};

#endif // FILEWRITER_H
