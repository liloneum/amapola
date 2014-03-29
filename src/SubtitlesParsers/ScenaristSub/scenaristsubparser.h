#ifndef SCENARISTSUBPARSER_H
#define SCENARISTSUBPARSER_H

#include "mysubtitlefileparser.h"
#include "myfilewriter.h"
#include "subexportdialog.h"

class ScenaristSubParser : public MySubtitleFileParser
{
public:
    ScenaristSubParser();
    bool save(MyFileWriter & file, QList<MySubtitles> subtitlesList, SubExportDialog* exportDialog);
    QList<MySubtitles> open(MyFileReader file);
    bool readSample(MyFileReader file);
};

#endif // SCENARISTSUBPARSER_H
