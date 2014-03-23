
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
    myapplication.cpp \
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
    myapplication.h \
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

FORMS    += mainwindow.ui \
    mywaveform.ui \
    myvideoplayer.ui \
    mysettings.ui \
    subexportdialog.ui \
    Tools/findtool.ui

#CONFIG += qwt


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../../usr/local/qwt-6.1.0-rc3/lib/release/ -lqwt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../../usr/local/qwt-6.1.0-rc3/lib/debug/ -lqwt
else:unix: LIBS += -L$$PWD/../../../../../../../usr/local/qwt-6.1.0-rc3/lib/ -lqwt

INCLUDEPATH += $$PWD/../../../../../../../usr/local/qwt-6.1.0-rc3/include
DEPENDPATH += $$PWD/../../../../../../../usr/local/qwt-6.1.0-rc3/include
