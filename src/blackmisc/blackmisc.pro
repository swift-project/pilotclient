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
DEFINES += Q_OS_LINUX

HEADERS += \
    logmessage.h \
    log.h \
    display.h \
    debug.h \
    context.h \
    config.h \
    config_manager.h \
    serialize.h \
    com_client.h \
    com_server.h \
    com_client_buffer.h \
    message.h \
    com_handler.h \
    message_factory.h \
    message_handler.h \
    type_info.h \
    message_dispatcher.h \
    message_system.h \
    gui_messages.h

SOURCES += \
    logmessage.cpp \
    log.cpp \
    display.cpp \
    debug.cpp \
    context.cpp \
    config.cpp \
    config_manager.cpp \
    serialize.cpp \
    com_client.cpp \
    com_server.cpp \
    com_client_buffer.cpp \
    message.cpp \
    com_handler.cpp \
    message_factory.cpp \
    message_handler.cpp \
    type_info.cpp \
    message_dispatcher.cpp \
    message_system.cpp

DESTDIR = ../../lib




