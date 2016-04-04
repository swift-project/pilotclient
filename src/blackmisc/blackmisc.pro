load(common_pre)

QT       += network dbus xml

TARGET = blackmisc
TEMPLATE = lib
CONFIG += c++11

contains(BLACK_CONFIG, Static) {
    CONFIG += staticlib
}

buildconfig.input = buildconfig.cpp.in
CONFIG(debug, debug|release) {
    buildconfig.output = $$OUT_PWD/debug/buildconfig_gen.cpp
    GENERATED_SOURCES += $$OUT_PWD/debug/buildconfig_gen.cpp

} else {
    buildconfig.output = $$OUT_PWD/release/buildconfig_gen.cpp
    GENERATED_SOURCES += $$OUT_PWD/release/buildconfig_gen.cpp
}
QMAKE_SUBSTITUTES += buildconfig

INCLUDEPATH += ..
# DEPENDPATH += . .. // BlackMisc should be independent
# PRECOMPILED_HEADER = stdpch.h
precompile_header:!isEmpty(PRECOMPILED_HEADER) {
    DEFINES += USING_PCH
}

DEFINES += LOG_IN_FILE BUILD_BLACKMISC_LIB
RESOURCES += blackmisc.qrc

# lupdate (from cmd, e.g. lupdate blackmisc.pro)
CODECFORTR = UTF-8
TRANSLATIONS += translations/blackmisc_i18n_de.ts \
                translations/blackmisc_i18n_fr.ts \
                translations/blackmisc_i18n_en.ts

HEADERS +=  *.h \
            $$PWD/pq/*.h \
            $$PWD/aviation/*.h \
            $$PWD/math/*.h \
            $$PWD/network/*.h \
            $$PWD/geo/*.h \
            $$PWD/input/*.h \
            $$PWD/audio/*.h \
            $$PWD/audio/settings/*.h \
            $$PWD/simulation/*.h \
            $$PWD/simulation/fscommon/*.h \
            $$PWD/simulation/fsx/*.h \
            $$PWD/simulation/xplane/*.h \
            $$PWD/simulation/data/*.h \
            $$PWD/weather/*.h

SOURCES +=  *.cpp \
            $$PWD/pq/*.cpp \
            $$PWD/aviation/*.cpp \
            $$PWD/math/*.cpp \
            $$PWD/network/*.cpp \
            $$PWD/input/*.cpp \
            $$PWD/geo/*.cpp \
            $$PWD/audio/*.cpp \
            $$PWD/audio/settings/*.cpp \
            $$PWD/simulation/*.cpp \
            $$PWD/simulation/fscommon/*.cpp \
            $$PWD/simulation/fsx/*.cpp \
            $$PWD/simulation/xplane/*.cpp \
            $$PWD/simulation/data/*.cpp \
            $$PWD/weather/*.cpp

win32 {
    LIBS *= -lShell32 -lDbghelp
}

DESTDIR = $$DestRoot/lib
DLLDESTDIR = $$DestRoot/bin

OTHER_FILES += $$TRANSLATIONS readme.txt buildconfig.cpp.in

win32 {
    dlltarget.path = $$PREFIX/bin
    INSTALLS += dlltarget
} else {
    target.path = $$PREFIX/lib
    INSTALLS += target
}

load(common_post)
