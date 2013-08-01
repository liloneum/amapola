#include "myattributesconverter.h"
#include <QString>
#include <QFont>
#include <QFontMetrics>
#include <QRegExp>

MyAttributesConverter::MyAttributesConverter()
{
}

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

QString MyAttributesConverter::isItalic(bool italic) {

    if ( italic == true ) {
        return "yes";
    }
    else
        return "no";
}

QString MyAttributesConverter::isUnderlined(bool underlined) {

    if ( underlined == true ) {
        return "yes";
    }
    else
        return "no";
}

qint16 MyAttributesConverter::fontHeightToSize(QString fontName, QString fontHeight) {

    QFont font(fontName);
    qint16 font_size_int = fontHeight.toInt();
    font.setPointSize(font_size_int);

    qint16 font_height;
    QFontMetrics font_metrics(font);
    font_height = font_metrics.height();
    qint16 i = 0;
    while (font_height > font_size_int) {
        font.setPointSize(font_size_int - i);
        QFontMetrics font_metrics2(font);
        font_height = font_metrics2.height();
        i++;
    }

    font_size_int = font_size_int - i;

    //return font_size_int;
    return 72;
}

qint16 MyAttributesConverter::fontSizeToHeight(QString fontName, QString fontSize) {
    
    QFont font(fontName);
    qint16 font_size_int = fontSize.toInt();
    font.setPointSize(font_size_int);

    QFontMetrics font_metrics(font);

    qint16 font_height = font_metrics.height();

    return font_height;
}

QString MyAttributesConverter::toTimeHMSms(QString time) {

    QRegExp time_format1("^\\d{2}:\\d{2}:\\d{2}\\.\\d+$");
    //time_format1.setPatternSyntax(QRegExp::Wildcard);
    QRegExp time_format2("^\\d{2}:\\d{2}:\\d{2}:\\d{3}$");
    //time_format2.setPatternSyntax(QRegExp::Wildcard);

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
