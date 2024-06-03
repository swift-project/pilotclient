# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_library(SimpleCrypt STATIC)
target_sources(SimpleCrypt
    PRIVATE
        simplecrypt.cpp
        simplecrypt.h
)
target_link_libraries(SimpleCrypt
    PUBLIC
        Qt::Core
)
target_include_directories(SimpleCrypt
    PUBLIC
        ${PROJECT_SOURCE_DIR}/third_party
)
set_property(TARGET SimpleCrypt PROPERTY POSITION_INDEPENDENT_CODE ON)
