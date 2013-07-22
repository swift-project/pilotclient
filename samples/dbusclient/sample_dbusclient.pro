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

DBUS_INTERFACES += planemanager.xml atcmanager.xml fsdclient.xml


SOURCES += main.cpp \
    dbusclient.cpp

HEADERS += \
    dbusclient.h
