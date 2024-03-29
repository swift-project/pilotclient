# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

if(SWIFT_WIN64 OR APPLE)
    add_library(externals_xplm STATIC IMPORTED GLOBAL)
elseif(UNIX)
    add_library(externals_xplm INTERFACE)
endif()


target_include_directories(externals_xplm INTERFACE ${PROJECT_SOURCE_DIR}/third_party/externals/common/include ${PROJECT_SOURCE_DIR}/third_party/externals/common/include/XPLM)

if(SWIFT_WIN64)
    set_target_properties(externals_xplm PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/third_party/externals/win32-msvc/64/lib/XPLM_64.lib)
elseif(APPLE)
    set_target_properties(externals_xplm PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/third_party/externals/macx-clang/64/lib/XPLM.framework/XPLM)
endif()

if(SWIFT_WIN64 OR APPLE)
    add_library(externals_xpwidgets STATIC IMPORTED GLOBAL)
    target_include_directories(externals_xpwidgets INTERFACE ${PROJECT_SOURCE_DIR}/third_party/externals/common/include)

    if(SWIFT_WIN64)
       set_target_properties(externals_xpwidgets PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/third_party/externals/win32-msvc/64/lib/XPWidgets_64.lib)
    elseif(APPLE)
        set_target_properties(externals_xpwidgets PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/third_party/externals/macx-clang/64/lib/XPWidgets.framework/XPWidgets)
    endif()
endif()
