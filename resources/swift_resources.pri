win32:isEmpty(MINGW_IN_SHELL):  COPY = xcopy /yis
else:                           COPY = cp -r

win32 {
    QMAKE_PRE_LINK += $$COPY $$shell_path($$PWD/data) \
                          $$shell_path($$OUT_PWD/../../bin/resources)
}
else {
    QMAKE_PRE_LINK += mkdir -p $$shell_path($$OUT_PWD/../../bin) && \
                          $$COPY $$shell_path($$PWD/data) \
                          $$shell_path($$OUT_PWD/../../bin)
}
