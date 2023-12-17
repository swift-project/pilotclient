# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_library(externals_qwt_plot SHARED IMPORTED GLOBAL)

target_include_directories(externals_qwt_plot INTERFACE ${PROJECT_SOURCE_DIR}/third_party/externals/common/include/qwt)

if(UNIX AND NOT APPLE)
    set_target_properties(externals_qwt_plot PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/third_party/externals/linux-g++/64/lib/libqwt.so)
elseif(APPLE)
    set_target_properties(externals_qwt_plot PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/third_party/externals/macx-clang/64/lib/qwt.framework/qwt)
elseif(SWIFT_WIN32)
    set_target_properties(externals_qwt_plot PROPERTIES IMPORTED_IMPLIB ${PROJECT_SOURCE_DIR}/third_party/externals/win32-msvc/32/lib/qwt.lib)
    set_target_properties(externals_qwt_plot PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/third_party/externals/win32-msvc/32/lib/qwt.dll)
elseif(SWIFT_WIN64)
    set_target_properties(externals_qwt_plot PROPERTIES IMPORTED_IMPLIB ${PROJECT_SOURCE_DIR}/third_party/externals/win32-msvc/64/lib/qwt.lib)
    set_target_properties(externals_qwt_plot PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/third_party/externals/win32-msvc/64/lib/qwt.dll)
endif()

target_link_libraries(externals_qwt_plot INTERFACE Qt::OpenGL)

if(APPLE)
    # Special handling for qwt.framework
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/third_party/externals/macx-clang/64/lib/qwt.framework DESTINATION lib)
else()
    install(IMPORTED_RUNTIME_ARTIFACTS externals_qwt_plot
            LIBRARY DESTINATION lib
            RUNTIME DESTINATION bin
    )
endif()
