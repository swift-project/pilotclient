QT       += core dbus

TARGET = sample_quantities_avionics_xmlplugin
TEMPLATE = lib

CONFIG += plugin

DEPENDPATH += . ../../src/blackmisc
INCLUDEPATH += . ../../src

LIBS += -L../../lib -lblackmisc \
        ../blackmiscquantities_dbus/release/dummy.obj \
        ../blackmiscquantities_dbus/release/moc_dummy.obj \
        ../blackmiscquantities_dbus/release/dummynoq.obj

win32:!win32-g++*:  PRE_TARGETDEPS +=   ../../lib/blackmisc.lib \
                                        ../../samples/blackmiscquantities_dbus/release/dummy.obj
else:  PRE_TARGETDEPS +=    ../../lib/libblackmisc.a \
                            ../../samples/blackmiscquantities_dbus/release/dummy.obj

DESTDIR = ../../bin

HEADERS += *.h
SOURCES += *.cpp

