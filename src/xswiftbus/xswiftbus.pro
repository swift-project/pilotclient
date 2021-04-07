load(common_pre)

TEMPLATE = lib

CONFIG += shared plugin
CONFIG -= qt

CONFIG += c++17

INCLUDEPATH += $$EXTERNALSROOT/common/include/XPLM

LIBS += -levent_core -ldbus-1

OTHER_FILES += \
    org.swift_project.xswiftbus.*.xml \
    xswiftbus.conf

win32 {
    equals(WORD_SIZE,64): LIBS += -lXPLM_64 -lXPWidgets_64
    equals(WORD_SIZE,32): LIBS += -lXPLM -lXPWidgets
}
else:macx {
    LIBS += -framework XPLM -framework XPWidgets \
            -framework Cocoa -framework CoreFoundation
    DEFINES += XUTILS_EXCLUDE_MAC_CRAP=1
}
else:unix {
    # Flags needed because there is no XPLM link library
    QMAKE_LFLAGS += -shared -rdynamic -nodefaultlibs -undefined_warning -Wl,--version-script=$$PWD/xswiftbus.map
}

DEPENDPATH += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src

SOURCES += $$files($$PWD/*.cpp)
HEADERS += $$files($$PWD/*.h)

# Using the $$files function so we can remove some with -= below
SOURCES += $$files(xplanemp2/src/*.cpp)
SOURCES += $$files(xplanemp2/src/*.c)
HEADERS += $$files(xplanemp2/src/*.h)

SOURCES += $$files(xplanemp2/src/obj8/*.cpp)
HEADERS += $$files(xplanemp2/src/obj8/*.h)

HEADERS += $$files(xplanemp2/include/*.h)
INCLUDEPATH += ./xplanemp2 ./xplanemp2/include ./xplanemp2/src ./xplanemp2/obj8

!macx: SOURCES -= xplanemp2/src/AplFSUtil.cpp

unix:!macx {
    INCLUDEPATH *= /usr/include/dbus-1.0
    exists (/usr/lib/x86_64-linux-gnu){
    INCLUDEPATH *= /usr/lib/x86_64-linux-gnu/dbus-1.0/include
    } else {
    INCLUDEPATH *= /usr/lib/dbus-1.0/include
    }
}

# PlatformUtils also not used
SOURCES -= $$files(xplanemp2/src/PlatformUtils.*.cpp)
HEADERS -= xplanemp2/src/PlatformUtils.h

# Externals required by xplanemp2
CONFIG += opengl
msvc {
    CONFIG(debug, debug|release): LIBS += -lpngd -lzd -lopengl32 -lglu32
    CONFIG(release, debug|release): LIBS += -lpng -lz -lopengl32 -lglu32
}
else:win32: LIBS += -lpng -lz -lopengl32 -lglu32
else: LIBS += -lpng -lz

msvc: DEFINES += _CRT_SECURE_NO_WARNINGS

!swiftConfig(allowNoisyWarnings) {
    gcc|llvm:QMAKE_CXXFLAGS_WARN_ON *= -Wno-missing-field-initializers
}

# Required by X-Plane SDK and xplanemp2
win32:DEFINES += IBM=1
linux:DEFINES += LIN=1
macx:DEFINES += APL=1
DEFINES += XPLM200=1
DEFINES += XPLM210=1
DEFINES += XPLM300=1
DEFINES += XPLM_DEPRECATED=1

# Name will be used in xplanemp2 log messages
DEFINES += XPMP_CLIENT_NAME=\\\"xswiftbus\\\"
DEFINES += XPMP_CLIENT_LONGNAME=\\\"xswiftbus\\\"

DEFINES += XSWIFTBUS_VERSION=\\\"$$swiftConfig(version.full).$$VER_REV\\\"

!qtc_run:isEmpty(XSWIFTBUS_COMMIT): error(Missing XSWIFTBUS_COMMIT variable)
DEFINES += XSWIFTBUS_COMMIT=\\\"$$XSWIFTBUS_COMMIT\\\"

# X-Plane plugins must follow a prescribed filename and directory structure.
TARGET_EXT = .xpl
win32:TARGET = win
linux:TARGET = lin
macx:TARGET = mac

equals(WORD_SIZE,64) {
    XSWIFTBUS_DIR = xswiftbus/64
    DEFINES += WORD_SIZE_64
}
equals(WORD_SIZE,32) {
    XSWIFTBUS_DIR = xswiftbus/32
}
XSWIFTBUS_DESTDIR = $$DestRoot/$$XSWIFTBUS_DIR

# Default MSVC project name is $$TARGET, so use a better name
QMAKE_PROJECT_NAME = xswiftbus

# QMake ignores TARGET_EXT on Unix
     macx: QMAKE_POST_LINK += mkdir -p $$XSWIFTBUS_DESTDIR && cp $$OUT_PWD/lib$${TARGET}.dylib $$XSWIFTBUS_DESTDIR/$${TARGET}.xpl
else:unix: QMAKE_POST_LINK += mkdir -p $$XSWIFTBUS_DESTDIR && cp $$OUT_PWD/lib$${TARGET}.so    $$XSWIFTBUS_DESTDIR/$${TARGET}.xpl
else:      DESTDIR = $$XSWIFTBUS_DESTDIR

target.path = $$PREFIX/$$XSWIFTBUS_DIR
target.files *= $$XSWIFTBUS_DESTDIR/$${TARGET}.xpl
target.CONFIG += no_check_exist
INSTALLS += target

dep_target.path = $$PREFIX/$$XSWIFTBUS_DIR
dep_target.CONFIG += no_check_exist
win32 {
    dep_target.files *= $$DestRoot/bin/dbus-daemon.exe
    win32-g++ {
        dep_target.files *= $$DestRoot/bin/libdbus-1-3.dll
        dep_target.files *= $$DestRoot/bin/libevent_core.dll
    }
    else {
        dep_target.files *= $$DestRoot/bin/dbus-1-3.dll
        dep_target.files *= $$DestRoot/bin/expat.dll
        dep_target.files *= $$DestRoot/bin/event_core.dll
    }

    dbus_share.path = $$PREFIX/$$XSWIFTBUS_DIR/share/dbus-1
    dbus_share.files = $$DestRoot/share/dbus-1/session.conf
    dbus_share.CONFIG = no_check_exist
    INSTALLS += dbus_share

    legacy_data_target.path = $$PREFIX/xswiftbus
    legacy_data_target.files *= LegacyData
} else:macx: {
    dep_target.files *= $$DestRoot/lib/libdbus-1.3.dylib
    dep_target.files *= $$DestRoot/lib/libevent_core.2.1.8.dylib

    legacy_data_target.path = $$PREFIX/xswiftbus
    legacy_data_target.files *= LegacyData

    # We rely on legacy_data_target to be called after dep_target. So the library should already be installed and can be modified in place.
    # We cannot modify the original library since this is xswiftbus specific.
    legacy_data_target.depends += fix_plugin_rpath
    fix_plugin_rpath.target = fix_plugin_rpath
    fix_plugin_rpath.commands += install_name_tool -change \"@rpath/libevent_core.2.1.8.dylib\" \"@loader_path/libevent_core.2.1.8.dylib\" $$shell_path($$PREFIX/$$XSWIFTBUS_DIR/mac.xpl) &&
    fix_plugin_rpath.commands += install_name_tool -change \"@rpath/libdbus-1.3.dylib\" \"@loader_path/libdbus-1.3.dylib\" $$shell_path($$PREFIX/$$XSWIFTBUS_DIR/mac.xpl)
    QMAKE_EXTRA_TARGETS += fix_plugin_rpath

} else:unix: {
    dep_target.files *= $$DestRoot/lib/libevent_core-*.so.*
    dep_target.files *= $$DestRoot/lib/libpng*.so.*

    legacy_data_target.path = $$PREFIX/xswiftbus
    legacy_data_target.files *= LegacyData
}

win32-g++ {
    equals(WORD_SIZE,32) {
        dep_target.files *= $$[QT_INSTALL_BINS]/libgcc_s_dw2-1.dll
    }
    equals(WORD_SIZE,64) {
        dep_target.files *= $$[QT_INSTALL_BINS]/libgcc_s_seh-1.dll
    }
    dep_target.files *= $$[QT_INSTALL_BINS]/libwinpthread-1.dll
    dep_target.files *= $$[QT_INSTALL_BINS]/libstdc++-6.dll
}

conf_target.path = $$PREFIX/xswiftbus
conf_target.files *= xswiftbus.conf


INSTALLS += dep_target
INSTALLS += legacy_data_target
INSTALLS += conf_target

load(common_post)
