#include "dcsubinteropparser.h"
#include <QDomDocument>
#include <QMessageBox>
#include <QUuid>
#include "myattributesconverter.h"
#include <QXmlStreamReader>
#include <QTextEdit>
#include <QApplication>
#include <QDesktopWidget>


// See "Subtitle Specification (XML File Format) for DLP CinemaTM Projection Technology" documentation

// Define default values
#define FONT_ID_DEFAULT_VALUE ""
#define FONT_COLOR_DEFAULT_VALUE "FFFFFFFF"
#define FONT_SHADOW_EFFECT_DEFAULT_VALUE "yes"
#define FONT_BORDER_EFFECT_DEFAULT_VALUE "no"
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


DcSubInteropParser::DcSubInteropParser() {

    mNewSubtitle.clear();
    mSubtitlesList.clear();

    TextFont font_init;

    font_init.setFontId( FONT_ID_DEFAULT_VALUE );
    font_init.setFontColor( FONT_COLOR_DEFAULT_VALUE );
    font_init.setFontShadowEffect( FONT_SHADOW_EFFECT_DEFAULT_VALUE );
    font_init.setFontShadowEffectColor( FONT_EFFECT_COLOR_DEFAULT_VALUE );
    font_init.setFontBorderEffect( FONT_BORDER_EFFECT_DEFAULT_VALUE );
    font_init.setFontBorderEffectColor( FONT_EFFECT_COLOR_DEFAULT_VALUE );
    font_init.setFontItalic( FONT_ITALIC_DEFAULT_VALUE );
    font_init.setFontScript( FONT_SCRIPT_DEFAULT_VALUE );
    font_init.setFontSize( FONT_SIZE_DEFAULT_VALUE );
    font_init.setFontUnderlined( FONT_UNDERLINED_DEFAULT_VALUE );

    mFontList.append(font_init);

    mPreferredEffect = "";
    mCurrentEffect = "";
}

// Read a sample of the file (10 lines). Try to found "DCSubtitle" tag
bool DcSubInteropParser::readSample(MyFileReader file) {

    QXmlStreamReader xml_reader;
    QStringList lines_list = file.lines();

    for ( qint32 i = 0; i < lines_list.count(); i++ ) {

        xml_reader.addData(lines_list[i]);
        xml_reader.readNext();

        if ( xml_reader.isStartElement() ) {

            if ( xml_reader.name() == "DCSubtitle" ) {
                return true;
            }
        }

        if ( i == 10 ) {
            return false;
        }
    }
    return false;
}

