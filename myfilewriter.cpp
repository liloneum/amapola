#include "myfilewriter.h"
#include <QFile>
#include <QTextStream>
#include <QTextCodec>

MyFileWriter::MyFileWriter(QString fileName, QString textCodec) {

    mFileName = fileName;
    mTextCodec = textCodec;

}

void MyFileWriter::write(QString buffer) {

    mData += buffer;
}

void MyFileWriter::toFile() {

    if ( !mFileName.isEmpty() ) {
        QFile file_write(mFileName);
        if ( !file_write.open(QIODevice::WriteOnly) ) {
            // error message
        } else {

            QTextStream stream_out(&file_write);

            if ( !mTextCodec.isEmpty() ) {
                QTextCodec* text_codec = QTextCodec::codecForName(mTextCodec.toLocal8Bit());
                stream_out.setCodec(text_codec);
            }

            stream_out << mData;
            stream_out.flush();
            file_write.close();
        }
    }
}

