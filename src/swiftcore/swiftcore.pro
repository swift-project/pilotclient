load(common_pre)

REQUIRES += contains(BLACK_CONFIG,SwiftCore)

QT       += core dbus network xml multimedia gui svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = swiftcore
TEMPLATE = app

SOURCES += *.cpp
HEADERS += *.h
FORMS   += *.ui
CONFIG  += blackmisc blacksound blackinput blackcore blackgui

DEPENDPATH += . $$SourceRoot/src/blackmisc \
                $$SourceRoot/src/blacksound \
                $$SourceRoot/src/blackcore \
                $$SourceRoot/src/blackinput

INCLUDEPATH += . $$SourceRoot/src

OTHER_FILES += *.qss *.ico *.rc
RC_FILE = swiftcore.rc
DISTFILES += swiftcore.rc

DESTDIR = $$DestRoot/bin

target.path = $$PREFIX/bin
INSTALLS += target

load(common_post)
