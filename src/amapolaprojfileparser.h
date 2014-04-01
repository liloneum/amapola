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
    QList<Subtitles> open(FileReader file);
    void save(FileWriter & file, QList<Subtitles> subtitlesList, QString videoFilePath);
};

#endif // AMAPOLAPROJFILEPARSER_H
