#ifndef SUBRIPPARSER_H
#define SUBRIPPARSER_H

#include "mysubtitlefileparser.h"
#include "myfilereader.h"
#include "myfilewriter.h"
#include "mysubtitles.h"
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
