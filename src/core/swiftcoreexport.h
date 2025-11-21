// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_SWIFT_COREEXPORT_H
#define SWIFT_CORE_SWIFT_COREEXPORT_H

#include <QtGlobal>

/*!
 * \def SWIFT_CORE_EXPORT
 * Export a class or function from the library
 */

#ifndef WITH_STATIC
#    ifdef BUILD_SWIFT_CORE_LIB
#        define SWIFT_CORE_EXPORT Q_DECL_EXPORT
#    else
#        define SWIFT_CORE_EXPORT Q_DECL_IMPORT
#    endif
#else
#    define SWIFT_CORE_EXPORT
#endif

#endif // SWIFT_CORE_SWIFT_COREEXPORT_H
