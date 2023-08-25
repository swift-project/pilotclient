// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_BLACKCOREEXPORT_H
#define BLACKCORE_BLACKCOREEXPORT_H

#include <QtGlobal>

/*!
 * \def BLACKCORE_EXPORT
 * Export a class or function from the library
 */

#ifndef WITH_STATIC
#    if defined(BUILD_BLACKCORE_LIB)
#        define BLACKCORE_EXPORT Q_DECL_EXPORT
#    else
#        define BLACKCORE_EXPORT Q_DECL_IMPORT
#    endif
#else
#    define BLACKCORE_EXPORT
#endif

#endif // guard
