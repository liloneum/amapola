#ifndef BDNPARSER_H
#define BDNPARSER_H

#include "mysubtitlefileparser.h"
#include "myfilereader.h"
#include "myfilewriter.h"
#include "mysubtitles.h"
#include "subexportdialog.h"

class BDNparser : public MySubtitleFileParser
{
public:
    BDNparser();
    QList<MySubtitles> open(MyFileReader file);
    void save(MyFileWriter & file, QList<MySubtitles> subtitlesList, SubExportDialog* exportDialog);
    bool readSample(MyFileReader file);
};

#endif // BDNPARSER_H
