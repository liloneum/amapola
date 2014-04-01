#ifndef DCSUBSMPTEPARSER_H
#define DCSUBSMPTEPARSER_H

#include "filereader.h"
#include "filewriter.h"
#include "subtitles.h"
#include "subtitlefileparser.h"
#include "subexportdialog.h"
#include <QDomElement>
#include <QFile>

class DcSubSmpteParser : public SubtitleFileParser
{
public:
    DcSubSmpteParser();
    QList<Subtitles> open(FileReader file);
    bool save(FileWriter & file, QList<Subtitles> subtitlesList, SubExportDialog* exportDialog);
    bool readSample(FileReader file);

private:
    void changeFont(QDomElement xmlElement);
    void parseTree(QDomElement xmlElement);
    void writeFont(QDomElement* xmlElement, TextFont previousFont, TextFont newFont);
    QString dcstFilter(QFile* file);

    // TextFont containers list
    QList<TextFont> mFontList;
    // Fond name list
    QList<QString> mFontIdList;
    // Subtitles container
    Subtitles mNewSubtitle;
    // Subtitles containers list
    QList<Subtitles> mSubtitlesList;
    // TimeCodeRate
    qint16 mTimeCodeRate;

    QString mPreferredEffect;
    QString mCurrentEffect;
};

#endif // DCSUBSMPTEPARSER_H
