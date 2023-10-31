# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_library(weatherdatagfs SHARED
        weatherdatagfs.cpp
        weatherdatagfs.h
        weatherdatagfs.json

        # g2clib
        g2clib/drstemplates.h
        g2clib/grib2.h
        g2clib/gridtemplates.h
        g2clib/pdstemplates.h
        g2clib/cmplxpack.c
        g2clib/compack.c
        g2clib/comunpack.c
        g2clib/drstemplates.c
        g2clib/g2_addfield.c
        g2clib/g2_addgrid.c
        g2clib/g2_addlocal.c
        g2clib/g2_create.c
        g2clib/g2_free.c
        g2clib/g2_getfld.c
        g2clib/g2_gribend.c
        g2clib/g2_info.c
        g2clib/g2_miss.c
        g2clib/g2_unpack1.c
        g2clib/g2_unpack2.c
        g2clib/g2_unpack3.c
        g2clib/g2_unpack4.c
        g2clib/g2_unpack5.c
        g2clib/g2_unpack6.c
        g2clib/g2_unpack7.c
        g2clib/gbits.c
        g2clib/getdim.c
        g2clib/getpoly.c
        g2clib/gridtemplates.c
        g2clib/int_power.c
        g2clib/misspack.c
        g2clib/mkieee.c
        g2clib/pack_gp.c
        g2clib/pdstemplates.c
        g2clib/rdieee.c
        g2clib/reduce.c
        g2clib/seekgb.c
        g2clib/simpack.c
        g2clib/simunpack.c
        g2clib/specpack.c
        g2clib/specunpack.c
)

set_target_properties(weatherdatagfs PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin/plugins/weatherdata)
set_target_properties(weatherdatagfs PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin/plugins/weatherdata)


target_include_directories(weatherdatagfs PUBLIC ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(weatherdatagfs
        PUBLIC
        Qt::Core
        misc
        core
)

install(TARGETS weatherdatagfs
    LIBRARY DESTINATION bin/plugins/weatherdata
    RUNTIME DESTINATION bin/plugins/weatherdata
)

if(APPLE)
    set_target_properties(weatherdatagfs PROPERTIES INSTALL_RPATH @loader_path/../../../lib)
elseif(UNIX)
    set_target_properties(weatherdatagfs PROPERTIES INSTALL_RPATH \$ORIGIN/../../../lib)
endif()
