#-------------------------------------------------
#
# Project created by QtCreator 2012-04-27T10:59:14
#
#-------------------------------------------------

QT += core gui opengl

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
    defaultplayerintelligence.h \
    random.h \
    propertyeditor.h \
    stringpropertyeditor.h \
    vector2dpropertyeditor.h

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
    defaultplayerintelligence.cpp \
    random.cpp \
    stringpropertyeditor.cpp \
    vector2dpropertyeditor.cpp

FORMS += mainwindow.ui
