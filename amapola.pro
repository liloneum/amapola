
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
    mytextedit2.cpp \
    myattributesconverter.cpp \
    SubtitlesParsers/DcSubtitle/dcsubparser.cpp \
    mysubtitlefileparser.cpp

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
    mytextedit2.h \
    myattributesconverter.h \
    SubtitlesParsers/DcSubtitle/dcsubparser.h \
    mysubtitlefileparser.h

FORMS    += mainwindow.ui \
    mywaveform.ui \
    myvideoplayer.ui \
    mysettings.ui \
    mytextedit2.ui

CONFIG += qwt
