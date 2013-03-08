
QT       += core gui widgets printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = foamcut_gui
TEMPLATE = app


SOURCES += main.cpp \
    mainwindow.cpp  \
    qcustomplot.cpp \
    importwizard.cpp \
    datimportpage.cpp

HEADERS  += qcustomplot.h \
    mainwindow.h \
    importwizard.h \
    datimportpage.h

FORMS += \
    mainwindow.ui \
    datimportpage.ui

macx: LIBS += -L$$PWD/../../../build/foamcut/kernel/ -lfoamcut_kernel

INCLUDEPATH += $$PWD/../kernel
DEPENDPATH += $$PWD/../kernel
