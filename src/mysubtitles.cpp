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
//          - effect (border, shadow, background, none)
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
    mDurationAuto = true;
}

void MySubtitles::clear() {

    mStartTime = "";
    mEndTime = "";
    mText.clear();
    mDurationAuto = true;
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

void MySubtitles::setText(QList<TextLine> textLineList) {

    mText = textLineList;
}

void MySubtitles::insertText(TextLine textLine, TextFont font, qint16 index) {

    textLine.setFont(font);

    mText.insert(index, textLine);
}

void MySubtitles::removeTextAt(qint16 index) {

    mText.removeAt(index);
}

void MySubtitles::setDurationAuto(bool value) {

    mDurationAuto = value;
}

// Assessors
QString MySubtitles::startTime() {

    return mStartTime;
}

QString MySubtitles::endTime() {

    return mEndTime;
}

QList<TextLine>& MySubtitles::text() {

    return mText;
}

bool MySubtitles::isDurationAuto() {

    return mDurationAuto;
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

TextFont& TextLine::Font() {

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

    // attribute Shadow Effect
    mShadowEffect = "";

    // attribute Shadow EffectColor
    mShadowEffectColor = "";

    // attribute Border Effect
    mBorderEffect = "";

    // attribute Border EffectColor
    mBorderEffectColor = "";

    // attribute Background Effect
    mBackgroundEffect = "";

    // attribute Background EffectColor
    mBackgroundEffectColor = "";

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

void TextFont::setFontShadowEffect(QString fontEffect) {
    mShadowEffect = fontEffect;
}

void TextFont::setFontShadowEffectColor(QString fontEffectColor) {
    mShadowEffectColor = fontEffectColor;
}

void TextFont::setFontBorderEffect(QString fontEffect) {
    mBorderEffect = fontEffect;
}

void TextFont::setFontBorderEffectColor(QString fontEffectColor) {
    mBorderEffectColor = fontEffectColor;
}

void TextFont::setFontBackgroundEffect(QString fontEffect) {
    mBackgroundEffect = fontEffect;
}

void TextFont::setFontBackgroundEffectColor(QString fontEffectColor) {
    mBackgroundEffectColor = fontEffectColor;
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

QString TextFont::fontShadowEffect() {
    return mShadowEffect;
}

QString TextFont::fontShadowEffectColor() {
    return mShadowEffectColor;
}

QString TextFont::fontBorderEffect() {
    return mBorderEffect;
}

QString TextFont::fontBorderEffectColor() {
    return mBorderEffectColor;
}

QString TextFont::fontBackgroundEffect() {
    return mBackgroundEffect;
}

QString TextFont::fontBackgroundEffectColor() {
    return mBackgroundEffectColor;
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

    if ( this->fontShadowEffect() == font.fontShadowEffect() ) {
        font.setFontShadowEffect("");
    }
    else is_different = true;

    if ( this->fontShadowEffectColor() == font.fontShadowEffectColor() ) {
        font.setFontShadowEffectColor("");
    }
    else is_different = true;

    if ( this->fontBorderEffect() == font.fontBorderEffect() ) {
        font.setFontBorderEffect("");
    }
    else is_different = true;

    if ( this->fontBorderEffectColor() == font.fontBorderEffectColor() ) {
        font.setFontBorderEffectColor("");
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
