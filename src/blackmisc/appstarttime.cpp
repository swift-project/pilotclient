/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/appstarttime.h"

namespace BlackMisc
{
    const QDateTime &getApplicationStartTimeUtc()
    {
        static const QDateTime gApplicationStartTimeUtc = QDateTime::currentDateTimeUtc();
        return gApplicationStartTimeUtc;
    }
}
