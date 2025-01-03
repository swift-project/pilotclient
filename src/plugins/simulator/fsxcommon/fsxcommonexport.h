// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SIMPLUGIN_FSXCOMMON_SIMULATORFSXCOMMON_MACROS_H
#define SWIFT_SIMPLUGIN_FSXCOMMON_SIMULATORFSXCOMMON_MACROS_H

#include <QtGlobal>

/*!
 * \def FSXCOMMON_EXPORT
 * FSXCommon Export Macro
 */

#ifndef WITH_STATIC
#    if defined(BUILD_FSXCOMMON_LIB)
#        define FSXCOMMON_EXPORT Q_DECL_EXPORT
#    else
#        define FSXCOMMON_EXPORT Q_DECL_IMPORT
#    endif
#else
#    define FSXCOMMON_EXPORT
#endif

#endif // SWIFT_SIMPLUGIN_FSXCOMMON_SIMULATORFSXCOMMON_MACROS_H
