#-------------------------------------------------
#
# Project created by QtCreator 2017-04-30T19:58:37
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ASCII_Assistent
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    text.cpp \
    text_zeilenweise.cpp \
    umwandeln.cpp \
    wenndannsonst.cpp \
    formel.cpp \
    myfunktion.cpp

HEADERS  += mainwindow.h \
    text.h \
    text_zeilenweise.h \
    umwandeln.h \
    wenndannsonst.h \
    formel.h \
    myfunktion.h \
    myDefines.h

FORMS    += mainwindow.ui
