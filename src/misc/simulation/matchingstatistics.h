// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_MATCHINGSTATISTICS_H
#define SWIFT_MISC_SIMULATION_MATCHINGSTATISTICS_H

#include "misc/collection.h"
#include "misc/mixin/mixinmetatype.h"
#include "misc/sequence.h"
#include "misc/simulation/matchingstatisticsentry.h"
#include "misc/swiftmiscexport.h"
#include "misc/timestampobjectlist.h"

SWIFT_DECLARE_SEQUENCE_MIXINS(swift::misc::simulation, CMatchingStatisticsEntry, CMatchingStatistics)

namespace swift::misc::simulation
{
    //! Value object for matching statistics.
    class SWIFT_MISC_EXPORT CMatchingStatistics :
        public swift::misc::CSequence<CMatchingStatisticsEntry>,
        public swift::misc::ITimestampObjectList<CMatchingStatisticsEntry, CMatchingStatistics>,
        public swift::misc::mixin::MetaType<CMatchingStatistics>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CMatchingStatistics)
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
        bool containsAircraftAirlineCombination(const QString &aircraftDesignator,
                                                const QString &airlineDesignator) const;

        //! Contains given aircraft / airline combination
        bool containsAircraftAirlineCombination(const QString &sessionId, const QString &aircraftDesignator,
                                                const QString &airlineDesignator) const;

        //! Increase count if found
        bool increaseCountIfFound(CMatchingStatisticsEntry::EntryType type, const QString &sessionId,
                                  const QString &aircraftDesignator, const QString &airlineDesignator = {});

        //! Add a combination, normally with no duplicates (in that case count is increased
        void addAircraft(CMatchingStatisticsEntry::EntryType type, const QString &sessionId, const QString &modelSetId,
                         const QString &description, const QString &aircraftDesignator, bool avoidDuplicates = true);

        //! Add a combination, normally with no duplicates (in that case count is increased
        void addAircraftAirlineCombination(CMatchingStatisticsEntry::EntryType type, const QString &sessionId,
                                           const QString &modelSetId, const QString &description,
                                           const QString &aircraftDesignator, const QString &airlineDesignator,
                                           bool avoidDuplicates = true);
    };
} // namespace swift::misc::simulation

Q_DECLARE_METATYPE(swift::misc::simulation::CMatchingStatistics)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::simulation::CMatchingStatisticsEntry>)

#endif // SWIFT_MISC_SIMULATION_MATCHINGSTATISTICS_H
