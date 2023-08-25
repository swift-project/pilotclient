# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: CC0-1.0

load(common_pre)

# GUI is required for the matrix classes
# Network for host info etc.
QT       += network dbus xml multimedia qml

TARGET = blackcore
TEMPLATE = lib
CONFIG += blackconfig blackmisc blackinput blacksound precompile_header

swiftConfig(static) {
    CONFIG += staticlib
}

INCLUDEPATH += ..
DEPENDPATH += . ..

PRECOMPILED_HEADER = pch/pch.h
INCLUDEPATH += pch

DEFINES += LOG_IN_FILE BUILD_BLACKCORE_LIB

HEADERS += *.h
HEADERS += $$files($$PWD/application/*.h)
HEADERS += $$files($$PWD/audio/*.h)
HEADERS += $$files($$PWD/context/*.h)
HEADERS += $$files($$PWD/data/*.h)
HEADERS += $$files($$PWD/db/*.h)
HEADERS += $$files($$PWD/vatsim/*.h)
HEADERS += $$files($$PWD/fsd/*.h)
HEADERS += $$files($$PWD/afv/*.h)
HEADERS += $$files($$PWD/afv/audio/*.h)
HEADERS += $$files($$PWD/afv/clients/*.h)
HEADERS += $$files($$PWD/afv/crypto/*.h)
HEADERS += $$files($$PWD/afv/connection/*.h)
HEADERS += $$files($$PWD/afv/model/*.h)

SOURCES += *.cpp
SOURCES += $$files($$PWD/context/*.cpp)
SOURCES += $$files($$PWD/data/*.cpp)
SOURCES += $$files($$PWD/db/*.cpp)
SOURCES += $$files($$PWD/vatsim/*.cpp)
SOURCES += $$files($$PWD/fsd/*.cpp)
SOURCES += $$files($$PWD/afv/audio/*.cpp)
SOURCES += $$files($$PWD/afv/clients/*.cpp)
SOURCES += $$files($$PWD/afv/crypto/*.cpp)
SOURCES += $$files($$PWD/afv/connection/*.cpp)
SOURCES += $$files($$PWD/afv/model/*.cpp)

LIBS *= -lvatsimauth
LIBS *= -lsodium

DESTDIR = $$DestRoot/lib
DLLDESTDIR = $$DestRoot/bin

OTHER_FILES += readme.txt *.xml

win32 {
    dlltarget.path = $$PREFIX/bin
    INSTALLS += dlltarget
} else {
    target.path = $$PREFIX/lib
    INSTALLS += target
}

load(common_post)
