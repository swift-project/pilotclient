/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "remoteaircraftproviderdummy.h"

using namespace BlackMisc::Aviation;

namespace BlackMisc
{
    namespace Simulation
    {

        CRemoteAircraftProviderDummy::CRemoteAircraftProviderDummy(QObject *parent) : QObject(parent)
        { }

        const CSimulatedAircraftList &CRemoteAircraftProviderDummy::remoteAircraft() const
        {
            return m_aircraft;
        }

        CSimulatedAircraftList &CRemoteAircraftProviderDummy::remoteAircraft()
        {
            return m_aircraft;
        }

        CAircraftPartsList CRemoteAircraftProviderDummy::remoteAircraftParts(const BlackMisc::Aviation::CCallsign &callsign, qint64 cutoffTimeBefore) const
        {
            if (cutoffTimeBefore < 0) { return m_parts.findByCallsign(callsign); }
            return m_parts.findByCallsign(callsign).findBefore(cutoffTimeBefore);
        }

        CAircraftSituationList CRemoteAircraftProviderDummy::remoteAircraftSituations(const BlackMisc::Aviation::CCallsign &callsign) const
        {
            return m_situations.findByCallsign(callsign);
        }

        int CRemoteAircraftProviderDummy::remoteAircraftSituationsCount(const CCallsign &callsign) const
        {
            return remoteAircraftSituations(callsign).size();
        }

        CCallsignSet CRemoteAircraftProviderDummy::remoteAircraftSupportingParts() const
        {
            return m_parts.getCallsigns();
        }

        bool CRemoteAircraftProviderDummy::isRemoteAircraftSupportingParts(const CCallsign &callsign) const
        {
            return remoteAircraftParts(callsign).size() > 0;
        }

        bool CRemoteAircraftProviderDummy::connectRemoteAircraftProviderSignals(std::function<void (const CAircraftSituation &)> situationSlot, std::function<void (const CAircraftParts &)> partsSlot, std::function<void (const CCallsign &)> removedAircraftSlot)
        {
            bool s1 = connect(this, &CRemoteAircraftProviderDummy::addedRemoteAircraftSituation, situationSlot);
            bool s2 = connect(this, &CRemoteAircraftProviderDummy::addedRemoteAircraftParts, partsSlot);
            bool s3 = connect(this, &CRemoteAircraftProviderDummy::removedRemoteAircraft, removedAircraftSlot);
            return s1 && s2 && s3;
        }

        bool CRemoteAircraftProviderDummy::updateAircraftEnabled(const CCallsign &callsign, bool enabledForRendering, const QString &originator)
        {
            Q_UNUSED(originator);
            CPropertyIndexVariantMap vm(CSimulatedAircraft::IndexEnabled, CVariant::fromValue(enabledForRendering));
            int n = this->m_aircraft.applyIfCallsign(callsign, vm);
            return n > 0;
        }

        bool CRemoteAircraftProviderDummy::updateAircraftModel(const CCallsign &callsign, const CAircraftModel &model, const QString &originator)
        {
            Q_UNUSED(originator);
            CPropertyIndexVariantMap vm(CSimulatedAircraft::IndexModel, model.toCVariant());
            int n = this->m_aircraft.applyIfCallsign(callsign, vm);
            return n > 0;
        }

        bool CRemoteAircraftProviderDummy::updateFastPositionEnabled(const CCallsign &callsign, bool enableFastPositionUpdates, const QString &originator)
        {
            Q_UNUSED(originator);
            CPropertyIndexVariantMap vm(CSimulatedAircraft::IndexFastPositionUpdates, CVariant::fromValue(enableFastPositionUpdates));
            int n = this->m_aircraft.applyIfCallsign(callsign, vm);
            return n > 0;
        }

        void CRemoteAircraftProviderDummy::insertNewSituation(const CAircraftSituation &situation)
        {
            this->m_situations.push_front(situation);
            this->m_situations.sortLatestFirst(); // like in real world, latest should be first
            emit addedRemoteAircraftSituation(situation);
        }

        void CRemoteAircraftProviderDummy::insertNewAircraftParts(const CAircraftParts &parts)
        {
            this->m_parts.push_front(parts);
            this->m_parts.sortLatestFirst(); // like in real world, latest should be first
            emit addedRemoteAircraftParts(parts);
        }

        void CRemoteAircraftProviderDummy::clear()
        {
            m_situations.clear();
            m_parts.clear();
            m_aircraft.clear();
        }

    } // namespace
} // namespace
