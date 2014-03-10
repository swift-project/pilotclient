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
!exists("$$EXTERNALDIR/include") {
    error("Could not find externals in $$EXTERNALDIR. Please install it!")
}

# Everything is fine. Add the include path
INCLUDEPATH *= $$EXTERNALDIR/include

# and the library path depending on the used compiler

win32:contains(QMAKE_TARGET.arch, x86_64) {
    LIBS *= -L$$EXTERNALDIR/vs2010_64/lib
}
win32:contains(QMAKE_TARGET.arch, x86) {
    LIBS *= -L$$EXTERNALDIR/vs2010_32/lib
    LIBS += -luser32
}

win32-g++ {
    WIN_FIND = $$(SYSTEMROOT)\system32\find
    MINGW64 = $$system($$QMAKE_CXX -Q --help=target | $$WIN_FIND \"-m64\")
    contains(MINGW64,[enabled]) {
        LIBS *= -L$$EXTERNALDIR/mingw64/lib
    }
    else {
        LIBS *= -L$$EXTERNALDIR/mingw32/lib
    }
    LIBS += -luser32
}

linux-g++ {
    GCC64 = $$system($$QMAKE_CXX -Q --help=target | grep m64)
    contains(GCC64,[enabled]) {
        LIBS *= -L$$EXTERNALDIR/linux64/lib
    }
    else {
        LIBS *= -L$$EXTERNALDIR/linux32/lib
    }
}

linux-g++-32 {
    LIBS *= -L$$EXTERNALDIR/linux32/lib
}

linux-g++-64 {
    LIBS *= -L$$EXTERNALDIR/linux64/lib
}

macx-clang {
    LIBS *= -L$$EXTERNALDIR/macx-clang64/lib
}
