#ifndef SCENARISTSUBPARSER_H
#define SCENARISTSUBPARSER_H

#include "subtitlefileparser.h"
#include "filewriter.h"
#include "subexportdialog.h"

class ScenaristSubParser : public SubtitleFileParser
{
public:
    ScenaristSubParser();
    bool save(FileWriter & file, QList<Subtitles> subtitlesList, SubExportDialog* exportDialog);
    QList<Subtitles> open(FileReader file);
    bool readSample(FileReader file);
};

#endif // SCENARISTSUBPARSER_H
