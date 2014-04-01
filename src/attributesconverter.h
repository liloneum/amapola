#ifndef ATTRIBUTESCONVERTER_H
#define ATTRIBUTESCONVERTER_H

#include <Qt>
#include <QColor>
#include <QPushButton>
#include <subtitles.h>

// Class to define some converters statics methods
class AttributesConverter
{
public:
    AttributesConverter();
    static Qt::Alignment vAlignFromString(QString vAlignAttribute);
    static Qt::Alignment hAlignFromString(QString hAlignAttribute);
    static QString dirToHorVer(QString direction);
    static QString dirToLtrTtb(QString direction);
    static QString isItalic(bool italic);
    static QString isUnderlined(bool underlined);
    static QString boolToString(bool boolean);
    static qint16 fontHeightToSize(QString fontName, QString fontHeight);
    static qint16 fontSizeToHeight(QString fontName, QString fontSize);
    static QString toTimeHMSms(QString time);
    static QString toTimeHMSticks(QString time);
    static QString framesToTimeHMSms(QString time, qreal timeCodeRate);
    static QString timeHMSmsToFrames(QString time, qreal timeCodeRate);
    static qint64 timeStrHMStoMs(QString time);
    static qint64 roundToFrame(qint64 timeMs, qreal frameRate);
    static void roundSubListToFrame(qreal frameRate, QList<Subtitles> & subList);
    static qint32 timeMsToFrames(qint64 timeMs, qreal frameRate);
    static QString colorToString(QColor colorRgba);
    static QColor stringToColor(QString colorStr);
    static QString simplifyColorStr(QString colorStr);
    static void setColorToButton(QPushButton* button, QColor color);
    static QString htmlToPlainText(QString htmlText);
    static QString plainTextToHtml(QString plainText);
    static QString simplifyRichTextFilter(QString in);
};

#endif // ATTRIBUTESCONVERTER_H
