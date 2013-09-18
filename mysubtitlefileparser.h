#ifndef MYSUBTITLEFILEPARSER_H
#define MYSUBTITLEFILEPARSER_H

#include "myfilereader.h"
#include "myfilewriter.h"
#include "mysubtitles.h"

// Class with virtual methods used to parse subtitles file
class MySubtitleFileParser
{
public:
    //MySubtitleFileParser();
    virtual QList<MySubtitles> open(MyFileReader file) = 0;
    virtual void save(MyFileWriter & file, QList<MySubtitles> subtitlesList) = 0;
};

#endif // MYSUBTITLEFILEPARSER_H
