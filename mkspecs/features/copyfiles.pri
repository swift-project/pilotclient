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
PRE_TARGETDEPS += compiler_copy_files_make_all
