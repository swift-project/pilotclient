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
# Detect 32 or 64 bits
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
    contains(MINGW64,[enabled]) {
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
# No FSX for 64 bits
###########################

equals(WORD_SIZE,64): BLACK_CONFIG -= FSX

################################
# Defines for conditional compilation
################################

contains(BLACK_CONFIG, BlackSound) { DEFINES += WITH_BLACKSOUND }
contains(BLACK_CONFIG, BlackInput) { DEFINES += WITH_BLACKINPUT }
contains(BLACK_CONFIG, BlackSim) { DEFINES += WITH_BLACKSIM }
contains(BLACK_CONFIG, BlackCore) { DEFINES += WITH_BLACKCORE }
contains(BLACK_CONFIG, BlackGui) { DEFINES += WITH_BLACKGUI }
contains(BLACK_CONFIG, FSX) { DEFINES += WITH_FSX }
contains(BLACK_CONFIG, XPlane) { DEFINES += WITH_XPLANE }
