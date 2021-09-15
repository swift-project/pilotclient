/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_PERCALLSIGN_H
#define BLACKMISC_AVIATION_PERCALLSIGN_H

#include "blackmisc/aviation/callsign.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/statusmessagelist.h"

#include <QHash>

namespace BlackMisc::Aviation
{
    //! Status message per callsign
    using CStatusMessagePerCallsign = QHash<CCallsign, CStatusMessage>;

    //! Status messages (list) per callsign
    using CStatusMessageListPerCallsign = QHash<CCallsign, CStatusMessageList>;

    //! Timestamp der callsign
    using CTimestampPerCallsign = QHash<CCallsign, qint64>;

    //! Lenght per callsign
    using CLengthPerCallsign = QHash<CCallsign, PhysicalQuantities::CLength>;

} // namespace

#endif // guard
