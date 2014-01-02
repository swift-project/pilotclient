QT       += core dbus

TARGET = sample_blackmisc_dbus
TEMPLATE = app

CONFIG   += console c++11
CONFIG   -= app_bundle

# Causes nmake to run qdbusxml2cpp to automatically generate the dbus adaptor and interface classes,
# then automatically adds them to the sources to compile
DBUS_ADAPTORS += BlackMiscTest.Testservice.xml BlackMiscTest.Datacontext.xml
DBUS_INTERFACES += BlackMiscTest.Testservice.xml BlackMiscTest.Datacontext.xml
QDBUSXML2CPP_INTERFACE_HEADER_FLAGS = -i blackmisc/blackmiscfreefunctions.h -i blackmisc/blackmiscallvalueclasses.h
QDBUSXML2CPP_ADAPTOR_HEADER_FLAGS = -i blackmisc/blackmiscfreefunctions.h -i blackmisc/blackmiscallvalueclasses.h

DEPENDPATH += . ../../src/blackmisc ../../src/blackcore
INCLUDEPATH += . ../../src

LIBS += -L../../lib -lblackcore -lblackmisc
win32:!win32-g++*: PRE_TARGETDEPS += ../../lib/blackmisc.lib \
                                     ../../lib/blackcore.lib
else:              PRE_TARGETDEPS += ../../lib/libblackmisc.a \
                                     ../../lib/libblackcore.a

DESTDIR = ../../bin

HEADERS += *.h
SOURCES += *.cpp

OTHER_FILES += readme.txt BlackMiscTest.Testservice.xml BlackMiscTest.Datacontext.xml
