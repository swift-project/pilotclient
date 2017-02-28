load(common_pre)

TARGET = blackconfig
TEMPLATE = lib
CONFIG += staticlib

buildconfig_gen.input = buildconfig_gen.cpp.in
buildconfig_gen.output = $$BuildRoot/buildconfig_gen.cpp
GENERATED_SOURCES += $$BuildRoot/buildconfig_gen.cpp
QMAKE_SUBSTITUTES += buildconfig_gen

INCLUDEPATH += ..

DEFINES += LOG_IN_FILE
HEADERS +=  *.h
SOURCES +=  *.cpp
DESTDIR = $$DestRoot/lib
OTHER_FILES += buildconfig_gen.cpp.in

load(common_post)
