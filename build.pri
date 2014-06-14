###########################
# QMake options
###########################

CONFIG           += qt
CONFIG           += warn_on
CONFIG           += c++11

# workaround QTBUG-39142
win32-msvc2013 {
    PLATFORM_TOOLSET = $$(PlatformToolset)
    equals(PLATFORM_TOOLSET,CTP_Nov2013) {
        DEFINES += Q_COMPILER_INITIALIZER_LISTS
    }
}

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
