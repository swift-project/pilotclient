// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SIMPLUGIN_MSFS2024_SIMULATORMSFS2024_MACROS_H
#define SWIFT_SIMPLUGIN_MSFS2024_SIMULATORMSFS2024_MACROS_H

#include <QtGlobal>

/*!
 * \def MSFS2024_EXPORT
 * MSFS2024 Export Macro
 */

#ifndef WITH_STATIC
#    if defined(BUILD_MSFS2024_LIB)
#        define MSFS2024_EXPORT Q_DECL_EXPORT
#    else
#        define MSFS2024_EXPORT Q_DECL_IMPORT
#    endif
#else
#    define MSFS2024_EXPORT
#endif

#endif // SWIFT_SIMPLUGIN_MSFS2024_SIMULATORMSFS2024_MACROS_H
