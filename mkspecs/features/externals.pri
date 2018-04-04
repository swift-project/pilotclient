################################
# Path to external dependencies
################################

# If you want to manually set the external path, uncomment the following line
# EXTERNALSROOT = /path/to/externals

isEmpty(EXTERNALSROOT) {
    EXTERNALSROOT = $$(SWIFT_EXTERNALS_DIR)
}

isEmpty(EXTERNALSROOT) {
    EXTERNALSROOT = $$SourceRoot/externals
}

!exists("$$EXTERNALSROOT/common/include") {
    error("Could not find externals in $${EXTERNALSROOT}. Please install them!")
}

################################
# Externals
################################

EXTERNALS_SPEC = $$basename(QMAKESPEC)
msvc {
    clang_cl: EXTERNALS_SPEC = win32-msvc
    win32-msvc {
        # From Qt 5.8.1 onwards, QMAKESPEC is win32-msvc without the version number
        # see https://codereview.qt-project.org/#/c/162754/
        lessThan(MSVC_VER, 14.0) | greaterThan(MSVC_VER, 15.0) {
            error(This version of Visual Studio is not supported (MSVC_VER = $$MSVC_VER))
        }
    }
}

win32 {
    INCLUDEPATH *= $$EXTERNALSROOT/common/include
}
else:macx {
    INCLUDEPATH *= $$EXTERNALSROOT/common/include
}
else:unix: {
    QMAKE_CXXFLAGS *= -idirafter $$EXTERNALSROOT/common/include
}

equals(WORD_SIZE,64) {
    EXTERNALS_BIN_DIR = $$EXTERNALSROOT/$$EXTERNALS_SPEC/64/bin
    EXTERNALS_LIB_DIR = $$EXTERNALSROOT/$$EXTERNALS_SPEC/64/lib
    INCLUDEPATH *= $$EXTERNALSROOT/$$EXTERNALS_SPEC/64/include
}
equals(WORD_SIZE,32) {
    EXTERNALS_BIN_DIR = $$EXTERNALSROOT/$$EXTERNALS_SPEC/32/bin
    EXTERNALS_LIB_DIR = $$EXTERNALSROOT/$$EXTERNALS_SPEC/32/lib
    INCLUDEPATH *= $$EXTERNALSROOT/$$EXTERNALS_SPEC/32/include
}

LIBS *= -L$$EXTERNALS_LIB_DIR
macx: LIBS *= -F$$EXTERNALS_LIB_DIR
win32: LIBS *= -luser32
