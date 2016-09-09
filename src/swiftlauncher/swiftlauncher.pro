load(common_pre)

QT       += core dbus gui svg network xml multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = swiftlauncher
TEMPLATE = app

SOURCES += *.cpp
HEADERS += *.h
FORMS   += *.ui
CONFIG  += blackconfig blackmisc blacksound blackinput blackcore blackgui

DEPENDPATH += . $$SourceRoot/src/blackmisc \
                $$SourceRoot/src/blacksound \
                $$SourceRoot/src/blackcore \
                $$SourceRoot/src/blackgui \
                $$SourceRoot/src/blackinput

INCLUDEPATH += . $$SourceRoot/src

OTHER_FILES += *.qss *.ico *.rc *.icns
RESOURCES += swiftlauncher.qrc
RC_FILE = swiftlauncher.rc
DISTFILES += swiftlauncher.rc
ICON = swiftlauncher.icns

DESTDIR = $$DestRoot/bin

target.path = $$PREFIX/bin
INSTALLS += target

load(common_post)
