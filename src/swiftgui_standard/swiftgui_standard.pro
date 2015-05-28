include ($$SourceRoot/config.pri)
include ($$SourceRoot/build.pri)

QT       += core dbus gui svg network xml multimedia
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = swiftgui_std
TEMPLATE = app

SOURCES += *.cpp
HEADERS += *.h
FORMS   += *.ui
CONFIG  += blackmisc blacksound blackinput blackcore blackgui

macx {
    CONFIG  += app_bundle

    deployment.files = ../blackgui/qss
    deployment.path = Contents/MacOS
    QMAKE_BUNDLE_DATA += deployment
}

DEPENDPATH += . $$SourceRoot/src/blackmisc \
                $$SourceRoot/src/blacksound \
                $$SourceRoot/src/blackcore \
                $$SourceRoot/src/blackinput

INCLUDEPATH += . $$SourceRoot/src
DESTDIR = $$BuildRoot/bin
OTHER_FILES += *.qss *.ico *.rc
RC_FILE = swift.rc
DISTFILES += swift.rc

include ($$SourceRoot/libraries.pri)
