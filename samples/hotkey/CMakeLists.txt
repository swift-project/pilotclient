# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_executable(samples_hotkey
        main.cpp
        samplehotkey.h
        )
target_link_libraries(samples_hotkey gui misc Qt::Core)
