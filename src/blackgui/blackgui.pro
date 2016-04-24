load(common_pre)

QT       += network dbus gui svg widgets

TARGET = blackgui
TEMPLATE = lib
CONFIG += blackmisc blackcore

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
# HEADERS += $$PWD/components/data/*.h
SOURCES += $$PWD/components/*.cpp
# SOURCES += $$PWD/components/data/*.cpp
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

OTHER_FILES += ./qss/*.qss ./qss/*.css ./qss/*.ini *.ico *.rc
COPY_FILES += $$PWD/qss/*

win32 {
    dlltarget.path = $$PREFIX/bin
    INSTALLS += dlltarget
} else {
    target.path = $$PREFIX/lib
    INSTALLS += target
}

package_utils.path = $$PREFIX/qss
package_utils.files += qss/*.qss
package_utils.files += qss/*.css
package_utils.files += qss/*.ini
INSTALLS += package_utils

load(common_post)
