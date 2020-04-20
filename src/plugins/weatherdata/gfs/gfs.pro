load(common_pre)

QT       += core network dbus

TARGET = weatherdatagfs
TEMPLATE = lib

CONFIG += plugin shared
CONFIG += blackmisc blackcore blackconfig

DEPENDPATH += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src

include (g2clib/g2clib.pri)
SOURCES += *.cpp
HEADERS += *.h
DISTFILES += weatherdatagfs.json

DESTDIR = $$DestRoot/bin/plugins/weatherdata

win32 {
    dlltarget.path = $$PREFIX/bin/plugins/weatherdata
    INSTALLS += dlltarget
} else {
    target.path = $$PREFIX/bin/plugins/weatherdata
    INSTALLS += target
}

load(common_post)
