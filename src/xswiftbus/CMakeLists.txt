# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

if(SWIFT_WIN32)
    message(FATAL_ERROR "xswiftbus only available in 64 bit environments")
endif()

add_library(xswiftbus SHARED
        org.swift_project.xswiftbus.service.xml
        org.swift_project.xswiftbus.traffic.xml
        org.swift_project.xswiftbus.weather.xml
        xswiftbus.conf
        command.h
        config.cpp
        config.h
        datarefs.h
        dbuscallbacks.h
        dbusconnection.cpp
        dbusconnection.h
        dbusdispatcher.cpp
        dbusdispatcher.h
        dbuserror.cpp
        dbuserror.h
        dbusmessage.cpp
        dbusmessage.h
        dbusobject.cpp
        dbusobject.h
        dbusserver.cpp
        dbusserver.h
        drawable.h
        main.cpp
        menus.cpp
        menus.h
        messages.cpp
        messages.h
        navdatareference.cpp
        navdatareference.h
        plugin.cpp
        plugin.h
        service.cpp
        service.h
        settings.cpp
        settings.h
        terrainprobe.cpp
        terrainprobe.h
        traffic.cpp
        traffic.h
        utils.cpp
        utils.h
        weather.cpp
        weather.h

        xplanemp2/src/CSL.cpp
        xplanemp2/src/CSL.h
        xplanemp2/src/CSLLibrary.cpp
        xplanemp2/src/CSLLibrary.h
        xplanemp2/src/CullInfo.cpp
        xplanemp2/src/CullInfo.h
        xplanemp2/src/PlanesHandoff.c
        xplanemp2/src/PlaneType.cpp
        xplanemp2/src/PlaneType.h
        xplanemp2/src/Renderer.cpp
        xplanemp2/src/Renderer.h
        xplanemp2/src/TCASOverride.cpp
        xplanemp2/src/TCASOverride.h
        xplanemp2/src/XPMPMultiplayer.cpp
        xplanemp2/src/XPMPMultiplayerVars.cpp
        xplanemp2/src/XPMPMultiplayerVars.h
        xplanemp2/src/XPMPPlane.cpp
        xplanemp2/src/XPMPPlane.h
        xplanemp2/src/XStringUtils.cpp
        xplanemp2/src/XStringUtils.h
        xplanemp2/src/XUtils.cpp
        xplanemp2/src/XUtils.h

        xplanemp2/src/obj8/Obj8Attachment.cpp
        xplanemp2/src/obj8/Obj8Attachment.h
        xplanemp2/src/obj8/Obj8Common.h
        xplanemp2/src/obj8/Obj8CSL.cpp
        xplanemp2/src/obj8/Obj8CSL.h
        xplanemp2/src/obj8/Obj8InstanceData.cpp
        xplanemp2/src/obj8/Obj8InstanceData.h
        )

if(APPLE)
    target_sources(xswiftbus PRIVATE
            xplanemp2/src/AplFSUtil.cpp
            xplanemp2/src/AplFSUtil.h
)
endif()

target_include_directories(xswiftbus PUBLIC
        ${PROJECT_SOURCE_DIR}/src
        xplanemp2
        xplanemp2/include
        xplanemp2/src)


if(SWIFT_WIN64)
    target_link_libraries(xswiftbus PUBLIC externals_xplm externals_xpwidgets)
elseif(APPLE)
    target_link_libraries(xswiftbus PRIVATE externals_xplm externals_xpwidgets "-framework Cocoa" "-framework CoreFoundation")
    target_compile_definitions(xswiftbus PRIVATE XUTILS_EXCLUDE_MAC_CRAP)
elseif(UNIX)
    # Flags needed because there is no XPLM link library
    target_link_options(xswiftbus PUBLIC -nodefaultlibs -Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/xswiftbus.map)
endif()


target_link_libraries(xswiftbus PUBLIC externals_event externals_dbus externals_xplm)


if(SWIFT_WIN32)
    set(PNG_LIBRARIES ${swift_SOURCE_DIR}/externals/win32-msvc/32/lib/png.lib)
    set(ZLIB_LIBRARIES ${swift_SOURCE_DIR}/externals/win32-msvc/32/lib/z.lib)
