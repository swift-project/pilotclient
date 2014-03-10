include (../../config.pri)
include (../../build.pri)

QT       += network dbus

TARGET = blackmisc
TEMPLATE = lib
CONFIG += staticlib c++11

INCLUDEPATH += ..
# DEPENDPATH += . .. // BlackMisc should be independent
# PRECOMPILED_HEADER = stdpch.h
precompile_header:!isEmpty(PRECOMPILED_HEADER) {
    DEFINES += USING_PCH
}

DEFINES += LOG_IN_FILE
RESOURCES += blackmisc.qrc

# lupdate (from cmd, e.g. lupdate blackmisc.pro)
CODECFORTR = UTF-8
TRANSLATIONS += translations/blackmisc_i18n_de.ts \
                translations/blackmisc_i18n_fr.ts \
                translations/blackmisc_i18n_en.ts

HEADERS += *.h
SOURCES += *.cpp
DESTDIR = ../../lib
OTHER_FILES += $$TRANSLATIONS readme.txt

include (../../libraries.pri)
