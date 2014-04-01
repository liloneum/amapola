#include "filereader.h"
#include <QFile>
#include <QTextStream>
#include <QTextCodec>

// Class to read text datas in a specified file "fileName" with specified format "textCodec"
FileReader::FileReader(const QString fileName, QString textCodec)
{
    mFileName = fileName;
    mNumberOfLines = 0;
    mErrorMsg = "";
}

// Read the lines of the specified file "fileName" with specified encoding "textCodec"
bool FileReader::readFile(const QString fileName, QString textCodec) {

    if(!fileName.isEmpty()) {
        QFile file_read(fileName);
        if ( !file_read.open(QIODevice::ReadOnly) ) {
            mErrorMsg = "Can't open file" +mFileName;
            return false;
        }

        // Open a stream from the file
        QTextStream in(&file_read);
        // Set the text encoding - DEBUG : textCodec not used
//        in.setCodec("ISO6937");

        do {
            mLines.append(in.readLine());
            mNumberOfLines++;
        } while (!in.atEnd());

        file_read.close();

        return true;
    }
    mErrorMsg = "No file name specified";
    return false;
}

bool FileReader::readRawData(const QString fileName) {

    if(!fileName.isEmpty()) {
        QFile file_read(fileName);
        if ( !file_read.open(QIODevice::ReadOnly) ) {
            mErrorMsg = "Can't open file" +mFileName;
            return false;
        }

        // Open a stream from the file
        QDataStream in(&file_read);
        quint8 buffer;

        do {
            in >> buffer;
            mData.append(buffer);
        } while (!in.atEnd());

        file_read.close();

        return true;
    }
    mErrorMsg = "No file name specified";
    return false;
}

QList<quint8>* FileReader::data() {

    return &mData;
}

// Get the list of string lines
QStringList FileReader::lines() {

    return mLines;
}

// Get the file name
QString FileReader::getFileName() {
    return mFileName;
}

// Get the error message
QString FileReader::errorMsg() {
    return mErrorMsg;
}
