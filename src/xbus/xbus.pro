load(common_pre)

QT       += core gui widgets dbus network

TEMPLATE = lib

CONFIG += shared plugin
CONFIG += blackmisc

INCLUDEPATH += $$EXTERNALSROOT/common/include/XPLM

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
    QMAKE_LFLAGS += -shared -rdynamic -nodefaultlibs -undefined_warning -Wl,--version-script=$$PWD/xbus.map
}

DEPENDPATH += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src

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
    CONFIG(debug, debug|release): LIBS += -lpngd -lzd -lopengl32 -lglu32
    CONFIG(release, debug|release): LIBS += -lpng -lz -lopengl32 -lglu32
}
else:win32: LIBS += -lpng -lz -lopengl32 -lglu32
else: LIBS += -lpng -lz

win32-msvc*: DEFINES += _CRT_SECURE_NO_WARNINGS

# Required by X-Plane SDK and libxplanemp
win32:DEFINES += IBM=1
linux:DEFINES += LIN=1
macx:DEFINES += APL=1
DEFINES += XPLM200=1
# XPLM210 is required for new features in libxplanemp,
# but means we lose support for X-Plane 9 and earlier versions.
# TODO add config option to select minimum X-Plane version when building
DEFINES += XPLM210=1

# Name will be used in libxplanemp log messages
DEFINES += XPMP_CLIENT_NAME=\\\"xbus\\\"
DEFINES += XPMP_CLIENT_LONGNAME=\\\"xbus\\\"

# X-Plane plugins must follow a prescribed filename and directory structure.
TARGET_EXT = .xpl
win32:TARGET = win
linux:TARGET = lin
macx:TARGET = mac
macx {
    # a single dylib file contains both 32bit and 64bit binaries
    XBUS_DIR = xbus
    XBUS_DESTDIR = $$DestRoot/$$XBUS_DIR
} else {
    equals(WORD_SIZE,64) {
        XBUS_DIR = xbus/64
        DEFINES += WORD_SIZE_64
    }
    equals(WORD_SIZE,32) {
        XBUS_DIR = xbus
    }
    XBUS_DESTDIR = $$DestRoot/$$XBUS_DIR
}

# Default MSVC project name is $$TARGET, so use a better name
QMAKE_PROJECT_NAME = xbus

# QMake ignores TARGET_EXT on Unix
     macx: QMAKE_POST_LINK += mkdir -p $${XBUS_DESTDIR} && cp $$OUT_PWD/lib$${TARGET}.dylib $$XBUS_DESTDIR/$${TARGET}.xpl
else:unix: QMAKE_POST_LINK += mkdir -p $${XBUS_DESTDIR} && cp $$OUT_PWD/lib$${TARGET}.so    $$XBUS_DESTDIR/$${TARGET}.xpl
else:      DESTDIR = $$XBUS_DESTDIR

target.path = $$PREFIX/$$XBUS_DIR
target.files *= $$XBUS_DESTDIR/$${TARGET}.xpl
target.CONFIG += no_check_exist
INSTALLS += target

