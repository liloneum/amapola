#ifndef SUBRIPPARSER_H
#define SUBRIPPARSER_H

#include "myfilereader.h"
#include "mysubtitles.h"

class SubRipParser
{
public:
    SubRipParser();
    QList<MySubtitles> open(MyFileReader file);
};

#endif // SUBRIPPARSER_H
