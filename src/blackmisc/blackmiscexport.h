/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_BLACKMISCEXPORT_H
#define BLACKMISC_BLACKMISCEXPORT_H

#include <QtGlobal>

/*!
 * \def BLACKMISC_EXPORT
 * Export a class or function from the library
 */
#ifndef WITH_STATIC
#  if defined(BUILD_BLACKMISC_LIB)
#    define BLACKMISC_EXPORT Q_DECL_EXPORT
#  else
#    define BLACKMISC_EXPORT Q_DECL_IMPORT
#  endif
#else
#  define BLACKMISC_EXPORT
#endif

/*!
 * \def BLACKMISC_EXPORT_DECLARE_TEMPLATE
 * Export explicit template declaration
 */
#if defined(Q_OS_WIN) && defined(Q_CC_GNU)
#  define BLACKMISC_EXPORT_DECLARE_TEMPLATE BLACKMISC_EXPORT
#else
#  define BLACKMISC_EXPORT_DECLARE_TEMPLATE
#endif

/*!
 * \def BLACK_NO_INLINE
 * Prevent function inlining
 */
#ifdef Q_CC_MSVC
#define BLACK_NO_INLINE __declspec(noinline)
#else
#define BLACK_NO_INLINE __attribute__((noinline))
#endif

#endif // guard
