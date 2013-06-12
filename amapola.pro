
QT       += core gui multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = amapola
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    myvideoplayer.cpp \
    mywaveform.cpp

HEADERS  += mainwindow.h \
    myvideoplayer.h \
    mywaveform.h

FORMS    += mainwindow.ui \
    mywaveform.ui \
    myvideoplayer.ui

CONFIG += qwt
