QT       += network

TARGET = blackmisc
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += ..

DEPENDPATH += . ..

#PRECOMPILED_HEADER = stdpch.h

precompile_header:!isEmpty(PRECOMPILED_HEADER) {
 DEFINES += USING_PCH
 }

DEFINES += LOG_IN_FILE

HEADERS += *.h

SOURCES += *.cpp

DESTDIR = ../../lib




