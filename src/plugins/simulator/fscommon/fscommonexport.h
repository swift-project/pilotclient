/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
