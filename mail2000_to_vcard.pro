#-------------------------------------------------
#
# Project created by QtCreator 2016-11-17T20:19:22
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mail2000_to_vcard
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ContactReader.cpp

HEADERS  += mainwindow.h \
    ContactReader.h

FORMS    += mainwindow.ui
