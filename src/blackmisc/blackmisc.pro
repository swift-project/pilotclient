# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: CC0-1.0

load(common_pre)

QT       += network dbus xml multimedia

TARGET = blackmisc
TEMPLATE = lib
CONFIG += blackconfig precompile_header

swiftConfig(static) {
    CONFIG += staticlib
}

INCLUDEPATH += ..
# DEPENDPATH += . .. // BlackMisc should be independent

PRECOMPILED_HEADER = pch/pch.h
INCLUDEPATH += pch

DEFINES += LOG_IN_FILE BUILD_BLACKMISC_LIB
RESOURCES += blackmisc.qrc

# lupdate (from cmd, e.g. lupdate blackmisc.pro)
CODECFORTR = UTF-8
TRANSLATIONS += translations/blackmisc_i18n_de.ts \
                translations/blackmisc_i18n_fr.ts \
                translations/blackmisc_i18n_en.ts

HEADERS +=  *.h \
            $$files($$PWD/audio/*.h) \
#            $$files($$PWD/audio/settings/*.h) \
            $$files($$PWD/aviation/*.h) \
            $$files($$PWD/db/*.h) \
            $$files($$PWD/geo/*.h) \
            $$files($$PWD/input/*.h) \
            $$files($$PWD/math/*.h) \
            $$files($$PWD/mixin/*.h) \
            $$files($$PWD/network/*.h) \
            $$files($$PWD/network/data/*.h) \
            $$files($$PWD/network/settings/*.h) \
            $$files($$PWD/network/external/*.h) \
            $$files($$PWD/pq/*.h) \
            $$files($$PWD/sharedstate/*.h) \
            $$files($$PWD/sharedstate/dbus/*.h) \
            $$files($$PWD/simulation/*.h) \
            $$files($$PWD/simulation/data/*.h) \
            $$files($$PWD/simulation/settings/*.h) \
            $$files($$PWD/simulation/flightgear/*.h) \
            $$files($$PWD/simulation/fscommon/*.h) \
            $$files($$PWD/simulation/fsx/*.h) \
            $$files($$PWD/simulation/xplane/*.h) \
            $$files($$PWD/thirdparty/*.h) \
            $$files($$PWD/test/*.h) \
            $$files($$PWD/weather/*.h) \

SOURCES +=  *.cpp \
            $$files($$PWD/audio/*.cpp) \
#           $$files($$PWD/audio/settings/*.cpp) \
            $$files($$PWD/aviation/*.cpp) \
            $$files($$PWD/db/*.cpp) \
            $$files($$PWD/geo/*.cpp) \
            $$files($$PWD/input/*.cpp) \
            $$files($$PWD/math/*.cpp) \
            $$files($$PWD/mixin/*.cpp) \
            $$files($$PWD/network/*.cpp) \
            $$files($$PWD/network/settings/*.cpp) \
            $$files($$PWD/network/external/*.cpp) \
            $$files($$PWD/pq/*.cpp) \
            $$files($$PWD/sharedstate/*.cpp) \
            $$files($$PWD/sharedstate/dbus/*.cpp) \
            $$files($$PWD/simulation/*.cpp) \
            $$files($$PWD/simulation/data/*.cpp) \
            $$files($$PWD/simulation/settings/*.cpp) \
            $$files($$PWD/simulation/flightgear/*.cpp) \
            $$files($$PWD/simulation/fscommon/*.cpp) \
            $$files($$PWD/simulation/fsx/*.cpp) \
            $$files($$PWD/simulation/xplane/*.cpp) \
            $$files($$PWD/thirdparty/*.cpp) \
            $$files($$PWD/test/*.cpp) \
            $$files($$PWD/weather/*.cpp) \

macx {
    HEADERS += $$files($$PWD/macos/microphoneaccess.h)
    OBJECTIVE_SOURCES += $$files($$PWD/macos/microphoneaccess.mm)
}

INCLUDEPATH *= $$EXTERNALSROOT/common/include/crashpad
INCLUDEPATH *= $$EXTERNALSROOT/common/include/crashpad/mini_chromium

win32 {
    LIBS *= -lShell32 -lDbghelp -lversion
    # Remove the one below once the Reg functions are removed again from CIdentifier
    LIBS *= -lAdvapi32
}
win32-g++ {
    LIBS *= -lpsapi
}

msvc {
    CONFIG(debug, debug|release): LIBS *= -lclientd -lutild -lbased -lRpcrt4 -lAdvapi32
    CONFIG(release, debug|release): LIBS *= -lclient -lutil -lbase -lRpcrt4 -lAdvapi32
}
macx: LIBS += -lclient -lutil -lbase -lbsm -framework AVFoundation -framework Security -framework CoreFoundation -framework ApplicationServices -framework Foundation
unix:!macx: LIBS *= -lclient -lutil -lbase

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
    copy_command = rsync -avzl
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
    copy_command = rsync -avzl
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
