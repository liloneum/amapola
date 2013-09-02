#include "myfilereader.h"
#include <QFile>
#include <QTextStream>
#include <QTextCodec>

MyFileReader::MyFileReader(const QString fileName, QString textCodec)
{
    //mLines.resize(1);
    //mLines.fill("");

    mNumberOfLines = 0;

    if(!readFile(fileName, textCodec)) {
        // Handle error
    }
}

bool MyFileReader::readFile(const QString fileName, QString textCodec) {

    if(!fileName.isEmpty()) {
        QFile file_read(fileName);
        if ( !file_read.open(QIODevice::ReadOnly) ) {
            // Error msg
            return false;
        }

        QTextStream in(&file_read);
        in.setCodec("ISO8859-15");

        do {
            mLines.append(in.readLine());
            mNumberOfLines++;
        } while (!in.atEnd());

        file_read.close();

        return true;
    }
    return false;
}

QVector<QString> MyFileReader::lines() {

    return mLines;
}
