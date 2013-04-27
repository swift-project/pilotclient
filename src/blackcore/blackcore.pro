# GUI is required for the matrix classes
QT       += network

TARGET = blackcore
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += ..
DEPENDPATH += . ..

linux-g++* {
    QMAKE_CXXFLAGS += -std=c++0x
}

#PRECOMPILED_HEADER = stdpch.h

precompile_header:!isEmpty(PRECOMPILED_HEADER) {
    DEFINES += USING_PCH
}

DEFINES += LOG_IN_FILE

HEADERS += *.h
SOURCES += *.cpp

DESTDIR = ../../lib
