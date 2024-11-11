// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_VERIFY_H
#define BLACKMISC_VERIFY_H

#include "blackmisc/blackmiscexport.h"

namespace BlackMisc::Private
{
    //! \private Do nothing.
    inline void noop() {}

    //! \private Called by SWIFT_VERIFY when the condition is false.
    BLACKMISC_EXPORT SWIFT_NO_INLINE void failedVerify(const char *condition, const char *filename, int line, const char *context, const char *message, bool audit);
}

//! @{
/*!
 * A weaker kind of assert. Still indicates a programmer mistake, but one which is recoverable.
 * In debug builds, triggers a debugger breakpoint. In release builds, generates a warning.
 */
#define SWIFT_VERIFY_X(COND, WHERE, WHAT) ((COND) ? BlackMisc::Private::noop() : BlackMisc::Private::failedVerify(#COND, __FILE__, __LINE__, WHERE, WHAT, false))
#define SWIFT_VERIFY(COND) SWIFT_VERIFY_X(COND, nullptr, nullptr)
//! @}

//! @{
/*!
 * A weaker kind of verify. Indicative of a serious but recoverable problem originating in an external data source.
 * In debug builds under debugging, triggers a debugger breakpoint. Otherwise generates a warning.
 * Not a substitute for proper validation. A failed audit in production is suggestive of insufficient validation.
 */
#define SWIFT_AUDIT_X(COND, WHERE, WHAT) ((COND) ? BlackMisc::Private::noop() : BlackMisc::Private::failedVerify(#COND, __FILE__, __LINE__, WHERE, WHAT, true))
#define SWIFT_AUDIT(COND) SWIFT_AUDIT_X(COND, nullptr, nullptr)
//! @}

#endif
