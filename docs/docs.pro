# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: CC0-1.0

load(common_pre)

TEMPLATE = lib
CONFIG += staticlib
CONFIG -= qt

OTHER_FILES += Doxyfile.qmake
OTHER_FILES += src/*.dox
OTHER_FILES += src/wiki/*.dox

win32: DOXYGEN_BIN = $$system($$(SYSTEMROOT)\system32\where doxygen 2> nul)
else: DOXYGEN_BIN = $$system(which doxygen 2> /dev/null)

!isEmpty(DOXYGEN_BIN) {
    DOXYFILE = Doxyfile.qmake
    DOXY_INPUT = .
    win32-g++ {
        doxy.commands  = set DOXY_SRC_ROOT=$$SourceRoot &
        doxy.commands += doxygen $$PWD/$$DOXYFILE
    }
    else:win32 {
        doxy.commands  = set DOXY_SRC_ROOT=$$SourceRoot &
        doxy.commands += doxygen $$PWD\\$$DOXYFILE
    }
    else {
        doxy.commands  = DOXY_SRC_ROOT="$$SourceRoot"
        doxy.commands += doxygen $$PWD/$$DOXYFILE
    }

    doxy.CONFIG = no_link target_predeps
    doxy.depends = $$PWD/$$DOXYFILE
    doxy.input = DOXY_INPUT
    doxy.name = DOXY
    doxy.output = html/index.html

    QMAKE_EXTRA_COMPILERS += doxy
}

load(common_post)
