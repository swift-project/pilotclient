/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
