#ifndef SUBTITLEFILEPARSER_H
#define SUBTITLEFILEPARSER_H

#include "filereader.h"
#include "filewriter.h"
#include "subtitles.h"
#include "subexportdialog.h"

// Class with virtual methods used to parse subtitles file
class SubtitleFileParser
{
public:
    //SubtitleFileParser();
    virtual QList<Subtitles> open(FileReader file) = 0;
    virtual bool save(FileWriter & file, QList<Subtitles> subtitlesList, SubExportDialog* exportDialog) = 0;
    virtual bool readSample(FileReader file) =0;
};

#endif // SUBTITLEFILEPARSER_H
