#include "myattributesconverter.h"
#include <QString>
#include <QFont>
#include <QFontMetrics>
#include <QRegExp>

// Class to define some converters statics methods
MyAttributesConverter::MyAttributesConverter()
{
}

// Convert an horizontal alignment string to Qt::Aligment enum
Qt::Alignment MyAttributesConverter::hAlignFromString(QString hAlignAttribute) {

    if ( hAlignAttribute == "left") {
        return Qt::AlignLeft;
    }
    else if ( hAlignAttribute == "right") {
        return Qt::AlignRight;
    }
    else if ( hAlignAttribute == "center") {
        return Qt::AlignCenter;
    }
    else {
        return Qt::AlignCenter;
    }
}

// Convert a vertical alignment string to Qt::Aligment enum
Qt::Alignment MyAttributesConverter::vAlignFromString(QString vAlignAttribute) {

    if ( vAlignAttribute == "top" ) {
        return Qt::AlignTop;
    }
    else if ( vAlignAttribute == "bottom" ) {
        return Qt::AlignBottom;
    }
    else if ( vAlignAttribute == "center" ) {
        return Qt::AlignCenter;
    }
    else {
        return Qt::AlignCenter;
    }
}

// Convert a boolean to string "yes" or "no"
QString MyAttributesConverter::isItalic(bool italic) {

    if ( italic == true ) {
        return "yes";
    }
    else
        return "no";
}

// Convert a boolean to string "yes" or "no"
QString MyAttributesConverter::isUnderlined(bool underlined) {

    if ( underlined == true ) {
        return "yes";
    }
    else
        return "no";
}

// Convert time given with "ticks" or "1/10 second" to time with millisecond,
// with HH:MM:SS:zzz format.
// One tick equal to 4 milliseconds
QString MyAttributesConverter::toTimeHMSms(QString time) {

    QRegExp time_format1("^\\d{2}:\\d{2}:\\d{2}\\.\\d+$");
    QRegExp time_format2("^\\d{2}:\\d{2}:\\d{2}:\\d{3}$");

    // Format is HH:MM:SS.zzz (or .zz, or .z) where "z" is given in 1/10 second
    if ( time_format1.exactMatch(time) ) {

        qint32 tenth_sec = time.section(".",-1).toInt();
        qint32 millisecond;
        if ( tenth_sec >= 100 ) {return time;}
        else if ( tenth_sec >= 10 ) { millisecond = tenth_sec * 10;}
        else { millisecond = tenth_sec * 100; }

        qBound(0, millisecond, 999);

        time.replace(time.indexOf("."),
                     3,
                     "." + QString::number(millisecond));

        return time;
    }
    // Format is HH:MM:SS.zzz, where "zzz" is given in "ticks"
    else if ( time_format2.exactMatch(time) ) {

        qint32 ticks = time.section(":", -1).toInt();
        qint32 millisecond;
        millisecond = ticks * 4;
        qBound(0, millisecond, 999);

        QString str_ms;
        if ( millisecond >= 100 ) { str_ms = QString::number(millisecond);}
        else if ( millisecond >= 10 ) { str_ms = "0" + QString::number(millisecond);}
        else { str_ms = "00" + QString::number(millisecond); }

        time.replace(time.lastIndexOf(":"),
                     4,
                     "." + str_ms);

        return time;
    }
    else {
        return ("");
    }
}

// Convert time given with millisecond to time with "ticks".
// Where a tick equal 4 milliseconds.
QString MyAttributesConverter::toTimeHMSticks(QString time) {

    qint32 millisecond = time.section(".",-1).toInt();
    qint32 ticks = millisecond / 4;

    QString str_ticks;
    if ( ticks >= 100 ) { str_ticks = QString::number(ticks);}
    else if ( ticks >= 10 ) { str_ticks = "0" + QString::number(ticks);}
    else { str_ticks = "00" + QString::number(ticks); }

    time.replace(time.lastIndexOf("."),
                 4,
                 ":" + str_ticks);

    return time;

}
