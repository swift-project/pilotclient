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
message("Found externals: $$EXTERNALDIR")
INCLUDEPATH *= $$EXTERNALDIR/include

# and the library path depending on the used compiler

win32:contains(QMAKE_TARGET.arch, x86_64) {
    LIBS *= -L$$EXTERNALDIR/vs2010_64/lib
}
win32:contains(QMAKE_TARGET.arch, x86) {
    LIBS *= -L$$EXTERNALDIR/vs2010_32/lib
}

win32-g++ {
    MINGW = $$system($$QMAKE_CXX -Q --help=target | find \"-m64\")
    contains(MINGW,-m64) {
        LIBS *= -L$$EXTERNALDIR/mingw64/lib
    }
    else {
        LIBS *= -L$$EXTERNALDIR/mingw32/lib
    }
}

linux-g++-32 {
    LIBS *= -L$$EXTERNALDIR/linux32/lib
}

linux-g++-64 {
    LIBS *= -L$$EXTERNALDIR/linux64/lib
}







