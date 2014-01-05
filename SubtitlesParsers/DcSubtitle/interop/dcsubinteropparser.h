#ifndef DCSUBINTEROPPARSER_H
#define DCSUBINTEROPPARSER_H

#include "myfilereader.h"
#include "myfilewriter.h"
#include "mysubtitles.h"
#include "mysubtitlefileparser.h"
#include "subexportdialog.h"
#include <QFile>
#include <QDomElement>
#include <QString>
#include <QFont>
#include <QColor>
#include <QList>

class DcSubInteropParser : public MySubtitleFileParser
{
public:
    DcSubInteropParser();
    QList<MySubtitles> open(MyFileReader file);
    bool save(MyFileWriter & file, QList<MySubtitles> subtitlesList, SubExportDialog* exportDialog);
    bool readSample(MyFileReader file);
    QString convertVposToAmapolaRef(QString fontId, QString fontSize, QString vAlign, QString vPos);
    QString convertVposFromAmapolaRef(QString fontId, QString fontSize, QString vAlign, QString vPos);

private:
    void changeFont(QDomElement xmlElement);
    void parseTree(QDomElement xmlElement);
    void writeFont(QDomElement* xmlElement, TextFont previousFont, TextFont newFont);

    // TextFont containers list
    QList<TextFont> mFontList;
    // Font name list
    QList<QString> mFontIdList;
    // Subtitles container
    MySubtitles mNewSubtitle;
    // MySubtitles containers list
    QList<MySubtitles> mSubtitlesList;

    QString mPreferredEffect;
    QString mCurrentEffect;
};

#endif // DCSUBINTEROPPARSER_H
