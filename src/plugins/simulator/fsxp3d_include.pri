equals(WORD_SIZE,64) {
    INCLUDEPATH *= $$EXTERNALSROOT/common/include/simconnect/P3D-v4.1
    LIBS *= -L$$EXTERNALS_LIB_DIR/P3D-v4.1
    LIBS *= -lAdvapi32
    CONFIG(debug, debug|release): LIBS *= -lSimConnectDebug
    else:                         LIBS *= -lSimConnect
}
equals(WORD_SIZE,32) {
    INCLUDEPATH *= $$EXTERNALSROOT/common/include/simconnect/FSX-XPack
    LIBS *= -L$$EXTERNALS_LIB_DIR/FSX-XPack
    LIBS *= -lSimConnect
}
