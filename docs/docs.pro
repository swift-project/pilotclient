load(common_pre)

REQUIRES += contains(BLACK_CONFIG,Doxygen)

TEMPLATE = lib
CONFIG += staticlib
CONFIG -= qt

OTHER_FILES += Doxyfile.qmake
OTHER_FILES += src/*.dox
OTHER_FILES += src/wiki/*.dox

win32: DOXYGEN_BIN = $$system(where doxygen)
else: DOXYGEN_BIN = $$system(which doxygen)

isEmpty(DOXYGEN_BIN) {
        message("Doxygen not found")
} else {
    DOXYFILE = Doxyfile.qmake

    DOXY_INPUT = .
    win32-g++ {
            doxy.commands = set DOXY_SRC_ROOT=$$SourceRoot & \
                            set QDOC_PATH=$$[QT_INSTALL_DOCS] & \
                            doxygen $$PWD/$$DOXYFILE
    }
    else:win32 {
            doxy.commands = set DOXY_SRC_ROOT=$$SourceRoot & \
                            set QDOC_PATH=$$[QT_INSTALL_DOCS] & \
                            doxygen $$PWD\\$$DOXYFILE
    }
    else {
            doxy.commands = DOXY_SRC_ROOT="$$SourceRoot" \
                            QDOC_PATH="$$[QT_INSTALL_DOCS]" \
                            doxygen $$PWD/$$DOXYFILE
    }

    doxy.CONFIG = no_link target_predeps
    doxy.depends = $$PWD/$$DOXYFILE
    doxy.input = DOXY_INPUT
    doxy.name = DOXY
    doxy.output = html/index.html

    QMAKE_EXTRA_COMPILERS += doxy
}

load(common_post)
