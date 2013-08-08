#-------------------------------------------------
#
# Project created by QtCreator 2013-07-07T15:31:07
#
#-------------------------------------------------

QT       += core dbus

QT       -= gui

TARGET = DBusClient
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

DBUS_INTERFACES += dbus/aircraftmanager.xml dbus/atcmanager.xml dbus/fsdclient.xml

QDBUSXML2CPP_INTERFACE_HEADER_FLAGS = -i remote_aircraft.h
QDBUSXML2CPP_ADAPTOR_HEADER_FLAGS = -i remote_aircraft.h

DEPENDPATH += . ../../src
INCLUDEPATH += . ../../src

HEADERS += *.h
SOURCES += *.cpp
