#-------------------------------------------------
#
# Project created by QtCreator 2016-08-18T16:27:34
#
#-------------------------------------------------


QT       += core gui
QT       += network
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = marks
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    depart.cpp

HEADERS  += mainwindow.h \
    depart.h

FORMS    += mainwindow.ui

RESOURCES += \
    files.qrc
