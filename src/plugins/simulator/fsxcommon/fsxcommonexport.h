/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_FSXCOMMON_SIMULATORFSXCOMMON_MACROS_H
#define BLACKSIMPLUGIN_FSXCOMMON_SIMULATORFSXCOMMON_MACROS_H

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

#endif // guard
