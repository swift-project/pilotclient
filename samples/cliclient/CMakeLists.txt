# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_executable(samples_cliclient
        main.cpp
        reader.cpp
        reader.h
        samplecliclient.h
        )
target_link_libraries(samples_cliclient core misc Qt::Core)
