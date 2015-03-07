/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "interpolator.h"
#include "blackmisc/logmessage.h"
#include <functional>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;


namespace BlackCore
{

    IInterpolator::IInterpolator(IRemoteAircraftProviderReadOnly *provider, const QString &workerName, QObject *parent) :
        CContinuousWorker(parent, workerName),
        CRemoteAircraftProviderSupportReadOnly(provider)
    {
        Q_ASSERT(provider);
        this->m_situationsByCallsign = this->remoteAircraftSituations().splitPerCallsign();
        this->m_partsByCallsign = this->remoteAircraftParts().splitPerCallsign();
        bool c = provider->connectRemoteAircraftProviderSignals(
                     std::bind(&IInterpolator::ps_onAddedAircraftSituation, this, std::placeholders::_1),
                     std::bind(&IInterpolator::ps_onAddedAircraftParts, this, std::placeholders::_1),
                     std::bind(&IInterpolator::ps_onRemovedAircraft, this, std::placeholders::_1)
                 );
        Q_ASSERT(c);
        Q_UNUSED(c);
    }

    CAircraftSituationList IInterpolator::getInterpolatedSituations(qint64 currentTimeMsSinceEpoch)
    {
        QReadLocker l(&m_lockSituations);
        const CSituationsPerCallsign situationsCopy(m_situationsByCallsign);
        l.unlock();

        CAircraftSituationList latestInterpolations;
        if (currentTimeMsSinceEpoch < 0) { currentTimeMsSinceEpoch = QDateTime::currentMSecsSinceEpoch(); }
        for (const CCallsign &cs : situationsCopy.keys())
        {
            InterpolationStatus status;
            CAircraftSituation situation = getInterpolatedSituation(cs, currentTimeMsSinceEpoch, status, &situationsCopy);
            if (status.allTrue())
            {
                latestInterpolations.push_back(situation);
            }
            else
            {
                // not OK can mean not enough situations
                // further logging could go here
            }
        }
        return latestInterpolations;
    }

    IInterpolator::CSituationsPerCallsign IInterpolator::getSituationsByCallsign() const
    {
        QReadLocker l(&m_lockSituations);
        return m_situationsByCallsign;
    }

    CAircraftPartsList IInterpolator::getAndRemovePartsBeforeTime(const CCallsign &callsign, qint64 cutoffTime, BlackCore::IInterpolator::PartsStatus &partsStatus)
    {
        static const CAircraftPartsList empty;
        Q_ASSERT_X(!callsign.isEmpty(), "getAndRemovePartsBeforeTime", "empty callsign");
        partsStatus.reset();
        QWriteLocker l(&m_lockParts);
        if (this->m_partsByCallsign.contains(callsign))
        {
            partsStatus.supportsParts = true;
            return this->m_partsByCallsign[callsign].findBeforeAndRemove(cutoffTime);
        }
        else
        {
            // did we ever have parts for this callsign
            partsStatus.supportsParts = m_aircraftSupportingParts.contains(callsign);
            return empty;
        }
    }

    void IInterpolator::clear()
    {
        QWriteLocker s(&m_lockSituations);
        QWriteLocker p(&m_lockParts);
        m_situationsByCallsign.clear();
        m_partsByCallsign.clear();
    }

    CAircraftSituationList IInterpolator::getSituationsForCallsign(const CCallsign &callsign) const
    {
        QReadLocker l(&m_lockSituations);
        static const CAircraftSituationList empty;
        if (!m_situationsByCallsign.contains(callsign)) { return empty; }
        return m_situationsByCallsign[callsign];
    }

    CAircraftPartsList IInterpolator::getPartsForCallsign(const CCallsign &callsign) const
    {
        QReadLocker l(&m_lockParts);
        static const CAircraftPartsList empty;
        if (!m_partsByCallsign.contains(callsign)) { return empty; }
        return m_partsByCallsign[callsign];
    }

    void IInterpolator::enableDebugMessages(bool enabled)
    {
        this->m_withDebugMsg = enabled;
    }

    void IInterpolator::forceSortingOfAddedValues(bool sort)
    {
        this->m_forceSortWhenAddingValues = sort;
    }

    void IInterpolator::ps_onAddedAircraftSituation(const CAircraftSituation &situation)
    {
        QWriteLocker lock(&m_lockSituations);
        const CCallsign callsign(situation.getCallsign());
        Q_ASSERT_X(!callsign.isEmpty(), "ps_onAddedAircraftSituation", "empty callsign");
        if (callsign.isEmpty()) { return; }
        if (this->m_withDebugMsg) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << situation.getCallsign() << situation.getMSecsSinceEpoch(); }

        // list from new to old
        CAircraftSituationList &l = this->m_situationsByCallsign[callsign];
        l.push_frontMaxElements(situation, MaxSituationsPerCallsign);
        if (this->m_forceSortWhenAddingValues) { l.sortLatestFirst(); }

        // check sort order
        Q_ASSERT(l.size() < 2 || l[0].getMSecsSinceEpoch() >= l[1].getMSecsSinceEpoch());
    }

    void IInterpolator::ps_onAddedAircraftParts(const CAircraftParts &parts)
    {
        QWriteLocker lock(&m_lockParts);
        const CCallsign callsign(parts.getCallsign());
        Q_ASSERT_X(!callsign.isEmpty(), "ps_onAddedAircraftParts", "empty callsign");
        if (callsign.isEmpty()) { return; }
        if (this->m_withDebugMsg) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << parts.getCallsign() << parts.getMSecsSinceEpoch(); }

        // list sorted from new to old
        CAircraftPartsList &l = this->m_partsByCallsign[callsign];
        l.push_frontMaxElements(parts, MaxPartsPerCallsign);
        if (this->m_forceSortWhenAddingValues) { l.sortLatestFirst(); }

        if (m_aircraftSupportingParts.contains(callsign)) { return; }
        m_aircraftSupportingParts.push_back(callsign); // mark as callsign which supports parts

        // check sort order
        Q_ASSERT(l.size() < 2 || l[0].getMSecsSinceEpoch() >= l[1].getMSecsSinceEpoch());
    }

    void IInterpolator::ps_onRemovedAircraft(const CCallsign &callsign)
    {
        QWriteLocker ls(&m_lockSituations);
        QWriteLocker lp(&m_lockParts);
        Q_ASSERT(!callsign.isEmpty());
        if (callsign.isEmpty()) { return; }
        if (this->m_withDebugMsg) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << callsign; }

        this->m_partsByCallsign.remove(callsign);
        this->m_situationsByCallsign.remove(callsign);
        this->m_aircraftSupportingParts.remove(callsign);
    }

    bool IInterpolator::InterpolationStatus::allTrue() const
    {
        return interpolationSucceeded && changedPosition;
    }

    void IInterpolator::InterpolationStatus::reset()
    {
        changedPosition = false;
        interpolationSucceeded = false;
    }

    bool IInterpolator::PartsStatus::allTrue() const
    {
        return supportsParts;
    }

    void IInterpolator::PartsStatus::reset()
    {
        supportsParts = false;
    }

} // namespace
