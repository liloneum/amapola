#include "dcsubsmpteparser.h"
#include <QDomDocument>
#include <QMessageBox>
#include <QUuid>
#include "myattributesconverter.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QTextEdit>
#include <QFile>
#include <QApplication>
#include <QDateTime>
#include <QFontDatabase>

// See "Subtitle Specification (XML File Format) for DLP CinemaTM Projection Technology" documentation

// Define default values
#define FONT_ID_DEFAULT_VALUE ""
#define FONT_COLOR_DEFAULT_VALUE "FFFFFFFF"
#define FONT_SHADOW_EFFECT_DEFAULT_VALUE "no"
#define FONT_BORDER_EFFECT_DEFAULT_VALUE "no"
#define FONT_EFFECT_COLOR_DEFAULT_VALUE "FF000000"
#define FONT_ITALIC_DEFAULT_VALUE "no"
#define FONT_SCRIPT_DEFAULT_VALUE "normal"
#define FONT_SIZE_DEFAULT_VALUE "42"
#define FONT_UNDERLINED_DEFAULT_VALUE "no"

#define TEXT_DIRECTION_DEFAULT_VALUE "ltr"
#define TEXT_HALIGN_DEFAULT_VALUE "center"
#define TEXT_HPOSITION_DEFAULT_VALUE "0"
#define TEXT_VALIGN_DEFAULT_VALUE "center"
#define TEXT_VPOSITION_DEFAULT_VALUE "0"


DcSubSmpteParser::DcSubSmpteParser() {

    mNewSubtitle.clear();
    mSubtitlesList.clear();
    mTimeCodeRate = 24;

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
    mFontIdList.clear();

    mPreferredEffect = "";
    mCurrentEffect = "";
}

