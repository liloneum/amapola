
QT       += core gui multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = amapola
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    myvideoplayer.cpp \
    mywaveform.cpp \
    mytextedit.cpp \
    mysubtitlestable.cpp

HEADERS  += mainwindow.h \
    myvideoplayer.h \
    mywaveform.h \
    mytextedit.h \
    mysubtitlestable.h

FORMS    += mainwindow.ui \
    mywaveform.ui \
    myvideoplayer.ui

CONFIG += qwt
