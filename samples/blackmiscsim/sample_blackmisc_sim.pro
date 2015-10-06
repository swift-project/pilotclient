load(common_pre)

QT       += core dbus network xml

TARGET = sample_blacksim
TEMPLATE = app

CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += blackmisc blacksim

DEPENDPATH += . ../../src/blackmisc
INCLUDEPATH += . ../../src

DESTDIR = $$DestRoot/bin

HEADERS += *.h
SOURCES += *.cpp

load(common_post)
