#ifndef MYFILEWRITER_H
#define MYFILEWRITER_H

#include <QString>

class MyFileWriter
{
public:
    MyFileWriter(QString fileName, QString textCodec);
    void write(QString buffer);
    void toFile();

private:
    QString mData;
    QString mTextCodec;
    QString mFileName;
};

#endif // MYFILEWRITER_H
