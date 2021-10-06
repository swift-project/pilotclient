/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_MATCHINGSTATISTICS_H
#define BLACKMISC_SIMULATION_MATCHINGSTATISTICS_H

#include "blackmisc/simulation/matchingstatisticsentry.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/timestampobjectlist.h"
#include "blackmisc/sequence.h"
#include "blackmisc/collection.h"
#include "blackmisc/mixin/mixinmetatype.h"

BLACK_DECLARE_SEQUENCE_MIXINS(BlackMisc::Simulation, CMatchingStatisticsEntry, CMatchingStatistics)

namespace BlackMisc::Simulation
{
    //! Value object for matching statistics.
    class BLACKMISC_EXPORT CMatchingStatistics :
        public BlackMisc::CSequence<CMatchingStatisticsEntry>,
        public BlackMisc::ITimestampObjectList<CMatchingStatisticsEntry, CMatchingStatistics>,
        public BlackMisc::Mixin::MetaType<CMatchingStatistics>
    {
    public:
        BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CMatchingStatistics)
        using CSequence::CSequence;

        //! Default constructor.
        CMatchingStatistics();

        //! Construct from a base class object.
        CMatchingStatistics(const CSequence<CMatchingStatisticsEntry> &other);

        //! Find by session id
        CMatchingStatistics findBySessionId(const QString &sessionId) const;

        //! Find entires denoting missing entries only
        CMatchingStatistics findMissingOnly() const;

        //! Contains session id
        bool containsSessionId(const QString &sessionId) const;

        //! Contains given aircraft / airline combination
        bool containsAircraftAirlineCombination(const QString &aircraftDesignator, const QString &airlineDesignator) const;

        //! Contains given aircraft / airline combination
        bool containsAircraftAirlineCombination(const QString &sessionId, const QString &aircraftDesignator, const QString &airlineDesignator) const;

        //! Increase count if found
        bool increaseCountIfFound(CMatchingStatisticsEntry::EntryType type, const QString &sessionId, const QString &aircraftDesignator, const QString &airlineDesignator = {});

        //! Add a combination, normally with no duplicates (in that case count is increased
        void addAircraft(CMatchingStatisticsEntry::EntryType type, const QString &sessionId, const QString &modelSetId, const QString &description, const QString &aircraftDesignator, bool avoidDuplicates = true);

        //! Add a combination, normally with no duplicates (in that case count is increased
        void addAircraftAirlineCombination(CMatchingStatisticsEntry::EntryType type, const QString &sessionId, const QString &modelSetId, const QString &description, const QString &aircraftDesignator, const QString &airlineDesignator, bool avoidDuplicates = true);
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Simulation::CMatchingStatistics)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Simulation::CMatchingStatisticsEntry>)

#endif // guard
