##############
## Aviation ##
##############
add_executable(tests_blackmisc_aviation_aircraftparts aviation/testaircraftparts/testaircraftparts.cpp)
target_link_libraries(tests_blackmisc_aviation_aircraftparts PUBLIC misc tests_test Qt::Core)

add_executable(tests_blackmisc_aviation_aircraftsituation aviation/testaircraftsituation/testaircraftsituation.cpp)
target_link_libraries(tests_blackmisc_aviation_aircraftsituation PUBLIC blackconfig misc tests_test Qt::Core)

add_executable(tests_blackmisc_aviation aviation/testaviation/testaviation.cpp)
target_link_libraries(tests_blackmisc_aviation PUBLIC misc tests_test Qt::Core)

add_executable(tests_blackmisc_aviation_flightplan aviation/testflightplan/testflightplan.cpp)
target_link_libraries(tests_blackmisc_aviation_flightplan PUBLIC misc tests_test Qt::Core)

##############
##    Geo   ##
##############
add_executable(tests_blackmisc_geo geo/testgeo/testgeo.cpp)
target_link_libraries(tests_blackmisc_geo PUBLIC misc tests_test Qt::Core)

##############
##   Input  ##
##############
add_executable(tests_blackmisc_input input/testinput/testinput.cpp)
target_link_libraries(tests_blackmisc_input PUBLIC misc tests_test Qt::Core)

##############
##   Math  ##
##############
add_executable(tests_blackmisc_math math/testmath/testmath.cpp)
target_link_libraries(tests_blackmisc_math PUBLIC misc tests_test Qt::Core)

##############
##    PQ    ##
##############
add_executable(tests_blackmisc_pq pq/testphysicalquantities/testphysicalquantities.cpp)
target_link_libraries(tests_blackmisc_pq PUBLIC misc tests_test Qt::Core)

################
## Simulation ##
################
add_executable(tests_blackmisc_simulation_interpolatorlinear simulation/testinterpolatorlinear/testinterpolatorlinear.cpp)
target_link_libraries(tests_blackmisc_simulation_interpolatorlinear PUBLIC misc tests_test Qt::Core)

add_executable(tests_blackmisc_simulation_interpolatormisc simulation/testinterpolatormisc/testinterpolatormisc.cpp)
target_link_libraries(tests_blackmisc_simulation_interpolatormisc PUBLIC misc tests_test Qt::Core)

add_executable(tests_blackmisc_simulation_interpolatorparts simulation/testinterpolatorparts/testinterpolatorparts.cpp)
target_link_libraries(tests_blackmisc_simulation_interpolatorparts PUBLIC misc tests_test Qt::Core)

add_executable(tests_blackmisc_simulation_xplane simulation/testxplane/testxplane.cpp)
target_link_libraries(tests_blackmisc_simulation_xplane PUBLIC misc tests_test Qt::Core)

################
##   Various  ##
################
add_executable(tests_blackmisc_compress testcompress/testcompress.cpp)
target_link_libraries(tests_blackmisc_compress PUBLIC misc tests_test Qt::Core)

add_executable(tests_blackmisc_containers testcontainers/testcontainers.cpp)
target_link_libraries(tests_blackmisc_containers PUBLIC misc tests_test Qt::Core)

add_executable(tests_blackmisc_datastream testdatastream/testdatastream.cpp)
target_link_libraries(tests_blackmisc_datastream PUBLIC misc tests_test Qt::Core)

add_executable(tests_blackmisc_dbus testdbus/testdbus.cpp)
target_link_libraries(tests_blackmisc_dbus PUBLIC misc tests_test Qt::Core Qt::DBus)

add_executable(tests_blackmisc_icon testicon/testicon.cpp)
target_link_libraries(tests_blackmisc_icon PUBLIC misc tests_test Qt::Core)

add_executable(tests_blackmisc_identifier testidentifier/testidentifier.cpp)
target_link_libraries(tests_blackmisc_identifier PUBLIC misc tests_test Qt::Core)

add_executable(tests_blackmisc_librarypath testlibrarypath/testlibrarypath.cpp)
target_link_libraries(tests_blackmisc_librarypath PUBLIC blackconfig misc tests_test Qt::Core)

add_executable(tests_blackmisc_process testprocess/testprocess.cpp)
target_link_libraries(tests_blackmisc_process PUBLIC misc tests_test Qt::Core)

add_executable(tests_blackmisc_propertyindex testpropertyindex/testpropertyindex.cpp)
target_link_libraries(tests_blackmisc_propertyindex PUBLIC misc tests_test Qt::Core)

#add_executable(tests_blackmisc_sharedstate testpropertyindex/testpropertyindex.cpp)
#target_link_libraries(tests_blackmisc_sharedstate PUBLIC misc tests_test Qt::Core)

add_executable(tests_blackmisc_slot testslot/testslot.cpp)
target_link_libraries(tests_blackmisc_slot PUBLIC misc tests_test Qt::Core)

add_executable(tests_blackmisc_statusmessage teststatusmessage/teststatusmessage.cpp)
target_link_libraries(tests_blackmisc_statusmessage PUBLIC misc tests_test Qt::Core)

add_executable(tests_blackmisc_stringutils teststringutils/teststringutils.cpp)
target_link_libraries(tests_blackmisc_stringutils PUBLIC misc tests_test Qt::Core)

add_executable(tests_blackmisc_valuecache testvaluecache/testvaluecache.cpp)
target_link_libraries(tests_blackmisc_valuecache PUBLIC misc tests_test Qt::Core)

add_executable(tests_blackmisc_variantandmap testvariantandmap/testvariantandmap.cpp)
target_link_libraries(tests_blackmisc_variantandmap PUBLIC misc tests_test Qt::Core)

#add_executable(tests_blackmisc_worker testworker/testworker.cpp)
#target_link_libraries(tests_blackmisc_worker PUBLIC misc tests_test Qt::Core)

add_executable(tests_blackmisc_weather weather/testweather/testweather.cpp)
target_link_libraries(tests_blackmisc_weather PUBLIC misc tests_test Qt::Core)
