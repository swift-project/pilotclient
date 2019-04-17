/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_INPUT_ACTIONHOTKEYDEFS_H
#define BLACKMISC_INPUT_ACTIONHOTKEYDEFS_H

#include "blackmisc/icon.h"
#include "blackmisc/blackmiscexport.h"

#include <QString>

namespace BlackMisc
{
    namespace Input
    {
        //! PTT key
        BLACKMISC_EXPORT const QString &pttHotkeyAction();

        //! PTT key
        BLACKMISC_EXPORT const QPixmap &pttHotkeyIcon();
    } // ns
} // ns

#endif // guard