// Read a sample of the file (10 lines). Try to found "SubtitleReel" tag
bool DcSubSmpteParser::readSample(MyFileReader file) {

    QXmlStreamReader xml_reader;
    QStringList lines_list = file.lines();

    for ( qint32 i = 0; i < lines_list.count(); i++ ) {

        xml_reader.addData(lines_list[i]);
        xml_reader.readNext();

        if ( xml_reader.isStartElement() ) {

            if ( ( xml_reader.name() == "dcst:SubtitleReel" ) || ( xml_reader.name() == "SubtitleReel" ) ) {
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
void DcSubSmpteParser::parseTree(QDomElement xmlElement) {

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

            start_time = MyAttributesConverter::framesToTimeHMSms(start_time, mTimeCodeRate);

            if ( !start_time.isEmpty() ) {

                mNewSubtitle.setStartTime(start_time);
            }


            //TimeOut
            QString end_time = xmlElement.attribute("TimeOut");

            end_time = MyAttributesConverter::framesToTimeHMSms(end_time, mTimeCodeRate);

            if ( !end_time.isEmpty() ) {

                mNewSubtitle.setEndTime(end_time);
            }
        }
        // If the tag name is "Text"
        else if (xmlElement.tagName() == "Text") {

            if ( !xmlElement.text().isEmpty() ) {

                TextLine new_text;

                // attribute Direction
                if ( !xmlElement.attribute("Direction").isNull() ) {
                new_text.setTextDirection( xmlElement.attribute("Direction") );
                }
                else {
                    new_text.setTextDirection( TEXT_DIRECTION_DEFAULT_VALUE );
                }

                // attribute Halign
                if ( !xmlElement.attribute("Halign").isNull() ) {
                new_text.setTextHAlign( xmlElement.attribute("Halign") );
                }
                else {
                    new_text.setTextHAlign( TEXT_HALIGN_DEFAULT_VALUE );
                }

                // attribute Hposition
                if ( !xmlElement.attribute("Hposition").isNull() ) {
                new_text.setTextHPosition( xmlElement.attribute("Hposition") );
                }
                else {
                    new_text.setTextHPosition( TEXT_HPOSITION_DEFAULT_VALUE );
                }

                // attribute Valign
                if ( !xmlElement.attribute("Valign").isNull() ) {
                new_text.setTextVAlign( xmlElement.attribute("Valign") );
                }
                else {
                    new_text.setTextVAlign( TEXT_VALIGN_DEFAULT_VALUE );
                }

                // attribute Vposition
                if ( !xmlElement.attribute("Vposition").isNull() ) {
                new_text.setTextVPosition( xmlElement.attribute("Vposition") );
                }
                else {
                     new_text.setTextVPosition( TEXT_VPOSITION_DEFAULT_VALUE );
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
QList<MySubtitles> DcSubSmpteParser::open(MyFileReader file) {

    QFile file_read( file.getFileName() );

    // Try to open the file
    if ( !file_read.open(QIODevice::ReadOnly) ) {
        // QMessageBox::warning(this, "Loading DCSub smpte", "Failed to load file.");
        return mSubtitlesList;
    }

    // Copy content of the file in QDomDocument
    QDomDocument doc("dcsub");
    if( !doc.setContent( this->dcstFilter(&file_read) ) ) {
        // QMessageBox::warning(this, "Loading DCSub smpte", "Failed to load file." );
        file_read.close();
        return mSubtitlesList;
    }

    file_read.close();

    // Search for a "SubtitleReel" tag name
    QDomElement xml_root = doc.documentElement();
    if(xml_root.tagName() != "SubtitleReel") {
        // QMessageBox::warning( this, "Loading DCSub smpte", "Invalid file." );
        return mSubtitlesList;
    }

    // Retrieve the TimeCodeRate
    QDomElement xml_time_code_rate = xml_root.firstChildElement("TimeCodeRate");
    mTimeCodeRate = xml_time_code_rate.text().toInt();

    // Retrieve the font ID
    QDomElement xml_load_font = xml_root.firstChildElement("LoadFont");
    mFontList.first().setFontId( xml_load_font.attribute("ID") );

    // Search for a "SubtitleList" tag name
    QDomElement xml_sub_list = xml_root.firstChildElement("SubtitleList");
    if(xml_sub_list.tagName() != "SubtitleList") {
        // QMessageBox::warning( this, "Loading DCSub smpte", "Subtitle list is empty" );
        return mSubtitlesList;
    }

    QDomElement xml_element = xml_sub_list.firstChildElement();

    // Parse all the tree
    parseTree(xml_element);

    // Save the last subtitle
    if ( mNewSubtitle.isValid() ) {
        mSubtitlesList.append(mNewSubtitle);
        mNewSubtitle.clear();
    }

    return mSubtitlesList;
}

// Create an xml DCSub document from the subtitle list
bool DcSubSmpteParser::save(MyFileWriter & file, QList<MySubtitles> subtitlesList, SubExportDialog *exportDialog) {

    QDomDocument doc("dcsub_smpte");

    QDomProcessingInstruction xml_version = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(xml_version);

    // Create the root node "SubtitleReel"
    QDomElement xml_root = doc.createElement("SubtitleReel");
//    xml_root.setAttribute("Version", exportDialog->version());
//    doc.appendChild(xml_root);
    xml_root.setAttribute("dcst", "http://www.smpte-ra.org/schemas/428-7/2007/DCST");
    doc.appendChild(xml_root);
    xml_root.setAttribute("xs", "http://www.w3.org/2001/XMLSchema");
    doc.appendChild(xml_root);

    // Generate an UUID an create the node "SubtitleID"
    QDomElement xml_SubId = doc.createElement("Id");
    xml_root.appendChild( xml_SubId );
    QDomText subid_text = doc.createTextNode( "urn:uuid:" +exportDialog->subId() );
    xml_SubId.appendChild(subid_text);

    // Add "ContentTitleText" node
    QDomElement xml_MovTitl = doc.createElement("ContentTitleText");
    xml_root.appendChild(xml_MovTitl);
    QDomText movtitl_text = doc.createTextNode( exportDialog->movieTitle() );
    xml_MovTitl.appendChild(movtitl_text);

    // Add "AnnotationText" node
    QDomElement xml_Annot = doc.createElement("AnnotationText");
    xml_root.appendChild(xml_Annot);
    QDomText annot_text = doc.createTextNode( exportDialog->comment() );
    xml_Annot.appendChild(annot_text);

    // Add "IssueDate" node
    QDomElement xml_Date = doc.createElement("IssueDate");
    xml_root.appendChild(xml_Date);
    QDateTime current_date = QDateTime::currentDateTime();
    QDomText date_text = doc.createTextNode(current_date.toString("yyyy-MM-ddThh:mm:ss"));
    xml_Date.appendChild(date_text);

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

    // Add "EditRate" node
    QDomElement xml_EditRate = doc.createElement("EditRate");
    xml_root.appendChild(xml_EditRate);
    QDomText editrate_text = doc.createTextNode( exportDialog->editRate() +" 1");
    xml_EditRate.appendChild(editrate_text);

    // Add "TimeCodeRate" node
    if ( exportDialog->timeCodeRate() != "" ) {

        mTimeCodeRate = exportDialog->timeCodeRate().toInt();
    }

    QDomElement xml_timecoderate = doc.createElement("TimeCodeRate");
    xml_root.appendChild(xml_timecoderate);
    QDomText timecoderate_text = doc.createTextNode( QString::number(mTimeCodeRate) );
    xml_timecoderate.appendChild(timecoderate_text);

    // Add "StartTime" node
    QDomElement xml_starttime = doc.createElement("StartTime");
    xml_root.appendChild(xml_starttime);
    QDomText starttime_text = doc.createTextNode( MyAttributesConverter::timeHMSmsToFrames(exportDialog->startTime(), mTimeCodeRate) );
    xml_starttime.appendChild(starttime_text);

    // Add "LoadFont" node
    QDomElement xml_LoadFont = doc.createElement("LoadFont");
    QString font_id;
    QString font_uri;

    if ( exportDialog->fontPath() != "" ) {

        font_uri = exportDialog->fontPath();
        int id = QFontDatabase::addApplicationFont(font_uri);
        font_id = QFontDatabase::applicationFontFamilies(id).at(0);
    }
    else {
        font_id = subtitlesList.first().text().first().Font().fontId();
        font_uri = font_id +".ttf";
        font_uri.replace(" ","");
    }

    mFontIdList.append(font_id);
    mFontList.first().setFontId(font_id);
    xml_LoadFont.setAttribute("ID", font_id);
    QDomText loadfont_text = doc.createTextNode(font_uri);
    xml_LoadFont.appendChild(loadfont_text);
    xml_root.appendChild(xml_LoadFont);

    // Add "SubtitleList" node
    QDomElement xml_sublist = doc.createElement("SubtitleList");
    xml_root.appendChild(xml_sublist);


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
    xml_sublist.appendChild(xml_font0);

    // For each subtitles
    for ( qint32 i = 0; i < subtitlesList.size(); i++ ) {

        MySubtitles current_subtitle = subtitlesList.at(i);

        // Add "Subtitle" tag with attributes
        QDomElement xml_Subtitle = doc.createElement("Subtitle");
        xml_Subtitle.setAttribute("SpotNumber", QString::number(i+1));

        if ( exportDialog->fadeInTime() != "00:00:00.000" ) {
            xml_Subtitle.setAttribute("FadeUpTime", MyAttributesConverter::timeHMSmsToFrames(exportDialog->fadeInTime(), mTimeCodeRate));
        }

        if ( exportDialog->fadeOutTime() != "00:00:00.000" ) {
            xml_Subtitle.setAttribute("FadeDownTime", MyAttributesConverter::timeHMSmsToFrames(exportDialog->fadeOutTime(), mTimeCodeRate));
        }

        xml_Subtitle.setAttribute("TimeIn", MyAttributesConverter::timeHMSmsToFrames(current_subtitle.startTime(), mTimeCodeRate ));
        xml_Subtitle.setAttribute("TimeOut", MyAttributesConverter::timeHMSmsToFrames(current_subtitle.endTime(), mTimeCodeRate ));
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
                xml_text.setAttribute("Halign", TEXT_HALIGN_DEFAULT_VALUE);
            }
            else {
                xml_text.setAttribute("Halign", text_line.textHAlign());
            }

            if ( text_line.textHPosition() == "") {
                xml_text.setAttribute("Hposition", TEXT_HPOSITION_DEFAULT_VALUE);
            }
            else {
                xml_text.setAttribute("Hposition", text_line.textHPosition());
            }

            if ( text_line.textVAlign() == "") {
                xml_text.setAttribute("Valign", TEXT_VALIGN_DEFAULT_VALUE);
            }
            else {
                xml_text.setAttribute("Valign", text_line.textVAlign());
            }

            if ( text_line.textVPosition() == "") {
                xml_text.setAttribute("Vposition", TEXT_VPOSITION_DEFAULT_VALUE);
            }
            else {
                xml_text.setAttribute("Vposition", text_line.textVPosition());
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
void DcSubSmpteParser::writeFont(QDomElement* xmlElement, TextFont previousFont, TextFont newFont) {

    if ( previousFont.findDiff(newFont) ) {

        if ( newFont.fontId() != "" ) {
            if ( mFontIdList.indexOf(newFont.fontId()) >= 0 ) {
                xmlElement->setAttribute("ID", newFont.fontId());
            }
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
void DcSubSmpteParser::changeFont(QDomElement xmlElement) {

    if ( ( xmlElement.isNull() ) || ( xmlElement.tagName() != "Font") ) {
        return;
    }

    TextFont new_font;

    // attribute ID
    if ( !xmlElement.attribute("ID").isNull() ) {
        new_font.setFontId( xmlElement.attribute("ID") );
    }
    else {
        new_font.setFontId( mFontList.last().fontId() );
    }

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

QString DcSubSmpteParser::dcstFilter(QFile *file) {

    QString out;
    QXmlStreamReader reader(file);
    QXmlStreamWriter writer(&out);
    QString element_name;

    while ( !reader.atEnd() ) {

        switch ( reader.readNext() ) {
        case QXmlStreamReader::StartElement:

            element_name = reader.name().toString();

            if ( element_name.contains("dcst:") ) {

                element_name.remove("dcst:");
            }

            writer.writeStartElement(element_name);
            writer.writeAttributes( reader.attributes() );
            break;

        case QXmlStreamReader::Characters:

            writer.writeCharacters(reader.text().toString());
            break;

        case QXmlStreamReader::EndElement:

            writer.writeEndElement();
            break;

        default:
            break;
        }
    }

    return out;
}