dep_target.path = $$PREFIX/$$XBUS_DIR
win32 {
    dep_target.files *= $$DestRoot/lib/blackmisc.dll
    dep_target.files *= $$[QT_INSTALL_BINS]/Qt5Core$${DLL_DEBUG_SUFFIX}.dll
    dep_target.files *= $$[QT_INSTALL_BINS]/Qt5Gui$${DLL_DEBUG_SUFFIX}.dll
    dep_target.files *= $$[QT_INSTALL_BINS]/Qt5Widgets$${DLL_DEBUG_SUFFIX}.dll
    dep_target.files *= $$[QT_INSTALL_BINS]/Qt5DBus$${DLL_DEBUG_SUFFIX}.dll
    dep_target.files *= $$[QT_INSTALL_BINS]/Qt5Network$${DLL_DEBUG_SUFFIX}.dll
    dep_target.files *= $$[QT_INSTALL_BINS]/Qt5Xml$${DLL_DEBUG_SUFFIX}.dll
    win32-g++: dep_target.files *= $$[QT_INSTALL_BINS]/libdbus-1-3.dll
    else: dep_target.files *= $$[QT_INSTALL_BINS]/dbus-1-3.dll
    dep_target.CONFIG += no_check_exist

    legacy_data_target.path = $$PREFIX/xbus
    legacy_data_target.files *= LegacyData
} else:macx: {
    dep_target.files *= $$PREFIX/lib/libblackmisc.0.dylib
    dep_target.extra += rsync -avz --exclude \'Headers*\' --exclude \'*debug*\' $$[QT_INSTALL_LIBS]/QtCore.framework/ $${PREFIX}/$$XBUS_DIR/QtCore.framework/ &&
    dep_target.extra += rsync -avz --exclude \'Headers*\' --exclude \'*debug*\' $$[QT_INSTALL_LIBS]/QtGui.framework/ $${PREFIX}/$$XBUS_DIR/QtGui.framework/ &&
    dep_target.extra += rsync -avz --exclude \'Headers*\' --exclude \'*debug*\' $$[QT_INSTALL_LIBS]/QtWidgets.framework/ $${PREFIX}/$$XBUS_DIR/QtWidgets.framework/ &&
    dep_target.extra += rsync -avz --exclude \'Headers*\' --exclude \'*debug*\' $$[QT_INSTALL_LIBS]/QtDBus.framework/ $${PREFIX}/$$XBUS_DIR/QtDBus.framework/ &&
    dep_target.extra += rsync -avz --exclude \'Headers*\' --exclude \'*debug*\' $$[QT_INSTALL_LIBS]/QtNetwork.framework/ $${PREFIX}/$$XBUS_DIR/QtNetwork.framework/ &&
    dep_target.extra += rsync -avz --exclude \'Headers*\' --exclude \'*debug*\' $$[QT_INSTALL_LIBS]/QtXml.framework/ $${PREFIX}/$$XBUS_DIR/QtXml.framework/
    dep_target.CONFIG += no_check_exist

    legacy_data_target.path = $$PREFIX/xbus
    legacy_data_target.files *= LegacyData

    # We rely on legacy_data_target to be called after dep_target. So the library should already be installed and can be modified in place.
    # We cannot modify the original library since this is xbus specific.
    legacy_data_target.depends += fix_plugin_rpath
    fix_plugin_rpath.target = fix_plugin_rpath
    fix_plugin_rpath.commands += install_name_tool -change \"@rpath/libblackmisc.0.dylib\" \"@loader_path/libblackmisc.0.dylib\" $$shell_path($$PREFIX/$$XBUS_DIR/mac.xpl) &&
    fix_plugin_rpath.commands += install_name_tool -change \"@rpath/QtWidgets.framework/Versions/5/QtWidgets\" \"@loader_path/QtWidgets.framework/Versions/5/QtWidgets\" $$shell_path($$PREFIX/$$XBUS_DIR/mac.xpl) &&
    fix_plugin_rpath.commands += install_name_tool -change \"@rpath/QtGui.framework/Versions/5/QtGui\" \"@loader_path/QtGui.framework/Versions/5/QtGui\" $$shell_path($$PREFIX/$$XBUS_DIR/mac.xpl) &&
    fix_plugin_rpath.commands += install_name_tool -change \"@rpath/QtDBus.framework/Versions/5/QtDBus\" \"@loader_path/QtDBus.framework/Versions/5/QtDBus\" $$shell_path($$PREFIX/$$XBUS_DIR/mac.xpl) &&
    fix_plugin_rpath.commands += install_name_tool -change \"@rpath/QtNetwork.framework/Versions/5/QtNetwork\" \"@loader_path/QtNetwork.framework/Versions/5/QtNetwork\" $$shell_path($$PREFIX/$$XBUS_DIR/mac.xpl) &&
    fix_plugin_rpath.commands += install_name_tool -change \"@rpath/QtCore.framework/Versions/5/QtCore\" \"@loader_path/QtCore.framework/Versions/5/QtCore\" $$shell_path($$PREFIX/$$XBUS_DIR/mac.xpl) &&
    fix_plugin_rpath.commands += install_name_tool -change \"@rpath/QtXml.framework/Versions/5/QtXml\" \"@loader_path/QtXml.framework/Versions/5/QtXml\" $$shell_path($$PREFIX/$$XBUS_DIR/libblackmisc.0.dylib)
    QMAKE_EXTRA_TARGETS += fix_plugin_rpath

    fix_plugin_rpath.depends += fix_misc_rpath
    fix_misc_rpath.target = fix_misc_rpath
    fix_misc_rpath.commands += install_name_tool -id \"@loader_path/libblackmisc.0.dylib\" $$shell_path($$PREFIX/$$XBUS_DIR/libblackmisc.0.dylib) &&
    fix_misc_rpath.commands += install_name_tool -change \"@rpath/QtGui.framework/Versions/5/QtGui\" \"@loader_path/QtGui.framework/Versions/5/QtGui\" $$shell_path($$PREFIX/$$XBUS_DIR/libblackmisc.0.dylib) &&
    fix_misc_rpath.commands += install_name_tool -change \"@rpath/QtDBus.framework/Versions/5/QtDBus\" \"@loader_path/QtDBus.framework/Versions/5/QtDBus\" $$shell_path($$PREFIX/$$XBUS_DIR/libblackmisc.0.dylib) &&
    fix_misc_rpath.commands += install_name_tool -change \"@rpath/QtNetwork.framework/Versions/5/QtNetwork\" \"@loader_path/QtNetwork.framework/Versions/5/QtNetwork\" $$shell_path($$PREFIX/$$XBUS_DIR/libblackmisc.0.dylib) &&
    fix_misc_rpath.commands += install_name_tool -change \"@rpath/QtCore.framework/Versions/5/QtCore\" \"@loader_path/QtCore.framework/Versions/5/QtCore\" $$shell_path($$PREFIX/$$XBUS_DIR/libblackmisc.0.dylib) &&
    fix_misc_rpath.commands += install_name_tool -change \"@rpath/QtXml.framework/Versions/5/QtXml\" \"@loader_path/QtXml.framework/Versions/5/QtXml\" $$shell_path($$PREFIX/$$XBUS_DIR/libblackmisc.0.dylib)
    QMAKE_EXTRA_TARGETS += fix_misc_rpath

} else:unix: {
    dep_target.files *= $$PREFIX/lib/libblackmisc.so*
    dep_target.files *= $$[QT_INSTALL_LIBS]/libQt5Core.so.5
    dep_target.files *= $$[QT_INSTALL_LIBS]/libQt5Gui.so.5
    dep_target.files *= $$[QT_INSTALL_LIBS]/libQt5Widgets.so.5
    dep_target.files *= $$[QT_INSTALL_LIBS]/libQt5DBus.so.5
    dep_target.files *= $$[QT_INSTALL_LIBS]/libQt5Network.so.5
    dep_target.files *= $$[QT_INSTALL_LIBS]/libQt5Xml.so.5
    dep_target.files *= $$[QT_INSTALL_LIBS]/libicui18n.so.56
    dep_target.files *= $$[QT_INSTALL_LIBS]/libicuuc.so.56
    dep_target.files *= $$[QT_INSTALL_LIBS]/libicudata.so.56
    dep_target.CONFIG += no_check_exist

    legacy_data_target.path = $$PREFIX/xbus
    legacy_data_target.files *= LegacyData
}

win32-g++ {
    dep_target.files *= $$[QT_INSTALL_BINS]/libgcc_s_dw2-1.dll
    dep_target.files *= $$[QT_INSTALL_BINS]/libwinpthread-1.dll

    # libstdc++-6.dll needs a workaround since copy does not accept a filepath with '+' in it
    dep_target.depends += copy_libstdc
    copy_libstdc.target = copy_libstdc
    source_path = $$[QT_INSTALL_BINS]/libstdc++-6.dll
    dest_path = $$PREFIX/$$XBUS_DIR
    copy_libstdc.commands = xcopy /Y $$shell_path($$source_path) $$shell_path($$dest_path)
    QMAKE_EXTRA_TARGETS += copy_libstdc
}

INSTALLS += dep_target
INSTALLS += legacy_data_target

load(common_post)
