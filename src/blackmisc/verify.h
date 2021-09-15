/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_VERIFY_H
#define BLACKMISC_VERIFY_H

#include "blackmisc/blackmiscexport.h"

//! \cond
#ifdef Q_CC_MSVC
#define BLACK_NO_INLINE __declspec(noinline)
#else
#define BLACK_NO_INLINE __attribute__((noinline))
#endif
//! \endcond

namespace BlackMisc::Private
{
    //! \private Do nothing.
    inline void noop() {}

    //! \private Called by BLACK_VERIFY when the condition is false.
    BLACKMISC_EXPORT BLACK_NO_INLINE void failedVerify(const char *condition, const char *filename, int line, const char *context, const char *message, bool audit);
}

/*!
 * A weaker kind of assert. Still indicates a programmer mistake, but one which is recoverable.
 * In debug builds, triggers a debugger breakpoint. In release builds, generates a warning.
 */
//! @{
#define BLACK_VERIFY_X(COND, WHERE, WHAT) ((COND) ? BlackMisc::Private::noop() : BlackMisc::Private::failedVerify(#COND, __FILE__, __LINE__, WHERE, WHAT, false))
#define BLACK_VERIFY(COND) BLACK_VERIFY_X(COND, nullptr, nullptr)
//! @}

/*!
 * A weaker kind of verify. Indicative of a serious but recoverable problem originating in an external data source.
 * In debug builds under debugging, triggers a debugger breakpoint. Otherwise generates a warning.
 * Not a substitute for proper validation. A failed audit in production is suggestive of insufficient validation.
 */
//! @{
#define BLACK_AUDIT_X(COND, WHERE, WHAT) ((COND) ? BlackMisc::Private::noop() : BlackMisc::Private::failedVerify(#COND, __FILE__, __LINE__, WHERE, WHAT, true))
#define BLACK_AUDIT(COND) BLACK_AUDIT_X(COND, nullptr, nullptr)
//! @}

#endif
