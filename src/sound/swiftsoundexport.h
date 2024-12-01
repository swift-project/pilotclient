// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SOUND_SWIFT_SOUNDEXPORT_H
#define SWIFT_SOUND_SWIFT_SOUNDEXPORT_H

#include <QtGlobal>

/*!
 * \def SWIFT_SOUND_EXPORT
 * Export a class or function from the library
 */

#ifndef WITH_STATIC
#    if defined(BUILD_SWIFT_SOUND_LIB)
#        define SWIFT_SOUND_EXPORT Q_DECL_EXPORT
#    else
#        define SWIFT_SOUND_EXPORT Q_DECL_IMPORT
#    endif
#else
#    define SWIFT_SOUND_EXPORT
#endif

#endif // SWIFT_SOUND_SWIFT_SOUNDEXPORT_H
