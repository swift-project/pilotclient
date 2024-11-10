# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

macro(get_xswiftbus_commit_id)
    set(XSWIFTBUS_DEPENDENTS  src/xswiftbus src/misc/simulation/xplane/qtfreeutils.* src/misc/simulation/settings/xswiftbussettingsqtfree.*)

    execute_process(COMMAND git log -n 1 --format=%h -- ${XSWIFTBUS_DEPENDENTS} WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} OUTPUT_VARIABLE XSWIFTBUS_COMMIT_ID)

    if(XSWIFTBUS_COMMIT_ID STREQUAL "")
        message(FATAL_ERROR "Unable to get xswiftbus commit id")
    endif()

    # Strip newline
    string(STRIP ${XSWIFTBUS_COMMIT_ID} XSWIFTBUS_COMMIT_ID)

endmacro()
