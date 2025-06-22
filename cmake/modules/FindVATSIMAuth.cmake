# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1


macro(CheckPackageFound MSG)
    include(FindPackageHandleStandardArgs)

    find_package_handle_standard_args(VATSIMAuth
            REQUIRED_VARS VATSIM_AUTH_FOUND
            FAIL_MESSAGE "VATSIMAuth not found. ${MSG}"
    )
endmacro()

if (NOT DEFINED VATSIM_AUTH_PATH)
    CheckPackageFound("VATSIM_AUTH_PATH not set")
    return()
endif()

if (NOT EXISTS ${VATSIM_AUTH_PATH})
    CheckPackageFound("VATSIM_AUTH_PATH does not exist")
    return()
endif()

if (NOT EXISTS "${VATSIM_AUTH_PATH}/common/include/vatsim/vatsimauth.h" OR NOT EXISTS "${VATSIM_AUTH_PATH}/linux-g++/64/lib/libvatsimauth.so")
    CheckPackageFound("VATSIM_AUTH_PATH does not contain VATSIMAuth")
    return()
endif()


add_library(VATSIMAuth::VATSIMAuth SHARED IMPORTED GLOBAL)

target_include_directories(VATSIMAuth::VATSIMAuth INTERFACE ${VATSIM_AUTH_PATH}/common/include)





if(UNIX AND NOT APPLE)
    set_target_properties(VATSIMAuth::VATSIMAuth PROPERTIES IMPORTED_LOCATION ${VATSIM_AUTH_PATH}/linux-g++/64/lib/libvatsimauth.so)
elseif(APPLE)
    set_target_properties(VATSIMAuth::VATSIMAuth PROPERTIES IMPORTED_LOCATION ${VATSIM_AUTH_PATH}/macx-clang/64/lib/libvatsimauth.dylib)
elseif(SWIFT_WIN32)
    set_target_properties(VATSIMAuth::VATSIMAuth PROPERTIES IMPORTED_IMPLIB ${VATSIM_AUTH_PATH}/win32-msvc/32/lib/vatsimauth.lib)
    set_target_properties(VATSIMAuth::VATSIMAuth PROPERTIES IMPORTED_LOCATION ${VATSIM_AUTH_PATH}/win32-msvc/32/lib/vatsimauth.dll)
elseif(SWIFT_WIN64)
    set_target_properties(VATSIMAuth::VATSIMAuth PROPERTIES IMPORTED_IMPLIB ${VATSIM_AUTH_PATH}/win32-msvc/64/lib/vatsimauth.lib)
    set_target_properties(VATSIMAuth::VATSIMAuth PROPERTIES IMPORTED_LOCATION ${VATSIM_AUTH_PATH}/win32-msvc/64/lib/vatsimauth.dll)
endif()

if(UNIX AND NOT APPLE)
    # https://gitlab.kitware.com/cmake/cmake/-/issues/19860#note_642079
    set_target_properties(VATSIMAuth::VATSIMAuth PROPERTIES IMPORTED_NO_SONAME TRUE)
endif()

install(IMPORTED_RUNTIME_ARTIFACTS VATSIMAuth::VATSIMAuth
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
)


set(VATSIM_AUTH_FOUND TRUE)
CheckPackageFound("Found")
