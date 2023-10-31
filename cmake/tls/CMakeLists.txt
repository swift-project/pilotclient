# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

if(WIN32)
    add_library(externals_ssl SHARED IMPORTED GLOBAL)
    if(SWIFT_WIN32)
        set_target_properties(externals_ssl PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/externals/win32-msvc/32/lib/libssl-1_1.dll)
    elseif(SWIFT_WIN64)
        set_target_properties(externals_ssl PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/externals/win32-msvc/64/lib/libssl-1_1-x64.dll)
    endif()
    install(IMPORTED_RUNTIME_ARTIFACTS externals_ssl)

    add_library(externals_crypto SHARED IMPORTED GLOBAL)
    if(SWIFT_WIN32)
        set_target_properties(externals_crypto PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/externals/win32-msvc/32/lib/libcrypto-1_1.dll)
    elseif(SWIFT_WIN64)
        set_target_properties(externals_crypto PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/externals/win32-msvc/64/lib/libcrypto-1_1-x64.dll)
    endif()
    install(IMPORTED_RUNTIME_ARTIFACTS externals_crypto)

endif()
