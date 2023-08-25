// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKSOUND_BLACKSOUNDEXPORT_H
#define BLACKSOUND_BLACKSOUNDEXPORT_H

#include <QtGlobal>

/*!
 * \def BLACKSOUND_EXPORT
 * Export a class or function from the library
 */

#ifndef WITH_STATIC
#    if defined(BUILD_BLACKSOUND_LIB)
#        define BLACKSOUND_EXPORT Q_DECL_EXPORT
#    else
#        define BLACKSOUND_EXPORT Q_DECL_IMPORT
#    endif
#else
#    define BLACKSOUND_EXPORT
#endif

#endif // guard
