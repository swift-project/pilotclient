# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

file(GLOB UNUSED_DLLS ${CMAKE_INSTALL_PREFIX}/vcredist/*.dll)
file(REMOVE ${UNUSED_DLLS})
