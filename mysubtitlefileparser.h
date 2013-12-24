#ifndef MYSUBTITLEFILEPARSER_H
#define MYSUBTITLEFILEPARSER_H

#include "myfilereader.h"
#include "myfilewriter.h"
#include "mysubtitles.h"
#include "subexportdialog.h"

// Class with virtual methods used to parse subtitles file
class MySubtitleFileParser
{
public:
    //MySubtitleFileParser();
    virtual QList<MySubtitles> open(MyFileReader file) = 0;
    virtual bool save(MyFileWriter & file, QList<MySubtitles> subtitlesList, SubExportDialog* exportDialog) = 0;
    virtual bool readSample(MyFileReader file) =0;
};

#endif // MYSUBTITLEFILEPARSER_H
