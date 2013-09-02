#include "mysubtitles.h"

#include <QColor>


MySubtitles::MySubtitles()
{
    mStartTime = "";
    mEndTime = "";
    //mText = "";
    mText.clear();
}

void MySubtitles::clear() {

    mStartTime = "";
    mEndTime = "";
    //mText = "";
    mText.clear();
}

void MySubtitles::setStartTime(QString startTime) {

    mStartTime = startTime;
}

void MySubtitles::setEndTime(QString endTime) {

    mEndTime = endTime;
}

//void MySubtitles::setText(QString text) {

//    mText = text;
//}

void MySubtitles::setText(TextLine textLine, TextFont font) {

    textLine.setFont(font);

    mText.append(textLine);
}

QString MySubtitles::startTime() {

    return mStartTime;
}

QString MySubtitles::endTime() {

    return mEndTime;
}

//QString MySubtitles::text() {

//    return mText;
//}

QList<TextLine> MySubtitles::text() {

    return mText;
}

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
}

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

