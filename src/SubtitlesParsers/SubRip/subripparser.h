#ifndef SUBRIPPARSER_H
#define SUBRIPPARSER_H

#include "subtitlefileparser.h"
#include "filereader.h"
#include "filewriter.h"
#include "subtitles.h"
#include "subexportdialog.h"

class SubRipParser : public MySubtitleFileParser
{
public:
    SubRipParser();
    QList<MySubtitles> open(MyFileReader file);
    bool save(MyFileWriter & file, QList<MySubtitles> subtitlesList, SubExportDialog* exportDialog);
    bool readSample(MyFileReader file);
};

#endif // SUBRIPPARSER_H
