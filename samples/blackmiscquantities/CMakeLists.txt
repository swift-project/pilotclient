# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_executable(samples_pq
        main.cpp
        sampleblackmiscquantities.h
        samplesaviation.cpp
        samplesaviation.h
        samplesgeo.cpp
        samplesgeo.h
        samplesphysicalquantities.cpp
        samplesphysicalquantities.h
)
target_link_libraries(samples_pq core misc Qt::Core)
