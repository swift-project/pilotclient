# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_executable(samples_miscsim
        main.cpp
        samplemiscsim.h
        samplesfscommon.cpp
        samplesfscommon.h
#        samplesfsuipc.cpp
#        samplesfsuipc.h
        samplesfsx.cpp
        samplesfsx.h
        samplesmodelmapping.cpp
        samplesmodelmapping.h
        samplesp3d.cpp
        samplesp3d.h
        samplesvpilotrules.cpp
        samplesvpilotrules.h
        sampleutils.cpp
        sampleutils.h
        )
target_link_libraries(samples_miscsim core misc Qt::Core)

if(WIN32)
        target_link_libraries(samples_miscsim fscommon)
endif()
