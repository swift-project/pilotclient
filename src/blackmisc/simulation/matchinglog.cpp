/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "matchinglog.h"
#include <QStringList>

namespace BlackMisc
{
    namespace Simulation
    {
        //! Log flag to string
        const QString &matchingLogFlagToString(MatchingLogFlag logFlag)
        {
            static const QString logNothing("nothing");
            static const QString logModelstring("model string");
            static const QString logStepwiseReduce("step wise reduce");
            static const QString logScoring("scoring");
            static const QString logCombinedDefaultType("combined default type");
            static const QString logSimplified("simplified");
            static const QString logAll("all");

            switch (logFlag)
            {
            case MatchingLogCombinedDefaultType: return logCombinedDefaultType;
            case MatchingLogNothing:        return logNothing;
            case MatchingLogModelstring:    return logModelstring;
            case MatchingLogStepwiseReduce: return logStepwiseReduce;
            case MatchingLogScoring:        return logScoring;
            case MatchingLogSimplified:     return logSimplified;
            case MatchingLogAll:            return logAll;
            default: break;
            }

            static const QString unknown("unknown");
            return unknown;
        }

        //! Log flag to string
        QString matchingLogToString(MatchingLog log)
        {
            if (log == MatchingLogNothing) { return matchingLogFlagToString(MatchingLogNothing); }
            QStringList l;
            if (log.testFlag(MatchingLogCombinedDefaultType)) { l << matchingLogFlagToString(MatchingLogCombinedDefaultType); }
            if (log.testFlag(MatchingLogModelstring))    { l << matchingLogFlagToString(MatchingLogModelstring); }
            if (log.testFlag(MatchingLogStepwiseReduce)) { l << matchingLogFlagToString(MatchingLogStepwiseReduce); }
            if (log.testFlag(MatchingLogScoring))        { l << matchingLogFlagToString(MatchingLogScoring); }
            return l.join(", ");
        }
    } // ns
} // ns
