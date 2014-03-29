#ifndef DCSUBSMPTEPARSER_H
#define DCSUBSMPTEPARSER_H

#include "filereader.h"
#include "filewriter.h"
#include "subtitles.h"
#include "subtitlefileparser.h"
#include "subexportdialog.h"
#include <QDomElement>
#include <QFile>

class DcSubSmpteParser : public MySubtitleFileParser
{
public:
    DcSubSmpteParser();
    QList<MySubtitles> open(MyFileReader file);
    bool save(MyFileWriter & file, QList<MySubtitles> subtitlesList, SubExportDialog* exportDialog);
    bool readSample(MyFileReader file);

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
    MySubtitles mNewSubtitle;
    // MySubtitles containers list
    QList<MySubtitles> mSubtitlesList;
    // TimeCodeRate
    qint16 mTimeCodeRate;

    QString mPreferredEffect;
    QString mCurrentEffect;
};

#endif // DCSUBSMPTEPARSER_H
