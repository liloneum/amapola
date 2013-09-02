#include "myattributesconverter.h"
#include <QString>

MyAttributesConverter::MyAttributesConverter()
{
}

Qt::Alignment MyAttributesConverter::vAlignFromString(QString vAlignAttribute) {

    if ( vAlignAttribute == "left") {
        return Qt::AlignLeft;
    }
    else if ( vAlignAttribute == "right") {
        return Qt::AlignRight;
    }
    else if ( vAlignAttribute == "center") {
        return Qt::AlignCenter;
    }
    else {
        return Qt::AlignCenter;
    }
}

Qt::Alignment MyAttributesConverter::hAlignFromString(QString hAlignAttribute) {

    if ( hAlignAttribute == "top" ) {
        return Qt::AlignTop;
    }
    else if ( hAlignAttribute == "bottom" ) {
        return Qt::AlignBottom;
    }
    else if ( hAlignAttribute == "center" ) {
        return Qt::AlignCenter;
    }
    else {
        return Qt::AlignCenter;
    }
}

bool MyAttributesConverter::isItalic(QString italic) {

    if ( italic == "yes" ) {
        return true;
    }
    else if ( italic == "no" ) {
        return false;
    }
    else return false;
}
