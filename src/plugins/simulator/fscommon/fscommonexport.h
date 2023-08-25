// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKSIMPLUGIN_FSCOMMON_SIMULATORFSCOMMON_MACROS_H
#define BLACKSIMPLUGIN_FSCOMMON_SIMULATORFSCOMMON_MACROS_H

#include <QtGlobal>

/*!
 * \def FSCOMMON_EXPORT
 * FSCommon Export Macro
 */

#ifndef WITH_STATIC
#    if defined(BUILD_FSCOMMON_LIB)
#        define FSCOMMON_EXPORT Q_DECL_EXPORT
#    else
#        define FSCOMMON_EXPORT Q_DECL_IMPORT
#    endif
#else
#    define FSCOMMON_EXPORT
#endif

#endif // guard
