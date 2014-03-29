#include "bdnparser.h"
#include <QApplication>
#include <QProgressDialog>
#include <QXmlStreamWriter>
#include "attributesconverter.h"
#include "imagesexporter.h"

BDNparser::BDNparser()
{
}

bool BDNparser::save(MyFileWriter &file, QList<MySubtitles> subtitlesList, SubExportDialog *exportDialog) {

    QByteArray out;
    QXmlStreamWriter writer(&out);
    writer.setAutoFormatting(true);
    writer.setCodec("UTF-8");

    writer.writeStartDocument();

    writer.writeComment("Created with Amapola v0.1");

    writer.writeStartElement("BDN");
        writer.writeAttribute("Version", "0.93");
        writer.writeAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
        writer.writeAttribute("xsi:noNamespaceSchemaLocation", "BD-03-006-0093b BDN File Format.xsd");

    writer.writeStartElement("Description");
        writer.writeStartElement("Name");
            writer.writeAttribute("Title", exportDialog->movieTitle());
        writer.writeEndElement();

        writer.writeStartElement("Language");
            writer.writeAttribute("Code", exportDialog->language());
        writer.writeEndElement();

        QString image_size_str = exportDialog->imageSize();
        QSize image_size;
        if ( ( image_size_str == "480i" ) || ( image_size_str == "480p" ) ) {
            image_size.setWidth(720);
            image_size.setHeight(480);
        }
        else if ( image_size_str == "576i" ) {
            image_size.setWidth(720);
            image_size.setHeight(576);
        }
        else if ( image_size_str == "720p" ) {
            image_size.setWidth(1280);
            image_size.setHeight(720);
        }
        else {
            image_size.setWidth(1920);
            image_size.setHeight(1080);
        }

        QString drop_frame_str;

        if ( exportDialog->tapeTypeDrope() == true ) {
            drop_frame_str = "true";
        }
        else {
            drop_frame_str = "false";
        }

        qreal frame_rate = exportDialog->frameRate();
        QString frame_rate_str = QString::number(frame_rate);
        writer.writeStartElement("Format");
            writer.writeAttribute("VideoFormat", image_size_str);
            writer.writeAttribute("FrameRate", frame_rate_str);
            writer.writeAttribute("DropFrame", drop_frame_str);
        writer.writeEndElement();


        QString content_in = MyAttributesConverter::timeHMSmsToFrames(exportDialog->startTime(), frame_rate);
        QString content_out = MyAttributesConverter::timeHMSmsToFrames(subtitlesList.last().endTime(), frame_rate);
        QString first_event_in = MyAttributesConverter::timeHMSmsToFrames(subtitlesList.first().startTime(), frame_rate);
        quint16 number_of_sub = subtitlesList.size();
        writer.writeStartElement("Events");
            writer.writeAttribute("ContentInTC", content_in);
            writer.writeAttribute("ContentOutTC", content_out);
            writer.writeAttribute("FirstEventInTC", first_event_in);
            writer.writeAttribute("LastEventOutTC", content_out);
            writer.writeAttribute("NumberofEvents", QString::number(number_of_sub));
            writer.writeAttribute("Type", "Graphic");
        writer.writeEndElement();

    writer.writeEndElement(); // Description

    writer.writeStartElement("Events");

    QSize video_native_size = qApp->property("prop_resolution_px").toSize();
    ImagesExporter image_exporter(video_native_size);

    QString file_path = file.fileName();
    file_path = file_path.remove(file_path.lastIndexOf('.'), file_path.size());
    QString file_name = file.fileName().section('/',-1);
    QString image_path_name_prefix = file_path;
    QString image_extension = ".png";

    QProgressDialog progress_dialog("Exporting images...", "Cancel...", 0, subtitlesList.size(), exportDialog);
    progress_dialog.setWindowModality(Qt::WindowModal);

    for ( qint16 i = 0; i < subtitlesList.size(); i++ ) {

        MySubtitles current_sub = subtitlesList.at(i);

        writer.writeStartElement("Event");

            writer.writeAttribute("Forced", "false");
            writer.writeAttribute("InTC", MyAttributesConverter::timeHMSmsToFrames(current_sub.startTime(), frame_rate));
            writer.writeAttribute("OutTC", MyAttributesConverter::timeHMSmsToFrames(current_sub.endTime(), frame_rate));

            writer.writeStartElement("Graphic");

            QString number_str;
            QString image_file_name = image_path_name_prefix +number_str.sprintf("%04d", (i+1)) +image_extension;

            progress_dialog.setValue(i);

            if (progress_dialog.wasCanceled()) {
                return false;
            }

            image_exporter.createImage(current_sub, image_file_name, image_size, false, "png", Qt::transparent, 4);


            int x;
            int y;
            int width;
            int height;

            image_exporter.imageRect().getRect(&x, &y, &width, &height);

                writer.writeAttribute("X", QString::number(x));
                writer.writeAttribute("Y", QString::number(y));
                writer.writeAttribute("Width", QString::number(width));
                writer.writeAttribute("Height", QString::number(height));
                writer.writeCharacters(image_file_name.section('/', -1));

            writer.writeEndElement(); // Graphics

        writer.writeEndElement(); // Event
    }

    writer.writeEndElement(); // Events

    writer.writeEndElement(); // BDN

    writer.writeEndDocument();

    file.writeText(out);

    progress_dialog.setValue(subtitlesList.size());

    return true;
}


QList<MySubtitles> BDNparser::open(MyFileReader file) {
    QList<MySubtitles> sub_list;
    return sub_list;
}

bool BDNparser::readSample(MyFileReader file) {
    return false;
}
