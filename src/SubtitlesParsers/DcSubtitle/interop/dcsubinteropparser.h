#ifndef DCSUBINTEROPPARSER_H
#define DCSUBINTEROPPARSER_H

#include "filereader.h"
#include "filewriter.h"
#include "subtitles.h"
#include "subtitlefileparser.h"
#include "subexportdialog.h"
#include <QFile>
#include <QDomElement>
#include <QString>
#include <QFont>
#include <QColor>
#include <QList>

class DcSubInteropParser : public SubtitleFileParser
{
public:
    DcSubInteropParser();
    QList<Subtitles> open(FileReader file);
    bool save(FileWriter & file, QList<Subtitles> subtitlesList, SubExportDialog* exportDialog);
    bool readSample(FileReader file);
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
    Subtitles mNewSubtitle;
    // Subtitles containers list
    QList<Subtitles> mSubtitlesList;

    QString mPreferredEffect;
    QString mCurrentEffect;
};

#endif // DCSUBINTEROPPARSER_H
