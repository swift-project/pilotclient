QT       += core gui xml svg network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TARGET = blackd
TEMPLATE = app

DEPENDPATH += . ..

INCLUDEPATH += ..

SOURCES  += *.cpp

HEADERS  += *.h

FORMS    += blackd.ui

RESOURCES += blackd.qrc

LIBS += -L../../lib -lblackmisc -lblackcore

win32:  PRE_TARGETDEPS += ../../lib/blackmisc.lib \
                          ../../lib/blackcore.lib
else:   PRE_TARGETDEPS += ../../lib/libblackmisc.a \
                          ../../lib/libblackcore.a

DESTDIR = ../../bin