elseif(SWIFT_WIN64)
    set(PNG_LIBRARIES ${swift_SOURCE_DIR}/externals/win32-msvc/64/lib/png.lib)
    set(ZLIB_LIBRARIES ${swift_SOURCE_DIR}/externals/win32-msvc/64/lib/z.lib)

else()
    find_package(PNG REQUIRED)
    find_package(ZLIB REQUIRED)
endif()

# Externals required by xplanemp2
if(WIN32)
    find_package(OpenGL REQUIRED)

    target_link_libraries(xswiftbus PUBLIC ${PNG_LIBRARIES} ${ZLIB_LIBRARIES} OpenGL::GL OpenGL::GLU)
elseif(UNIX AND NOT APPLE)
    target_link_libraries(xswiftbus PUBLIC ${PNG_LIBRARIES} ${ZLIB_LIBRARIES})
endif()

if(MSVC)
    target_compile_definitions(xswiftbus PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

if(UNIX AND NOT APPLE)
    target_compile_definitions(xswiftbus PUBLIC LIN=1)
elseif(WIN32)
    target_compile_definitions(xswiftbus PUBLIC IBM=1)
elseif(APPLE)
    target_compile_definitions(xswiftbus PUBLIC APL=1)
endif()


target_compile_definitions(xswiftbus PUBLIC XPLM200=1 XPLM210=1 XPLM300=1 XPLM_DEPRECATED=1)

target_compile_definitions(xswiftbus PUBLIC XPMP_CLIENT_NAME="xswiftbus")
target_compile_definitions(xswiftbus PUBLIC XPMP_CLIENT_LONGNAME="xswiftbus")
target_compile_definitions(xswiftbus PUBLIC XSWIFTBUS_VERSION="${swift_version_major}.${swift_version_minor}.${swift_rev_count}")

if(APPLE)
    set_target_properties(xswiftbus PROPERTIES INSTALL_RPATH @loader_path/)
endif()

include(${PROJECT_SOURCE_DIR}/cmake/xswiftbus_tools.cmake)
get_xswiftbus_commit_id()

target_compile_definitions(xswiftbus PUBLIC XSWIFTBUS_COMMIT="${XSWIFTBUS_COMMIT_ID}")

set_target_properties(xswiftbus PROPERTIES PREFIX "")
set_target_properties(xswiftbus PROPERTIES SUFFIX ".xpl")

if(UNIX AND NOT APPLE)
    set_target_properties(xswiftbus PROPERTIES OUTPUT_NAME lin)
elseif(APPLE)
    set_target_properties(xswiftbus PROPERTIES OUTPUT_NAME mac)
elseif(WIN32)
    set_target_properties(xswiftbus PROPERTIES OUTPUT_NAME win)
endif()

install(TARGETS xswiftbus
    LIBRARY DESTINATION xswiftbus/64
    RUNTIME DESTINATION xswiftbus/64
    PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ
)
configure_file(readme.txt.in readme.txt)


if(WIN32)
    set(DBUS_SESSION_BUS_LISTEN_ADDRESS "autolaunch:")
    set(DBUS_SESSION_CONF_MAYBE_AUTH_EXTERNAL "<!--<auth>EXTERNAL</auth>-->")
    configure_file(${swift_SOURCE_DIR}/src/blackmisc/share/dbus-1/session.conf.in ${CMAKE_CURRENT_BINARY_DIR}/session.conf)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/session.conf DESTINATION xswiftbus/64/share/dbus-1/)

    install(FILES ${swift_SOURCE_DIR}/externals/win32-msvc/64/bin/dbus-daemon.exe DESTINATION xswiftbus/64/)
    install(FILES ${swift_SOURCE_DIR}/externals/win32-msvc/64/lib/expat.dll DESTINATION xswiftbus/64/)
endif()

install(DIRECTORY LegacyData DESTINATION xswiftbus)
install(FILES xswiftbus.conf ${CMAKE_CURRENT_BINARY_DIR}/readme.txt DESTINATION xswiftbus)
