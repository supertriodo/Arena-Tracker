#-------------------------------------------------
#
# Project created by QtCreator 2014-12-31T12:10:21
#
#-------------------------------------------------

QT       += core gui network widgets

TARGET = ArenaTracker
TEMPLATE = app

CONFIG += link_pkgconfig
PKGCONFIG += opencv

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
    Sources/secretcard.cpp \
    Sources/drafthandler.cpp \
    Sources/heartharenamentor.cpp \
    Sources/draftcard.cpp \
    Sources/utility.cpp \
    Sources/drawcard.cpp

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
    Sources/secretcard.h \
    Sources/drafthandler.h \
    Sources/heartharenamentor.h \
    Sources/draftcard.h \
    Sources/utility.h \
    Sources/drawcard.h

FORMS    += mainwindow.ui

RESOURCES += \
    arenatracker.qrc

win32: RC_ICONS = ArenaTracker.ico
