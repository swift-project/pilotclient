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

llvm {
    QMAKE_CXXFLAGS_WARN_ON *= $$clangArg(-isystem$$system_path($$EXTERNALSROOT/common/include/qwt))
}

# needed for "ShellScalingApi.h" only
# win32 { LIBS *= -lSHCore }

DEPENDPATH += . ..

PRECOMPILED_HEADER = pch/pch.h
INCLUDEPATH += pch

DEFINES += LOG_IN_FILE BUILD_BLACKGUI_LIB QWT_DLL

HEADERS += *.h
SOURCES += *.cpp
HEADERS += $$files($$PWD/models/*.h)
SOURCES += $$files($$PWD/models/*.cpp)
HEADERS += $$files($$PWD/menus/*.h)
SOURCES += $$files($$PWD/menus/*.cpp)

HEADERS += $$files($$PWD/views/*.h)
SOURCES += $$files($$PWD/views/*.cpp)
FORMS   += $$files($$PWD/views/*.ui)

HEADERS += $$files($$PWD/components/*.h)
HEADERS += $$files($$PWD/settings/*.h)
SOURCES += $$files($$PWD/components/*.cpp)
SOURCES += $$files($$PWD/settings/*.cpp)
FORMS   += $$files($$PWD/components/*.ui)

HEADERS += $$files($$PWD/filters/*.h)
SOURCES += $$files($$PWD/filters/*.cpp)
FORMS   += $$files($$PWD/filters/*.ui)

HEADERS += $$files($$PWD/editors/*.h)
SOURCES += $$files($$PWD/editors/*.cpp)
FORMS   += $$files($$PWD/editors/*.ui)

HEADERS += $$files($$PWD/graphs/*.h)
SOURCES += $$files($$PWD/graphs/*.cpp)

FORMS   += $$files($$PWD/*.ui)

win32 {
    HEADERS += $$files($$PWD/win/*.h)
    SOURCES += $$files($$PWD/win/*.cpp)
}

win32 {
    msvc {
        CONFIG(debug, debug|release): LIBS *= -lqwtd
        CONFIG(release, debug|release): LIBS *= -lqwt
    } else {
        LIBS *= -lqwt
    }
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
COPY_FILES += $$files($$PWD/share/qss/*.qss)
COPY_FILES += $$files($$PWD/share/qss/*.css)

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
