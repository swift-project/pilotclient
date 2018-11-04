swiftConfig(templight) {
    QMAKE_CXXFLAGS += -Xtemplight -profiler -Xtemplight -ignore-system
    QMAKE_CXXFLAGS += -Xtemplight -blacklist -Xtemplight $$PWD/templight_blacklist
    clang_cl {
        QMAKE_CC = clang-cl
        QMAKE_CXX = templight-cl
        moc_predefs.commands = clang-cl -Bx$$shell_quote($$shell_path($$QMAKE_QMAKE)) $$QMAKE_CXXFLAGS \
                               -E -Za ${QMAKE_FILE_IN} 2>NUL >${QMAKE_FILE_OUT} $$escape_expand(\n)$$LITERAL_HASH
    }
    else:clang {
        QMAKE_CC = clang
        QMAKE_CXX = templight++
        moc_predefs.commands = clang++ $$QMAKE_CXXFLAGS -dM -E -o ${QMAKE_FILE_OUT} ${QMAKE_FILE_IN}
    }
    else:error(Templight requires a clang mkspec)

    templight-convert.commands = templight-convert -f callgrind -i ${QMAKE_FILE_IN}.trace.pbf -o ${QMAKE_FILE_OUT}
    templight-convert.CONFIG = no_link
    templight-convert.input = OBJECTS
    templight-convert.output = ${QMAKE_FILE_IN_PATH}/callgrind.${QMAKE_FILE_IN_BASE}
    QMAKE_EXTRA_COMPILERS += templight-convert

    load(touch)
    win32:!mingw: templight-convert-cookie.commands = $$TOUCH $** && $$TOUCH $@
    else:         templight-convert-cookie.commands = $$TOUCH $<  && $$TOUCH $@
    templight-convert-cookie.depends = compiler_templight-convert_make_all
    templight-convert-cookie.target = templight-convert.cookie
    QMAKE_EXTRA_TARGETS += templight-convert-cookie
    !isEmpty(SOURCES): PRE_TARGETDEPS += templight-convert.cookie
}
