QT       += core gui xml svg network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = blackd
TEMPLATE = app

DEPENDPATH += ../blackmisc ../blackcore

INCLUDEPATH += ..

SOURCES  += *.cpp
HEADERS  += *.h

FORMS    += blackd.ui
RESOURCES += blackd.qrc

win32-msvc* {
    PRE_TARGETDEPS +=  ../../lib/blackmisc.lib
    PRE_TARGETDEPS +=  ../../lib/blackcore.lib
    LIBS += ../../lib/blackmisc.lib
    LIBS += ../../lib/blackcore.lib
}

!win32-msvc* {
    PRE_TARGETDEPS += ../../lib/libblackmisc.a
    PRE_TARGETDEPS += ../../lib/libblackcore.a
    LIBS += ../../lib/libblackmisc.a
    LIBS += ../../lib/libblackcore.a
}

DESTDIR = ../../bin



