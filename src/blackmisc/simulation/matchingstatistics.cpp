// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/simulation/matchingstatistics.h"

BLACK_DEFINE_SEQUENCE_MIXINS(BlackMisc::Simulation, CMatchingStatisticsEntry, CMatchingStatistics)

namespace BlackMisc::Simulation
{
    CMatchingStatistics::CMatchingStatistics()
    {}

    CMatchingStatistics::CMatchingStatistics(const CSequence<CMatchingStatisticsEntry> &other) : CSequence<CMatchingStatisticsEntry>(other)
    {}

    CMatchingStatistics CMatchingStatistics::findBySessionId(const QString &sessionId) const
    {
        return this->findBy(&CMatchingStatisticsEntry::getSessionId, sessionId);
    }

    CMatchingStatistics CMatchingStatistics::findMissingOnly() const
    {
        return this->findBy(&CMatchingStatisticsEntry::isMissing, true);
    }

    bool CMatchingStatistics::containsSessionId(const QString &sessionId) const
    {
        return this->contains(&CMatchingStatisticsEntry::getSessionId, sessionId);
    }

    bool CMatchingStatistics::containsAircraftAirlineCombination(const QString &aircraftDesignator, const QString &airlineDesignator) const
    {
        return aircraftDesignator.isEmpty() ?
                   this->contains(&CMatchingStatisticsEntry::getAircraftDesignator, aircraftDesignator) :
                   this->contains(&CMatchingStatisticsEntry::getAircraftDesignator, aircraftDesignator, &CMatchingStatisticsEntry::getAirlineDesignator, airlineDesignator);
    }

    bool CMatchingStatistics::containsAircraftAirlineCombination(const QString &sessionId, const QString &aircraftDesignator, const QString &airlineDesignator) const
    {
        return aircraftDesignator.isEmpty() ?
                   this->contains(&CMatchingStatisticsEntry::getSessionId, sessionId, &CMatchingStatisticsEntry::getAircraftDesignator, aircraftDesignator) :
                   this->contains(&CMatchingStatisticsEntry::getSessionId, sessionId, &CMatchingStatisticsEntry::getAircraftDesignator, aircraftDesignator, &CMatchingStatisticsEntry::getAirlineDesignator, airlineDesignator);
    }

    bool CMatchingStatistics::increaseCountIfFound(CMatchingStatisticsEntry::EntryType type, const QString &sessionId, const QString &aircraftDesignator, const QString &airlineDesignator)
    {
        bool found = false;
        for (CMatchingStatisticsEntry &entry : *this)
        {
            if (entry.matches(type, sessionId, aircraftDesignator, airlineDesignator))
            {
                entry.increaseCount();
                found = true;
            }
        }
        return found;
    }

    void CMatchingStatistics::addAircraft(CMatchingStatisticsEntry::EntryType type, const QString &sessionId, const QString &modelSetId, const QString &description, const QString &aircraftDesignator, bool avoidDuplicates)
    {
        if (avoidDuplicates)
        {
            const bool didIncrease = this->increaseCountIfFound(type, sessionId, aircraftDesignator);
            if (didIncrease) { return; }
        }
        this->push_back(CMatchingStatisticsEntry(type, sessionId, modelSetId, description, aircraftDesignator));
    }

    void CMatchingStatistics::addAircraftAirlineCombination(CMatchingStatisticsEntry::EntryType type, const QString &sessionId, const QString &modelSetId, const QString &description, const QString &aircraftDesignator, const QString &airlineDesignator, bool avoidDuplicates)
    {
        if (avoidDuplicates)
        {
            const bool didIncrease = this->increaseCountIfFound(type, sessionId, aircraftDesignator, airlineDesignator);
            if (didIncrease) { return; }
        }
        this->push_back(CMatchingStatisticsEntry(type, sessionId, modelSetId, description, aircraftDesignator, airlineDesignator));
    }
} // namespace
