# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: CC0-1.0

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

win32-clang-msvc:       EXTERNALS_SPEC = win32-msvc
else:win32-clang-g++:   EXTERNALS_SPEC = win32-g++
else:linux:             EXTERNALS_SPEC = linux-g++
else:                   EXTERNALS_SPEC = $$basename(QMAKESPEC)

msvc {
    INCLUDEPATH *= $$EXTERNALSROOT/common/include
}
else:mingw {
    QMAKE_CXXFLAGS += -isystem $$EXTERNALSROOT/common/include
}
else:macx {
    INCLUDEPATH *= $$EXTERNALSROOT/common/include
}
else {
    QMAKE_CXXFLAGS += -idirafter $$EXTERNALSROOT/common/include
}

llvm {
    QMAKE_CXXFLAGS_WARN_ON *= $$clangArg(-isystem$$system_path($$EXTERNALSROOT/common/include))
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
