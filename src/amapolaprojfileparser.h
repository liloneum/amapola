#ifndef AMAPOLAPROJFILEPARSER_H
#define AMAPOLAPROJFILEPARSER_H

#include "filereader.h"
#include "filewriter.h"
#include "subtitles.h"
#include <QList>

class AmapolaProjFileParser
{
public:
    AmapolaProjFileParser();
    QList<MySubtitles> open(MyFileReader file);
    void save(MyFileWriter & file, QList<MySubtitles> subtitlesList, QString videoFilePath);
};

#endif // AMAPOLAPROJFILEPARSER_H
