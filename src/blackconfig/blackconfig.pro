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

win32: GIT_BIN = $$system($$(SYSTEMROOT)\system32\where git 2> nul)
else: GIT_BIN = $$system(which git 2> /dev/null)

isEmpty(GIT_BIN) {
    GIT_HEAD_SHA1="<unknown>"
    GIT_COMMIT_TS="0"
} else {
    GIT_HEAD_SHA1=$$system(git rev-parse --short HEAD)
    GIT_COMMIT_TS=$$system(git log -1 --date=format:'%Y%m%d%H%M' --pretty=format:%cd)
}

load(common_post)
