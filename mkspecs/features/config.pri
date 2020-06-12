# Make sure _QMAKE_CACHE_ is initialized (otherwise config.log goes to the root directory)
cache()

################################
# Parse json config files
################################

# Copy json keys/values from one variable prefix to another, optionally caching the result
defineTest(copyJsonFromTo) {
    from = $$1
    to = $$2
    options = $$3

    for(key, $${from}._KEYS_) {
        $${to}._KEYS_ *= $$key
        export($${to}._KEYS_)
        copyJsonFromTo($${from}.$$key, $${to}.$$key, $$options)
    }
    contains(options, cache):defined($${from}._KEYS_, var): cache($${to}._KEYS_)

    defined($$from, var) {
        contains(options, overwrite)|!defined($$to, var) {
            eval(newValue = \$\${$$from})
            $$to = $$newValue
            export($$to)
        }
        contains(options, cache):defined($$to, var): cache($$to)
    }
}

# Options that control how the config works
!swiftNoDefaultJson: SWIFT_CONFIG_JSON = default.json $$SWIFT_CONFIG_JSON
equals(_PRO_FILE_PWD_, $$SourceRoot): CONFIG *= swiftNoCacheConfig

# Parse the json files (or get the values from the cache)
for(jsonFile, SWIFT_CONFIG_JSON) {
    contains(jsonFile, [^A-Za-z0-9_\\.]): error(Illegal character in filename \"$$jsonFile\")
    jsonPath = $$SourceRoot/$$jsonFile
    !exists($$jsonPath): error($$jsonPath not found)
    SWIFT_CONFIG_JSON_PATHS += $$jsonPath
    SWIFT_CONFIG_JSON_SHELL_PATHS += $$shell_path($$jsonPath)
    export(SWIFT_CONFIG_JSON_PATHS)
    export(SWIFT_CONFIG_JSON_SHELL_PATHS)
    swiftNoCacheConfig {
        message(Parsing $${jsonFile}...)
        jsonBlob = $$cat($$jsonPath, blob)
        parseJson(jsonBlob, SWIFT_CONFIG_PARSED.$$jsonFile)
        copyJsonFromTo(SWIFT_CONFIG_PARSED.$$jsonFile, SWIFT_CONFIG_PARSED_ALL, overwrite)
    }
}
swiftNoCacheConfig {
    copyJsonFromTo(SWIFT_CONFIG_PARSED_ALL, SWIFT_CONFIG)
    copyJsonFromTo(SWIFT_CONFIG, SWIFT_CONFIG_CACHE, cache overwrite)
}
else {
    copyJsonFromTo(SWIFT_CONFIG_CACHE, SWIFT_CONFIG)
}

# Automatically run qmake again when a json file changed
QMAKE_INTERNAL_INCLUDED_FILES *= $$SWIFT_CONFIG_JSON_PATHS

# The line above doesn't work with Qt Creator's MSVC kit, so we report an error as a fallback
load(touch)
system($$TOUCH $$system_path($$BuildRoot/swift_config.cookie)) {
    swift_config_changed.target = $$shell_path($$BuildRoot/swift_config.cookie)
    swift_config_changed.depends = $$SWIFT_CONFIG_JSON_SHELL_PATHS
    swift_config_changed.commands = @echo ERROR: swift config changed, please run qmake again >&2 && exit 1
    QMAKE_EXTRA_TARGETS += swift_config_changed
    PRE_TARGETDEPS += $${swift_config_changed.target}
}
else:msvc {
    warning(Failed to create swift_config.cookie, you will need to manually run qmake again after any changes in $$join(SWIFT_CONFIG_JSON, ", ").)
}

################################
# Functions to read json config
################################

defineTest(swiftConfig) {
    equals(SWIFT_CONFIG.$$1, true): return(true)
    return(false)
}
defineReplace(swiftConfig) {
    eval(value = \$\${SWIFT_CONFIG.$$1})
    eval(value = $$value)
    return($$value)
}

################################
# Functions to modify json config
################################

defineTest(setSwiftConfig) {
    SWIFT_CONFIG.$$1 = $$2
    export(SWIFT_CONFIG.$$1)
}
defineTest(enableSwiftConfig) {
    for(arg, ARGS): setSwiftConfig($$arg, true)
}
defineTest(disableSwiftConfig) {
    for(arg, ARGS): setSwiftConfig($$arg, false)
}

################################
# Functions for checking dependencies
################################

load(configure)

defineTest(CheckMandatoryDependency) {
    !qtCompileTest($${1}): error(Cannot find $${1} development package. Make sure it is installed. Inspect config.log for more information.)
}
defineTest(CheckOptionalDependency) {
    qtCompileTest($${1})
}

################################
# Global compiler macros
################################

swiftConfig(static) { DEFINES *= WITH_STATIC }
swiftConfig(assertsInRelease) { DEFINES *= QT_FORCE_ASSERTS }
msvc { DEFINES *= BLACK_USE_CRASHPAD }
macos { DEFINES *= BLACK_USE_CRASHPAD }
