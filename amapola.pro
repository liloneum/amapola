
QT       += core gui multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = amapola
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    myvideoplayer.cpp

HEADERS  += mainwindow.h \
    myvideoplayer.h

FORMS    += mainwindow.ui \
    myvideoplayer.ui
