#include "dcsub.h"
#include <QXmlSimpleReader>
#include <QDomDocument>
#include <QMessageBox>
#include <mainwindow.h>

#define FONT_ID_DEFAULT_VALUE ""
#define FONT_COLOR_DEFAULT_VALUE "FFFFFFFF"
#define FONT_EFFECT_DEFAULT_VALUE "shadow"
#define FONT_EFFECT_COLOR_DEFAULT_VALUE "FF000000"
#define FONT_ITALIC_DEFAULT_VALUE "no"
#define FONT_SCRIPT_DEFAULT_VALUE "normal"
#define FONT_SIZE_DEFAULT_VALUE "42"
#define FONT_UNDERLINED_DEFAULT_VALUE "no"

#define TEXT_DIRECTION_DEFAULT_VALUE "horizontal"
#define TEXT_HALIGN_DEFAULT_VALUE "center"
#define TEXT_HPOSITION_DEFAULT_VALUE "0"
#define TEXT_VALIGN_DEFAULT_VALUE "center"
#define TEXT_VPOSITION_DEFAULT_VALUE "0"


DcSub::DcSub() {

    mNewSubtitle.clear();
    mSubtitlesList.clear();

    TextFont font_init;

    font_init.setFontId( FONT_ID_DEFAULT_VALUE );
    font_init.setFontColor( FONT_COLOR_DEFAULT_VALUE );
    font_init.setFontEffect( FONT_EFFECT_DEFAULT_VALUE );
    font_init.setFontEffectColor( FONT_EFFECT_COLOR_DEFAULT_VALUE );
    font_init.setFontItalic( FONT_ITALIC_DEFAULT_VALUE );
    font_init.setFontScript( FONT_SCRIPT_DEFAULT_VALUE );
    font_init.setFontSize( FONT_SIZE_DEFAULT_VALUE );
    font_init.setFontUnderlined( FONT_UNDERLINED_DEFAULT_VALUE );

    mFontList.append(font_init);

}


void DcSub::parseTree(QDomElement xmlElement) {

    bool font_inside_text = false;
    bool font_inside_whole_text = false;

    while ( !xmlElement.isNull() ) {

        if (xmlElement.tagName() == "Font") {

            changeFont(xmlElement);
        }
        else if (xmlElement.tagName() == "Subtitle") {

            // attribute SpotNumber ignored, not usefull
            // attribute FadeUpTime, ignored
            // attribute FadeDownTime, ignored

            if ( mNewSubtitle.isValid() ) {
                mSubtitlesList.append(mNewSubtitle);
                mNewSubtitle.clear();
            }

            // TimeIn
            QString start_time = xmlElement.attribute("TimeIn");

            if ( !start_time.isEmpty() ) {

                start_time.replace(start_time.lastIndexOf(":"), 1, ".");
                mNewSubtitle.setStartTime(start_time);
            }

            //TimeOut
            QString end_time = xmlElement.attribute("TimeOut");

            if ( !end_time.isEmpty() ) {

                end_time.replace(end_time.lastIndexOf(":"), 1, ".");
                mNewSubtitle.setEndTime(end_time);
            }
        }
        else if (xmlElement.tagName() == "Text") {

            if ( !xmlElement.text().isEmpty() ) {

                // attribute Direction
                if ( !xmlElement.attribute("Direction").isNull() ) {
                mNewText.setTextDirection( xmlElement.attribute("Direction") );
                }
                else {
                    mNewText.setTextDirection( TEXT_DIRECTION_DEFAULT_VALUE );
                }

                // attribute HAlign
                if ( !xmlElement.attribute("HAlign").isNull() ) {
                mNewText.setTextHAlign( xmlElement.attribute("HAlign") );
                }
                else {
                    mNewText.setTextHAlign( TEXT_HALIGN_DEFAULT_VALUE );
                }

                // attribute HPosition
                if ( !xmlElement.attribute("HPosition").isNull() ) {
                mNewText.setTextHPosition( xmlElement.attribute("HPosition") );
                }
                else {
                    mNewText.setTextHPosition( TEXT_HPOSITION_DEFAULT_VALUE );
                }

                // attribute VAlign
                if ( !xmlElement.attribute("VAlign").isNull() ) {
                mNewText.setTextVAlign( xmlElement.attribute("VAlign") );
                }
                else {
                    mNewText.setTextVAlign( TEXT_VALIGN_DEFAULT_VALUE );
                }

                // attribute VPosition
                if ( !xmlElement.attribute("VPosition").isNull() ) {
                mNewText.setTextVPosition( xmlElement.attribute("VPosition") );
                }
                else {
                     mNewText.setTextVPosition( TEXT_VPOSITION_DEFAULT_VALUE );
                }

                QString text(xmlElement.text());
                QDomNodeList font_node_list = xmlElement.elementsByTagName("Font");
                QString text_with_font_changed;

                for ( qint32 j = 0; j < font_node_list.size(); j++ ) {

                    QDomElement xml_font_changed = font_node_list.at(j).toElement();

                    font_inside_text = true;

                    text_with_font_changed = xml_font_changed.text();
                    if ( text_with_font_changed == text) {
                        changeFont(xml_font_changed);
                        font_inside_whole_text = true;
                        continue;
                    }
                    if ( xml_font_changed.hasAttribute("Italic") ) {

                        if (xml_font_changed.attribute("Italic") == "yes") {
                            text.replace(text.indexOf(text_with_font_changed),
                                         text_with_font_changed.size(),
                                         "<i>" + text_with_font_changed + "</i>");
                        }
                    }
                    if ( xml_font_changed.hasAttribute("Color") ) {

                        QString current_color;
                        current_color.setNum(mColor.rgba(), 16);

                        if ( xml_font_changed.attribute("Color") != current_color ){
                            text.replace(text.indexOf(text_with_font_changed),
                                         text_with_font_changed.size(),
                                         "<font color = #" + xml_font_changed.attribute("Color") + ">"
                                         + text_with_font_changed + "</font>");
                        }
                    }
                }

                mNewText.setLine(text);
                mNewSubtitle.setText(mNewText, mFontList.last());

                if ( font_inside_whole_text == true ) {
                    mFontList.removeLast();
                    font_inside_whole_text =false;
                }
            }
        }

        if ( !xmlElement.firstChild().isNull() ) {
            if ( font_inside_text == false) {
                parseTree( xmlElement.firstChild().toElement() );
            }
        }

        if ( xmlElement.tagName() == "Font" ) {
            mFontList.removeLast();
        }

        xmlElement = xmlElement.nextSibling().toElement();
    }
}


