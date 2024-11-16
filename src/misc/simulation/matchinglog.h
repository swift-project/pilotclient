// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_MATCHINGLOG_H
#define SWIFT_MISC_SIMULATION_MATCHINGLOG_H

#include <QDBusArgument>
#include <QMetaType>
#include <QStringList>

#include "misc/swiftmiscexport.h"

namespace swift::misc::simulation
{
    //! What to log
    enum MatchingLogFlag
    {
        MatchingLogNothing = 0,
        MatchingLogIntro = 1 << 0,
        MatchingLogModelstring = 1 << 1,
        MatchingLogStepwiseReduce = 1 << 2,
        MatchingLogScoring = 1 << 3,
        MatchingLogCombinedDefaultType = 1 << 4,
        MatchingLogSimplified = MatchingLogIntro,
        MatchingLogAll = MatchingLogModelstring | MatchingLogStepwiseReduce | MatchingLogScoring | MatchingLogCombinedDefaultType
    };
    Q_DECLARE_FLAGS(MatchingLog, MatchingLogFlag)

    //! Log flag to string
    SWIFT_MISC_EXPORT QString matchingLogToString(MatchingLog log);

    //! Log flag to string
    SWIFT_MISC_EXPORT const QString &matchingLogFlagToString(MatchingLogFlag logFlag);
} // namespace swift::misc::simulation

Q_DECLARE_METATYPE(swift::misc::simulation::MatchingLog)
Q_DECLARE_METATYPE(swift::misc::simulation::MatchingLogFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(swift::misc::simulation::MatchingLog)

#endif // guard
