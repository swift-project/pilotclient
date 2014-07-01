# externals.pri
# Sets up the include and library directories for external dependencies

# If you want to manually set the external path, uncomment the following line
# EXTERNALDIR = /path/to/externals

# Test if enviromental variable for externals is set

isEmpty(EXTERNALDIR) {
    EXTERNALDIR = $$(VATSIM_EXTERNAL_DIR)
}

# if no env variable is set, we use the standard path in the client folder.
isEmpty(EXTERNALDIR) {
    EXTERNALDIR = $$TOPSRCDIR/externals
}

# Test the folder if it exists and has a include subfolder
!exists("$$EXTERNALDIR/common/include") {
    error("Could not find externals in $$EXTERNALDIR. Please install it!")
}

# Everything is fine. Add the include path
INCLUDEPATH *= $$EXTERNALDIR/common/include

# and the library path depending on the used compiler
win32-msvc2010 {
    equals(WORD_SIZE,64): LIBS *= -L$$EXTERNALDIR/vs2010_64/lib
    equals(WORD_SIZE,32): LIBS *= -L$$EXTERNALDIR/vs2010_32/lib
    equals(WORD_SIZE,32): LIBS += -luser32
}
win32-msvc2013 {
    INCLUDEPATH *= $$EXTERNALDIR/win32-vs2013/include
    equals(WORD_SIZE,64): LIBS *= -L$$EXTERNALDIR/win32-vs2013/lib64
    equals(WORD_SIZE,32): LIBS *= -L$$EXTERNALDIR/win32-vs2013/lib32
    equals(WORD_SIZE,32): LIBS += -luser32
}
win32-g++ {
    INCLUDEPATH *= $$EXTERNALDIR/win32-g++/include
    equals(WORD_SIZE,64): LIBS *= -L$$EXTERNALDIR/win32-g++/lib64
    equals(WORD_SIZE,32): LIBS *= -L$$EXTERNALDIR/win32-g++/lib32
    LIBS += -luser32
}
linux-g++* {
    equals(WORD_SIZE,64): LIBS *= -L$$EXTERNALDIR/linux64/lib
    equals(WORD_SIZE,32): LIBS *= -L$$EXTERNALDIR/linux32/lib
}
macx-clang {
    INCLUDEPATH *= $$EXTERNALDIR/macx-clang/include
    equals(WORD_SIZE,64): LIBS *= -L$$EXTERNALDIR/macx-clang/lib64 -F$$EXTERNALDIR/macx-clang/lib64
    equals(WORD_SIZE,32): LIBS *= -L$$EXTERNALDIR/macx-clang/lib32 -F$$EXTERNALDIR/macx-clang/lib32
}
