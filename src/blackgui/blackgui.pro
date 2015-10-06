load(common_pre)

QT       += network dbus gui svg
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

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

HEADERS += $$PWD/views/*.h
SOURCES += $$PWD/views/*.cpp

HEADERS += $$PWD/components/*.h
SOURCES += $$PWD/components/*.cpp
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

win32:isEmpty(MINGW_IN_SHELL):  COPY = xcopy /yi
else:                           COPY = cp -r

win32 {
    QMAKE_POST_LINK += $$COPY $$shell_path($$PWD/qss)           \
                              $$shell_path($$DestRoot/bin/qss)
}
else {
    QMAKE_POST_LINK += mkdir -p $$shell_path($$DestRoot/bin) && \
                       $$COPY $$shell_path($$PWD/qss)           \
                              $$shell_path($$DestRoot/bin)
}

load(common_post)
