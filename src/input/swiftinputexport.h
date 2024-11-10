// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_INPUT_SWIFT_INPUTEXPORT_H
#define SWIFT_INPUT_SWIFT_INPUTEXPORT_H

#include <QtGlobal>

/*!
 * \def SWIFT_INPUT_EXPORT
 * Export a class or function from the library
 */

#ifndef WITH_STATIC
#    if defined(BUILD_SWIFT_INPUT_LIB)
#        define SWIFT_INPUT_EXPORT Q_DECL_EXPORT
#    else
#        define SWIFT_INPUT_EXPORT Q_DECL_IMPORT
#    endif
#else
#    define SWIFT_INPUT_EXPORT
#endif

#endif // guard
