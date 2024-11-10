// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_AVIATION_PERCALLSIGN_H
#define SWIFT_MISC_AVIATION_PERCALLSIGN_H

#include "misc/aviation/callsign.h"
#include "misc/pq/length.h"
#include "misc/statusmessagelist.h"

#include <QHash>

namespace swift::misc::aviation
{
    //! Status message per callsign
    using CStatusMessagePerCallsign = QHash<CCallsign, CStatusMessage>;

    //! Status messages (list) per callsign
    using CStatusMessageListPerCallsign = QHash<CCallsign, CStatusMessageList>;

    //! Timestamp der callsign
    using CTimestampPerCallsign = QHash<CCallsign, qint64>;

    //! Lenght per callsign
    using CLengthPerCallsign = QHash<CCallsign, physical_quantities::CLength>;

} // namespace

#endif // guard
