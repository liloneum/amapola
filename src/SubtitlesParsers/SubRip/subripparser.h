#ifndef SUBRIPPARSER_H
#define SUBRIPPARSER_H

#include "subtitlefileparser.h"
#include "filereader.h"
#include "filewriter.h"
#include "subtitles.h"
#include "subexportdialog.h"

class SubRipParser : public SubtitleFileParser
{
public:
    SubRipParser();
    QList<Subtitles> open(FileReader file);
    bool save(FileWriter & file, QList<Subtitles> subtitlesList, SubExportDialog* exportDialog);
    bool readSample(FileReader file);
};

#endif // SUBRIPPARSER_H
