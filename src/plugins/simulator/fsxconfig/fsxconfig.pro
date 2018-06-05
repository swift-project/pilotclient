load(common_pre)

SOURCES += *.cpp
HEADERS += *.h

REQUIRES += contains(BLACK_CONFIG,FSX)
TARGET = simulatorfsxconfig
DISTFILES += simulatorfsx.json
include(../fsxp3d.pri)

load(common_post)
