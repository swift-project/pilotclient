# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_library(fscommon SHARED
#        fsuipc.h
#        fsuipcdummy.cpp
#        fsuipcimpl.cpp
        simulatorfscommon.cpp
        simulatorfscommon.h
        simulatorfscommonfunctions.h
        fscommonexport.h
)

target_compile_definitions(fscommon PRIVATE BUILD_FSCOMMON_LIB)

target_link_libraries(fscommon
        PUBLIC
        Qt::Core
        misc
        plugincommon
        PRIVATE
        core

)

set_target_properties(fscommon PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin)
set_target_properties(fscommon PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin)

install(TARGETS fscommon
        LIBRARY DESTINATION bin
        RUNTIME DESTINATION bin
)