QList<MySubtitles> DcSub::open(QString fileName) {

    QFile file_read(fileName);
    //QList<MySubtitles> subtitles_list;

    if ( !file_read.open(QIODevice::ReadOnly) ) {
        // QMessageBox::warning(this, "loading", "Failed to load file.");
        return mSubtitlesList;
    }

    QDomDocument doc("dcsub");
    if( !doc.setContent(&file_read)) {
        // QMessageBox::warning(this, "Loading", "Failed to load file." );
        file_read.close();
        return mSubtitlesList;
    }

    file_read.close();

    QDomElement xml_root = doc.documentElement();
    if(xml_root.tagName() != "DCSubtitle") {
        // QMessageBox::warning( this, "Loading", "Invalid file." );
        return mSubtitlesList;
    }

    QDomElement xml_load_font = xml_root.firstChildElement("LoadFont");
    mFontList.first().setFontId( xml_load_font.attribute("Id") );

    QDomElement xml_element = xml_load_font.nextSibling().toElement();

    parseTree(xml_element);

    if ( mNewSubtitle.isValid() ) {
        mSubtitlesList.append(mNewSubtitle);
        mNewSubtitle.clear();
    }

    return mSubtitlesList;
}


void DcSub::changeFont(QDomElement xmlElement) {

    if ( ( xmlElement.isNull() ) || ( xmlElement.tagName() != "Font") ) {
        return;
    }

    TextFont new_font;

    // attribute Id
    if ( !xmlElement.attribute("Id").isNull() ) {
        new_font.setFontId( xmlElement.attribute("Id") );
    }
    else {
        new_font.setFontId( mFontList.last().fontId() );
    }

    // attribute Color
    //mColor = QColor::fromRgba(xmlElement.attribute("Color").toUInt(&ok,16));
    if ( !xmlElement.attribute("Color").isNull() ) {
        new_font.setFontColor( xmlElement.attribute("Color") );
    }
    else {
        new_font.setFontColor( mFontList.last().fontColor() );
    }

    // attribute Effect
    if ( !xmlElement.attribute("Effect").isNull() ) {
        new_font.setFontEffect( xmlElement.attribute("Effect") );
    }
    else {
        new_font.setFontEffect( mFontList.last().fontEffect() );
    }

    // attribute EffectColor
    if ( !xmlElement.attribute("EffectColor").isNull() ) {
        new_font.setFontEffectColor( xmlElement.attribute("EffectColor") );
    }
    else {
        new_font.setFontEffectColor( mFontList.last().fontEffectColor() );
    }

    // attribute Size
    //mSize = xmlElement.attribute("Size").toInt();
    if ( !xmlElement.attribute("Size").isNull() ) {
        new_font.setFontSize( xmlElement.attribute("Size") );
    }
    else {
        new_font.setFontSize( mFontList.last().fontSize() );
    }

    // attribute AspectAdjust

    // attribute Italic
    if ( !xmlElement.attribute("Italic").isNull() ) {
        new_font.setFontItalic( xmlElement.attribute("Italic") );
    }
    else {
        new_font.setFontItalic( mFontList.last().fontItalic() );
    }

    // attribute Underlined
    if ( !xmlElement.attribute("Underlined").isNull() ) {
        new_font.setFontUnderlined( xmlElement.attribute("Underlined") );
    }
    else {
        new_font.setFontUnderlined( mFontList.last().fontUnderlined() );
    }

    // attribute Script
    if ( !xmlElement.attribute("Script").isNull() ) {
        new_font.setFontScript( xmlElement.attribute("Script") );
    }
    else {
        new_font.setFontScript( mFontList.last().fontScript() );
    }

    mFontList.append(new_font);

}
