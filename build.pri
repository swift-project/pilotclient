###########################
# QMake options
###########################

CONFIG           += qt
CONFIG           += warn_on
CONFIG           += silent
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
