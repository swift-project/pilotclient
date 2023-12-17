# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

if(SWIFT_BUILD_FS9_PLUGIN)
    set(DPLAY_PATH ${PROJECT_SOURCE_DIR}/third_party/externals/win32-msvc/32/include)
    if(NOT EXISTS "${DPLAY_PATH}/dplay8.h")
        message(WARNING "DPLAY_PATH does not point to dplay include directory. FS9 support disabled.")
        set(SWIFT_BUILD_FS9_PLUGIN OFF PARENT_SCOPE)
    endif()

    if(SWIFT_BUILD_FS9_PLUGIN)
        add_library(externals_dplay INTERFACE IMPORTED GLOBAL)
        target_include_directories(externals_dplay INTERFACE ${DPLAY_PATH})
        target_link_libraries(externals_dplay INTERFACE dxguid)
    endif()
endif()
