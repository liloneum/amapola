#ifndef SUBRIPPARSER_H
#define SUBRIPPARSER_H

#include "mysubtitlefileparser.h"
#include "myfilereader.h"
#include "myfilewriter.h"
#include "mysubtitles.h"

class SubRipParser : public MySubtitleFileParser
{
public:
    SubRipParser();
    QList<MySubtitles> open(MyFileReader file);
    void save(MyFileWriter & file, QList<MySubtitles> subtitlesList);
};

#endif // SUBRIPPARSER_H
