// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKSIMPLUGIN_SIMULATORPLUGINCOMMON_MACROS_H
#define BLACKSIMPLUGIN_SIMULATORPLUGINCOMMON_MACROS_H

#include <QtGlobal>

/*!
 * \def SIMULATORPLUGINCOMMON_EXPORT
 * SimulatorPluginCommon Export Macro
 */

#ifndef WITH_STATIC
#    if defined(BUILD_SIMULATORPLUGINCOMMON_LIB)
#        define SIMULATORPLUGINCOMMON_EXPORT Q_DECL_EXPORT
#    else
#        define SIMULATORPLUGINCOMMON_EXPORT Q_DECL_IMPORT
#    endif
#else
#    define SIMULATORPLUGINCOMMON_EXPORT
#endif

#endif // guard
