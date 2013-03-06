
QT       += core gui widgets printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = foamcut_gui
TEMPLATE = app


SOURCES += main.cpp \
    mainwindow.cpp  \
    qcustomplot.cpp

HEADERS  += qcustomplot.h \
    mainwindow.h

FORMS += \
    mainwindow.ui
