// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKINPUT_BLACKINPUTEXPORT_H
#define BLACKINPUT_BLACKINPUTEXPORT_H

#include <QtGlobal>

/*!
 * \def BLACKINPUT_EXPORT
 * Export a class or function from the library
 */

#ifndef WITH_STATIC
#    if defined(BUILD_BLACKINPUT_LIB)
#        define BLACKINPUT_EXPORT Q_DECL_EXPORT
#    else
#        define BLACKINPUT_EXPORT Q_DECL_IMPORT
#    endif
#else
#    define BLACKINPUT_EXPORT
#endif

#endif // guard
