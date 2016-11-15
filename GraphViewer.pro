#-------------------------------------------------
#
# Project created by QtCreator 2016-11-10T21:18:37
#
#-------------------------------------------------

QT       += core gui charts printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GraphViewer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    chartview.cpp \
    chart.cpp

HEADERS  += mainwindow.h \
    chartview.h \
    chart.h

FORMS    += mainwindow.ui
