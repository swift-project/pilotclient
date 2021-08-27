/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_MATCHINGLOG_H
#define BLACKMISC_SIMULATION_MATCHINGLOG_H

#include "blackmisc/blackmiscexport.h"
#include <QStringList>
#include <QMetaType>
#include <QDBusArgument>

namespace BlackMisc
{
    namespace Simulation
    {
        //! What to log
        enum MatchingLogFlag
        {
            MatchingLogNothing             = 0,
            MatchingLogIntro               = 1 << 0,
            MatchingLogModelstring         = 1 << 1,
            MatchingLogStepwiseReduce      = 1 << 2,
            MatchingLogScoring             = 1 << 3,
            MatchingLogCombinedDefaultType = 1 << 4,
            MatchingLogSimplified = MatchingLogIntro,
            MatchingLogAll        = MatchingLogModelstring | MatchingLogStepwiseReduce | MatchingLogScoring | MatchingLogCombinedDefaultType
        };
        Q_DECLARE_FLAGS(MatchingLog, MatchingLogFlag)

        //! Log flag to string
        BLACKMISC_EXPORT QString matchingLogToString(MatchingLog log);

        //! Log flag to string
        BLACKMISC_EXPORT const QString &matchingLogFlagToString(MatchingLogFlag logFlag);
    } // ns
} // ns

Q_DECLARE_METATYPE(BlackMisc::Simulation::MatchingLog)
Q_DECLARE_METATYPE(BlackMisc::Simulation::MatchingLogFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(BlackMisc::Simulation::MatchingLog)

#endif // guard
