# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1


if (UNIX AND NOT APPLE)
    add_library(externals_dbus SHARED IMPORTED GLOBAL)
    find_package(PkgConfig REQUIRED)
    pkg_search_module(DBUS REQUIRED dbus-1)
    target_include_directories(externals_dbus INTERFACE ${DBUS_INCLUDE_DIRS})
    set_target_properties(externals_dbus PROPERTIES IMPORTED_LOCATION ${DBUS_LINK_LIBRARIES})
    add_library(dbus-1 ALIAS externals_dbus)
endif ()
