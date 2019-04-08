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

namespace BlackMisc
{
    namespace Simulation
    {
        //! Lookup log.messages
        enum ReverseLookupLoggingFlag
        {
            RevLogDisabled          = 0,
            RevLogEnabled           = 1 << 0,
            RevLogSimplifiedInfo    = 1 << 1,
            RevLogEnabledSimplified = RevLogEnabled | RevLogSimplifiedInfo
        };
        Q_DECLARE_FLAGS(ReverseLookupLogging, ReverseLookupLoggingFlag)
    } // ns
} // ns

Q_DECLARE_METATYPE(BlackMisc::Simulation::ReverseLookupLogging)
Q_DECLARE_METATYPE(BlackMisc::Simulation::ReverseLookupLoggingFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(BlackMisc::Simulation::ReverseLookupLogging)

/*!
 * Operator for streaming enums to QDBusArgument.
 */
inline QDBusArgument &operator <<(QDBusArgument &arg, const BlackMisc::Simulation::ReverseLookupLogging &value)
{
    arg.beginStructure();
    arg << static_cast<int>(value);
    arg.endStructure();
    return arg;
}

/*!
 * Operator for streaming enums from QDBusArgument.
 */
inline const QDBusArgument &operator >>(const QDBusArgument &arg, BlackMisc::Simulation::ReverseLookupLogging &value)
{
    int temp;
    arg.beginStructure();
    arg >> temp;
    arg.endStructure();
    value = static_cast<BlackMisc::Simulation::ReverseLookupLogging>(temp);
    return arg;
}

#endif // guard
