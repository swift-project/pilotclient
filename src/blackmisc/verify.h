/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_VERIFY_H
#define BLACKMISC_VERIFY_H

#include "blackmisc/blackmiscexport.h"
#include <QtGlobal>

namespace BlackMisc
{
    namespace Private
    {
        //! \private Do nothing.
        inline void noop() {}

        //! \private Called by BLACK_VERIFY when the condition is false.
        BLACKMISC_EXPORT void failedVerify(const char *condition, const char *filename, int line, const char *context = nullptr, const char *message = nullptr);
    }
}

/*!
 * A weaker kind of assert. Still indicates a programmer mistake, but one which is recoverable.
 * In debug builds, triggers a debugger breakpoint. In release builds, generates a warning.
 */
//! @{
#define BLACK_VERIFY_X(COND, WHERE, WHAT) ((COND) ? BlackMisc::Private::noop() : BlackMisc::Private::failedVerify(#COND, __FILE__, __LINE__, WHERE, WHAT))
#define BLACK_VERIFY(COND) BLACK_VERIFY_X(COND, nullptr, nullptr)
//! @}

#endif
