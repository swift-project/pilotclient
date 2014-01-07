include (../../externals.pri)

# GUI is required for the matrix classes
# Network for host info etc.
QT       += network dbus xml

TARGET = blackcore
TEMPLATE = lib
CONFIG += staticlib c++11

INCLUDEPATH += ..
DEPENDPATH += . ..

# linux-g++* { QMAKE_CXXFLAGS += -std=c++0x }
# PRECOMPILED_HEADER = stdpch.h

precompile_header:!isEmpty(PRECOMPILED_HEADER) {
    DEFINES += USING_PCH
}

# Causes nmake to run qdbusxml2cpp to automatically generate the dbus adaptor and interface classes,
# then automatically adds them to the sources to compile
# !! Make sure the plugin is available as release build and known QT_PLUGIN_PATH
QDBUSXML2CPP_ADAPTOR_HEADER_FLAGS = -i blackmisc/blackmiscfreefunctions.h -i blackmisc/blackmiscallvalueclasses.h
DBUS_ADAPTORS += blackcore.contextnetwork.xml
DBUS_ADAPTORS += blackcore.contextsettings.xml

# QDBUSXML2CPP_INTERFACE_HEADER_FLAGS = -i blackmisc/blackmiscfreefunctions.h -i blackmisc/blackmiscallvalueclasses.h
# DBUS_INTERFACES += blackcore.contextnetwork.xml

DEFINES += LOG_IN_FILE

HEADERS += *.h
SOURCES += *.cpp

win32:!win32-g++*: PRE_TARGETDEPS += ../../lib/blackmisc.lib
else:              PRE_TARGETDEPS += ../../lib/libblackmisc.a

DESTDIR = ../../lib

OTHER_FILES += readme.txt *.xml
