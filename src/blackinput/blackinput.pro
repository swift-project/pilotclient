# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: CC0-1.0

load(common_pre)

QT += core dbus
macx: QT += widgets

TARGET = blackinput
TEMPLATE = lib
CONFIG   += blackmisc

swiftConfig(static) {
    CONFIG += staticlib
}

INCLUDEPATH += ..
DEPENDPATH += . ..

DEFINES += LOG_IN_FILE BUILD_BLACKINPUT_LIB

HEADERS += *.h
SOURCES += *.cpp

win32 {
    HEADERS += $$files($$PWD/win/*.h)
    SOURCES += $$files($$PWD/win/*.cpp)

    DEFINES += DIRECTINPUT_VERSION=0x0800

    LIBS *= -ldxguid -lole32 -ldinput8 -lUser32
}

unix:!macx {
    HEADERS += $$files($$PWD/linux/*.h)
    SOURCES += $$files($$PWD/linux/*.cpp)
}

macx {
    HEADERS += $$files($$PWD/macos/*.h)
    OBJECTIVE_SOURCES += $$files($$PWD/macos/*.mm)
    LIBS += -framework CoreFoundation -framework ApplicationServices -framework Foundation -framework AppKit
}

DESTDIR = $$DestRoot/lib
DLLDESTDIR = $$DestRoot/bin

OTHER_FILES +=
RESOURCES +=

win32 {
    dlltarget.path = $$PREFIX/bin
    INSTALLS += dlltarget
} else {
    target.path = $$PREFIX/lib
    INSTALLS += target
}

load(common_post)
