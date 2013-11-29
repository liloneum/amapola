#include "amapolaprojfileparser.h"
#include "myattributesconverter.h"
#include "mysubtitles.h"
#include <QApplication>

AmapolaProjFileParser::AmapolaProjFileParser() {

}

QList<MySubtitles> AmapolaProjFileParser::open(MyFileReader file) {

    qint32 sub_count = 0;
    qint16 text_lines_count = 0;
    bool last_sub = false;

    QStringList lines = file.lines();

    QList<MySubtitles> subtitles_list;

    for ( qint32 i = 0; i < lines.size(); i++ ) {

        QString current_line = lines.at(i);

        // There are subtitle to load
        if ( last_sub == false ) {

            if ( current_line.contains("AMAPOLASUB") ) {

                MySubtitles new_subtitle;
                subtitles_list.append(new_subtitle);
                sub_count++;
                text_lines_count = 0;
            }
            else if ( current_line.contains("StartTime") ) {

                QString start_time = current_line.section("->",1,1);
                subtitles_list[sub_count - 1].setStartTime(start_time);
            }
            else if ( current_line.contains("EndTime") ) {

                QString end_time = current_line.section("->",1,1);
                subtitles_list[sub_count - 1].setEndTime(end_time);
            }
            else if ( current_line.contains("DurationAuto") ) {

                bool duration_auto = current_line.section("->",1,1).contains("yes");
                subtitles_list[sub_count - 1].setDurationAuto(duration_auto);
            }
            else if ( current_line.contains("TextLine") ) {

                QString line = current_line.section("->",1,1);

                TextLine text_line;
                text_line.setLine(line);

                TextFont text_font;

                subtitles_list[sub_count - 1].setText(text_line, text_font);

                text_lines_count++;
            }
            else if ( current_line.contains("Halign") ) {

                QString h_align = current_line.section("->",1,1);
                subtitles_list[sub_count - 1].text()[text_lines_count - 1].setTextHAlign(h_align);
            }
            else if ( current_line.contains("Valign") ) {

                QString v_align = current_line.section("->",1,1);
                subtitles_list[sub_count - 1].text()[text_lines_count - 1].setTextVAlign(v_align);
            }
            else if ( current_line.contains("HPos") ) {

                QString h_pos = current_line.section("->",1,1);
                subtitles_list[sub_count - 1].text()[text_lines_count - 1].setTextHPosition(h_pos);
            }
            else if ( current_line.contains("VPos") ) {

                QString v_pos = current_line.section("->",1,1);
                subtitles_list[sub_count - 1].text()[text_lines_count - 1].setTextVPosition(v_pos);
            }
            else if ( current_line.contains("TextDirection") ) {

                QString direction = current_line.section("->",1,1);
                subtitles_list[sub_count - 1].text()[text_lines_count - 1].setTextDirection(direction);
            }
            else if ( current_line.contains("FontName") ) {

                QString font_name = current_line.section("->",1,1);
                subtitles_list[sub_count - 1].text()[text_lines_count - 1].Font().setFontId(font_name);
            }
            else if ( current_line.contains("FontSize") ) {

                QString font_size = current_line.section("->",1,1);
                subtitles_list[sub_count - 1].text()[text_lines_count - 1].Font().setFontSize(font_size);
            }
            else if ( current_line.contains("FontBorderEffect") ) {

                QString font_border_effect = current_line.section("->",1,1);
                subtitles_list[sub_count - 1].text()[text_lines_count - 1].Font().setFontBorderEffect(font_border_effect);
            }
            else if ( current_line.contains("FontBorderColor") ) {

                QString font_border_effect_color = current_line.section("->",1,1);
                subtitles_list[sub_count - 1].text()[text_lines_count - 1].Font().setFontBorderEffectColor(font_border_effect_color);
            }
            else if ( current_line.contains("FontShadowEffect") ) {

                QString font_shadow_effect = current_line.section("->",1,1);
                subtitles_list[sub_count - 1].text()[text_lines_count - 1].Font().setFontShadowEffect(font_shadow_effect);
            }
            else if ( current_line.contains("FontShadowColor") ) {

                QString font_shadow_effect_color = current_line.section("->",1,1);
                subtitles_list[sub_count - 1].text()[text_lines_count - 1].Font().setFontShadowEffectColor(font_shadow_effect_color);
            }
            else if ( current_line.contains("FontColor") ) {

                QString font_color = current_line.section("->",1,1);
                subtitles_list[sub_count - 1].text()[text_lines_count - 1].Font().setFontColor(font_color);
            }
            else if ( current_line.contains("FontItalic") ) {

                QString font_italic = current_line.section("->",1,1);
                subtitles_list[sub_count - 1].text()[text_lines_count - 1].Font().setFontItalic(font_italic);
            }
            else if ( current_line.contains("FontUnderlined") ) {

                QString font_underlined = current_line.section("->",1,1);
                subtitles_list[sub_count - 1].text()[text_lines_count - 1].Font().setFontUnderlined(font_underlined);
            }
            else if ( current_line.contains("FontScript") ) {

                QString font_script = current_line.section("->",1,1);
                subtitles_list[sub_count - 1].text()[text_lines_count - 1].Font().setFontScript(font_script);
            }
            else if ( current_line.contains("VideoFileName") ) {

                QString video_file_name = current_line.section("->",1,1);
                qApp->setProperty("currentVideoFileName", video_file_name);
                last_sub = true;
            }
        } // All subtitles list is loaded
        else {

            if ( current_line.contains("SubMinInterval") ) {

                QString sub_min_interval = current_line.section("->",1,1);
                qApp->setProperty("prop_SubMinInterval_frame", sub_min_interval.toInt());
            }
            else if ( current_line.contains("SubMinDuration") ) {

                QString sub_min_duration = current_line.section("->",1,1);
                qApp->setProperty("prop_SubMinDuration_ms", sub_min_duration.toInt());
            }
            else if ( current_line.contains("FrameRate") ) {

                QString frame_rate = current_line.section("->",1,1);
                qApp->setProperty("prop_FrameRate_fps", frame_rate.toDouble());
            }
            else if ( current_line.contains("MaxCharPerLine") ) {

                QString max_char_per_line = current_line.section("->",1,1);
                qApp->setProperty("prop_MaxCharPerLine", max_char_per_line.toInt());
            }
            else if ( current_line.contains("MaxCharPerSec") ) {

                QString max_char_per_sec = current_line.section("->",1,1);
                qApp->setProperty("prop_MaxCharPerSec", max_char_per_sec.toDouble());
            }
            else if ( current_line.contains("DefaultFontSize") ) {

                QString default_font_size = current_line.section("->",1,1);
                qApp->setProperty("prop_FontSize_pt", default_font_size.toInt());
            }
            else if ( current_line.contains("DefaultFontName") ) {

                QString default_font_name = current_line.section("->",1,1);
                qApp->setProperty("prop_FontName", default_font_name);
            }
            else if ( current_line.contains("DefaultFontItalic") ) {

                QString default_font_itlaic = current_line.section("->",1,1);
                qApp->setProperty("prop_FontItalic", default_font_itlaic);
            }
            else if ( current_line.contains("DefaultFontUnderlined") ) {

                QString default_font_underlined = current_line.section("->",1,1);
                qApp->setProperty("prop_FontUnderlined", default_font_underlined);
            }
            else if ( current_line.contains("DefaultFontBorder") ) {

                QString default_font_border = current_line.section("->",1,1);
                qApp->setProperty("prop_FontBorder", default_font_border);
            }
            else if ( current_line.contains("DefaultFontShadow") ) {

                QString default_font_shadow = current_line.section("->",1,1);
                qApp->setProperty("prop_FontShadow", default_font_shadow);
            }
            else if ( current_line.contains("DefaultBorderColor") ) {

                QString default_font_border_color = current_line.section("->",1,1);
                qApp->setProperty("prop_FontBorderColor", default_font_border_color);
            }
            else if ( current_line.contains("DefaultShadowColor") ) {

                QString default_font_shadow_color = current_line.section("->",1,1);
                qApp->setProperty("prop_FontShadowColor", default_font_shadow_color);
            }
            else if ( current_line.contains("DefaultFontColor") ) {

                QString default_font_color = current_line.section("->",1,1);
                qApp->setProperty("prop_FontColor_rgba", default_font_color);
            }
            else if ( current_line.contains("DefaultValign") ) {

                QString default_v_align = current_line.section("->",1,1);
                qApp->setProperty("prop_Valign", default_v_align);
            }
            else if ( current_line.contains("DefaultHalign") ) {

                QString default_h_align = current_line.section("->",1,1);
                qApp->setProperty("prop_Halign", default_h_align);
            }
            else if ( current_line.contains("DefaultVposition") ) {

                QString default_v_pos = current_line.section("->",1,1);
                qApp->setProperty("prop_Vposition_percent", QString::number(default_v_pos.toDouble(), 'f', 1) );
            }
            else if ( current_line.contains("DefaultHposition") ) {

                QString default_h_pos = current_line.section("->",1,1);
                qApp->setProperty("prop_Hposition_percent", QString::number(default_h_pos.toDouble(), 'f', 1) );
            }
        }
    }

    return subtitles_list;
}

