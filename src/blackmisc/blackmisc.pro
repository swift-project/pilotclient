QT       += network dbus

TARGET = blackmisc
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += ..
# DEPENDPATH += . .. // BlackMisc should be independent
# PRECOMPILED_HEADER = stdpch.h
precompile_header:!isEmpty(PRECOMPILED_HEADER) {
    DEFINES += USING_PCH
}

DEFINES += LOG_IN_FILE
RESOURCES += blackmisc.qrc

# lupdate
CODECFORTR = UTF-8
TRANSLATIONS += translations/blackmisc_i18n_de.ts \
                translations/blackmisc_i18n_fr.ts \
                translations/blackmisc_i18n_en.ts

HEADERS += *.h
SOURCES += *.cpp
DESTDIR = ../../lib
OTHER_FILES += $$TRANSLATIONS
