
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
    mysubtitlefileparser.cpp \
    mytextedit.cpp \
    mywaveformslider.cpp \
    amapolaprojfileparser.cpp \
    SubtitlesParsers/DcSubtitle/interop/dcsubinteropparser.cpp \
    SubtitlesParsers/DcSubtitle/smpte/dcsubsmpteparser.cpp \
    subexportdialog.cpp \
    subimportmanager.cpp \
    SubtitlesParsers/EBU/ebuparser.cpp

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
    mysubtitlefileparser.h \
    mytextedit.h \
    mywaveformslider.h \
    amapolaprojfileparser.h \
    SubtitlesParsers/DcSubtitle/interop/dcsubinteropparser.h \
    SubtitlesParsers/DcSubtitle/smpte/dcsubsmpteparser.h \
    subexportdialog.h \
    subimportmanager.h \
    SubtitlesParsers/EBU/ebuparser.h

FORMS    += mainwindow.ui \
    mywaveform.ui \
    myvideoplayer.ui \
    mysettings.ui \
    subexportdialog.ui

CONFIG += qwt
