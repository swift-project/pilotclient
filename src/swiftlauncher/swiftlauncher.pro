load(common_pre)

QT += core dbus network widgets

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

macx {
    # Modifies plugin path
    qtconf.path = $$PREFIX/bin/swiftlauncher.app/Contents/Resources
    qtconf.files = qt.conf
    INSTALLS += qtconf
}

load(common_post)
