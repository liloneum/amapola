#ifndef MYATTRIBUTESCONVERTER_H
#define MYATTRIBUTESCONVERTER_H

#include <Qt>

class MyAttributesConverter
{
public:
    MyAttributesConverter();
    static Qt::Alignment vAlignFromString(QString vAlignAttribute);
    static Qt::Alignment hAlignFromString(QString hAlignAttribute);
    static QString isItalic(bool italic);
    static QString isUnderlined(bool underlined);
    static qint16 fontHeightToSize(QString fontName, QString fontHeight);
    static qint16 fontSizeToHeight(QString fontName, QString fontSize);
    static QString toTimeHMSms(QString time);
    static QString toTimeHMSticks(QString time);
};

#endif // MYATTRIBUTESCONVERTER_H
