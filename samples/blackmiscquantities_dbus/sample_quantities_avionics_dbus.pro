QT       += core dbus
QT       -= gui

TARGET = sample_quantities_avionics_dbus
TEMPLATE = app

CONFIG   += console
CONFIG   -= app_bundle

# Causes nmake to run qdbusxml2cpp to automatically generate the dbus adaptor and interface classes,
# then automatically adds them to the sources to compile
DBUS_ADAPTORS += BlackMiscTest.Testservice.xml
DBUS_INTERFACES += BlackMiscTest.Testservice.xml
QDBUSXML2CPP_INTERFACE_HEADER_FLAGS = -i blackmisc/blackmiscfreefunctions.h
QDBUSXML2CPP_ADAPTOR_HEADER_FLAGS = -i blackmisc/blackmiscfreefunctions.h

DEPENDPATH += . ../../src/blackmisc
INCLUDEPATH += . ../../src

LIBS += -L../../lib -lblackmisc

win32:!win32-g++*: PRE_TARGETDEPS += ../../lib/blackmisc.lib
else:              PRE_TARGETDEPS += ../../lib/libblackmisc.a

DESTDIR = ../../bin

HEADERS += *.h
SOURCES += *.cpp

OTHER_FILES += readme.txt BlackMiscTest.Testservice.xml
