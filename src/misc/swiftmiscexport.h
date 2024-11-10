// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SWIFT_MISCEXPORT_H
#define SWIFT_MISC_SWIFT_MISCEXPORT_H

#include <QtGlobal>

/*!
 * \def SWIFT_MISC_EXPORT
 * Export a class or function from the library
 */
#ifndef WITH_STATIC
#    if defined(BUILD_SWIFT_MISC_LIB)
#        define SWIFT_MISC_EXPORT Q_DECL_EXPORT
#    else
#        define SWIFT_MISC_EXPORT Q_DECL_IMPORT
#    endif
#else
#    define SWIFT_MISC_EXPORT
#endif

/*!
 * \def SWIFT_MISC_EXPORT_DECLARE_TEMPLATE
 * Export explicit template declaration
 */
#if defined(Q_OS_WIN) && defined(Q_CC_GNU)
#    define SWIFT_MISC_EXPORT_DECLARE_TEMPLATE SWIFT_MISC_EXPORT
#else
#    define SWIFT_MISC_EXPORT_DECLARE_TEMPLATE
#endif

/*!
 * \def SWIFT_NO_INLINE
 * Prevent function inlining
 */
#ifdef Q_CC_MSVC
#    define SWIFT_NO_INLINE __declspec(noinline)
#else
#    define SWIFT_NO_INLINE __attribute__((noinline))
#endif

#endif // guard
