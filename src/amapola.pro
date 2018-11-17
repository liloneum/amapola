
QT       += core gui multimedia multimediawidgets xml

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

TARGET = amapola
TEMPLATE = app

include(SubtitlesParsers/SubtitlesParsers.pri)
include(Tools/Tools.pri)

SOURCES += main.cpp\
    mainwindow.cpp \
    videoplayer.cpp \
    waveform.cpp \
    subtitlestable.cpp \
    filereader.cpp \
    subtitles.cpp \
    filewriter.cpp \
    settings.cpp \
    attributesconverter.cpp \
    subtitlefileparser.cpp \
    textedit.cpp \
    waveformslider.cpp \
    application.cpp \
    amapolaprojfileparser.cpp \
    subexportdialog.cpp \
    subimportmanager.cpp \
    imagesexporter.cpp \
    inputsizedialog.cpp \

HEADERS += mainwindow.h \
    videoplayer.h \
    waveform.h \
    subtitlestable.h \
    filereader.h \
    subtitles.h \
    filewriter.h \
    settings.h \
    attributesconverter.h \
    subtitlefileparser.h \
    textedit.h \
    waveformslider.h \
    application.h \
    amapolaprojfileparser.h \
    subexportdialog.h \
    subimportmanager.h \
    imagesexporter.h \
    inputsizedialog.h

FORMS += mainwindow.ui \
    waveform.ui \
    videoplayer.ui \
    settings.ui \
    subexportdialog.ui

CONFIG += qwt

mac {
    QWT_DIR = /usr/local
    LIBS += -F$${QWT_DIR}/lib -framework qwt
}
!mac {
    QWT_DIR = $$PWD/../lib/qwt
    LIBS += -L$${QWT_DIR}/lib -lqwt
}
INCLUDEPATH +=  $${QWT_DIR}/include/

OTHER_FILES += \
    ../lib/README.md
