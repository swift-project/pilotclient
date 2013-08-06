#-------------------------------------------------
#
# Project created by QtCreator 2013-07-06T22:37:57
#
#-------------------------------------------------

QT       += core dbus

QT       -= gui

TARGET = DBusServer
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

DBUS_ADAPTORS += dbus/aircraftmanager.xml dbus/atcmanager.xml dbus/fsdclient.xml

DEPENDPATH += . ../../src
INCLUDEPATH += . ../../src

HEADERS += *.h
SOURCES += *.cpp

QDBUSXML2CPP_INTERFACE_HEADER_FLAGS = -i remote_aircraft.h
QDBUSXML2CPP_ADAPTOR_HEADER_FLAGS = -i remote_aircraft.h

LIBS += -L../../lib -lblackcore -lblackmisc

win32:!win32-g++*: PRE_TARGETDEPS += ../../lib/blackmisc.lib \
                                     ../../lib/blackcore.lib
else:              PRE_TARGETDEPS += ../../lib/libblackmisc.a \
                                     ../../lib/libblackcore.a

DESTDIR = ../../bin
