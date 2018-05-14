load(common_pre)

QT += core dbus network widgets

TARGET = simulatorfsxcommon
TEMPLATE = lib

CONFIG += staticlib
CONFIG += blackconfig blackmisc blackcore blackgui

DEPENDPATH += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src

equals(WORD_SIZE,64) {
    INCLUDEPATH *= $$EXTERNALSROOT/common/include/simconnect/P3D-v4.1
}
equals(WORD_SIZE,32) {
    INCLUDEPATH *= $$EXTERNALSROOT/common/include/simconnect/FSX-XPack
}

SOURCES += *.cpp
HEADERS += *.h
FORMS += *.ui

DESTDIR = $$DestRoot/lib

load(common_post)
