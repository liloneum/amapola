#include "myattributesconverter.h"
#include <QString>
#include <QFont>
#include <QFontMetrics>
#include <QRegExp>
#include <QTime>
#include <QTextEdit>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <mysubtitles.h>


#define SEC_TO_MSEC 1000

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

// Convert a direction "left-to-right", "right-to-left", "top-to-bottom", "bottom-to-top"
// into a direction "horizontal", "vertical"
QString MyAttributesConverter::dirToHorVer(QString direction) {

    if ( ( direction == "ltr" ) || ( direction == "rtl") ) {
        return "horizontal";
    }
    else if ( ( direction == "ttb" ) || ( direction == "btt" ) ) {
        return "vertical";
    }
    else {
        return "";
    }
}

// Convert a direction "horizontal", "vertical"
// Into a direction "left-to-right", "top-to-bottom"
QString MyAttributesConverter::dirToLtrTtb(QString direction) {

    if ( direction == "horizontal" ) {
        return "ltr";
    }
    else if ( direction == "vertical" ) {
        return "ttb";
    }
    else {
        return "";
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

// Convert a boolean to string "yes" or "no"
QString MyAttributesConverter::boolToString(bool boolean) {

    if ( boolean == true ) {
        return "yes";
    }
    else
        return "no";
}

// Convert time given with "ticks" or "1/10 second" to time with millisecond,
// with HH:MM:SS.zzz format.
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

        millisecond = qBound(0, millisecond, 999);

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
        millisecond = qBound(0, millisecond, 999);

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

// Convert time given in frame to time with millisecond,
// with HH:MM:SS.zzz format.
QString MyAttributesConverter::framesToTimeHMSms(QString time, qint16 timeCodeRate) {

    qint16 time_frames = time.section(":", -1).toInt();

    if ( time_frames >= timeCodeRate ) {
        time_frames = timeCodeRate - 1;
    }

    qint32 millisecond = qint32( ( (qreal)1000 / (qreal)timeCodeRate ) * (qreal)time_frames );
    millisecond = qBound(0, millisecond, 999);

    QString str_ms;
    if ( millisecond >= 100 ) { str_ms = QString::number(millisecond);}
    else if ( millisecond >= 10 ) { str_ms = "0" + QString::number(millisecond);}
    else { str_ms = "00" + QString::number(millisecond); }

    time.replace(time.lastIndexOf(":"),
                 4,
                 "." + str_ms);

    return time;
}

QString MyAttributesConverter::timeHMSmsToFrames(QString time, qint16 timeCodeRate) {

    qint32 millisecond = time.section(".", -1).toInt();
    qint32 frames = qint16( (qreal)millisecond / ( (qreal)1000 / (qreal)timeCodeRate ) );
    frames = qBound(0, frames, (timeCodeRate - 1));

    QString str_frames;
    if ( frames >= 10 ) { str_frames = QString::number(frames);}
    else { str_frames = "0" +QString::number(frames); }

    time.replace(time.lastIndexOf("."),
                 4,
                 ":" + str_frames);

    return time;
}

// Convert a time with format HH:MM:SS:zzz, where zzz are millisecond
// to time in millisecond
qint64 MyAttributesConverter::timeStrHMStoMs(QString time) {

    QTime time_base(0, 0, 0, 0);
    QTime time_HMS;

    time_HMS = QTime::fromString(time, "hh:mm:ss.zzz");

    if ( time_HMS.isValid() ) {
        return qAbs( time_HMS.msecsTo(time_base) );
    }
    else {
        return -1;
    }
}

// Round the given time to the nearest frame time
qint64 MyAttributesConverter::roundToFrame(qint64 timeMs, qreal frameRate) {

    qreal frame_duration_ms = (qreal)SEC_TO_MSEC / frameRate;

    return  qRound( (qreal)qRound( (qreal)timeMs / frame_duration_ms )  * frame_duration_ms );
}

// Round the given subtitles list to the nearest frame time
void MyAttributesConverter::roundSubListToFrame(qreal frameRate, QList<MySubtitles> & subList) {

    qint32 start_time_ms;
    qint32 end_time_ms;
    MySubtitles subtitle;
    QTime time_base(0, 0, 0, 0);

    QList<MySubtitles>::iterator it;
    for ( it = subList.begin(); it != subList.end(); ++it ) {

        subtitle = *it;
        start_time_ms = qAbs( QTime::fromString(subtitle.startTime(), "hh:mm:ss.zzz").msecsTo(time_base) );
        start_time_ms = MyAttributesConverter::roundToFrame(start_time_ms, frameRate);
        it->setStartTime( time_base.addMSecs(start_time_ms).toString("hh:mm:ss.zzz") );

        end_time_ms = qAbs( QTime::fromString(subtitle.endTime(), "hh:mm:ss.zzz").msecsTo(time_base) );
        end_time_ms = MyAttributesConverter::roundToFrame(end_time_ms, frameRate);
        it->setEndTime( time_base.addMSecs(end_time_ms).toString("hh:mm:ss.zzz") );
    }
}

qint32 MyAttributesConverter::timeMsToFrames(qint64 timeMs, qreal frameRate) {

    qreal frame_duration_ms = (qreal)SEC_TO_MSEC / frameRate;

    return qRound( (qreal)timeMs / frame_duration_ms );
}


// Convert a QColor in string with format AARRGGBB
QString MyAttributesConverter::colorToString(QColor colorRgba) {

    QString color_str = "";
    QList<int> argb_list;

    argb_list.append( colorRgba.alpha() );
    argb_list.append( colorRgba.red() );
    argb_list.append( colorRgba.green() );
    argb_list.append( colorRgba.blue() );


    for ( int i = 0; i < argb_list.count(); i++ ) {

        if ( argb_list.at(i) == 0 ) {
            color_str += "00";
        }
        else {
            color_str += (QString::number(argb_list.at(i), 16 )).toUpper();
        }
    }

    return color_str;
}

// Convert a AARRGGBB string in QColor argb
QColor MyAttributesConverter::stringToColor(QString colorStr) {

    bool ok;

    return QColor::fromRgba( colorStr.toUInt(&ok, 16) );
}

// Simplify a color AARRGGBB formated.
// Set A, R, G, B parmaters to 0 or FF.
QString MyAttributesConverter::simplifyColorStr(QString colorStr) {

    bool ok;
    QString simplified_color_str = "FF";

    for ( quint16 i = 1; i < 4; i++ ) {

        quint8 color_composant = colorStr.mid((i * 2), 2).toInt(&ok, 16);

        if ( color_composant <= (255 / 2) ) {
            simplified_color_str.append("00");
        }
        else {
            simplified_color_str.append("FF");
        }
    }

    return simplified_color_str;
}

// Fill QPushButton icon with the given color
void MyAttributesConverter::setColorToButton(QPushButton* button, QColor color) {

    QPixmap pix_map(button->iconSize().width(), button->iconSize().height());
    pix_map.fill( color );
    button->setIcon(QIcon(pix_map));
}

QString MyAttributesConverter::htmlToPlainText(QString htmlText) {

    QTextEdit text_edit;

    text_edit.setText(htmlText);

    return text_edit.toPlainText();
}

QString MyAttributesConverter::plainTextToHtml(QString plainText) {

    QTextEdit text_edit;
    text_edit.setHtml(plainText);
    return simplifyRichTextFilter(text_edit.toHtml());
}

// Richtext simplification filter helpers: Elements to be discarded
static inline bool filterElement(const QStringRef &name)
{
    return name != QStringLiteral("meta") && name != QStringLiteral("style") && name != QStringLiteral("br");
}

// Richtext simplification filter helpers: Filter attributes of elements
static inline void filterAttributes(const QStringRef &name,
                                    QXmlStreamAttributes *atts,
                                    bool *paragraphAlignmentFound)
{
    typedef QXmlStreamAttributes::iterator AttributeIt;

    if (atts->isEmpty())
        return;

     // No style attributes for <body>
    if (name == QStringLiteral("body")) {
        atts->clear();
        return;
    }

    // Clean out everything except 'align' for 'p'
    if (name == QStringLiteral("p")) {
        for (AttributeIt it = atts->begin(); it != atts->end(); ) {
            if (it->name() == QStringLiteral("align")) {
                ++it;
                *paragraphAlignmentFound = true;
            } else {
                it = atts->erase(it);
            }
        }
        return;
    }

    if (name == QStringLiteral("span")) {
        for (AttributeIt it = atts->begin(); it != atts->end(); ) {
            if (it->name() == QStringLiteral("style")) {
                QString style_value = it->value().toString();
                if ( style_value.contains("background-color") ) {
                    int from_index = style_value.indexOf("background-color");
                    int lenght = style_value.indexOf(";", from_index) - from_index + 1;
                    style_value.remove(from_index, lenght);
                    it = atts->erase(it);
                    atts->insert(it, QXmlStreamAttribute("style", style_value));
                }
                ++it;
            }
            else {
                ++it;
            }
        }
        return;
    }
}

// Richtext simplification filter helpers: Check for blank QStringRef.
static inline bool isWhiteSpace(const QStringRef &in)
{
    const int count = in.size();
    for (int i = 0; i < count; i++)
        if (!in.at(i).isSpace())
            return false;
    return true;
}

// Richtext simplification filter: Remove hard-coded font settings,
// <style> elements, <p> attributes other than 'align' and
// and unnecessary meta-information.
QString MyAttributesConverter::simplifyRichTextFilter(QString in)
{
    unsigned elementCount = 0;
    bool paragraphAlignmentFound = false;
    QString out;
    QXmlStreamReader reader(in);
    QXmlStreamWriter writer(&out);
    writer.setAutoFormatting(false);
    writer.setAutoFormattingIndent(0);

    while (!reader.atEnd()) {
        switch (reader.readNext()) {
        case QXmlStreamReader::StartElement:
            elementCount++;
            if ( filterElement(reader.name()) ) {
                const QStringRef name = reader.name();
                QXmlStreamAttributes attributes = reader.attributes();
                filterAttributes(name, &attributes, &paragraphAlignmentFound);
                writer.writeStartElement(name.toString());
                if (!attributes.isEmpty())
                    writer.writeAttributes(attributes);
            } else {
                reader.readElementText(); // Skip away all nested elements and characters.
            }
            break;
        case QXmlStreamReader::Characters:
            if (!isWhiteSpace(reader.text()))
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
