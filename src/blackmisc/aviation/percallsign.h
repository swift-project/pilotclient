// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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
