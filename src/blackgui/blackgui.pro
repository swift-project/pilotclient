include (../../config.pri)
include (../../build.pri)

QT       += network dbus gui svg
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = blackgui
TEMPLATE = lib
CONFIG += staticlib
CONFIG += blackmisc blackcore

INCLUDEPATH += ..
DEPENDPATH += . ..

# PRECOMPILED_HEADER = stdpch.h
precompile_header:!isEmpty(PRECOMPILED_HEADER) {
    DEFINES += USING_PCH
}

DEFINES += LOG_IN_FILE

win32:!win32-g++*: PRE_TARGETDEPS += ../../lib/blackmisc.lib ../../lib/blackcore.lib
else:              PRE_TARGETDEPS += ../../lib/libblackmisc.a ../../lib/libblackcore.a


HEADERS += *.h
SOURCES += *.cpp
HEADERS += $$PWD/models/*.h
SOURCES += $$PWD/models/*.cpp
HEADERS += $$PWD/views/*.h
SOURCES += $$PWD/views/*.cpp
HEADERS += $$PWD/components/*.h
SOURCES += $$PWD/components/*.cpp

FORMS   += $$PWD/components/*.ui
FORMS   += $$PWD/*.ui
FORMS   += $$PWD/views/*.ui


win32 {
    HEADERS += $$PWD/win/*.h
    SOURCES += $$PWD/win/*.cpp
}

RESOURCES +=

DESTDIR = ../../lib
OTHER_FILES += ./qss/*.qss ./qss/*.css ./qss/*.ini

win32:isEmpty(MINGW_IN_SHELL):  COPY = xcopy /yi
else:                           COPY = cp -r

win32 {
    QMAKE_POST_LINK += $$COPY $$shell_path($$PWD/qss) \
                          $$shell_path($$OUT_PWD/../../bin/qss)
}
else {
    QMAKE_POST_LINK += mkdir -p $$shell_path($$OUT_PWD/../../bin) && \
                          $$COPY $$shell_path($$PWD/qss) \
                          $$shell_path($$OUT_PWD/../../bin)
}

include (../../libraries.pri)
