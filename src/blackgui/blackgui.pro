load(common_pre)

QT       += core dbus gui network svg widgets multimedia

TARGET = blackgui
TEMPLATE = lib
CONFIG += blackconfig blackmisc blackcore precompile_header

swiftConfig(static) {
    CONFIG += staticlib
}

INCLUDEPATH += ..

msvc {
    INCLUDEPATH *= $$EXTERNALSROOT/common/include/qwt
}
else:mingw {
    QMAKE_CXXFLAGS += -isystem $$EXTERNALSROOT/common/include/qwt
}
else:macx {
    INCLUDEPATH *= $$EXTERNALSROOT/common/include/qwt
}
else {
    QMAKE_CXXFLAGS += -idirafter $$EXTERNALSROOT/common/include/qwt
}

DEPENDPATH += . ..

PRECOMPILED_HEADER = pch/pch.h
INCLUDEPATH += pch

DEFINES += LOG_IN_FILE BUILD_BLACKGUI_LIB QWT_DLL

HEADERS += *.h
SOURCES += *.cpp
HEADERS += $$PWD/models/*.h
SOURCES += $$PWD/models/*.cpp
HEADERS += $$PWD/menus/*.h
SOURCES += $$PWD/menus/*.cpp

HEADERS += $$PWD/views/*.h
SOURCES += $$PWD/views/*.cpp
FORMS   += $$PWD/views/*.ui

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

HEADERS += $$PWD/graphs/*.h
SOURCES += $$PWD/graphs/*.cpp

FORMS   += $$PWD/*.ui

win32 {
    HEADERS += $$PWD/win/*.h
    SOURCES += $$PWD/win/*.cpp
}

win32 {
    CONFIG(debug, debug|release): LIBS *= -lqwtd
    CONFIG(release, debug|release): LIBS *= -lqwt
}
else:macx {
    macx: LIBS *= -framework qwt
}
else:unix {
    LIBS *= -lqwt
}

# RESOURCES +=

DESTDIR = $$DestRoot/lib
DLLDESTDIR = $$DestRoot/bin

OTHER_FILES += ./share/qss/*.qss ./share/qss/*.qss.in ./share/qss/*.css
COPY_FILES += $$PWD/share/qss/*.qss
COPY_FILES += $$PWD/share/qss/*.css

win32 {
    dlltarget.path = $$PREFIX/bin
    INSTALLS += dlltarget
} else {
    target.path = $$PREFIX/lib
    INSTALLS += target
}

package_utils.path = $$PREFIX/share/qss
package_utils.files += share/qss/*.qss
package_utils.files += share/qss/*.css
package_utils.files += share/qss/*.ini
INSTALLS += package_utils

load(common_post)
