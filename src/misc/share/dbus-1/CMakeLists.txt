# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

if(WIN32)
    set(DBUS_SESSION_BUS_LISTEN_ADDRESS "autolaunch:")
    set(DBUS_SESSION_CONF_MAYBE_AUTH_EXTERNAL "<!--<auth>EXTERNAL</auth>-->")
    configure_file(session.conf.in ${CMAKE_BINARY_DIR}/out/share/dbus-1/session.conf)
    install(FILES ${CMAKE_BINARY_DIR}/out/share/dbus-1/session.conf DESTINATION share/dbus-1/)
endif()
