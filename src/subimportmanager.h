#ifndef SUBIMPORTMANAGER_H
#define SUBIMPORTMANAGER_H

#include "mysubtitlefileparser.h"
#include "myfilereader.h"

class SubImportManager
{
public:
    SubImportManager();
    static MySubtitleFileParser* findSubFileParser(MyFileReader fileReader);
};

#endif // SUBIMPORTMANAGER_H
