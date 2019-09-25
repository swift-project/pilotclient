load(common_pre)

QT += core dbus network widgets quickwidgets multimedia

TARGET = swiftguistd
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
RC_FILE = swift.rc
DISTFILES += swift.rc
ICON = swift.icns

DESTDIR = $$DestRoot/bin

target.path = $$PREFIX/bin
INSTALLS += target

macx {
    QMAKE_TARGET_BUNDLE_PREFIX = "org.swift-project"
    QMAKE_INFO_PLIST = Info.plist

    # Modifies plugin path
    qtconf.path = $$PREFIX/bin/swiftguistd.app/Contents/Resources
    qtconf.files = qt.conf
    INSTALLS += qtconf
}

load(common_post)
