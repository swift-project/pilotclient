# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_executable(samples_weatherdata
        main.cpp
        reader.cpp
        reader.h
        sampleweatherdata.h
        weatherdataprinter.cpp
        weatherdataprinter.h
        )
target_link_libraries(samples_weatherdata core misc Qt::Core)
