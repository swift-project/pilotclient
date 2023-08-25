# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: CC0-1.0

copy_files.name = COPY
copy_files.input = COPY_FILES
copy_files.CONFIG = no_link

copy_files.output_function = fileCopyDestination
defineReplace(fileCopyDestination) {
    return($$DestRoot/$$relative_path($$1))
}

win32:isEmpty(MINGW_IN_SHELL) {
    copy_files.commands = copy /y ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
}
else {
    copy_files.commands = mkdir -p `dirname ${QMAKE_FILE_OUT}` && \
                          cp ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
}

QMAKE_EXTRA_COMPILERS += copy_files


# This CONFIG should be all that is needed to add a dependency to "make all"
# but some snafu with absolute vs. relative paths necessitates this workaround
# with PRE_TARGETDEPS instead.
#copy_files.CONFIG += target_predeps

load(touch)
copy_files_cookie.depends = compiler_copy_files_make_all
copy_files_cookie.target = copy_files.cookie
win32:!mingw: copy_files_cookie.commands = $$TOUCH $** && $$TOUCH $@
else:         copy_files_cookie.commands = $$TOUCH $<  && $$TOUCH $@
QMAKE_EXTRA_TARGETS += copy_files_cookie

!isEmpty($${copy_files.input}) {
    PRE_TARGETDEPS += $${copy_files_cookie.target}
}

# Disable the extra compiler when the .pro file is being parsed by the MSVC
# project generator to compute dependencies. This works around a bug where
# relative paths were being treated as absolute, leading to qmake creating
# empty directories in the root of the current drive.
contains(TEMPLATE, "vc.*"):!build_pass:QMAKE_EXTRA_COMPILERS -= copy_files

# Work around QTBUG-79178
equals(TEMPLATE, vcsubdirs):QMAKE_EXTRA_COMPILERS -= copy_files