// Parse the xml tree to retrieve the subtitles infos.
// This function is RECURSIVE to be able to handle "Font" tag in cascade
// So be carefull if you change something in the code
void DcSubInteropParser::parseTree(QDomElement xmlElement) {

    bool font_inside_text = false;
    bool font_inside_whole_text = false;

    while ( !xmlElement.isNull() ) {

        // If the tag name is "Font", retrieve the font parameters changed
        if (xmlElement.tagName() == "Font") {

            changeFont(xmlElement);
        }
        // If the tag name is "Subtitle"
        else if (xmlElement.tagName() == "Subtitle") {

            // attribute SpotNumber ignored, not usefull
            // attribute FadeUpTime, ignored
            // attribute FadeDownTime, ignored

            // Save previous subtitle and clear the container
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
        // If the tag name is "Text"
        else if (xmlElement.tagName() == "Text") {

            if ( !xmlElement.text().isEmpty() ) {

                TextLine new_text;

                QString font_id = mFontList.last().fontId();
                QString font_size = mFontList.last().fontSize();

                // attribute Direction
                if ( !xmlElement.attribute("Direction").isNull() ) {
                new_text.setTextDirection( MyAttributesConverter::dirToLtrTtb( xmlElement.attribute("Direction") ) );
                }
                else {
                    new_text.setTextDirection( TEXT_DIRECTION_DEFAULT_VALUE );
                }

                // attribute HAlign
                if ( !xmlElement.attribute("HAlign").isNull() ) {
                    new_text.setTextHAlign( xmlElement.attribute("HAlign") );
                }
                else {
                    new_text.setTextHAlign( TEXT_HALIGN_DEFAULT_VALUE );
                }

                // attribute HPosition
                if ( !xmlElement.attribute("HPosition").isNull() ) {
                    new_text.setTextHPosition( xmlElement.attribute("HPosition") );
                }
                else {
                    new_text.setTextHPosition( TEXT_HPOSITION_DEFAULT_VALUE );
                }

                // attribute VAlign
                if ( !xmlElement.attribute("VAlign").isNull() ) {
                    new_text.setTextVAlign( xmlElement.attribute("VAlign") );
                }
                else {
                    new_text.setTextVAlign( TEXT_VALIGN_DEFAULT_VALUE );
                }

                // attribute VPosition
                if ( !xmlElement.attribute("VPosition").isNull() ) {
                    new_text.setTextVPosition( this->convertVposToAmapolaRef(font_id, font_size, new_text.textVAlign(), xmlElement.attribute("VPosition")) );
                }
                else {
                     new_text.setTextVPosition( this->convertVposToAmapolaRef(font_id, font_size, new_text.textVAlign(), TEXT_VPOSITION_DEFAULT_VALUE) );
                }


                // Check for "Font" tag inside text
                QDomNodeList font_node_list = xmlElement.elementsByTagName("Font");
                QString text(xmlElement.text());
                QString text_with_font_changed;

                for ( qint32 j = 0; j < font_node_list.size(); j++ ) {

                    QDomElement xml_font_changed = font_node_list.at(j).toElement();

                    font_inside_text = true;

                    text_with_font_changed = xml_font_changed.text();
                    // If the "Font" tag catch whole text, change the font parameter
                    if ( text_with_font_changed == text) {
                        changeFont(xml_font_changed);
                        font_inside_whole_text = true;
                        continue;
                    }
                    // If the "Font" tag catch only a part of the text, look for "Italic", "Color" attribute
                    if ( xml_font_changed.hasAttribute("Italic") ) {

                        // If "Italic" attribute = "yes", add "<i></i>" markup inside text
                        if (xml_font_changed.attribute("Italic") == "yes") {
                            text.replace(text.indexOf(text_with_font_changed),
                                         text_with_font_changed.size(),
                                         "<i>" + text_with_font_changed + "</i>");
                        }
                    }
                    if ( xml_font_changed.hasAttribute("Underlined") ) {

                        // If "Underlined" attribute = "yes", add "<u></u>" markup inside text
                        if (xml_font_changed.attribute("Underlined") == "yes") {
                            text.replace(text.indexOf(text_with_font_changed),
                                         text_with_font_changed.size(),
                                         "<u>" + text_with_font_changed + "</u>");
                        }
                    }
                    if ( xml_font_changed.hasAttribute("Color") ) {

                        QString current_color_str;
                        current_color_str = mFontList.last().fontColor();
                        QString new_color_str = xml_font_changed.attribute("Color");

                        // If "Color" attribute is different than current color, add "<font color = #RRGGBB></font>" markup inside text
                        if ( new_color_str != current_color_str ){

                            if ( new_color_str.count() == 8 ) {

                                new_color_str = new_color_str.remove(0, 2);

                                text.replace(text.indexOf(text_with_font_changed),
                                             text_with_font_changed.size(),
                                             "<font color = #" +new_color_str  + ">"
                                             + text_with_font_changed + "</font>");
                            }
                        }
                    }
                }

                // Set text and font attributes in MySubtitltes container
                new_text.setLine( MyAttributesConverter::plainTextToHtml(text) );
                mNewSubtitle.setText(new_text, mFontList.last());

                if ( font_inside_whole_text == true ) {
                    mFontList.removeLast();
                    font_inside_whole_text = false;
                }
            }
        }

        // Go to next child node if exist (Recursive)
        if ( !xmlElement.firstChild().isNull() ) {
            if ( font_inside_text == false) {
                parseTree( xmlElement.firstChild().toElement() );
            }
        }

        // Exit from "Font" tag
        if ( xmlElement.tagName() == "Font" ) {
            mFontList.removeLast();
        }

        // Go to next sibling node
        xmlElement = xmlElement.nextSibling().toElement();
    }
}

// Parse DCSub file, retrieve subtitles infos.
// Subtitles read are saved in "MySubtitles" container
QList<MySubtitles> DcSubInteropParser::open(MyFileReader file) {

    QFile file_read( file.getFileName() );

    // Try to open the file
    if ( !file_read.open(QIODevice::ReadOnly) ) {
        // QMessageBox::warning(this, "loading", "Failed to load file.");
        return mSubtitlesList;
    }

    // Copy content of the file in QDomDocument
    QDomDocument doc("dcsub");
    if( !doc.setContent(&file_read)) {
        // QMessageBox::warning(this, "Loading", "Failed to load file." );
        file_read.close();
        return mSubtitlesList;
    }

    file_read.close();

    // Search for a "DCSubtitle" tag name
    QDomElement xml_root = doc.documentElement();
    if(xml_root.tagName() != "DCSubtitle") {
        // QMessageBox::warning( this, "Loading", "Invalid file." );
        return mSubtitlesList;
    }

    // Retrieve the font Id
    QDomElement xml_load_font = xml_root.firstChildElement("LoadFont");
    QString font_id = xml_load_font.attribute("URI");
    font_id = font_id.remove(font_id.lastIndexOf('.'), 4);
    mFontList.first().setFontId( font_id );

    QDomElement xml_element = xml_load_font.nextSibling().toElement();

    // Parse all the tree
    parseTree(xml_element);

    // Save the last subtitle
    if ( mNewSubtitle.isValid() ) {
        mSubtitlesList.append(mNewSubtitle);
        mNewSubtitle.clear();
    }

    return mSubtitlesList;
}

QString DcSubInteropParser::convertVposToAmapolaRef(QString fontId, QString fontSize, QString vAlign, QString vPos) {

    QFont font(fontId, fontSize.toInt());
    QFontMetrics font_metrics(font);

    quint16 pixel_per_inch = qApp->desktop()->logicalDpiY();
    quint16 ref_height_px = pixel_per_inch * 11;


    if ( vAlign == "top" ) {

        quint16 font_ascent_px = font_metrics.ascent();
        qreal font_ascent_percent = ( (qreal)font_ascent_px / (qreal)ref_height_px ) * 100.0;

        return QString::number(( vPos.toDouble() - font_ascent_percent ), 'f', 1 );
    }
    else if ( vAlign == "center" ) {

        quint16 font_ascent_px = font_metrics.ascent();
        qreal font_ascent_percent = ( ( (qreal)font_ascent_px / 2.0 ) / (qreal)ref_height_px ) * 100.0;

        return QString::number(( vPos.toDouble() - font_ascent_percent ), 'f', 1 );
    }
    else  { // bottom

        quint16 font_descent_px = font_metrics.descent();
        qreal font_descent_percent = ( (qreal)font_descent_px / (qreal)ref_height_px ) * 100.0;

        return QString::number(( vPos.toDouble() - font_descent_percent ), 'f', 1 );
    }
}

QString DcSubInteropParser::convertVposFromAmapolaRef(QString fontId, QString fontSize, QString vAlign, QString vPos) {

    QFont font(fontId, fontSize.toInt());
    QFontMetrics font_metrics(font);

    quint16 pixel_per_inch = qApp->desktop()->logicalDpiY();
    quint16 ref_height_px = pixel_per_inch * 11;


    if ( vAlign == "top" ) {

        quint16 font_ascent_px = font_metrics.ascent();
        qreal font_ascent_percent = ( (qreal)font_ascent_px / (qreal)ref_height_px ) * 100.0;

        return QString::number(( vPos.toDouble() + font_ascent_percent ), 'f', 1 );
    }
    else if ( vAlign == "center" ) {

        quint16 font_ascent_px = font_metrics.ascent();
        qreal font_ascent_percent = ( ( (qreal)font_ascent_px / 2.0 ) / (qreal)ref_height_px ) * 100.0;

        return QString::number(( vPos.toDouble() + font_ascent_percent ), 'f', 1 );
    }
    else {  //bottom

        quint16 font_descent_px = font_metrics.descent();
        qreal font_descent_percent = ( (qreal)font_descent_px / (qreal)ref_height_px ) * 100.0;

        return QString::number(( vPos.toDouble() + font_descent_percent ), 'f', 1 );
    }
}

// Create an xml DCSub document from the subtitle list
bool DcSubInteropParser::save(MyFileWriter & file, QList<MySubtitles> subtitlesList, SubExportDialog *exportDialog) {

    QDomDocument doc("dcsub_interop");

    QDomProcessingInstruction xml_version = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(xml_version);

    // Create the root node "DCSubtitle"
    QDomElement xml_root = doc.createElement("DCSubtitle");
    xml_root.setAttribute("Version", exportDialog->version());
    doc.appendChild(xml_root);

    // Generate an UUID an create the node "SubtitleID"
    QDomElement xml_SubId = doc.createElement("SubtitleID");
    xml_root.appendChild( xml_SubId );
    QDomText subid_text = doc.createTextNode( exportDialog->subId() );
    xml_SubId.appendChild(subid_text);

    // Add "MovieTitle" node
    QDomElement xml_MovTitl = doc.createElement("MovieTitle");
    xml_root.appendChild(xml_MovTitl);
    QDomText movtitl_text = doc.createTextNode( exportDialog->movieTitle() );
    xml_MovTitl.appendChild(movtitl_text);

    // Add "ReelNumber" node
    QDomElement xml_RealNum = doc.createElement("ReelNumber");
    xml_root.appendChild(xml_RealNum);
    QDomText realnum_text = doc.createTextNode( exportDialog->reelNumber() );
    xml_RealNum.appendChild(realnum_text);

    // Add "Language" node
    QDomElement xml_Lang = doc.createElement("Language");
    xml_root.appendChild(xml_Lang);
    QDomText lang_text = doc.createTextNode( exportDialog->language() );
    xml_Lang.appendChild(lang_text);

    // Add "LoadFont" node
    QDomElement xml_LoadFont = doc.createElement("LoadFont");
    QString font_id;
    QString font_uri;

    if ( exportDialog->fontPath() != "" ) {

        font_uri = exportDialog->fontPath();
        font_id = font_uri;
        font_id = font_id.section('/',-1);
        font_id = font_id.section('.',0,0);
    }
    else {
        font_id = subtitlesList.first().text().first().Font().fontId();
        font_uri = font_id +".ttf";
    }

    mFontList.first().setFontId(font_id);
    xml_LoadFont.setAttribute("Id", font_id);
    xml_LoadFont.setAttribute("URI", font_uri);
    xml_root.appendChild(xml_LoadFont);

    mPreferredEffect = exportDialog->preferredEffect();

    // Add first "Font" tag with attributes
    TextFont text_font0;
    text_font0.setFontId(font_id);
    text_font0.setFontColor( qApp->property("prop_FontColor_rgba").toString() );
    text_font0.setFontShadowEffect( qApp->property("prop_FontShadow").toString() );
    text_font0.setFontShadowEffectColor( qApp->property("prop_FontShadowColor").toString() );
    text_font0.setFontBorderEffect( qApp->property("prop_FontBorder").toString() );
    text_font0.setFontBorderEffectColor( qApp->property("prop_FontBorderColor").toString() );
    text_font0.setFontItalic( qApp->property("prop_FontItalic").toString() );
    text_font0.setFontScript( FONT_SCRIPT_DEFAULT_VALUE );
    text_font0.setFontSize( qApp->property("prop_FontSize_pt").toString() );
    text_font0.setFontUnderlined( qApp->property("prop_FontUnderlined").toString() );

    QDomElement xml_font0 = doc.createElement("Font");
    TextFont empty_font;
    this->writeFont(&xml_font0, empty_font, text_font0);
    xml_root.appendChild(xml_font0);

    // For each subtitles
    for ( qint32 i = 0; i < subtitlesList.size(); i++ ) {

        MySubtitles current_subtitle = subtitlesList.at(i);

        // Add "Subtitle" tag with attributes
        QDomElement xml_Subtitle = doc.createElement("Subtitle");
        xml_Subtitle.setAttribute("SpotNumber", QString::number(i+1));
        xml_Subtitle.setAttribute("FadeUpTime", "TO DO");
        xml_Subtitle.setAttribute("FadeDownTime", "TO DO");
        xml_Subtitle.setAttribute("TimeIn", MyAttributesConverter::toTimeHMSticks( current_subtitle.startTime() ));
        xml_Subtitle.setAttribute("TimeOut", MyAttributesConverter::toTimeHMSticks( current_subtitle.endTime() ));
        xml_font0.appendChild(xml_Subtitle);

        QList<TextLine> text_list = current_subtitle.text();

        // Number of lines
        for ( qint32 j = 0; j < text_list.size(); j++ ) {

            // Add "Font" tag for each line if a font attribute is different than its parent font attribute
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

            // Add "Text" tag with position attributes
            QDomElement xml_text = doc.createElement("Text");

            if ( text_line.textDirection() == "") {
                xml_text.setAttribute("Direction", TEXT_DIRECTION_DEFAULT_VALUE);
            }
            else {
                xml_text.setAttribute("Direction", MyAttributesConverter::dirToHorVer( text_line.textDirection() ) );
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

            QString v_align;
            if ( text_line.textVAlign() == "") {
                v_align = TEXT_VALIGN_DEFAULT_VALUE;
            }
            else {
                v_align = text_line.textVAlign();
            }
            xml_text.setAttribute("VAlign", v_align);

            if ( text_line.textVPosition() == "") {
                xml_text.setAttribute("VPosition", this->convertVposFromAmapolaRef(text_font.fontId(), text_font.fontSize(), v_align, TEXT_VPOSITION_DEFAULT_VALUE) );
            }
            else {
                xml_text.setAttribute("VPosition", this->convertVposFromAmapolaRef(text_font.fontId(), text_font.fontSize(), v_align, text_line.textVPosition()) );
            }

            xml_current_element.appendChild(xml_text);
            xml_current_element = xml_text;

            QXmlStreamReader reader(text_line.Line());

            while ( !reader.atEnd() ) {

                switch (reader.readNext()) {
                case QXmlStreamReader::StartElement:

                    if ( reader.name() == "span" ) {

                        QXmlStreamAttributes attributes = reader.attributes();

                        if ( attributes.hasAttribute( QStringLiteral("style") ) ) {

                            QString style_str = attributes.value("style").toString();

                            if ( style_str.contains("color") ) {

                                QString color_str = style_str.mid( (style_str.indexOf("#") + 1), 6 );
                                color_str.prepend("FF");
                                color_str = color_str.toUpper();

                                if ( ( color_str != text_font.fontColor() ) &&
                                     ( color_str != text_font0.fontColor() ) ) {

                                    xml_current_element = xml_current_element.appendChild( doc.createElement("Font") ).toElement();
                                    xml_current_element.setAttribute("Color", color_str);
                                }
                            }

                            if ( style_str.contains("italic") ) {

                                if ( ( text_font.fontItalic() == "no" ) &&
                                     ( text_font0.fontItalic() == "no" ) ) {

                                    if ( xml_current_element.tagName() != "Font") {

                                        xml_current_element = xml_current_element.appendChild( doc.createElement("Font") ).toElement();
                                    }
                                    xml_current_element.setAttribute("Italic", "yes");
                                }
                            }

                            if ( style_str.contains("underline") ) {

                                if ( ( text_font.fontUnderlined() == "no" ) &&
                                     ( text_font0.fontUnderlined() == "no" ) ) {

                                    if ( xml_current_element.tagName() != "Font") {

                                        xml_current_element = xml_current_element.appendChild( doc.createElement("Font") ).toElement();
                                    }
                                    xml_current_element.setAttribute("Underlined", "yes");
                                }
                            }
                        }
                    }
                    break;

                case QXmlStreamReader::EndElement:

                    if ( reader.name() == "span" ) {
                        xml_current_element = xml_current_element.parentNode().toElement();
                    }
                    break;
                case QXmlStreamReader::Characters:

                    xml_current_element.appendChild( doc.createTextNode( reader.text().toString() ) );
                    break;
                default:
                    break;
                }
            }
        }
    }

    // Write the document in file
    QString xml = doc.toString();
    file.writeText( xml );

    return true;
}

// Compare old and new font attributes, return a font container with only the changed attributes setted
void DcSubInteropParser::writeFont(QDomElement* xmlElement, TextFont previousFont, TextFont newFont) {

    if ( previousFont.findDiff(newFont) ) {

        if ( newFont.fontId() != "" ) {
            xmlElement->setAttribute("Id", newFont.fontId());
        }

        if ( newFont.fontColor() != "" ) {
            xmlElement->setAttribute("Color", newFont.fontColor());
        } 


        if ( newFont.fontShadowEffect() != "" ) {

            if ( newFont.fontShadowEffect() == "yes" ) {

                if ( newFont.fontBorderEffect() == "yes" ) {

                    if ( mPreferredEffect == "shadow" ) {
                        xmlElement->setAttribute("Effect", "shadow");
                        mCurrentEffect = "shadow";
                    }
                    else {
                        xmlElement->setAttribute("Effect", "border");
                        mCurrentEffect = "border";
                    }
                }
                else if ( newFont.fontBorderEffect() == "no" ) {

                    xmlElement->setAttribute("Effect", "shadow");
                    mCurrentEffect = "shadow";
                }
                else if ( newFont.fontBorderEffect() == "" ) {

                    if ( previousFont.fontBorderEffect() == "yes" ) {

                        if ( mPreferredEffect == "shadow" ) {
                            xmlElement->setAttribute("Effect", "shadow");
                            mCurrentEffect = "shadow";
                        }
                        else {
                            xmlElement->setAttribute("Effect", "border");
                            mCurrentEffect = "border";
                        }
                    }
                    else if ( previousFont.fontBorderEffect() == "no" ) {
                        xmlElement->setAttribute("Effect", "shadow");
                        mCurrentEffect = "shadow";
                    }
                    else if ( previousFont.fontBorderEffect() == "" ) {
                        xmlElement->setAttribute("Effect", "shadow");
                        mCurrentEffect = "shadow";
                    }
                }
            }
            else if ( newFont.fontShadowEffect() == "no" ) {

                if ( newFont.fontBorderEffect() == "yes" ) {
                    xmlElement->setAttribute("Effect", "border");
                    mCurrentEffect = "border";
                }
                else if ( newFont.fontBorderEffect() == "no" ) {
                    xmlElement->setAttribute("Effect", "none");
                    mCurrentEffect = "none";
                }
                else if ( newFont.fontBorderEffect() == "" ) {

                    if ( previousFont.fontBorderEffect() == "yes" ) {

                        if ( mPreferredEffect == "shadow" ) {
                            xmlElement->setAttribute("Effect", "border");
                            mCurrentEffect = "border";
                        }
                    }
                    else if ( previousFont.fontBorderEffect() == "no" ) {
                        xmlElement->setAttribute("Effect", "none");
                        mCurrentEffect = "none";
                    }
                    else if ( previousFont.fontBorderEffect() == "" ) {
                        xmlElement->setAttribute("Effect", "none");
                        mCurrentEffect = "none";
                    }
                }
            }
        }
        else if ( newFont.fontBorderEffect() != "" ) {

            if ( newFont.fontBorderEffect() == "yes" ) {

                if ( newFont.fontShadowEffect() == "" ) {

                    if ( previousFont.fontShadowEffect() == "yes" ) {

                        if ( mPreferredEffect == "shadow" ) {
                            xmlElement->setAttribute("Effect", "shadow");
                            mCurrentEffect = "shadow";
                        }
                        else {
                            xmlElement->setAttribute("Effect", "border");
                            mCurrentEffect = "border";
                        }
                    }
                    else if ( previousFont.fontShadowEffect() == "no" ) {
                        xmlElement->setAttribute("Effect", "border");
                        mCurrentEffect = "border";
                    }
                    else if ( previousFont.fontShadowEffect() == "" ) {
                        xmlElement->setAttribute("Effect", "border");
                        mCurrentEffect = "border";
                    }
                }
            }
            else if ( newFont.fontShadowEffect() == "no" ) {

                if ( newFont.fontShadowEffect() == "" ) {

                    if ( previousFont.fontShadowEffect() == "yes" ) {

                        if ( mPreferredEffect == "border" ) {
                            xmlElement->setAttribute("Effect", "shadow");
                            mCurrentEffect = "shadow";
                        }
                    }
                    else if ( previousFont.fontShadowEffect() == "no" ) {
                        xmlElement->setAttribute("Effect", "none");
                        mCurrentEffect = "none";
                    }
                    else if ( previousFont.fontShadowEffect() == "" ) {
                        xmlElement->setAttribute("Effect", "none");
                        mCurrentEffect = "none";
                    }
                }
            }
        }

        if ( newFont.fontShadowEffectColor() != "" ) {

            if ( mCurrentEffect == "shadow" ) {

                xmlElement->setAttribute("EffectColor", newFont.fontShadowEffectColor());
            }
        }
        if ( newFont.fontBorderEffectColor() != "" ) {

            if ( mCurrentEffect == "border" ) {

                xmlElement->setAttribute("EffectColor", newFont.fontBorderEffectColor());
            }
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
            QString new_font_size = newFont.fontSize();
            xmlElement->setAttribute("Size", new_font_size);
        }
    }
}

// Append a new font container in the font list.
// When an attribute is null, retrieve the parent font attribute
void DcSubInteropParser::changeFont(QDomElement xmlElement) {

    if ( ( xmlElement.isNull() ) || ( xmlElement.tagName() != "Font") ) {
        return;
    }

    TextFont new_font;

    // attribute Id
//    if ( !xmlElement.attribute("Id").isNull() ) {
//        new_font.setFontId( xmlElement.attribute("Id") );
//    }
//    else {
        new_font.setFontId( mFontList.last().fontId() );
//    }

    // attribute Color
    if ( !xmlElement.attribute("Color").isNull() ) {
        new_font.setFontColor( xmlElement.attribute("Color") );
    }
    else {
        new_font.setFontColor( mFontList.last().fontColor() );
    }

    // attribute Effect
    if ( !xmlElement.attribute("Effect").isNull() ) {

        if ( xmlElement.attribute("Effect") == "none" ) {
            new_font.setFontShadowEffect("no");
            new_font.setFontBorderEffect("no");
        }
        else if ( xmlElement.attribute("Effect") == "shadow" ) {
            new_font.setFontShadowEffect("yes");
            new_font.setFontBorderEffect("no");
        }
        else if ( xmlElement.attribute("Effect") == "border" ) {
            new_font.setFontShadowEffect("no");
            new_font.setFontBorderEffect("yes");
        }
    }
    else {
        new_font.setFontShadowEffect( mFontList.last().fontShadowEffect() );
        new_font.setFontBorderEffect( mFontList.last().fontBorderEffect() );
    }

    // attribute EffectColor
    if ( !xmlElement.attribute("EffectColor").isNull() ) {
        new_font.setFontShadowEffectColor( xmlElement.attribute("EffectColor") );
        new_font.setFontBorderEffectColor( xmlElement.attribute("EffectColor") );
    }
    else {
        new_font.setFontShadowEffectColor( mFontList.last().fontShadowEffectColor() );
        new_font.setFontBorderEffectColor( mFontList.last().fontBorderEffectColor() );
    }

    // attribute Size
    QString font_size;
    if ( !xmlElement.attribute("Size").isNull() ) {
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
