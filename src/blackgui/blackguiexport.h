// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_BLACKGUIEXPORT_H
#define BLACKGUI_BLACKGUIEXPORT_H

#include <QtGlobal>

/*!
 * \def BLACKGUI_EXPORT
 * Export a class or function from the library
 */
#ifndef WITH_STATIC
#    if defined(BUILD_BLACKGUI_LIB)
#        define BLACKGUI_EXPORT Q_DECL_EXPORT
#    else
#        define BLACKGUI_EXPORT Q_DECL_IMPORT
#    endif
#else
#    define BLACKGUI_EXPORT
#endif

/*!
 * \def BLACKGUI_EXPORT_DECLARE_TEMPLATE
 * BLACKGUI Export explicit template declaration
 */
#if defined(Q_OS_WIN) && defined(Q_CC_GNU)
#    define BLACKGUI_EXPORT_DECLARE_TEMPLATE BLACKGUI_EXPORT
#else
#    define BLACKGUI_EXPORT_DECLARE_TEMPLATE
#endif

#endif // guard
