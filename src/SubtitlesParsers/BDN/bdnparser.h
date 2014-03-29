#ifndef BDNPARSER_H
#define BDNPARSER_H

#include "subtitlefileparser.h"
#include "filereader.h"
#include "filewriter.h"
#include "subtitles.h"
#include "subexportdialog.h"

class BDNparser : public MySubtitleFileParser
{
public:
    BDNparser();
    QList<MySubtitles> open(MyFileReader file);
    bool save(MyFileWriter & file, QList<MySubtitles> subtitlesList, SubExportDialog* exportDialog);
    bool readSample(MyFileReader file);
};

#endif // BDNPARSER_H
