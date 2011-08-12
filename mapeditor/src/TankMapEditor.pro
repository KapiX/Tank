#-------------------------------------------------
#
# Project created by QtCreator 2010-12-23T19:57:30
#
#-------------------------------------------------

QT       += core gui

TARGET = TankMapEditor
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    aboutdialog.cpp \
    tankmapwidget.cpp \
    qtwin.cpp \
    newwizard.cpp

HEADERS  += mainwindow.h \
    aboutdialog.h \
    tankmapwidget.h \
    qtwin.h \
    newwizard.h

FORMS    += mainwindow.ui \
    aboutdialog.ui \
    newwizard.ui

RESOURCES += \
    resources.qrc \
    resources.qrc

OTHER_FILES += \
    resources.rc

RC_FILE = resources.rc

TRANSLATIONS = tankme_pl.ts
CODECFORTR = UTF-8
CODECFORSRC = WINDOWS-1250
