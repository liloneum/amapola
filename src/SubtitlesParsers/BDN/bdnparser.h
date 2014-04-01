#ifndef BDNPARSER_H
#define BDNPARSER_H

#include "subtitlefileparser.h"
#include "filereader.h"
#include "filewriter.h"
#include "subtitles.h"
#include "subexportdialog.h"

class BDNparser : public SubtitleFileParser
{
public:
    BDNparser();
    QList<Subtitles> open(FileReader file);
    bool save(FileWriter & file, QList<Subtitles> subtitlesList, SubExportDialog* exportDialog);
    bool readSample(FileReader file);
};

#endif // BDNPARSER_H
