load(common_pre)

SOURCES += *.cpp
HEADERS += *.h

REQUIRES += contains(BLACK_CONFIG,P3D)
TARGET = simulatorp3dconfig
DISTFILES += simulatorp3d.json
include(../fsxp3d.pri)

load(common_post)
