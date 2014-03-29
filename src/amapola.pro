
QT       += core gui multimedia multimediawidgets xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = amapola
TEMPLATE = app

SOURCES += main.cpp\
    mainwindow.cpp \
    videoplayer.cpp \
    waveform.cpp \
    subtitlestable.cpp \
    filereader.cpp \
    SubtitlesParsers/SubRip/subripparser.cpp \
    subtitles.cpp \
    filewriter.cpp \
    settings.cpp \
    attributesconverter.cpp \
    subtitlefileparser.cpp \
    textedit.cpp \
    waveformslider.cpp \
    application.cpp \
    amapolaprojfileparser.cpp \
    SubtitlesParsers/DcSubtitle/interop/dcsubinteropparser.cpp \
    SubtitlesParsers/DcSubtitle/smpte/dcsubsmpteparser.cpp \
    subexportdialog.cpp \
    subimportmanager.cpp \
    SubtitlesParsers/EBU/ebuparser.cpp \
    imagesexporter.cpp \
    SubtitlesParsers/ScenaristSub/scenaristsubparser.cpp \
    inputsizedialog.cpp \
    SubtitlesParsers/BDN/bdnparser.cpp \
    Tools/findtool.cpp

HEADERS += mainwindow.h \
    videoplayer.h \
    waveform.h \
    subtitlestable.h \
    filereader.h \
    SubtitlesParsers/SubRip/subripparser.h \
    subtitles.h \
    filewriter.h \
    settings.h \
    attributesconverter.h \
    subtitlefileparser.h \
    textedit.h \
    waveformslider.h \
    application.h \
    amapolaprojfileparser.h \
    SubtitlesParsers/DcSubtitle/interop/dcsubinteropparser.h \
    SubtitlesParsers/DcSubtitle/smpte/dcsubsmpteparser.h \
    subexportdialog.h \
    subimportmanager.h \
    SubtitlesParsers/EBU/ebuparser.h \
    imagesexporter.h \
    SubtitlesParsers/ScenaristSub/scenaristsubparser.h \
    inputsizedialog.h \
    SubtitlesParsers/BDN/bdnparser.h \
    Tools/findtool.h

FORMS += mainwindow.ui \
    waveform.ui \
    videoplayer.ui \
    settings.ui \
    subexportdialog.ui \
    Tools/findtool.ui

CONFIG += qwt

QWT_DIR = $$PWD/../lib/qwt
INCLUDEPATH +=  $${QWT_DIR}/src/
LIBS += -L$${QWT_DIR}/lib -lqwt

OTHER_FILES += \
    ../lib/README.md
