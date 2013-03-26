TARGET = bb_driver_xplane
TEMPLATE = lib
CONFIG += dll

INCLUDEPATH += .. ../../

DESTDIR = ../../../bin

DEPENDPATH += .

HEADERS += \
    driver_xplane.h

SOURCES += \
    driver_xplane.cpp




