#ifndef MYATTRIBUTESCONVERTER_H
#define MYATTRIBUTESCONVERTER_H

#include <Qt>

class MyAttributesConverter
{
public:
    MyAttributesConverter();
    static Qt::Alignment vAlignFromString(QString vAlignAttribute);
    static Qt::Alignment hAlignFromString(QString hAlignAttribute);
    static bool isItalic(QString italic);
};

#endif // MYATTRIBUTESCONVERTER_H
