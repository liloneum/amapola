#include "myfilereader.h"
#include <QFile>
#include <QTextStream>
#include <QTextCodec>

// Class to read text datas in a specified file "fileName" with specified format "textCodec"
MyFileReader::MyFileReader(const QString fileName, QString textCodec)
{
    mFileName = fileName;
    mNumberOfLines = 0;
    mErrorMsg = "";
}

// Read the lines of the specified file "fileName" with specified encoding "textCodec"
bool MyFileReader::readFile(const QString fileName, QString textCodec) {

    if(!fileName.isEmpty()) {
        QFile file_read(fileName);
        if ( !file_read.open(QIODevice::ReadOnly) ) {
            mErrorMsg = "Can't open file" +mFileName;
            return false;
        }

        // Open a stream from the file
        QTextStream in(&file_read);
        // Set the text encoding - DEBUG : textCodec not used
//        in.setCodec("ISO8859-15");

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

// Get the list of string lines
QStringList MyFileReader::lines() {

    return mLines;
}

// Get the file name
QString MyFileReader::getFileName() {
    return mFileName;
}

// Get the error message
QString MyFileReader::errorMsg() {
    return mErrorMsg;
}
