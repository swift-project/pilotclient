# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

macro(get_version_number)
    execute_process(COMMAND python scripts/build.py --version
                WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                OUTPUT_VARIABLE SWIFT_VERSION
                OUTPUT_STRIP_TRAILING_WHITESPACE)

    string(REPLACE "." ";" SWIFT_SUBVERSIONS  ${SWIFT_VERSION})
    list(GET SWIFT_SUBVERSIONS 0 SWIFT_VERSION_MAJOR)
    list(GET SWIFT_SUBVERSIONS 1 SWIFT_VERSION_MINOR)
    list(GET SWIFT_SUBVERSIONS 2 SWIFT_VERSION_REVISION)

    # Git revision
    execute_process(COMMAND git rev-parse --short HEAD WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} OUTPUT_VARIABLE git_sha)
    string(STRIP ${git_sha} git_sha)
    set(swift_git_head ${git_sha})
endmacro()

macro(load_vatsim_key)
    # Dummy data
    set(SWIFT_VATSIM_ID "0")
    set(SWIFT_VATSIM_KEY "00000000000000000000000000000000")

    if(VATSIM_KEY_JSON)
        file(READ ${PROJECT_SOURCE_DIR}/${VATSIM_KEY_JSON} KEY_FILE)
        string(JSON SWIFT_VATSIM_ID GET ${KEY_FILE} vatsim id)
        string(JSON SWIFT_VATSIM_KEY GET ${KEY_FILE} vatsim key)
    endif()
endmacro()

function(configure_target_platform)
    set(SWIFT_WIN64 OFF PARENT_SCOPE)
    set(SWIFT_WIN32 OFF PARENT_SCOPE)

    if(WIN32)
        if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
            set(SWIFT_WIN64 ON PARENT_SCOPE)
        else()
            set(SWIFT_WIN32 ON PARENT_SCOPE)
        endif()
    endif()
endfunction()
