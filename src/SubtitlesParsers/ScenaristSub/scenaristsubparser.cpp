#include "scenaristsubparser.h"
#include <QApplication>
#include <QDateTime>
#include <QProgressDialog>
#include "myattributesconverter.h"
#include "mysubtitles.h"
#include "imagesexporter.h"

ScenaristSubParser::ScenaristSubParser()
{
}


bool ScenaristSubParser::save(MyFileWriter &file, QList<MySubtitles> subtitlesList, SubExportDialog *exportDialog) {

    QString file_path = file.fileName();
    file_path = file_path.remove(file_path.lastIndexOf('.'), file_path.size());
    QString file_name = file.fileName().section('/',-1);
    QString image_path_name_prefix = file_path;
    QString image_extension = ".tif";


    // Init the colors palette
    QList<QString> color_list;
    color_list.append(qApp->property("prop_Color1").toString());
    color_list.append(qApp->property("prop_Color2").toString());
    color_list.append(qApp->property("prop_Color3").toString());
    color_list.append(qApp->property("prop_Color4").toString());
    color_list.append(qApp->property("prop_Color5").toString());
    color_list.append(qApp->property("prop_Color6").toString());
    color_list.append(qApp->property("prop_Color7").toString());
    color_list.append(qApp->property("prop_FontBorderColor").toString());
    color_list.append("FFFFFFFF");

    QString comment;

    // Date
    QDateTime current_date = QDateTime::currentDateTime();
    comment.append("# DVD Subtitle file " +current_date.toString("yyyy-MM-dd hh:mm:ss")
                   +"\n"
                   +"#"
                   +"\n");

    // File name
    comment.append("# File name :           " +file_name
                   +"\n");

    // Language
    comment.append("# Language :            " +exportDialog->language()
                   +"\n");

    // Number of sub
    comment.append("# Number of subtitles : " +QString::number(subtitlesList.count())
                   +"\n"
                   +"#"
                   +"\n");

    // #
    comment.append("#");
    comment.append("\n");

    QString header;

    // Subtitle
    QString sub_name = file_name;
    sub_name.remove(sub_name.lastIndexOf('.'), sub_name.size());
    header.append("SubTitle          " +sub_name
                  +"\n");

    // Base time
    quint16 time_code_rate = qRound(qApp->property("prop_FrameRate_fps").toDouble());
    QString base_time = MyAttributesConverter::timeHMSmsToFrames(exportDialog->startTime(), time_code_rate);
    header.append("Base_Time         " +base_time);
    header.append("\n");

    // Display start NON_FORCED
    header.append("Display_Start     NON_FORCED");
    header.append("\n");

    // TV type
    QString tv_type = exportDialog->imageSize();
    tv_type.remove(tv_type.indexOf(' '), tv_type.size());
    header.append("TV_Type           " +tv_type
                  +"\n");

    // Pixel area, Display Area
    QSize image_size;
    if ( tv_type == "PAL" ) {
        // Tape type
        header.append("Tape_Type         NON_DROP");
        header.append("\n");

        header.append("Pixel_Area        (2 574)");
        header.append("\n");
        header.append("Display_Area      (0 2 719 574)");
        header.append("\n");
        image_size.setWidth(720);
        image_size.setHeight(576);
    }
    else {
        // Tape type
        if ( exportDialog->tapeTypeDrope() == true ) {
            header.append("Tape_Type         DROP");
            header.append("\n");
        }
        else {
            header.append("Tape_Type         NON_DROP");
            header.append("\n");
        }
        header.append("Pixel_Area        (2 479)");
        header.append("\n");
        header.append("Display_Area      (0 2 719 479)");
        header.append("\n");
        image_size.setWidth(720);
        image_size.setHeight(480);
    }

    // Output bit depth
    header.append("Output_Bit_Depth  2");
    header.append("\n");

    // Color
    header.append("Color             (1 2 8 9)");
    header.append("\n");

    // Contrast
    header.append("Contrast          (15 15 15 0)");
    header.append("\n");

    // E2
    header.append("E2                (255 0 0 = = =)");
    header.append("\n");

    // E1
    header.append("E1                (0 0 255 = = =)");
    header.append("\n");

    // PA
    header.append("E1                (0 0 0 = = =)");
    header.append("\n");

    // BG
    header.append("E1                (255 255 255 = = =)");
    header.append("\n");

    // Directory
    header.append("directory   " +file_path
                  +"\n");

    QString sub_data;
    // #
    sub_data.append("#######################################################");
    sub_data.append("\n");
    sub_data.append("SP_NUMBER      START        END       FILE_NAME");
    sub_data.append("\n");

    quint16 e2 = 0;
    quint16 e1 = 1;
    quint16 pa = 7;
    quint16 bg = 8;
    bool e2_changed = false;
    bool e1_changed = false;
    bool pa_changed = false;

    QSize video_native_size = qApp->property("prop_resolution_px").toSize();
    ImagesExporter image_exporter(video_native_size);

    QProgressDialog progress_dialog("Exporting images...", "Cancel...", 0, subtitlesList.size(), exportDialog);
    progress_dialog.setWindowModality(Qt::WindowModal);

    for ( qint16 i = 0; i < subtitlesList.size(); i++ ) {

        MySubtitles current_sub = subtitlesList.at(i);

        QList<TextLine> lines = current_sub.text();

        for ( qint16 j = 0; j < lines.size(); j++ ) {

            TextLine current_line = lines.at(j);
            TextFont current_font = current_line.Font();

            // Line color
            QString font_color = current_font.fontColor();

            // First line
            if ( j == 0 ) {

                if ( font_color != color_list.at(e2) ) {

                    qint16 color_index = color_list.indexOf(font_color);

                    if ( color_index < 0 ) {
                        color_list.append(font_color);
                        color_index = color_list.size() - 1;
                    }

                    e2 = color_index;
                    e2_changed = true;
                }

                current_sub.text()[j].Font().setFontColor("FFFF0000");
            }
            else {

                // Same color as first line
                if ( font_color == color_list.at(e2) ) {

                    current_sub.text()[j].Font().setFontColor("FFFF0000");
                }
                // Other color than first line
                else {

                    // If color e1 not changed yet
                    if ( e1_changed == false ) {

                        // If line color not the same as e1 color
                        if ( font_color != color_list.at(e1) ) {

                            qint16 color_index = color_list.indexOf(font_color);

                            if ( color_index < 0 ) {
                                color_list.append(font_color);
                                color_index = color_list.size() - 1;
                            }

                            e1 = color_index;
                            e1_changed = true;
                        }
                    }

                    current_sub.text()[j].Font().setFontColor("FF0000FF");
                }
            }

            // Char color
            QString text = current_line.Line();

            if ( text.contains("color:#") ) {

                for ( qint16 char_it = 0; char_it < text.size(); ) {

                    char_it = text.indexOf("color:#", char_it);

                    if ( char_it < 0 ) {
                        break;
                    }
                    else {

                        QString font_color2 = text.mid((text.indexOf("#", char_it) + 1), 6);
                        font_color2 = font_color2.prepend("FF").toUpper();

                        // If the char color is not the same as e2 color
                        if ( font_color2 != color_list.at(e2) ) {

                            // If e1 color not changed yet
                            if ( e1_changed == false ) {

                                if ( font_color2 != color_list.at(e1) ) {

                                    qint16 color_index = color_list.indexOf(font_color2);

                                    if ( color_index < 0 ) {
                                        color_list.append(font_color2);
                                        color_index = color_list.size() - 1;
                                    }

                                    e1 = color_index;
                                    e1_changed = true;
                                }
                            }

                            text.replace(char_it, 13, "color:#0000FF");
                        }
                        else {
                            text.replace(char_it, 13, "color:#FF0000");
                        }
                    }
                    char_it++;
                }

                current_sub.text()[j].setLine(text);
            }

            // Border color
            QString border_color = current_font.fontBorderEffectColor();

            if ( j == 0 ) {

                if ( border_color != color_list.at(pa) ) {

                    qint16 border_color_index = color_list.indexOf(font_color);

                    if ( border_color_index < 0 ) {
                        color_list.append(border_color);
                        border_color_index = color_list.size() - 1;
                    }

                    pa = border_color_index;
                    pa_changed = true;
                }
            }

            current_sub.text()[j].Font().setFontBorderEffectColor("FF000000");
            current_sub.text()[j].Font().setFontBackgroundEffect("no");
            current_sub.text()[j].Font().setFontShadowEffect("no");
        }

        progress_dialog.setValue(i);

        if (progress_dialog.wasCanceled()) {
            return false;
        }

        QString number_str;
        QString image_file_name = image_path_name_prefix +number_str.sprintf("%04d", (i+1)) +image_extension;
        image_exporter.createImage(current_sub, image_file_name, image_size, true, "TIF", Qt::white, 0);

        if ( ( e2_changed ) || ( e1_changed ) || ( pa_changed ) ) {

            sub_data.append("Color ("
                           +QString::number(e2 + 1) +" "
                           +QString::number(e1 + 1) +" "
                           +QString::number(pa + 1) +" "
                           +QString::number(bg + 1) +" "
                           +")"
                           +"\n");
            e2_changed = false;
            e1_changed = false;
            pa_changed = false;
        }

        sub_data.append(QString::number(i + 1) +"          ");

        time_code_rate = qRound(qApp->property("prop_FrameRate_fps").toDouble());
        QString start_time = MyAttributesConverter::timeHMSmsToFrames(current_sub.startTime(), time_code_rate);
        sub_data.append(start_time +"  ");

        QString end_time = MyAttributesConverter::timeHMSmsToFrames(current_sub.endTime(), time_code_rate);
        sub_data.append(end_time +"  ");

        sub_data.append(image_file_name.section('/', -1)
                        +"\n");
    }

    // Write the palette in the comment
    comment.append("# Palette (AARRGGBB) :");
    comment.append("\n");

    for ( qint16 color_it = 0; color_it < color_list.size(); color_it++ ) {

        comment.append("# " +QString::number(color_it + 1) +" - " +color_list.at(color_it)
                       +"\n");

    }

    // Write infos to the "sst" file
    file.writeText("st_format 2");
    file.writeText("\n");
    file.writeText(comment
                   +"\n"
                   +header
                   +"\n"
                   +sub_data);

    progress_dialog.setValue(subtitlesList.size());

    return true;
}

QList<MySubtitles> ScenaristSubParser::open(MyFileReader file) {
    QList<MySubtitles> sub_list;
    return sub_list;
}

bool ScenaristSubParser::readSample(MyFileReader file) {
    return false;
}
