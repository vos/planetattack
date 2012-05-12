#-------------------------------------------------
#
# Project created by QtCreator 2012-04-27T10:59:14
#
#-------------------------------------------------

QT += core gui opengl script scripttools

TARGET = PlanetAttack
TEMPLATE = app


HEADERS += \
    mainwindow.h \
    canvas.h \
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
    randomutil.h

SOURCES += main.cpp \
    mainwindow.cpp \
    canvas.cpp \
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
    randomutil.cpp

FORMS += mainwindow.ui

OTHER_FILES += README.md
