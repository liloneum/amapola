#include "subripparser.h"
#include <QRegExp>
#include <QStringList>
#include <mysubtitles.h>


// SubRip format

// 1
// 00:00:54,210 --> 00:00:58,072
// text line 1
// text line 2

// 2
// 00:00:58,147 --> 00:01:03,021
// text line 1

// 3
// 00:01:04,125 --> 00:01:05,222
// text line 1

SubRipParser::SubRipParser()
{
}

// Parse SubRip file, retrieve subtitles infos.
// Subtitles read are saved in "MySubtitles" container
QList<MySubtitles> SubRipParser::open(MyFileReader file) {

    QStringList lines = file.lines();

    QRegExp time_regexp("^(\\d+):(\\d+):(\\d+),(\\d+)\\s-->\\s(\\d+):(\\d+):(\\d+),(\\d+)");

    QList<MySubtitles> subtitles_list;

    for ( qint32 i = 0; i < lines.size(); i++ ) {

        // Read the subtitle time "start --> end"
        if ( time_regexp.exactMatch(lines[i]) ) {

            QString times = lines[i];
            times = times.remove(" ");

            QStringList times_list;
            times_list = times.split(QRegExp("-->"));
            times_list = times_list.replaceInStrings(",", ".");

            MySubtitles new_subtitle;

            new_subtitle.setStartTime(times_list.first());
            new_subtitle.setEndTime(times_list.last());

            // If text line 1 exist, retrieve it
            if ( !( lines[i + 1].isEmpty() ) ) {

                TextLine text_line;
                TextFont text_font;

                text_line.setLine(lines[i + 1]);

                new_subtitle.setText(text_line, text_font);

                // If text line 2 exist, retrieve it
                if ( !( lines[i + 2].isEmpty() ) ) {

                    text_line.setLine(lines[i + 2]);

                    new_subtitle.setText(text_line, text_font);
                }
            }

            subtitles_list.append(new_subtitle);
        }
    }
    return subtitles_list;
}


// Write the subtitles list in a file (SubRip format)
void SubRipParser::save(MyFileWriter & file, QList<MySubtitles> subtitlesList) {

    for ( qint32 i = 0; i < subtitlesList.size(); i++ ) {

        QString sub_num;
        sub_num.setNum(i + 1);

        QString time(subtitlesList[i].startTime() + " --> " + subtitlesList[i].endTime());
        time.replace(".", ",");

        QString text("");

        for ( qint32 j = 0; j < subtitlesList[i].text().size(); j++ ) {

            TextLine text_line = subtitlesList[i].text().at(j);

            if ( text.isEmpty() ) {
                text = text_line.Line();
            }
            else {
                text += "\n";
                text += text_line.Line();
            }
        }

        QString text_to_write( sub_num + "\n"
                               + time + "\n"
                               + text + "\n"
                               + "\n");

        file.write(text_to_write);

    }
}
