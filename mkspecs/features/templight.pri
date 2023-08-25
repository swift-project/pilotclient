# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: CC0-1.0

swiftConfig(templight) {
    CXXFLAGS_NO_TEMPLIGHT = $$QMAKE_CXXFLAGS
    QMAKE_CXXFLAGS += -Xtemplight -profiler -Xtemplight -ignore-system
    QMAKE_CXXFLAGS += -Xtemplight -blacklist -Xtemplight $$PWD/templight_blacklist
    clang_cl {
        QMAKE_CC = clang-cl
        QMAKE_CXX = templight-cl
        moc_predefs.commands = clang-cl $$CXXFLAGS_NO_TEMPLIGHT -E -Xclang -dM ${QMAKE_FILE_IN} 2>NUL >${QMAKE_FILE_OUT}
        moc_predefs.commands += $$escape_expand(\n)$$LITERAL_HASH
    }

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
