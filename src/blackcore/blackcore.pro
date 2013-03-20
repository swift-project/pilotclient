QT       += network

TARGET = blackcore
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += ..

DEPENDPATH += . ..

#PRECOMPILED_HEADER = stdpch.h

precompile_header:!isEmpty(PRECOMPILED_HEADER) {
 DEFINES += USING_PCH
 }

DEFINES += LOG_IN_FILE

HEADERS += \
    sim_callbacks.h \
    constants.h \
    fsd_protocol.h \
    ecef.h \
    fsd_client.h \
    fsd_messages.h \
    interpolator.h \
    mathematics.h \
    matrix_3d.h \
    multiplayer.h \
    ned.h \
    plane.h \
    pqdistance.h \
    pqphysicalquantity.h \
    pqfrequency.h \
    pqbase.h \
    pqspeed.h \
    pqangle.h \
    pqmass.h \
    pqpressure.h \
    pqtemperature.h \
    simulator.h \
    vector_3d.h \
    vector_geo.h

SOURCES += \
    ecef.cpp \
    fsd_client.cpp \
    fsd_messages.cpp \
    interpolator.cpp \
    mathematics.cpp \
    matrix_3d.cpp \
    multiplayer.cpp \
    ned.cpp \
    plane.cpp \
    pqdistance.cpp \
    pqphysicalquantity.cpp \
    pqfrequency.cpp \
    pqbase.cpp \
    pqspeed.cpp \
    pqangle.cpp \
    pqmass.cpp \
    pqpressure.cpp \
    pqtemperature.cpp \
    simulator.cpp \
    vector_3d.cpp \
    vector_geo.cpp \

DESTDIR = ../../lib


