QT       += core gui xml svg network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = blackd
TEMPLATE = app

DEPENDPATH += . ..

INCLUDEPATH += ..

SOURCES += main.cpp\
        blackd.cpp \
    qt_displayer.cpp \

HEADERS  += blackd.h \
    qt_displayer.h \

FORMS    += blackd.ui

RESOURCES += \
    blackd.qrc

win32-msvc* {
    PRE_TARGETDEPS += ../../lib/blackmisc.lib \
                      ../../lib/blackcore.lib

    LIBS += ../../lib/blackmisc.lib \
            ../../lib/blackcore.lib
}

!win32-msvc* {
    PRE_TARGETDEPS += ../../lib/libblackmisc.a \
                      ../../lib/libblackcore.a

    LIBS += ../../lib/libblackmisc.a \
            ../../lib/libblackcore.a
}

DESTDIR = ../../bin



