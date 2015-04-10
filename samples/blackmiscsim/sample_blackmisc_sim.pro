include (../../config.pri)
include (../../build.pri)

QT       += core dbus network xml

TARGET = sample_blacksim
TEMPLATE = app

CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += blackmisc blacksim

DEPENDPATH += . ../../src/blackmisc
INCLUDEPATH += . ../../src

DESTDIR = ../../bin

HEADERS += *.h
SOURCES += *.cpp

include (../../libraries.pri)
