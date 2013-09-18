#include "myfilewriter.h"
#include <QFile>
#include <QTextStream>
#include <QTextCodec>

// Class to write text datas in a specified file "fileName" with specified format "textCodec"
MyFileWriter::MyFileWriter(QString fileName, QString textCodec) {

    mFileName = fileName;
    mTextCodec = textCodec;
    mErrorMsg = "";
}

// Add datas
void MyFileWriter::write(QString buffer) {

    mData += buffer;
}

// Write the datas in the file
bool MyFileWriter::toFile() {

    if ( !mFileName.isEmpty() ) {
        QFile file_write(mFileName);
        if ( !file_write.open(QIODevice::WriteOnly) ) {
            mErrorMsg = "Can't open file" +mFileName;
            return false;
        } else {

            // Open a stream to the file
            QTextStream stream_out(&file_write);

            // If a codec is specified, force the stream to use it
            if ( !mTextCodec.isEmpty() ) {
                QTextCodec* text_codec = QTextCodec::codecForName(mTextCodec.toLocal8Bit());
                stream_out.setCodec(text_codec);
            }

            // Write all the datas and close the file
            stream_out << mData;
            stream_out.flush();
            file_write.close();

            return true;
        }
    }
    mErrorMsg = "no file name specified";
    return false;
}

QString MyFileWriter::errorMsg() {
    return mErrorMsg;
}
