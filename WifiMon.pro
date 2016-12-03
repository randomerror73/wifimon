#-------------------------------------------------
#
# Project created by QtCreator 2016-11-26T21:52:09
#
#-------------------------------------------------

QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WifiMon
TEMPLATE = app


SOURCES += main.cpp\
        wifimon.cpp

HEADERS  += wifimon.h

FORMS    += wifimon.ui

RC_ICONS = icons\wifi-on.ico

RESOURCES += \
    resources.qrc
