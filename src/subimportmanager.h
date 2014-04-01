#ifndef SUBIMPORTMANAGER_H
#define SUBIMPORTMANAGER_H

#include "subtitlefileparser.h"
#include "filereader.h"

class SubImportManager
{
public:
    SubImportManager();
    static SubtitleFileParser* findSubFileParser(FileReader fileReader);
};

#endif // SUBIMPORTMANAGER_H
