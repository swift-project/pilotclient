# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_library(externals_rapidjson INTERFACE)
target_include_directories(externals_rapidjson SYSTEM INTERFACE ${PROJECT_SOURCE_DIR}/externals/common/include)
