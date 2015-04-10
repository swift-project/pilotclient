/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_MACROS_H
#define BLACKGUI_MACROS_H

#include <QtGlobal>

#ifndef WITH_STATIC
#  if defined(BUILD_BLACKGUI_LIB)
#    define BLACKGUI_EXPORT Q_DECL_EXPORT
#  else
#    define BLACKGUI_EXPORT Q_DECL_IMPORT
#  endif
#else
#  define BLACKGUI_EXPORT
#endif

#endif // BLACKGUI_MACROS_H
