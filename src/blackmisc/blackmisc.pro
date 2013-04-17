QT       += network

TARGET = blackmisc
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
    blackmisc.h \
    logmessage.h \
    log.h \
    display.h \
    debug.h \
    context.h \
    config.h \
    config_manager.h \
    serialize.h \
    com_client.h \
    com_handler.h \
    com_server.h \
    com_client_buffer.h \
    message.h \
    message_factory.h \
    message_handler.h \
    type_info.h \
    message_dispatcher.h \
    message_system.h \
    gui_messages.h \
    pqphysicalquantity.h \
    pqfrequency.h \
    pqbase.h \
    pqspeed.h \
    pqangle.h \
    pqmass.h \
    pqpressure.h \
    pqtemperature.h \
    pqconstants.h \
    pqunits.h \
    pqallquantities.h \
    pqlength.h \
    pqtime.h \
    avheading.h \
    avtrack.h \
    avaltitude.h \
    avverticalpositions.h \
    aviobase.h \
    aviomodulator.h \
    aviocomsystem.h \
    avionavsystem.h \
    aviotransponder.h \
    avioadfsystem.h \
    aviation.h \
    basestreamstringifier.h \
    mathvector3dbase.h \
    mathvector3d.h \
    mathmatrixbase.h \
    mathmatrix3x3.h \
    mathmatrix3x1.h \
    mathematics.h \
    geolatitude.h \
    geolongitude.h \
    coordinategeodetic.h \
    coordinateecef.h \
    coordinatened.h \
    geoearthangle.h

SOURCES += \
    logmessage.cpp \
    log.cpp \
    display.cpp \
    debug.cpp \
    com_handler.cpp \
    context.cpp \
    config.cpp \
    config_manager.cpp \
    com_client.cpp \
    com_server.cpp \
    com_client_buffer.cpp \
    message.cpp \
    message_factory.cpp \
    message_handler.cpp \
    type_info.cpp \
    message_dispatcher.cpp \
    message_system.cpp \
    pqphysicalquantity.cpp \
    pqbase.cpp \
    pqunits.cpp \
    avheading.cpp \
    avtrack.cpp \
    avaltitude.cpp \
    avverticalpositions.cpp \
    aviomodulator.cpp \
    aviotransponder.cpp \
    mathvector3dbase.cpp \
    mathmatrixbase.cpp \
    mathmatrix3x3.cpp \
    mathematics.cpp

DESTDIR = ../../lib
