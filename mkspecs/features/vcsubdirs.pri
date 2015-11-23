# QMake generates an MSVC solution file for each SUBDIRS project,
# but we only want one solution file, so...

# Test whether we are generating an MSVC solution file.
equals(TEMPLATE, "vcsubdirs") {

    # subdirToProjectFile(subdir, basedir)
    #   This function returns the absolute path of the .pro file referenced by subdir.
    #   \param subdir Any value which is valid to appear in the SUBDIRS variable.
    #   \param basedir The absolute directory of the .pro file containing the SUBDIRS variable.
    #   \note For now, it only supports .file and .subdir modifiers in the top-level project.
    defineReplace(subdirToProjectFile) {
        SUBDIR = $$1
        BASEDIR = $$2
        BASENAME = $$basename(SUBDIR)
        SUFFIX = $$section(SUBDIR, ".", -1, -1)
        !isEmpty($${SUBDIR}.file):   return($$BASEDIR/$$eval($${SUBDIR}.file))
        !isEmpty($${SUBDIR}.subdir): return($$BASEDIR/$$eval($${SUBDIR}.subdir)/$${BASENAME}.pro)
        equals(SUFFIX, "pro"):       return($$BASEDIR/$$SUBDIR)
        else:                        return($$BASEDIR/$$SUBDIR/$${BASENAME}.pro)
    }

    # flattenSubdirs(subdirs, basedir)
    #   This function returns its subdirs argument, but when it sees a reference to a project
    #   which is itself a subdirs project, it is replaced with the SUBDIRS variable from that
    #   project, recursively.
    #   \param subdirs The value of a SUBDIRS variable.
    #   \param basedir The absolute directory of the .pro file containing the SUBDIRS variable.
    defineReplace(flattenSubdirs) {
        SUBDIRS = $$1
        BASEDIR = $$2
        for(subdir, SUBDIRS) {
            SUBDIR_PROJECT = $$subdirToProjectFile($$subdir, $$BASEDIR)
            INNER_REQUIRES = $$fromfile($$SUBDIR_PROJECT, REQUIRES)
            REQUIRES_OK = 1
            for(req, INNER_REQUIRES): !if($${req}): REQUIRES_OK = 0
            equals(REQUIRES_OK,1) {
                INNER_TEMPLATE = $$fromfile($$SUBDIR_PROJECT, TEMPLATE)
                equals(INNER_TEMPLATE, "vcsubdirs") {
                    INNER_SUBDIRS = $$fromfile($$SUBDIR_PROJECT, SUBDIRS)
                    SUBDIRS_FLAT += $$flattenSubdirs($$INNER_SUBDIRS, $$dirname(SUBDIR_PROJECT))
                }
                else: SUBDIRS_FLAT += $$SUBDIR_PROJECT
            }
        }
        return($$SUBDIRS_FLAT)
    }

    # Make sure we are in the top-level SUBDIRS project.
    !equals(_PRO_FILE_PWD_, $$SourceRoot): error(This line should never be reached)

    # Recursively replace nested SUBDIRS with the leaves of their subtrees.
    SUBDIRS = $$flattenSubdirs($$SUBDIRS, $$_PRO_FILE_PWD_)

    # Remove duplicates, because qmake.
    SUBDIRS = $$unique(SUBDIRS)

}
