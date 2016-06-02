load(common_pre)

QT       += network dbus gui svg widgets

TARGET = blackgui
TEMPLATE = lib
CONFIG += blackconfig blackmisc blackcore

contains(BLACK_CONFIG, Static) {
    CONFIG += staticlib
}

INCLUDEPATH += ..
DEPENDPATH += . ..

# PRECOMPILED_HEADER = stdpch.h
precompile_header:!isEmpty(PRECOMPILED_HEADER) {
    DEFINES += USING_PCH
}

DEFINES += LOG_IN_FILE BUILD_BLACKGUI_LIB

HEADERS += *.h
SOURCES += *.cpp
HEADERS += $$PWD/models/*.h
SOURCES += $$PWD/models/*.cpp
HEADERS += $$PWD/menus/*.h
SOURCES += $$PWD/menus/*.cpp

HEADERS += $$PWD/views/*.h
SOURCES += $$PWD/views/*.cpp

HEADERS += $$PWD/components/*.h
HEADERS += $$PWD/settings/*.h
SOURCES += $$PWD/components/*.cpp
SOURCES += $$PWD/settings/*.cpp
FORMS   += $$PWD/components/*.ui

HEADERS += $$PWD/filters/*.h
SOURCES += $$PWD/filters/*.cpp
FORMS   += $$PWD/filters/*.ui

HEADERS += $$PWD/editors/*.h
SOURCES += $$PWD/editors/*.cpp
FORMS   += $$PWD/editors/*.ui

FORMS   += $$PWD/*.ui

win32 {
    HEADERS += $$PWD/win/*.h
    SOURCES += $$PWD/win/*.cpp
}

RESOURCES +=

DESTDIR = $$DestRoot/lib
DLLDESTDIR = $$DestRoot/bin

OTHER_FILES += ./data/qss/*.qss ./data/qss/*.css ./data/qss/*.ini *.ico *.rc
COPY_FILES += $$PWD/data/qss/*

win32 {
    dlltarget.path = $$PREFIX/bin
    INSTALLS += dlltarget
} else {
    target.path = $$PREFIX/lib
    INSTALLS += target
}

package_utils.path = $$PREFIX/data/qss
package_utils.files += data/qss/*.qss
package_utils.files += data/qss/*.css
package_utils.files += data/qss/*.ini
INSTALLS += package_utils

load(common_post)
