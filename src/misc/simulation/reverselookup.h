// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_REVERSELOOKUP_H
#define SWIFT_MISC_SIMULATION_REVERSELOOKUP_H

#include <QMetaType>
#include <QDBusArgument>

namespace swift::misc::simulation
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

Q_DECLARE_METATYPE(swift::misc::simulation::ReverseLookupLogging)
Q_DECLARE_METATYPE(swift::misc::simulation::ReverseLookupLoggingFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(swift::misc::simulation::ReverseLookupLogging)

#endif // guard
