QT       += core gui xml svg network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += . ..

TARGET = BlackBox
TEMPLATE = app

SOURCES += main.cpp\
        blackbox.cpp \
        dialog_chat.cpp \
        dialog_connect.cpp

HEADERS  += blackbox.h \
        dialog_chat.h \
        dialog_connect.h

FORMS    += blackbox.ui dialog_connect.ui dialog_chat.ui

win32-msvc* {
    PRE_TARGETDEPS  += ../../lib/blackmisc.lib
    LIBS            += ../../lib/blackmisc.lib
}

!win32-msvc* {
    PRE_TARGETDEPS  += ../../lib/libblackmisc.a
    LIBS            += ../../lib/libblackmisc.a
}

DESTDIR = ../../bin
