load(common_pre)

QT += core dbus network testlib

TARGET = testnetwork
CONFIG   -= app_bundle
CONFIG   += blackconfig
CONFIG   += blackmisc
CONFIG   += blackcore
CONFIG   += testcase
CONFIG   += no_testcase_installs

TEMPLATE = app

DEPENDPATH += \
    . \
    $$SourceRoot/src \
    $$SourceRoot/tests \

INCLUDEPATH += \
    $$SourceRoot/src \
    $$SourceRoot/tests \

HEADERS += \
    expect.h

SOURCES += \
    testnetwork.cpp \
    expect.cpp

DESTDIR = $$DestRoot/bin

load(common_post)
