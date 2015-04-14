#-------------------------------------------------
#
# Project created by QtCreator 2014-12-31T12:10:21
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ArenaTracker
TEMPLATE = app


SOURCES += Sources/main.cpp\
	Sources/mainwindow.cpp \
    Sources/logloader.cpp \
    Sources/logworker.cpp \
    Sources/gamewatcher.cpp \
    Sources/webuploader.cpp \
    Sources/hscarddownloader.cpp \
    Sources/resizebutton.cpp \
    Sources/deckhandler.cpp \
    Sources/deckcard.cpp \
    Sources/handcard.cpp \
    Sources/enemyhandhandler.cpp \
    Sources/arenahandler.cpp \
    Sources/secretshandler.cpp \
    Sources/secretcard.cpp

HEADERS  += Sources/mainwindow.h \
    Sources/logloader.h \
    Sources/logworker.h \
    Sources/gamewatcher.h \
    Sources/webuploader.h \
    Sources/hscarddownloader.h \
    Sources/deckcard.h \
    Sources/resizebutton.h \
    Sources/deckhandler.h \
    Sources/handcard.h \
    Sources/enemyhandhandler.h \
    Sources/arenahandler.h \
    Sources/secretshandler.h \
    Sources/secretcard.h

FORMS    += mainwindow.ui

RESOURCES += \
    arenatracker.qrc
