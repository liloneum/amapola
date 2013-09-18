#ifndef MYFILEWRITER_H
#define MYFILEWRITER_H

#include <QString>

// Class to write text datas in a specified file "fileName" with specified format "textCodec"
class MyFileWriter
{
public:
    MyFileWriter(QString fileName, QString textCodec);
    void write(QString buffer);
    bool toFile();
    QString errorMsg();

private:
    // The datas buffer
    QString mData;
    // Name of the text encoding
    QString mTextCodec;
    // Name of the out file
    QString mFileName;
    // Error msg
    QString mErrorMsg;
};

#endif // MYFILEWRITER_H
