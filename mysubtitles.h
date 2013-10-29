#ifndef MYSUBTITLES_H
#define MYSUBTITLES_H

#include <QString>
#include <QList>
#include <QFont>
#include <QColor>

class TextFont
{
public:
    TextFont();
    void setFontId(QString fontId);
    void setFontColor(QString fontColor);
    void setFontEffect(QString fontEffect);
    void setFontEffectColor(QString fontEffectColor);
    void setFontSize(QString fontSize);
    void setFontItalic(QString fontItalic);
    void setFontUnderlined(QString fontUnderlined);
    void setFontScript(QString fontScript);

    QString fontId();
    QString fontColor();
    QString fontEffect();
    QString fontEffectColor();
    QString fontSize();
    QString fontItalic();
    QString fontUnderlined();
    QString fontScript();

    bool findDiff(TextFont & font);

private:
    // attribute Id
    QString mFontId;
    // attribute Color
    QString mColor;
    // attribute Effect
    QString mEffect;
    // attribute EffectColor
    QString mEffectColor;
    // attribute Size
    QString mSize;
    // attribute Italic
    QString mItalic;
    // attribute Underlined
    QString mUnderlined;
    // attribute Script
    QString mScript;
};



class TextLine
{
public:
    TextLine();

    void setLine(QString line);
    void setFont(TextFont font);
    void setTextVPosition(QString textVPosition);
    void setTextHPosition(QString textHPosition);
    void setTextVAlign(QString textVAlign);
    void setTextHAlign(QString textHAlign);
    void setTextDirection(QString textDirection);

    QString Line();
    TextFont& Font();
    QString textVPosition();
    QString textHPosition();
    QString textVAlign();
    QString textHAlign();
    QString textDirection();

private:
    QString mLine;
    TextFont mFont;
    QString mVPosition;
    QString mHPosition;
    QString mVAlign;
    QString mHAlign;
    QString mDirection;
};



class MySubtitles
{
public:
    MySubtitles();
    void clear();
    void setStartTime(QString startTime);
    void setEndTime(QString endTime);
    void setText(TextLine textLine, TextFont Font);
    void setText(QList<TextLine> textLineList);
    void insertText(TextLine textLine, TextFont font, qint16 index);
    void removeTextAt(qint16 index);
    void setDurationAuto(bool value);
    QString startTime();
    QString endTime();
    QList<TextLine>& text();
    bool isDurationAuto();
    bool isValid();

private:
    QString mStartTime;
    QString mEndTime;
    QList<TextLine> mText;
    bool mDurationAuto;
};



#endif // MYSUBTITLES_H
