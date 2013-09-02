#include "subripparser.h"
#include <QRegExp>
#include <QStringList>
#include <mysubtitles.h>

SubRipParser::SubRipParser()
{
}

QList<MySubtitles> SubRipParser::open(MyFileReader file) {

    QVector<QString> lines = file.lines();

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

            if ( !( lines[i + 1].isEmpty() ) ) {

                QString text;
                text = lines[i + 1];

                if ( !( lines[i + 2].isEmpty() ) ) {

                    text += "\n";
                    text += lines[i + 2];
                }

//                new_subtitle.setText(text);
            }

            subtitles_list.append(new_subtitle);

        }
    }
    return subtitles_list;
}

void SubRipParser::save(MyFileWriter & file, QList<MySubtitles> subtitlesList) {

    for ( qint32 i = 0; i < subtitlesList.size(); i++ ) {

        QString sub_num;
        sub_num.setNum(i + 1);

        QString time(subtitlesList[i].startTime() + " --> " + subtitlesList[i].endTime());
        time.replace(".", ",");

        QString text_to_write( sub_num + "\n"
                               + time + "\n"
//                               + subtitlesList[i].text() + "\n"
                               + "\n");

        file.write(text_to_write);

    }
}
