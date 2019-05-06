load(common_pre)

QT += core dbus network widgets

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

OTHER_FILES += *.qss *.ico *.rc *.icns
RC_FILE = swiftcore.rc
DISTFILES += swiftcore.rc
ICON = swiftcore.icns

DESTDIR = $$DestRoot/bin

target.path = $$PREFIX/bin
INSTALLS += target

macx {
    QMAKE_TARGET_BUNDLE_PREFIX = "org.swift-project"
    QMAKE_INFO_PLIST = Info.plist

    # Modifies plugin path
    qtconf.path = $$PREFIX/bin/swiftcore.app/Contents/Resources
    qtconf.files = qt.conf
    INSTALLS += qtconf
}

load(common_post)
