load(common_pre)

QT       += network dbus xml

TARGET = blackmisc
TEMPLATE = lib
CONFIG += blackconfig precompile_header

swiftConfig(static) {
    CONFIG += staticlib
}

INCLUDEPATH += ..
# DEPENDPATH += . .. // BlackMisc should be independent

PRECOMPILED_HEADER = pch/pch.h

DEFINES += LOG_IN_FILE BUILD_BLACKMISC_LIB
RESOURCES += blackmisc.qrc

# lupdate (from cmd, e.g. lupdate blackmisc.pro)
CODECFORTR = UTF-8
TRANSLATIONS += translations/blackmisc_i18n_de.ts \
                translations/blackmisc_i18n_fr.ts \
                translations/blackmisc_i18n_en.ts

HEADERS +=  *.h \
            $$PWD/audio/*.h \
            $$PWD/audio/settings/*.h \
            $$PWD/aviation/*.h \
            $$PWD/db/*.h \
            $$PWD/geo/*.h \
            $$PWD/input/*.h \
            $$PWD/math/*.h \
            $$PWD/network/*.h \
            $$PWD/network/data/*.h \
            $$PWD/network/settings/*.h \
            $$PWD/pq/*.h \
            $$PWD/simulation/*.h \
            $$PWD/simulation/data/*.h \
            $$PWD/simulation/settings/*.h \
			$$PWD/simulation/flightgear/*.h \
            $$PWD/simulation/fscommon/*.h \
            $$PWD/simulation/fsx/*.h \
            $$PWD/simulation/xplane/*.h \
            $$PWD/thirdparty/*.h \
            $$PWD/test/*.h \
            $$PWD/weather/*.h

SOURCES +=  *.cpp \
            $$PWD/audio/*.cpp \
#           $$PWD/audio/settings/*.cpp \
            $$PWD/aviation/*.cpp \
            $$PWD/db/*.cpp \
            $$PWD/geo/*.cpp \
            $$PWD/input/*.cpp \
            $$PWD/math/*.cpp \
            $$PWD/network/*.cpp \
            $$PWD/network/settings/*.cpp \
            $$PWD/pq/*.cpp \
            $$PWD/simulation/*.cpp \
            $$PWD/simulation/data/*.cpp \
            $$PWD/simulation/settings/*.cpp \
			$$PWD/simulation/flightgear/*.cpp \
            $$PWD/simulation/fscommon/*.cpp \
            $$PWD/simulation/fsx/*.cpp \
            $$PWD/simulation/xplane/*.cpp \
            $$PWD/thirdparty/*.cpp \
            $$PWD/test/*.cpp \
            $$PWD/weather/*.cpp

win32 {
    LIBS *= -lShell32 -lDbghelp -lversion
    # Remove the one below once the Reg functions are removed again from CIdentifier
    LIBS *= -lAdvapi32
}
win32-g++ {
    LIBS *= -lpsapi
}

DESTDIR = $$DestRoot/lib
DLLDESTDIR = $$DestRoot/bin

OTHER_FILES += $$TRANSLATIONS readme.txt share/dbus-1/session.conf.in

win32 {
    DBUS_SESSION_BUS_LISTEN_ADDRESS = "autolaunch:"
    DBUS_SESSION_CONF_MAYBE_AUTH_EXTERNAL = "<!--<auth>EXTERNAL</auth>-->"
}

macx {
    DBUS_SESSION_BUS_LISTEN_ADDRESS = "launchd:env=DBUS_LAUNCHD_SESSION_BUS_SOCKET"
    DBUS_SESSION_CONF_MAYBE_AUTH_EXTERNAL = "<auth>EXTERNAL</auth>"
}

win32|macx {
    dbus_session_conf.input = share/dbus-1/session.conf.in
    dbus_session_conf.output = $$DestRoot/share/dbus-1/session.conf
    QMAKE_SUBSTITUTES += dbus_session_conf
}

# Copy externals to build directory

win32 {
    !isEmpty(QMAKE_POST_LINK): QMAKE_POST_LINK += &&
    # Kill all dbus sessions before trying to copy. Otherwise copy fails if the executable is in use
    QMAKE_POST_LINK += taskkill /f /fi \"Imagename eq dbus-daemon.exe\" > nul

    copy_command = xcopy /Y
    source_path = $$EXTERNALS_BIN_DIR/*.exe
    dest_path = $$DestRoot/bin
}
else:macx {
    copy_command = cp
    source_path = $$EXTERNALS_BIN_DIR/*
    dest_path = $$DestRoot/bin
}
else:unix {
    copy_command = cp
    source_path = $$EXTERNALS_BIN_DIR/*
    dest_path = $$DestRoot/bin
}
!isEmpty(QMAKE_POST_LINK): QMAKE_POST_LINK += &&
QMAKE_POST_LINK += $$copy_command $$shell_path($$source_path) $$shell_path($$dest_path)

win32 {
    copy_command = xcopy /Y
    source_path = $$EXTERNALS_LIB_DIR/*.dll
    dest_path = $$DestRoot/bin
}
else:macx {
    copy_command = cp -a
    source_path = $$EXTERNALS_LIB_DIR/*.{dylib,framework}
    dest_path = $$DestRoot/lib
}
else:unix {
    copy_command = cp
    source_path = $$EXTERNALS_LIB_DIR/*.so*
    dest_path = $$DestRoot/lib
}
!isEmpty(QMAKE_POST_LINK): QMAKE_POST_LINK += &&
QMAKE_POST_LINK += $$copy_command $$shell_path($$source_path) $$shell_path($$dest_path)

win32 {
    dlltarget.path = $$PREFIX/bin
    INSTALLS += dlltarget
} else {
    target.path = $$PREFIX/lib
    INSTALLS += target
}

package_dbus_conf.path = $$PREFIX/share/dbus-1
package_dbus_conf.files += $$DestRoot/share/dbus-1/*.conf
INSTALLS += package_dbus_conf

load(common_post)
