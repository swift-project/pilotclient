// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_SIMULATION_REVERSELOOKUP_H
#define BLACKMISC_SIMULATION_REVERSELOOKUP_H

#include <QMetaType>
#include <QDBusArgument>

namespace BlackMisc::Simulation
{
    //! Lookup log.messages
    enum ReverseLookupLoggingFlag
    {
        RevLogDisabled = 0,
        RevLogEnabled = 1 << 0,
        RevLogSimplifiedInfo = 1 << 1,
        RevLogEnabledSimplified = RevLogEnabled | RevLogSimplifiedInfo
    };
    Q_DECLARE_FLAGS(ReverseLookupLogging, ReverseLookupLoggingFlag)
} // ns

Q_DECLARE_METATYPE(BlackMisc::Simulation::ReverseLookupLogging)
Q_DECLARE_METATYPE(BlackMisc::Simulation::ReverseLookupLoggingFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(BlackMisc::Simulation::ReverseLookupLogging)

#endif // guard
