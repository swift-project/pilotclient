include (../../config.pri)
include (../../build.pri)

QT       += core gui widgets dbus network

TEMPLATE = lib

CONFIG += shared plugin
CONFIG += blackmisc blackcore

win32 {
    equals(WORD_SIZE,64): LIBS += -lXPLM_64
    equals(WORD_SIZE,32): LIBS += -lXPLM
}
else:macx {
    LIBS += -framework XPLM -framework Cocoa -framework CoreFoundation
    DEFINES += XUTILS_EXCLUDE_MAC_CRAP=1
}
else:unix {
    # Flags needed because there is no XPLM link library
    QMAKE_LFLAGS += -shared -rdynamic -nodefaultlibs -undefined_warning -Wl,--version-script=$$PWD/xbus.map
}

DEPENDPATH += . ../../src
INCLUDEPATH += . ../../src

SOURCES += *.cpp
HEADERS += *.h

# Using the $$files function so we can remove some with -= below
SOURCES += $$files(libxplanemp/src/*.cpp)
HEADERS += $$files(libxplanemp/src/*.h) $$files(libxplanemp/include/*.h)
INCLUDEPATH += ./libxplanemp ./libxplanemp/include ./libxplanemp/src

# CSLLoaderThread not used, see libxplanemp/README.markdown
SOURCES -= libxplanemp/src/CSLLoaderThread.cpp
HEADERS -= libxplanemp/src/CSLLoaderThread.h

# PlatformUtils also not used
SOURCES -= $$files(libxplanemp/src/PlatformUtils.*.cpp)
HEADERS -= libxplanemp/src/PlatformUtils.h

# Externals required by libxplanemp
CONFIG += opengl
win32-msvc* {
    CONFIG(debug, debug|release): LIBS += -lpngd -lzd
    CONFIG(release, debug|release): LIBS += -lpng -lz
}
else: LIBS += -lpng -lz

win32-msvc*: DEFINES += _CRT_SECURE_NO_WARNINGS


#win32:!win32-g++*: PRE_TARGETDEPS += ../../../../lib/blackmisc.lib
#win32:!win32-g++*: PRE_TARGETDEPS += ../../../../lib/blackcore.lib


# Required by X-Plane SDK and libxplanemp
win32:DEFINES += IBM=1
linux:DEFINES += LIN=1
macx:DEFINES += APL=1
DEFINES += XPLM200=1
# XPLM210 is required for new features in libxplanemp,
# but means we lose support for X-Plane 9 and earlier versions.
# TODO add config option to select minimum X-Plane version when building
DEFINES += XPLM210=1


# X-Plane plugins must follow a prescribed filename and directory structure.
TARGET_EXT = .xpl
win32:TARGET = win
linux:TARGET = lin
macx:TARGET = mac
macx {
    # a single dylib file contains both 32bit and 64bit binaries
    XBUS_DESTDIR = ../../xbus
}
else {
    equals(WORD_SIZE,64): XBUS_DESTDIR = ../../xbus/64
    equals(WORD_SIZE,32): XBUS_DESTDIR = ../../xbus
}

# QMake ignores TARGET_EXT on Unix
     macx: QMAKE_POST_LINK += mkdir -p $${XBUS_DESTDIR} && cp $$OUT_PWD/lib$${TARGET}.dylib $$XBUS_DESTDIR/$${TARGET}.xpl
else:unix: QMAKE_POST_LINK += mkdir -p $${XBUS_DESTDIR} && cp $$OUT_PWD/lib$${TARGET}.so    $$XBUS_DESTDIR/$${TARGET}.xpl
else:      DESTDIR = $$XBUS_DESTDIR

include (../../libraries.pri)

# TODO refactor .pri files into common_pre.pri and common_post.pri
# to sort out this include order fiasco
INCLUDEPATH += $$EXTERNALDIR/common/include/XPLM
