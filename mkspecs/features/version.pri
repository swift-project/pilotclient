# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: CC0-1.0

win32: GIT_BIN = $$system($$(SYSTEMROOT)\system32\where git 2> nul)
else: GIT_BIN = $$system(which git 2> /dev/null)

# Count the number of commits since the version number was changed
defineReplace(gitRevCount) {
    !exists(../../default.json): error(default.json missing)
    CONFIG_LOG = $$system(git log --format=%H ../../default.json)
    for(sha, CONFIG_LOG) {
        jsonBlob = $$system(git show $${sha}:../../default.json)
        parseJson(jsonBlob, jsonData)
        v = $${jsonData.version.major}.$${jsonData.version.minor}
        equals(v, $$swiftConfig(version.full)): BASE_COMMIT = $$sha
    }
    isEmpty(BASE_COMMIT): warning(Failed to find commit that changed version number to '$$swiftConfig(version.full)')
    return($$system(git rev-list --count HEAD $$system_quote(^$$BASE_COMMIT)))
}

setSwiftConfig(version.full, $$swiftConfig(version.major).$$swiftConfig(version.minor))

VER_MAJ = $$swiftConfig(version.major)
VER_MIN = $$swiftConfig(version.minor)
VERSION = $$swiftConfig(version.full)

!qtc_run: !isEmpty(GIT_BIN) {
    isEmpty(VER_REV) {
        VER_REV = $$gitRevCount()
        cache(VER_REV)
    }
    win32: VERSION = $${VERSION}.$${VER_REV}
}

win32: CONFIG *= skip_target_version_ext
