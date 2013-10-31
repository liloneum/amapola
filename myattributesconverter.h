#ifndef MYATTRIBUTESCONVERTER_H
#define MYATTRIBUTESCONVERTER_H

#include <Qt>
#include <QColor>
#include <QPushButton>

// Class to define some converters statics methods
class MyAttributesConverter
{
public:
    MyAttributesConverter();
    static Qt::Alignment vAlignFromString(QString vAlignAttribute);
    static Qt::Alignment hAlignFromString(QString hAlignAttribute);
    static QString isItalic(bool italic);
    static QString isUnderlined(bool underlined);
    static QString boolToString(bool boolean);
    static qint16 fontHeightToSize(QString fontName, QString fontHeight);
    static qint16 fontSizeToHeight(QString fontName, QString fontSize);
    static QString toTimeHMSms(QString time);
    static QString toTimeHMSticks(QString time);
    static qint64 timeStrHMStoMs(QString time);
    static qint64 roundToFrame(qint64 timeMs, qreal frameRate);
    static qint32 timeMsToFrames(qint64 timeMs, qreal frameRate);
    static QString colorToString(QColor colorRgba);
    static QColor stringToColor(QString color_str);
    static void setColorToButton(QPushButton* button, QColor color);
};

#endif // MYATTRIBUTESCONVERTER_H
