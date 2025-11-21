// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_SWIFT_GUIEXPORT_H
#define SWIFT_GUI_SWIFT_GUIEXPORT_H

#include <QtGlobal>

/*!
 * \def SWIFT_GUI_EXPORT
 * Export a class or function from the library
 */
#ifndef WITH_STATIC
#    ifdef BUILD_SWIFT_GUI_LIB
#        define SWIFT_GUI_EXPORT Q_DECL_EXPORT
#    else
#        define SWIFT_GUI_EXPORT Q_DECL_IMPORT
#    endif
#else
#    define SWIFT_GUI_EXPORT
#endif

/*!
 * \def SWIFT_GUI_EXPORT_DECLARE_TEMPLATE
 * SWIFT_GUI Export explicit template declaration
 */
#if defined(Q_OS_WIN) && defined(Q_CC_GNU)
#    define SWIFT_GUI_EXPORT_DECLARE_TEMPLATE SWIFT_GUI_EXPORT
#else
#    define SWIFT_GUI_EXPORT_DECLARE_TEMPLATE
#endif

#endif // SWIFT_GUI_SWIFT_GUIEXPORT_H
