load(common_pre)

REQUIRES += contains(BLACK_CONFIG,Doxygen)

TEMPLATE = lib
CONFIG += staticlib
CONFIG -= qt

DOXYFILE = Doxyfile.qmake

DOXY_INPUT = .
win32-g++ {
        doxy.commands = set DOXY_SRC_ROOT=$$SourceRoot\\src & \
                        set QDOC_PATH=$$[QT_INSTALL_DOCS] & \
			doxygen $$PWD/$$DOXYFILE
}
else:win32 {
        doxy.commands = set DOXY_SRC_ROOT=$$SourceRoot\\src & \
                        set QDOC_PATH=$$[QT_INSTALL_DOCS] & \
			doxygen $$PWD\\$$DOXYFILE
}
else {
        doxy.commands = DOXY_SRC_ROOT="$$SourceRoot/src" \
                        QDOC_PATH="$$[QT_INSTALL_DOCS]" \
			doxygen $$PWD/$$DOXYFILE
}

doxy.CONFIG = no_link target_predeps
doxy.depends = $$PWD/$$DOXYFILE
doxy.input = DOXY_INPUT
doxy.name = DOXY
doxy.output = html/index.html
QMAKE_EXTRA_COMPILERS += doxy
OTHER_FILES += Doxyfile.qmake

load(common_post)
