#-------------------------------------------------
#
# Project created by QtCreator 2014-12-31T12:10:21
#
#-------------------------------------------------

QT       += core gui network widgets

TARGET = ArenaTracker
TEMPLATE = app

QT_CONFIG -= no-pkg-config

# MAC OS FIX (set your opencv headers path)
#unix:INCLUDEPATH += "/usr/local/Cellar/opencv/2.4.12/include/opencv headers"

CONFIG += link_pkgconfig
PKGCONFIG += opencv

SOURCES += Sources/main.cpp\
    Sources/mainwindow.cpp \
    Sources/logloader.cpp \
    Sources/logworker.cpp \
    Sources/gamewatcher.cpp \
    Sources/webuploader.cpp \
    Sources/hscarddownloader.cpp \
    Sources/deckhandler.cpp \
    Sources/enemyhandhandler.cpp \
    Sources/arenahandler.cpp \
    Sources/secretshandler.cpp \
    Sources/drafthandler.cpp \
    Sources/heartharenamentor.cpp \
    Sources/utility.cpp \
    Sources/Cards/deckcard.cpp \
    Sources/Cards/handcard.cpp \
    Sources/Cards/secretcard.cpp \
    Sources/Cards/draftcard.cpp \
    Sources/Cards/drawcard.cpp \
    Sources/Widgets/resizebutton.cpp \
    Sources/Widgets/draftscorewindow.cpp \
    Sources/Widgets/scorebutton.cpp \
    Sources/Widgets/movelistwidget.cpp \
    Sources/Widgets/movetabwidget.cpp \
    Sources/Widgets/movetreewidget.cpp \
    Sources/Widgets/moveverticalscrollarea.cpp \
    Sources/Widgets/cardwindow.cpp \
    Sources/versionchecker.cpp \
    Sources/hearthstatsuploader.cpp \
    Sources/enemydeckhandler.cpp

HEADERS  += Sources/mainwindow.h \
    Sources/logloader.h \
    Sources/logworker.h \
    Sources/gamewatcher.h \
    Sources/webuploader.h \
    Sources/hscarddownloader.h \
    Sources/deckhandler.h \
    Sources/enemyhandhandler.h \
    Sources/arenahandler.h \
    Sources/secretshandler.h \
    Sources/drafthandler.h \
    Sources/heartharenamentor.h \
    Sources/utility.h \
    Sources/Cards/deckcard.h \
    Sources/Cards/handcard.h \
    Sources/Cards/secretcard.h \
    Sources/Cards/draftcard.h \
    Sources/Cards/drawcard.h \
    Sources/Widgets/resizebutton.h \
    Sources/Widgets/draftscorewindow.h \
    Sources/Widgets/scorebutton.h \
    Sources/Widgets/movelistwidget.h \
    Sources/Widgets/ui_extended.h \
    Sources/Widgets/movetabwidget.h \
    Sources/Widgets/movetreewidget.h \
    Sources/Widgets/moveverticalscrollarea.h \
    Sources/Widgets/cardwindow.h \
    Sources/versionchecker.h \
    Sources/hearthstatsuploader.h \
    Sources/enemydeckhandler.h

FORMS    += mainwindow.ui

RESOURCES += \
    arenatracker.qrc

win32: RC_ICONS = ArenaTracker.ico
