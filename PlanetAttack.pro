#-------------------------------------------------
#
# Project created by QtCreator 2012-04-27T10:59:14
#
#-------------------------------------------------

QT += core gui network opengl script scripttools xml

TARGET = PlanetAttack
TEMPLATE = app

include(jsedit/jsedit.pri)

HEADERS += \
    game.h \
    canvas.h \
    mainwindow.h \
    planet.h \
    ship.h \
    spaceobject.h \
    drawable.h \
    player.h \
    gametime.h \
    humanplayer.h \
    computerplayer.h \
    intelligence.h \
    playerintelligence.h \
    propertyeditor.h \
    stringpropertyeditor.h \
    vector2dpropertyeditor.h \
    playerintelligencepropertyeditor.h \
    playerpropertyeditor.h \
    scripttemplates.h \
    scriptedplayerintelligence.h \
    randomutil.h \
    scriptwindow.h \
    scriptextensions.h \
    scenarioserializer.h \
    xmlscenarioserializer.h \
    multiplayer.h \
    multiplayerserver.h \
    multiplayerclient.h \
    multiplayerpacket.h \
    multiplayerserverwindow.h

SOURCES += main.cpp \
    game.cpp \
    canvas.cpp \
    mainwindow.cpp \
    planet.cpp \
    ship.cpp \
    spaceobject.cpp \
    drawable.cpp \
    player.cpp \
    gametime.cpp \
    humanplayer.cpp \
    computerplayer.cpp \
    intelligence.cpp \
    playerintelligence.cpp \
    stringpropertyeditor.cpp \
    vector2dpropertyeditor.cpp \
    playerintelligencepropertyeditor.cpp \
    playerpropertyeditor.cpp \
    scriptedplayerintelligence.cpp \
    randomutil.cpp \
    scriptwindow.cpp \
    xmlscenarioserializer.cpp \
    multiplayerserver.cpp \
    multiplayerclient.cpp \
    multiplayerpacket.cpp \
    multiplayerserverwindow.cpp

FORMS += mainwindow.ui \
    scriptwindow.ui \
    multiplayerserverwindow.ui

OTHER_FILES += README.md \
    scripts/random_ai.js

Debug:DEFINES += DEBUG MULTIPLAYERSERVER_DEBUG MULTIPLAYERCLIENT_DEBUG
