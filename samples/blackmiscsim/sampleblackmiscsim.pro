load(common_pre)

QT       += core dbus network

TARGET = sampleblackmiscsim
TEMPLATE = app

CONFIG   += console
CONFIG   += blackmisc blacksim blackcore
CONFIG  -= app_bundle

DEPENDPATH += . ../../src/blackmisc
DEPENDPATH += . ../../src/blackcore
INCLUDEPATH += . ../../src

DESTDIR = $$DestRoot/bin

HEADERS += *.h
SOURCES += *.cpp

target.path = $$PREFIX/bin
INSTALLS += target

load(common_post)
