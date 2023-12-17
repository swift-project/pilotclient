# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_library(externals_msgpack INTERFACE)
target_include_directories(externals_msgpack INTERFACE ${PROJECT_SOURCE_DIR}/third_party/externals/common/include)
