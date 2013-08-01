#include "dcsubparser.h"
#include <QXmlSimpleReader>
#include <QDomDocument>
#include <QMessageBox>
#include <QUuid>
#include <mainwindow.h>
#include "myattributesconverter.h"

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


DcSubParser::DcSubParser() {

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


void DcSubParser::parseTree(QDomElement xmlElement) {

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

            start_time = MyAttributesConverter::toTimeHMSms(start_time);

            if ( !start_time.isEmpty() ) {

                mNewSubtitle.setStartTime(start_time);
            }


            //TimeOut
            QString end_time = xmlElement.attribute("TimeOut");

            end_time = MyAttributesConverter::toTimeHMSms(end_time);

            if ( !end_time.isEmpty() ) {

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


QList<MySubtitles> DcSubParser::open(MyFileReader file) {

    QFile file_read( file.getFileName() );

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
    //mFontList.first().setFontSize( QString::number( MyAttributesConverter::fontHeightToSize(mFontList.first().fontId(), FONT_SIZE_DEFAULT_VALUE) ) );
    mFontList.first().setFontSize(FONT_SIZE_DEFAULT_VALUE);

    QDomElement xml_element = xml_load_font.nextSibling().toElement();

    parseTree(xml_element);

    if ( mNewSubtitle.isValid() ) {
        mSubtitlesList.append(mNewSubtitle);
        mNewSubtitle.clear();
    }

    return mSubtitlesList;
}

void DcSubParser::save(MyFileWriter & file, QList<MySubtitles> subtitlesList) {

    QDomDocument doc("dcsub");

    QDomProcessingInstruction xml_version = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(xml_version);

    // Create the root node "DCSubtitle"
    QDomElement xml_root = doc.createElement("DCSubtitle");
    xml_root.setAttribute("Version", "1.1");
    doc.appendChild(xml_root);

    // Generate an UUID an create the node "SubtitleID"
    QDomElement xml_SubId = doc.createElement("SubtitleID");
    xml_root.appendChild( xml_SubId );
    QDomText subid_text = doc.createTextNode( QUuid::createUuid().toString().remove('{').remove('}') );
    xml_SubId.appendChild(subid_text);

    // Add "MovieTitle" node
    QDomElement xml_MovTitl = doc.createElement("MovieTitle");
    xml_root.appendChild(xml_MovTitl);
    QDomText movtitl_text = doc.createTextNode("the movie title");
    xml_MovTitl.appendChild(movtitl_text);

    // Add "ReelNumber" node
    QDomElement xml_RealNum = doc.createElement("ReelNumber");
    xml_root.appendChild(xml_RealNum);
    QDomText realnum_text = doc.createTextNode("1");
    xml_RealNum.appendChild(realnum_text);

    // Add "Language" node
    QDomElement xml_Lang = doc.createElement("Language");
    xml_root.appendChild(xml_Lang);
    QDomText lang_text = doc.createTextNode("fr");
    xml_Lang.appendChild(lang_text);

    // Add "LoadFont" node
    QDomElement xml_LoadFont = doc.createElement("LoadFont");
    QString font_id = subtitlesList.first().text().first().Font().fontId();
    mFontList.first().setFontId(font_id);
    xml_LoadFont.setAttribute("Id", font_id);
    xml_LoadFont.setAttribute("URI", (font_id +".ttf"));
    xml_root.appendChild(xml_LoadFont);

    TextFont text_font0 = subtitlesList.first().text().first().Font();
    QDomElement xml_font0 = doc.createElement("Font");
    this->writeFont(&xml_font0, mFontList.first(), text_font0);
    xml_root.appendChild(xml_font0);

    for ( qint32 i = 0; i < subtitlesList.size(); i++ ) {

        MySubtitles current_subtitle = subtitlesList.at(i);

        QDomElement xml_Subtitle = doc.createElement("Subtitle");
        xml_Subtitle.setAttribute("SpotNumber", QString::number(i+1));
        xml_Subtitle.setAttribute("FadeUpTime", "TO DO");
        xml_Subtitle.setAttribute("FadeDownTime", "TO DO");
        xml_Subtitle.setAttribute("TimeIn", MyAttributesConverter::toTimeHMSticks( current_subtitle.startTime() ));
        xml_Subtitle.setAttribute("TimeOut", MyAttributesConverter::toTimeHMSticks( current_subtitle.endTime() ));
        xml_font0.appendChild(xml_Subtitle);

        QList<TextLine> text_list = current_subtitle.text();

        for ( qint32 j = 0; j < text_list.size(); j++ ) {

            TextLine text_line = text_list.at(j);
            TextFont text_font = text_line.Font();
            QDomElement xml_newfont = doc.createElement("Font");
            this->writeFont(&xml_newfont, text_font0, text_font);

            QDomElement xml_current_element;

            if ( xml_newfont.hasAttributes() ) {
                xml_Subtitle.appendChild(xml_newfont);
                xml_current_element = xml_newfont;
            }
            else {
                xml_current_element = xml_Subtitle;
            }

            QDomElement xml_text = doc.createElement("Text");

            if ( text_line.textDirection() == "") {
                xml_text.setAttribute("Direction", TEXT_DIRECTION_DEFAULT_VALUE);
            }
            else {
                xml_text.setAttribute("Direction", text_line.textDirection());
            }

            if ( text_line.textHAlign() == "") {
                xml_text.setAttribute("HAlign", TEXT_HALIGN_DEFAULT_VALUE);
            }
            else {
                xml_text.setAttribute("HAlign", text_line.textHAlign());
            }

            if ( text_line.textHPosition() == "") {
                xml_text.setAttribute("HPosition", TEXT_HPOSITION_DEFAULT_VALUE);
            }
            else {
                xml_text.setAttribute("HPosition", text_line.textHPosition());
            }

            if ( text_line.textVAlign() == "") {
                xml_text.setAttribute("VAlign", TEXT_VALIGN_DEFAULT_VALUE);
            }
            else {
                xml_text.setAttribute("VAlign", text_line.textVAlign());
            }

            if ( text_line.textVPosition() == "") {
                xml_text.setAttribute("VPosition", TEXT_VPOSITION_DEFAULT_VALUE);
            }
            else {
                xml_text.setAttribute("VPosition", text_line.textVPosition());
            }

            xml_current_element.appendChild(xml_text);

            QDomText text_text = doc.createTextNode(text_line.Line());
            xml_text.appendChild(text_text);
        }
    }

    QString xml = doc.toString();
    file.write( xml );
}

void DcSubParser::writeFont(QDomElement* xmlElement, TextFont previousFont, TextFont newFont) {

    if ( previousFont.findDiff(newFont) ) {

        if ( newFont.fontId() != "" ) {
            xmlElement->setAttribute("Id", newFont.fontId());
        }

        if ( newFont.fontColor() != "" ) {
            xmlElement->setAttribute("Color", newFont.fontColor());
        }

        if ( newFont.fontEffect() != "" ) {
            xmlElement->setAttribute("Effect", newFont.fontEffect());
        }

        if ( newFont.fontEffectColor() != "" ) {
            xmlElement->setAttribute("EffectColor", newFont.fontEffectColor());
        }

        if ( newFont.fontItalic() != "" ) {
            xmlElement->setAttribute("Italic", newFont.fontItalic());
        }

        if ( newFont.fontUnderlined() != "" ) {
            xmlElement->setAttribute("Underlined", newFont.fontUnderlined());
        }

        if ( newFont.fontScript() != "" ) {
            xmlElement->setAttribute("Script", newFont.fontScript());
        }

        if ( newFont.fontSize() != "" ) {
            //QString new_font_size = QString::number( MyAttributesConverter::fontSizeToHeight(previousFont.fontId(), newFont.fontSize()) );
            QString new_font_size = newFont.fontSize();
            xmlElement->setAttribute("Size", new_font_size);
        }
    }
}

void DcSubParser::changeFont(QDomElement xmlElement) {

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
    QString font_size;
    if ( !xmlElement.attribute("Size").isNull() ) {
        //font_size = QString::number( MyAttributesConverter::fontHeightToSize(new_font.fontId(), xmlElement.attribute("Size")) );
        font_size = xmlElement.attribute("Size");
    }
    else {
        font_size = mFontList.last().fontSize();
    }

    new_font.setFontSize( font_size );


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
