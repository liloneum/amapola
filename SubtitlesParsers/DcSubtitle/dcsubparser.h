#ifndef DCSUBPARSER_H
#define DCSUBPARSER_H

#include "myfilereader.h"
#include "myfilewriter.h"
#include "mysubtitles.h"
#include "mysubtitlefileparser.h"
#include <QFile>
#include <QDomElement>
#include <QString>
#include <QFont>
#include <QColor>
#include <QList>

class DcSubParser : public MySubtitleFileParser
{
public:
    DcSubParser();
    QList<MySubtitles> open(MyFileReader file);
    void save(MyFileWriter & file, QList<MySubtitles> subtitlesList);

private:
    void changeFont(QDomElement xmlElement);
    void parseTree(QDomElement xmlElement);
    void writeFont(QDomElement* xmlElement, TextFont previousFont, TextFont newFont);

    // TextFont containers list
    QList<TextFont> mFontList;
    // TextLine container
    TextLine mNewText;
    // TextFont container
    TextFont mNewFont;
    // Subtitles container
    MySubtitles mNewSubtitle;
    // MySubtitles containers list
    QList<MySubtitles> mSubtitlesList;

    // attribute Color
    QColor mColor;
};

#endif // DCSUBPARSER_H
