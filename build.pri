###########################
# QMake options
###########################

CONFIG           += qt
CONFIG           += warn_on
CONFIG           += c++11

###########################
# Debug/Release
###########################

win32 {

}
else {
    VER_MAJ           = $${BLACK_VER_MAJ}
    VER_MIN           = $${BLACK_VER_MIN}
    VER_PAT           = $${BLACK_VER_PAT}
    VERSION           = $${BLACK_VERSION}
}

###########################
# Detect 32 or 64 bit
###########################

win32-msvc* {
    win32:contains(QMAKE_TARGET.arch, x86_64) {
        WORD_SIZE = 64
    }
    else {
        WORD_SIZE = 32
    }
}
win32-g++ {
    WIN_FIND = $$(SYSTEMROOT)\system32\find
    MINGW64 = $$system($$QMAKE_CXX -Q --help=target | $$WIN_FIND \"-m64\")
    contains(MINGW64,[enabled]) {
        WORD_SIZE = 64
    }
    else {
        WORD_SIZE = 32
    }
}
linux-g++ {
    GCC64 = $$system($$QMAKE_CXX -Q --help=target | grep m64)
    contains(GCC64,[enabled]) {
        WORD_SIZE = 64
    }
    else {
        WORD_SIZE = 32
    }
}
linux-g++-32 {
    WORD_SIZE = 32
}
linux-g++-64 {
    WORD_SIZE = 64
}
macx-clang {
    # TODO
    WORD_SIZE = 64
}

###########################
# Profile build
###########################
contains(BLACK_CONFIG, ProfileRelease) {
    win32-msvc* {
        QMAKE_CXXFLAGS_RELEASE *= /Zi
        QMAKE_LFLAGS_RELEASE *= /DEBUG /PROFILE /INCREMENTAL:NO /OPT:REF /OPT:ICF
    }
}

# Needed to workaround C1128 error
# TODO check whether this is still needed after CValueObject refactoring
win32-msvc*: equals(WORD_SIZE,64): QMAKE_CXXFLAGS *= /bigobj

###########################
# No incremental build
###########################
# win32-msvc*:QMAKE_LFLAGS_DEBUG *= /INCREMENTAL:NO

###########################
# No gigantic MinGW obj files
###########################
win32-g++: QMAKE_CXXFLAGS_DEBUG += -Og

###########################
# No FSX or FS9 for 64 bit
###########################

equals(WORD_SIZE,64): BLACK_CONFIG -= FSX FS9

###########################
# Suppress stupid warnings
###########################
win32-msvc*:DEFINES *= _SCL_SECURE_NO_WARNINGS
# win32-msvc*:QMAKE_CXXFLAGS *=
win32-msvc*:QMAKE_CXXFLAGS_WARN_ON *= /wd4351
# win32-msvc*:QMAKE_CXXFLAGS_WARN_ON *= /Wall /W3 /wd4640 /wd4619 /wd4350 /wd4351 /wd4946

################################
# Defines for conditional compilation
################################

contains(BLACK_CONFIG, BlackSound) { DEFINES += WITH_BLACKSOUND }
contains(BLACK_CONFIG, BlackInput) { DEFINES += WITH_BLACKINPUT }
contains(BLACK_CONFIG, BlackSim) { DEFINES += WITH_BLACKSIM }
contains(BLACK_CONFIG, BlackCore) { DEFINES += WITH_BLACKCORE }
contains(BLACK_CONFIG, BlackGui) { DEFINES += WITH_BLACKGUI }
contains(BLACK_CONFIG, FSX) { DEFINES += WITH_FSX }
contains(BLACK_CONFIG, FS9) { DEFINES += WITH_FS9 }
contains(BLACK_CONFIG, XPlane) { DEFINES += WITH_XPLANE }
