cmake_minimum_required(VERSION 3.16)
project(ArenaTracker
    VERSION 21.12
    DESCRIPTION "A deck tracker that gives you a lot of extra info while playing Hearthstone"
    LANGUAGES CXX
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(libzip REQUIRED)
find_package(ZLIB REQUIRED)
find_package(OpenCV 2.4.11 REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS 
    Core
    Gui
    Network
    Widgets
    WebSockets
)

set(SOURCES
    Sources/arenahandler.cpp
    Sources/arenahandler.h
    Sources/Cards/deckcard.cpp
    Sources/Cards/deckcard.h
    Sources/Cards/draftcard.cpp
    Sources/Cards/draftcard.h
    Sources/Cards/drawcard.cpp
    Sources/Cards/drawcard.h
    Sources/Cards/handcard.cpp
    Sources/Cards/handcard.h
    Sources/Cards/popularcard.cpp
    Sources/Cards/popularcard.h
    Sources/Cards/rngcard.cpp
    Sources/Cards/rngcard.h
    Sources/Cards/secretcard.cpp
    Sources/Cards/secretcard.h
    Sources/Cards/synergycard.cpp
    Sources/Cards/synergycard.h
    Sources/constants.h
    Sources/deckhandler.cpp
    Sources/deckhandler.h
    Sources/detachwindow.cpp
    Sources/detachwindow.h
    Sources/drafthandler.cpp
    Sources/drafthandler.h
    Sources/drawcardhandler.cpp
    Sources/drawcardhandler.h
    Sources/enemydeckhandler.cpp
    Sources/enemydeckhandler.h
    Sources/enemyhandhandler.cpp
    Sources/enemyhandhandler.h
    Sources/gamewatcher.cpp
    Sources/gamewatcher.h
    Sources/graveyardhandler.cpp
    Sources/graveyardhandler.h
    Sources/heartharenamentor.cpp
    Sources/heartharenamentor.h
    Sources/hscarddownloader.cpp
    Sources/hscarddownloader.h
    Sources/LibSmtp/smtp.cpp
    Sources/LibSmtp/smtp.h
    Sources/logloader.cpp
    Sources/logloader.h
    Sources/logworker.cpp
    Sources/logworker.h
    Sources/main.cpp
    Sources/mainwindow.cpp
    Sources/mainwindow.h
    Sources/planhandler.cpp
    Sources/planhandler.h
    Sources/popularcardshandler.cpp
    Sources/popularcardshandler.h
    Sources/premiumhandler.cpp
    Sources/premiumhandler.h
    Sources/rngcardhandler.cpp
    Sources/rngcardhandler.h
    Sources/secretshandler.cpp
    Sources/secretshandler.h
    Sources/Synergies/draftdropcounter.cpp
    Sources/Synergies/draftdropcounter.h
    Sources/Synergies/draftitemcounter.cpp
    Sources/Synergies/draftitemcounter.h
    Sources/Synergies/statsynergies.cpp
    Sources/Synergies/statsynergies.h
    Sources/synergyhandler.cpp
    Sources/synergyhandler.h
    Sources/themehandler.cpp
    Sources/themehandler.h
    Sources/trackobotuploader.cpp
    Sources/trackobotuploader.h
    Sources/twitchhandler.cpp
    Sources/twitchhandler.h
    Sources/utility.cpp
    Sources/utility.h
    Sources/Utils/deckstringhandler.cpp
    Sources/Utils/deckstringhandler.h
    Sources/Utils/libzippp.cpp
    Sources/Utils/libzippp.h
    Sources/Utils/qcompressor.cpp
    Sources/Utils/qcompressor.h
    Sources/versionchecker.cpp
    Sources/versionchecker.h
    Sources/Widgets/bombwindow.cpp
    Sources/Widgets/bombwindow.h
    Sources/Widgets/cardlistwindow.cpp
    Sources/Widgets/cardlistwindow.h
    Sources/Widgets/cardwindow.cpp
    Sources/Widgets/cardwindow.h
    Sources/Widgets/draftherowindow.cpp
    Sources/Widgets/draftherowindow.h
    Sources/Widgets/draftmechanicswindow.cpp
    Sources/Widgets/draftmechanicswindow.h
    Sources/Widgets/draftscorewindow.cpp
    Sources/Widgets/draftscorewindow.h
    Sources/Widgets/extended_ui.h
    Sources/Widgets/GraphicItems/arrowgraphicsitem.cpp
    Sources/Widgets/GraphicItems/arrowgraphicsitem.h
    Sources/Widgets/GraphicItems/cardgraphicsitem.cpp
    Sources/Widgets/GraphicItems/cardgraphicsitem.h
    Sources/Widgets/GraphicItems/graphicsitemsender.cpp
    Sources/Widgets/GraphicItems/graphicsitemsender.h
    Sources/Widgets/GraphicItems/herographicsitem.cpp
    Sources/Widgets/GraphicItems/herographicsitem.h
    Sources/Widgets/GraphicItems/heropowergraphicsitem.cpp
    Sources/Widgets/GraphicItems/heropowergraphicsitem.h
    Sources/Widgets/GraphicItems/miniongraphicsitem.cpp
    Sources/Widgets/GraphicItems/miniongraphicsitem.h
    Sources/Widgets/GraphicItems/weapongraphicsitem.cpp
    Sources/Widgets/GraphicItems/weapongraphicsitem.h
    Sources/Widgets/hoverlabel.cpp
    Sources/Widgets/hoverlabel.h
    Sources/Widgets/lavabutton.cpp
    Sources/Widgets/lavabutton.h
    Sources/Widgets/mainwindow.ui
    Sources/Widgets/movelistwidget.cpp
    Sources/Widgets/movelistwidget.h
    Sources/Widgets/movetabwidget.cpp
    Sources/Widgets/movetabwidget.h
    Sources/Widgets/movetreewidget.cpp
    Sources/Widgets/movetreewidget.h
    Sources/Widgets/moveverticalscrollarea.cpp
    Sources/Widgets/moveverticalscrollarea.h
    Sources/Widgets/plangraphicsview.cpp
    Sources/Widgets/plangraphicsview.h
    Sources/Widgets/resizebutton.cpp
    Sources/Widgets/resizebutton.h
    Sources/Widgets/scorebutton.cpp
    Sources/Widgets/scorebutton.h
    Sources/Widgets/twitchbutton.cpp
    Sources/Widgets/twitchbutton.h
    Sources/Widgets/webenginepage.h
)

qt_add_big_resources(SOURCES arenatracker.qrc)

if(WIN32)
    enable_language("RC")
    set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/ArenaTracker.rc")
    add_executable(app ${SOURCES} ${APP_ICON_RESOURCE_WINDOWS})
else()
    add_executable(app ${SOURCES})
endif()

set_target_properties (app PROPERTIES OUTPUT_NAME Arena-Tracker)

if(APPLE)
    if(CMAKE_OSX_DEPLOYMENT_TARGET)
        target_compile_options(app PUBLIC -mmacosx-version-min=${CMAKE_OSX_DEPLOYMENT_TARGET})
        target_link_options(app PUBLIC -mmacosx-version-min=${CMAKE_OSX_DEPLOYMENT_TARGET})
    endif()
    set_target_properties(app PROPERTIES
        MACOSX_BUNDLE YES
        MACOSX_BUNDLE_BUNDLE_NAME ${CMAKE_PROJECT_NAME}
        MACOSX_BUNDLE_INFO_STRING "${PROJECT_VERSION}"
        MACOSX_BUNDLE_ICON_FILE "ArenaTracker.icns"
        MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.github.supertriodo.arenatracker"
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.github.supertriodo.arenatracker"
    )
    find_package(Iconv REQUIRED)
    target_link_libraries(app PUBLIC Iconv::Iconv)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(app PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Network
    Qt::WebSockets
    Qt::Widgets
    ZLIB::ZLIB
    libzip::zip
    ${OpenCV_LIBS}
)

if(MSYS)
    install (TARGETS app DESTINATION .)
elseif(UNIX AND NOT APPLE)
    include (GNUInstallDirs)
    set_target_properties (app PROPERTIES OUTPUT_NAME arena-tracker)
    install(TARGETS app DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
