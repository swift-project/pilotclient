QT       += core
QT       -= gui

TARGET = sample_geo2ecef
TEMPLATE = app

CONFIG   += console
CONFIG   -= app_bundle

DEPENDPATH += . ../../src

INCLUDEPATH += . ../../src

SOURCES += *.cpp

LIBS    += -L../../lib -lblackmisc -lblackcore

win32:  PRE_TARGETDEPS += ../../lib/blackmisc.lib \
                          ../../lib/blackcore.lib
else:   PRE_TARGETDEPS += ../../lib/libblackmisc.a \
                          ../../lib/libblackcore.a

DESTDIR = ../../bin



