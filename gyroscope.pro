#-------------------------------------------------
#
# Project created by QtCreator 2016-06-23T09:57:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets serialport

TARGET = gyroscope
TEMPLATE = app

INCLUDEPATH += $$PWD/include


SOURCES += src/main.cpp\
        src/MainWindow.cpp \
    src/settingsWidget.cpp

HEADERS  += include/MainWindow.h \
    include/settingsWidget.h

FORMS    += ui/MainWindow.ui \
    ui/settingswidget.ui

RESOURCES += \
    gyroscope.qrc
