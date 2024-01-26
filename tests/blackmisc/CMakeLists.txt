# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

include(${PROJECT_SOURCE_DIR}/cmake/swift_test.cmake)

##############
## Aviation ##
##############
add_swift_test(
        NAME misc_aviation_aircraftparts
        SOURCES aviation/testaircraftparts/testaircraftparts.cpp
        LINK_LIBRARIES misc tests_test Qt::Core
)

add_swift_test(
        NAME misc_aviation_aircraftsituation
        SOURCES aviation/testaircraftsituation/testaircraftsituation.cpp
        LINK_LIBRARIES blackconfig misc tests_test Qt::Core
)

add_swift_test(
        NAME misc_aviation
        SOURCES aviation/testaviation/testaviation.cpp
        LINK_LIBRARIES misc tests_test Qt::Core
)

add_swift_test(
        NAME misc_aviation_flightplan
        SOURCES aviation/testflightplan/testflightplan.cpp
        LINK_LIBRARIES misc tests_test Qt::Core
)

add_swift_test(
        NAME misc_aviation_flightplanaircraftinfo
        SOURCES aviation/testflightplan/testflightplanaircraftinfo.cpp
        LINK_LIBRARIES misc tests_test Qt::Core
)

##############
##    Geo   ##
##############
add_swift_test(
        NAME misc_geo
        SOURCES geo/testgeo/testgeo.cpp
        LINK_LIBRARIES misc tests_test Qt::Core
)

##############
##   Input  ##
##############
add_swift_test(
        NAME misc_input
        SOURCES input/testinput/testinput.cpp
        LINK_LIBRARIES misc tests_test Qt::Core
)

##############
##   Math  ##
##############
add_swift_test(
        NAME misc_math
        SOURCES math/testmath/testmath.cpp
        LINK_LIBRARIES misc tests_test Qt::Core
)

##############
##    PQ    ##
##############
add_swift_test(
        NAME misc_pq
        SOURCES pq/testphysicalquantities/testphysicalquantities.cpp
        LINK_LIBRARIES misc tests_test Qt::Core
)

################
## Simulation ##
################
add_swift_test(
        NAME misc_simulation_interpolatorlinear
        SOURCES simulation/testinterpolatorlinear/testinterpolatorlinear.cpp
        LINK_LIBRARIES misc tests_test Qt::Core
)

add_swift_test(
        NAME misc_simulation_interpolatormisc
        SOURCES simulation/testinterpolatormisc/testinterpolatormisc.cpp
        LINK_LIBRARIES misc tests_test Qt::Core
)

add_swift_test(
        NAME misc_simulation_interpolatorparts
        SOURCES simulation/testinterpolatorparts/testinterpolatorparts.cpp
        LINK_LIBRARIES misc tests_test Qt::Core
)

add_swift_test(
        NAME misc_simulation_xplane
        SOURCES simulation/testxplane/testxplane.cpp
        LINK_LIBRARIES misc tests_test Qt::Core
)

################
##   Various  ##
################
add_swift_test(
        NAME misc_compress
        SOURCES testcompress/testcompress.cpp
        LINK_LIBRARIES misc tests_test Qt::Core
)

add_swift_test(
        NAME misc_containers
        SOURCES testcontainers/testcontainers.cpp
        LINK_LIBRARIES misc tests_test Qt::Core
)

add_swift_test(
        NAME misc_datastream
        SOURCES testdatastream/testdatastream.cpp
        LINK_LIBRARIES misc tests_test Qt::Core
)

add_swift_test(
        NAME misc_dbus
        SOURCES testdbus/testdbus.cpp
        LINK_LIBRARIES misc tests_test Qt::Core Qt::DBus
)

add_swift_test(
        NAME misc_icon
        SOURCES testicon/testicon.cpp
        LINK_LIBRARIES misc tests_test Qt::Core
)

add_swift_test(
        NAME misc_identifier
        SOURCES testidentifier/testidentifier.cpp
        LINK_LIBRARIES misc tests_test Qt::Core
)

add_swift_test(
        NAME misc_process
        SOURCES testprocess/testprocess.cpp
        LINK_LIBRARIES misc tests_test Qt::Core
)

add_swift_test(
        NAME misc_propertyindex
        SOURCES testpropertyindex/testpropertyindex.cpp
        LINK_LIBRARIES misc tests_test Qt::Core
)

add_swift_test(
        NAME misc_sharedstate
        SOURCES testsharedstate/testsharedstate.cpp
        LINK_LIBRARIES misc tests_test Qt::Core
)

add_executable(tests_sharedstatetestserver testsharedstate/sharedstatetestserver/server.cpp testsharedstate/testsharedstate.h)
target_link_libraries(tests_sharedstatetestserver PRIVATE misc Qt::Core Qt::DBus)
add_dependencies(tests_misc_sharedstate tests_sharedstatetestserver)

add_swift_test(
        NAME misc_slot
        SOURCES testslot/testslot.cpp
        LINK_LIBRARIES misc tests_test Qt::Core
)

add_swift_test(
        NAME misc_statusmessage
        SOURCES teststatusmessage/teststatusmessage.cpp
        LINK_LIBRARIES misc tests_test Qt::Core
)

add_swift_test(
        NAME misc_stringutils
        SOURCES teststringutils/teststringutils.cpp
        LINK_LIBRARIES misc tests_test Qt::Core
)

add_swift_test(
        NAME misc_valuecache
        SOURCES testvaluecache/testvaluecache.cpp
        LINK_LIBRARIES misc tests_test Qt::Core
)

add_swift_test(
        NAME misc_variantandmap
        SOURCES testvariantandmap/testvariantandmap.cpp
        LINK_LIBRARIES misc tests_test Qt::Core
)

#add_swift_test(
#        NAME misc_worker
#        SOURCES testworker/testworker.cpp
#        LINK_LIBRARIES misc tests_test Qt::Core
#)

add_swift_test(
        NAME misc_weather
        SOURCES weather/testweather/testweather.cpp
        LINK_LIBRARIES misc tests_test Qt::Core
)
