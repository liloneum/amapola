
QT       += core gui multimedia multimediawidgets xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = amapola
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    myvideoplayer.cpp \
    mywaveform.cpp \
    mytextedit.cpp \
    mysubtitlestable.cpp \
    myfilereader.cpp \
    SubtitlesParsers/SubRip/subripparser.cpp \
    mysubtitles.cpp \
    myfilewriter.cpp \
    mysettings.cpp \
    SubtitlesParsers/DcSubtitle/dcsub.cpp \
    mytextedit2.cpp \
    myattributesconverter.cpp

HEADERS  += mainwindow.h \
    myvideoplayer.h \
    mywaveform.h \
    mytextedit.h \
    mysubtitlestable.h \
    myfilereader.h \
    SubtitlesParsers/SubRip/subripparser.h \
    mysubtitles.h \
    myfilewriter.h \
    mysettings.h \
    SubtitlesParsers/DcSubtitle/dcsub.h \
    mytextedit2.h \
    myattributesconverter.h

FORMS    += mainwindow.ui \
    mywaveform.ui \
    myvideoplayer.ui \
    mysettings.ui \
    mytextedit2.ui

CONFIG += qwt
