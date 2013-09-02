#ifndef DCSUB_H
#define DCSUB_H

#include "myfilereader.h"
#include "myfilewriter.h"
#include "mysubtitles.h"
#include <QFile>
#include <QDomElement>
#include <QString>
#include <QFont>
#include <QColor>
#include <QList>

class DcSub
{
public:
    DcSub();
    QList<MySubtitles> open(QString fileName);
    void save(MyFileWriter & file, QList<MySubtitles> subtitlesList);

private:
    void changeFont(QDomElement xmlElement);
    void parseTree(QDomElement xmlElement);

    QList<TextFont> mFontList;
    TextLine mNewText;
    TextFont mNewFont;
    MySubtitles mNewSubtitle;
    QList<MySubtitles> mSubtitlesList;

    // attribute Id
    QString mFontId;
    // attribute Color
    QColor mColor;
    // attribute Effect
    QString mEffect;
    // attribute EffectColor
    QColor mEffectColor;
    // attribute Size
    qint16 mSize;
    // attribute Italic
    bool mItalic;
    // attribute Underlined
    bool mUnderlined;
    // attribute Script
    QString mScript;
};

#endif // DCSUB_H
