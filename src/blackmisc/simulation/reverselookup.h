/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
