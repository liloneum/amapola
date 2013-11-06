
QT       += core gui multimedia multimediawidgets xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = amapola
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    myvideoplayer.cpp \
    mywaveform.cpp \
    mysubtitlestable.cpp \
    myfilereader.cpp \
    SubtitlesParsers/SubRip/subripparser.cpp \
    mysubtitles.cpp \
    myfilewriter.cpp \
    mysettings.cpp \
    myattributesconverter.cpp \
    SubtitlesParsers/DcSubtitle/dcsubparser.cpp \
    mysubtitlefileparser.cpp \
    mytextedit.cpp \
    mywaveformslider.cpp

HEADERS  += mainwindow.h \
    myvideoplayer.h \
    mywaveform.h \
    mysubtitlestable.h \
    myfilereader.h \
    SubtitlesParsers/SubRip/subripparser.h \
    mysubtitles.h \
    myfilewriter.h \
    mysettings.h \
    myattributesconverter.h \
    SubtitlesParsers/DcSubtitle/dcsubparser.h \
    mysubtitlefileparser.h \
    mytextedit.h \
    mywaveformslider.h

FORMS    += mainwindow.ui \
    mywaveform.ui \
    myvideoplayer.ui \
    mysettings.ui

CONFIG += qwt
