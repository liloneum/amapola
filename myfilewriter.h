#ifndef MYFILEWRITER_H
#define MYFILEWRITER_H

#include <QString>
#include <QList>

// Class to write text datas in a specified file "fileName" with specified format "textCodec"
class MyFileWriter
{
public:
    MyFileWriter(QString fileName, QString textCodec);
    void writeText(QString buffer);
    void writeRawData(QList<quint8> buffer);
    bool toFile(bool rawData = false);
    QString errorMsg();

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

#endif // MYFILEWRITER_H
