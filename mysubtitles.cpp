#include "mysubtitles.h"

#include <QColor>

// The "MySubtitles" class is a string container for text and font, position properties
// MySubtitles :
//  - Start time (format HH:MM:SS.zzz)
//  - End time (format HH:MM:SS.zzz)
//  - Text :
//      - line (1 line of text)
//      - horizontal alignment (left, center, right)
//      - vertical alignment (top, center, bottom)
//      - horizontal position (in %)
//      - vertical position (in %)
//      - direction (vertical (not used), horizontal).
//      - Font :
//          - name
//          - size (in pt)
//          - effect (border, shadow, none) - not used yet
//          - effect color (#AARRGGBB)
//          - color (#AARRGGBB)
//          - italic (yes / no)
//          - underlined (yes / no)
//          - script (normal, super, sub) - not used


//*************************************************//
//               Class MySubtitles                 //
//*************************************************//
MySubtitles::MySubtitles()
{
    mStartTime = "";
    mEndTime = "";
    mText.clear();
}

void MySubtitles::clear() {

    mStartTime = "";
    mEndTime = "";
    mText.clear();
}

// Mutators

void MySubtitles::setStartTime(QString startTime) {

    mStartTime = startTime;
}

void MySubtitles::setEndTime(QString endTime) {

    mEndTime = endTime;
}

void MySubtitles::setText(TextLine textLine, TextFont font) {

    textLine.setFont(font);

    mText.append(textLine);
}

// Assessors
QString MySubtitles::startTime() {

    return mStartTime;
}

QString MySubtitles::endTime() {

    return mEndTime;
}

QList<TextLine> MySubtitles::text() {

    return mText;
}

// A subtitle is valid if there are "time in", "time out", and text
bool MySubtitles::isValid() {

    if ( ( !mStartTime.isEmpty() ) && ( !mEndTime.isEmpty() ) && ( !mText.isEmpty() ) ) {
        return true;
    }
    else {
        return false;
    }
}


//*************************************************//
//               Class TextLine                   //
//*************************************************//

TextLine::TextLine() {

    mLine = "";
    mVPosition = "";
    mHPosition = "";
    mVAlign = "";
    mHAlign = "";
    mDirection = "";
}

// Mutators

void TextLine::setLine(QString line) {

    mLine = line;
}

void TextLine::setFont(TextFont font) {

    mFont = font;
}

void TextLine::setTextVPosition(QString textVPosition) {
    mVPosition = textVPosition;
}

void TextLine::setTextVAlign(QString textVAlign) {

    mVAlign = textVAlign;
}

void TextLine::setTextHPosition(QString textHPosition) {

    mHPosition = textHPosition;
}

void TextLine::setTextHAlign(QString textHAlign) {

    mHAlign = textHAlign;
}

void TextLine::setTextDirection(QString textDirection) {

    mDirection = textDirection;
}

// Assessors

QString TextLine::textVPosition() {

    return mVPosition;
}

QString TextLine::textVAlign() {

    return mVAlign;
}

QString TextLine::textHPosition() {

    return mHPosition;
}

QString TextLine::textHAlign() {

    return mHAlign;
}

QString TextLine::textDirection() {

    return mDirection;
}

QString TextLine::Line() {

    return mLine;
}

TextFont TextLine::Font() {

    return mFont;
}



//*************************************************//
//               Class TextFont                    //
//*************************************************//

TextFont::TextFont() {

    // attribute Id
    mFontId = "";

    // attribute Color
    mColor = "";

    // attribute Effect
    mEffect = "";

    // attribute EffectColor
    mEffectColor = "";

    // attribute Size
    mSize = "";

    // attribute Italic
    mItalic = "";

    // attribute Underlined
    mUnderlined = "";

    // attribute Script
    mScript = "";
}

// Mutators

void TextFont::setFontId(QString fontId) {
    mFontId = fontId;
}

void TextFont::setFontColor(QString fontColor) {
    mColor = fontColor;
}

void TextFont::setFontEffect(QString fontEffect) {
    mEffect = fontEffect;
}

void TextFont::setFontEffectColor(QString fontEffectColor) {
    mEffectColor = fontEffectColor;
}

void TextFont::setFontSize(QString fontSize) {
    mSize = fontSize;
}

void TextFont::setFontItalic(QString fontItalic) {
    mItalic = fontItalic;
}

void TextFont::setFontUnderlined(QString fontUnderlined) {
    mUnderlined = fontUnderlined;
}

void TextFont::setFontScript(QString fontScript) {
    mScript = fontScript;
}

// Assessors

QString TextFont::fontId() {
    return mFontId;
}

QString TextFont::fontColor() {
    return mColor;
}

QString TextFont::fontEffect() {
    return mEffect;
}

QString TextFont::fontEffectColor() {
    return mEffectColor;
}

QString TextFont::fontSize() {
    return mSize;
}

QString TextFont::fontItalic() {
    return mItalic;
}

QString TextFont::fontUnderlined() {
    return mUnderlined;
}

QString TextFont::fontScript() {
    return mScript;
}

// Compare two font containers and return a font containers
// with only the different properties. All the equal properties are setted to empty string
bool TextFont::findDiff(TextFont & font) {

    bool is_different = false;

    if ( this->fontId() == font.fontId() ) {
        font.setFontId("");
    }
    else is_different = true;

    if ( this->fontColor() == font.fontColor() ) {
        font.setFontColor("");
    }
    else is_different = true;

    if ( this->fontEffect() == font.fontEffect() ) {
        font.setFontEffect("");
    }
    else is_different = true;

    if ( this->fontEffectColor() == font.fontEffectColor() ) {
        font.setFontEffectColor("");
    }
    else is_different = true;

    if ( this->fontItalic() == font.fontItalic() ) {
        font.setFontItalic("");
    }
    else is_different = true;

    if ( this->fontUnderlined() == font.fontUnderlined() ) {
        font.setFontUnderlined("");
    }
    else is_different = true;

    if ( this->fontScript() == font.fontScript() ) {
        font.setFontScript("");
    }
    else is_different = true;

    if ( this->fontSize() == font.fontSize() ) {
        font.setFontSize("");
    }
    else is_different = true;

    return is_different;
}
