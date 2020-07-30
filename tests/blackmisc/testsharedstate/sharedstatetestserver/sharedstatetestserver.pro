load(common_pre)

QT += core dbus network

TARGET = sharedstatetestserver
CONFIG   -= app_bundle
CONFIG   += console
CONFIG   += blackconfig
CONFIG   += blackmisc

TEMPLATE = app

DEPENDPATH += \
    . \
    $$SourceRoot/src \
    $$SourceRoot/tests \

INCLUDEPATH += \
    $$SourceRoot/src \
    $$SourceRoot/tests \

SOURCES += server.cpp
HEADERS += ../testsharedstate.h

DESTDIR = $$DestRoot/bin

load(common_post)
