#-------------------------------------------------
#
# Project created by QtCreator 2014-12-31T12:10:21
#
#-------------------------------------------------

QT       += core gui network widgets

TARGET = ArenaTracker
TEMPLATE = app

QT_CONFIG -= no-pkg-config

CONFIG += link_pkgconfig
PKGCONFIG += opencv libzip
LIBS += -lz

SOURCES += Sources/main.cpp\
    Sources/mainwindow.cpp \
    Sources/logloader.cpp \
    Sources/logworker.cpp \
    Sources/gamewatcher.cpp \
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
    Sources/enemydeckhandler.cpp \
    Sources/planhandler.cpp \
    Sources/Widgets/GraphicItems/arrowgraphicsitem.cpp \
    Sources/Widgets/GraphicItems/cardgraphicsitem.cpp \
    Sources/Widgets/GraphicItems/herographicsitem.cpp \
    Sources/Widgets/GraphicItems/heropowergraphicsitem.cpp \
    Sources/Widgets/GraphicItems/miniongraphicsitem.cpp \
    Sources/Widgets/GraphicItems/weapongraphicsitem.cpp \
    Sources/Widgets/plangraphicsview.cpp \
    Sources/Widgets/GraphicItems/graphicsitemsender.cpp \
    Sources/Utils/qcompressor.cpp \
    Sources/Widgets/bombwindow.cpp \
    Sources/Cards/rngcard.cpp \
    Sources/Widgets/secretswindow.cpp \
    Sources/trackobotuploader.cpp \
    Sources/LibXls/endian.c \
    Sources/LibXls/getopt.c \
    Sources/LibXls/ole.c \
    Sources/LibXls/xls.c \
    Sources/LibXls/xlstool.c \
    Sources/Utils/deckstringhandler.cpp \
    Sources/themehandler.cpp \
    Sources/Utils/libzippp.cpp \
    Sources/Widgets/draftitemcounter.cpp \
    Sources/synergyhandler.cpp

HEADERS  += Sources/mainwindow.h \
    Sources/logloader.h \
    Sources/logworker.h \
    Sources/gamewatcher.h \
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
    Sources/enemydeckhandler.h \
    Sources/planhandler.h \
    Sources/Widgets/GraphicItems/arrowgraphicsitem.h \
    Sources/Widgets/GraphicItems/cardgraphicsitem.h \
    Sources/Widgets/GraphicItems/herographicsitem.h \
    Sources/Widgets/GraphicItems/heropowergraphicsitem.h \
    Sources/Widgets/GraphicItems/miniongraphicsitem.h \
    Sources/Widgets/GraphicItems/weapongraphicsitem.h \
    Sources/Widgets/plangraphicsview.h \
    Sources/Widgets/GraphicItems/graphicsitemsender.h \
    Sources/Utils/qcompressor.h \
    Sources/Widgets/bombwindow.h \
    Sources/Cards/rngcard.h \
    Sources/Widgets/secretswindow.h \
    Sources/trackobotuploader.h \
    Sources/LibXls/brdb.c.h \
    Sources/LibXls/brdb.h \
    Sources/LibXls/endian.h \
    Sources/LibXls/ole.h \
    Sources/LibXls/xls.h \
    Sources/LibXls/xlsstruct.h \
    Sources/LibXls/xlstool.h \
    Sources/LibXls/xlstypes.h \
    Sources/LibXls/config.h \
    Sources/constants.h \
    Sources/Utils/deckstringhandler.h \
    Sources/themehandler.h \
    Sources/Utils/libzippp.h \
    Sources/Widgets/draftitemcounter.h \
    Sources/synergyhandler.h

FORMS    += mainwindow.ui

RESOURCES += \
    arenatracker.qrc

linux: LIBS += -L/home/triodo/Documentos/ArenaTracker/Libs
win32: RC_ICONS = ArenaTracker.ico
macx{
    ICON = ArenaTracker.icns
    LIBS += -liconv
}

#Deploy MAC
#First time errors
#(opencv development package not found)         Add PATH --> :/usr/local/bin

