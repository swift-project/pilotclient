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
                     std::bind(&IInterpolator::ps_onRemoveAircraft, this, std::placeholders::_1)
                 );
        Q_ASSERT(c);
        Q_UNUSED(c);
    }

    QList<CAircraftSituationList> IInterpolator::getSituationsTimeSplit(const CCallsign &callsign, qint64 splitTimeMsSinceEpoch) const
    {
        QReadLocker l(&m_situationsLock);
        static const QList<CAircraftSituationList> empty({ CAircraftSituationList(), CAircraftSituationList() });
        if (!this->m_situationsByCallsign.contains(callsign)) { return empty; }
        return this->m_situationsByCallsign[callsign].splitByTime(splitTimeMsSinceEpoch);
    }

    bool IInterpolator::hasEnoughAircraftSituations(const CCallsign &callsign) const
    {
        QReadLocker l(&m_situationsLock);
        if (!this->m_situationsByCallsign.contains(callsign)) { return false; }
        return this->m_situationsByCallsign[callsign].findBeforeNowMinusOffset(TimeOffsetMs).size() > 0;
    }

    CAircraftParts IInterpolator::getLatestPartsBeforeOffset(const CCallsign &callsign, qint64 timeOffset, bool *ok) const
    {
        QReadLocker l(&m_partsLock);
        static const CAircraftParts empty;
        if (ok) { *ok = false; }
        if (this->m_partsByCallsign.contains(callsign)) { return empty; }
        CAircraftPartsList partsList = this->m_partsByCallsign[callsign].findBeforeNowMinusOffset(timeOffset);
        l.unlock();
        if (partsList.isEmpty()) { return empty; }
        if (ok) { *ok = true; }
        return partsList.latestValue();
    }

    void IInterpolator::requestSituationsCalculationsForAllCallsigns(int requestId)
    {
        QReadLocker l(&m_situationsLock);
        Q_ASSERT(requestId >= 0);
        const QHash<BlackMisc::Aviation::CCallsign, BlackMisc::Aviation::CAircraftSituationList> situationsCopy = m_situationsByCallsign;
        l.unlock();

        CAircraftSituationList latestInterpolations;
        for (const CCallsign &cs : situationsCopy.keys())
        {
            bool ok;
            CAircraftSituation situation = getCurrentInterpolatedSituation(situationsCopy, cs, &ok);
            if (ok)
            {
                latestInterpolations.push_back(situation);
            }
            else
            {
                // not OK can mean not enough situations
                // further logging could go here
            }
        }

        QWriteLocker wl(&m_requestedInterpolationsLock);
        while (m_requestedInterpolations.size() >= MaxKeptInterpolationRequests - 1)
        {
            m_requestedInterpolations.erase(--m_requestedInterpolations.end());
        }
        m_requestedInterpolations.insert(requestId, latestInterpolations); // new to old

    }

    QHash<CCallsign, CAircraftSituationList> IInterpolator::getSituationsByCallsign() const
    {
        QReadLocker l(&m_requestedInterpolationsLock);
        return m_situationsByCallsign;
    }

    int IInterpolator::latestFinishedRequestId() const
    {
        QReadLocker l(&m_requestedInterpolationsLock);
        if (m_requestedInterpolations.isEmpty()) { return -1; }
        return m_requestedInterpolations.keys().first();
    }

    CAircraftSituationList IInterpolator::latestFinishedRequest() const
    {
        QReadLocker l(&m_requestedInterpolationsLock);
        static const CAircraftSituationList empty;
        if (m_requestedInterpolations.isEmpty()) { return empty; }
        return m_requestedInterpolations.values().first();
    }

    CAircraftSituationList IInterpolator::getRequest(int requestId, bool *ok) const
    {
        QReadLocker l(&m_requestedInterpolationsLock);
        static const CAircraftSituationList empty;
        if (!m_requestedInterpolations.contains(requestId))
        {
            if (ok) { *ok = false; }
            return empty;
        }
        if (ok) { *ok = true; }
        return m_requestedInterpolations[requestId];
    }

    void IInterpolator::ps_onAddedAircraftSituation(const CAircraftSituation &situation)
    {
        QWriteLocker lock(&m_situationsLock);
        Q_ASSERT(!situation.getCallsign().isEmpty());
        if (this->m_withDebugMsg) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << situation; }

        // list from new to old
        CAircraftSituationList &l = this->m_situationsByCallsign[situation.getCallsign()];
        if (l.size() >= MaxSituationsPerCallsign - 1)
        {
            l.truncate(MaxSituationsPerCallsign - 1);
        }
        l.insert(situation);
    }

    void IInterpolator::ps_onAddedAircraftParts(const CAircraftParts &parts)
    {
        QWriteLocker lock(&m_partsLock);
        Q_ASSERT(!parts.getCallsign().isEmpty());
        if (this->m_withDebugMsg) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << parts; }

        // list from new to old
        CAircraftPartsList &l = this->m_partsByCallsign[parts.getCallsign()];
        if (l.size() >= MaxPartsPerCallsign - 1)
        {
            l.truncate(MaxPartsPerCallsign - 1);
        }
        l.insert(parts);
    }

    void IInterpolator::ps_onRemoveAircraft(const CCallsign &callsign)
    {
        QWriteLocker ls(&m_situationsLock);
        QWriteLocker lp(&m_partsLock);
        Q_ASSERT(!callsign.isEmpty());
        if (this->m_withDebugMsg) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << callsign; }

        if (callsign.isEmpty()) { return; }
        this->m_partsByCallsign.remove(callsign);
        this->m_situationsByCallsign.remove(callsign);
    }

} // namespace
