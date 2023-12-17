# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_library(QJsonWebToken STATIC)
target_sources(QJsonWebToken
    PRIVATE
        qjsonwebtoken.cpp
        qjsonwebtoken.h
)
target_link_libraries(QJsonWebToken
    PUBLIC
        Qt::Core
)
target_include_directories(QJsonWebToken
    PUBLIC
        ${PROJECT_SOURCE_DIR}/third_party
)