// Save the subtitles, the video opened, all default parameters and current settings
void AmapolaProjFileParser::save(MyFileWriter &file, QList<MySubtitles> subtitlesList, QString videoFilePath) {

    qint32 sub_count = 0;

    // Save the subtitles
    QList<MySubtitles>::iterator sub_it;
    for ( sub_it = subtitlesList.begin(); sub_it != subtitlesList.end(); ++sub_it ) {

        sub_count++;

        MySubtitles current_sub = *sub_it;

        QString sub_to_write = "";

        sub_to_write += "AMAPOLASUB" +QString::number(sub_count) +"\n";
        sub_to_write += "StartTime->" +current_sub.startTime() +"\n";
        sub_to_write += "EndTime->" +current_sub.endTime() +"\n";
        sub_to_write += "DurationAuto->" +MyAttributesConverter::boolToString(current_sub.isDurationAuto()) +"\n";

        QList<TextLine> text_lines;
        text_lines = current_sub.text();

        QList<TextLine>::iterator line_it;
        for ( line_it = text_lines.begin(); line_it != text_lines.end(); ++line_it ) {

            TextLine current_text_line = *line_it;

            sub_to_write += "TextLine->" +current_text_line.Line() +"\n";
            sub_to_write += "Halign->" +current_text_line.textHAlign() +"\n";
            sub_to_write += "Valign->" +current_text_line.textVAlign() +"\n";
            sub_to_write += "HPos->" +current_text_line.textHPosition() +"\n";
            sub_to_write += "VPos->" +current_text_line.textVPosition() +"\n";
            sub_to_write += "TextDirection->" +current_text_line.textDirection() +"\n";

            TextFont current_line_font;
            current_line_font = current_text_line.Font();

            sub_to_write += "FontName->" +current_line_font.fontId() +"\n";
            sub_to_write += "FontSize->" +current_line_font.fontSize() +"\n";
            sub_to_write += "FontBorderEffect->" +current_line_font.fontBorderEffect() +"\n";
            sub_to_write += "FontBorderColor->" +current_line_font.fontBorderEffectColor() +"\n";
            sub_to_write += "FontShadowEffect->" +current_line_font.fontShadowEffect() +"\n";
            sub_to_write += "FontShadowColor->" +current_line_font.fontShadowEffectColor() +"\n";
            sub_to_write += "FontColor->" +current_line_font.fontColor() +"\n";
            sub_to_write += "FontItalic->" +current_line_font.fontItalic() +"\n";
            sub_to_write += "FontUnderlined->" +current_line_font.fontUnderlined() +"\n";
            sub_to_write += "FontScript->" +current_line_font.fontScript() +"\n";
        }

        sub_to_write += "\n\n";
        file.writeText(sub_to_write);
    }

    // Save the video file path name
    QString video_file_name = "";
    video_file_name = "\n\nVideoFileName->" +videoFilePath +"\n";
    file.writeText(video_file_name);

    // Save the settings
    QString properties = "";
    properties += "\n\n";

    properties += "SubMinInterval->" +qApp->property("prop_SubMinInterval_frame").toString() +"\n";
    properties += "SubMinDuration->" +qApp->property("prop_SubMinDuration_ms").toString() +"\n";
    properties += "FrameRate->" +qApp->property("prop_FrameRate_fps").toString() +"\n";
    properties += "MaxCharPerLine->" +qApp->property("prop_MaxCharPerLine").toString() +"\n";
    properties += "MaxCharPerSec->" +qApp->property("prop_MaxCharPerSec").toString() +"\n";


    // Save the default parameters
    properties += "DefaultFontSize->" +qApp->property("prop_FontSize_pt").toString() +"\n";
    properties += "DefaultFontName->" +qApp->property("prop_FontName").toString() +"\n";
    properties += "DefaultFontItalic->" +qApp->property("prop_FontItalic").toString() +"\n";
    properties += "DefaultFontUnderlined->" +qApp->property("prop_FontUnderlined").toString() +"\n";
    properties += "DefaultFontBorder->" +qApp->property("prop_FontBorder").toString() +"\n";
    properties += "DefaultFontShadow->" +qApp->property("prop_FontShadow").toString() +"\n";
    properties += "DefaultBorderColor->" +qApp->property("prop_FontBorderColor").toString() +"\n";
    properties += "DefaultShadowColor->" +qApp->property("prop_FontShadowColor").toString() +"\n";
    properties += "DefaultFontColor->" +qApp->property("prop_FontColor_rgba").toString() +"\n";
    properties += "DefaultValign->" +qApp->property("prop_Valign").toString() +"\n";
    properties += "DefaultHalign->" +qApp->property("prop_Halign").toString() +"\n";
    properties += "DefaultVposition->" +qApp->property("prop_Vposition_percent").toString() +"\n";
    properties += "DefaultHposition->" +qApp->property("prop_Hposition_percent").toString() +"\n";

    file.writeText(properties);
}
