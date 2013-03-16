TARGET = bb_driver_fsx
TEMPLATE = lib
CONFIG += dll

INCLUDEPATH += .. ../../

DESTDIR = ../../../bin

DEPENDPATH += .

HEADERS += \
    driver_fsx.h

SOURCES += \
    driver_fsx.cpp




