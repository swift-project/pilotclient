/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/aviation/callsign.h"
#include "blackmisc/geo/elevationplane.h"
#include "blackmisc/simulation/remoteaircraftproviderdummy.h"
#include "blackmisc/collection.h"
#include "blackmisc/compare.h"
#include "blackmisc/dictionary.h"
#include "blackmisc/propertyindexvariantmap.h"
#include "blackmisc/variant.h"

#include <QHash>

using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;

namespace BlackMisc
{
    namespace Simulation
    {
        CRemoteAircraftProviderDummy::CRemoteAircraftProviderDummy(QObject *parent) : QObject(parent)
        { }

        CSimulatedAircraftList CRemoteAircraftProviderDummy::getAircraftInRange() const
        {
            return m_aircraft;
        }

        int CRemoteAircraftProviderDummy::getAircraftInRangeCount() const
        {
            return m_aircraft.size();
        }

        CCallsignSet CRemoteAircraftProviderDummy::getAircraftInRangeCallsigns() const
        {
            return m_aircraft.getCallsigns();
        }

        CSimulatedAircraft CRemoteAircraftProviderDummy::getAircraftInRangeForCallsign(const CCallsign &callsign) const
        {
            return m_aircraft.findFirstByCallsign(callsign);
        }

        CAircraftModel CRemoteAircraftProviderDummy::getAircraftInRangeModelForCallsign(const CCallsign &callsign) const
        {
            return getAircraftInRangeForCallsign(callsign).getModel();
        }

        CAirspaceAircraftSnapshot CRemoteAircraftProviderDummy::getLatestAirspaceAircraftSnapshot() const
        {
            return CAirspaceAircraftSnapshot(m_aircraft);
        }

        CAircraftPartsList CRemoteAircraftProviderDummy::remoteAircraftParts(const BlackMisc::Aviation::CCallsign &callsign, qint64 cutoffTimeBefore) const
        {
            if (cutoffTimeBefore < 0) { return m_parts.value(callsign); }
            return m_parts.value(callsign).findBefore(cutoffTimeBefore);
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
            return CCollection<CCallsign>(m_parts.keys());
        }

        bool CRemoteAircraftProviderDummy::isRemoteAircraftSupportingParts(const CCallsign &callsign) const
        {
            return remoteAircraftParts(callsign).size() > 0;
        }

        QList<QMetaObject::Connection> CRemoteAircraftProviderDummy::connectRemoteAircraftProviderSignals(
            QObject *receiver,
            std::function<void (const CAircraftSituation &)>                                      situationSlot,
            std::function<void (const BlackMisc::Aviation::CCallsign &, const CAircraftParts &)>  partsSlot,
            std::function<void (const CCallsign &)>                                               removedAircraftSlot,
            std::function<void (const CAirspaceAircraftSnapshot &)>                               aircraftSnapshotSlot
        )
        {
            Q_ASSERT_X(receiver, Q_FUNC_INFO, "Missing receiver");
            QList<QMetaObject::Connection> c(
            {
                connect(this, &CRemoteAircraftProviderDummy::addedRemoteAircraftSituation, receiver, situationSlot) ,
                connect(this, &CRemoteAircraftProviderDummy::addedRemoteAircraftParts, receiver, partsSlot) ,
                connect(this, &CRemoteAircraftProviderDummy::removedRemoteAircraft, receiver, removedAircraftSlot) ,
                connect(this, &CRemoteAircraftProviderDummy::airspaceAircraftSnapshot, receiver, aircraftSnapshotSlot)
            });
            return c;
        }

        bool CRemoteAircraftProviderDummy::updateAircraftEnabled(const CCallsign &callsign, bool enabledForRendering)
        {
            CPropertyIndexVariantMap vm(CSimulatedAircraft::IndexEnabled, CVariant::fromValue(enabledForRendering));
            const int n = this->m_aircraft.applyIfCallsign(callsign, vm);
            return n > 0;
        }

        bool CRemoteAircraftProviderDummy::updateAircraftModel(const CCallsign &callsign, const CAircraftModel &model, const CIdentifier &originator)
        {
            Q_UNUSED(originator);
            CPropertyIndexVariantMap vm(CSimulatedAircraft::IndexModel, CVariant::from(model));
            const int n = this->m_aircraft.applyIfCallsign(callsign, vm);
            return n > 0;
        }

        bool CRemoteAircraftProviderDummy::updateAircraftNetworkModel(const CCallsign &callsign, const CAircraftModel &model, const CIdentifier &originator)
        {
            Q_UNUSED(originator);
            CPropertyIndexVariantMap vm(CSimulatedAircraft::IndexNetworkModel, CVariant::from(model));
            const int n = this->m_aircraft.applyIfCallsign(callsign, vm);
            return n > 0;
        }

        bool CRemoteAircraftProviderDummy::updateFastPositionEnabled(const CCallsign &callsign, bool enableFastPositionUpdates)
        {
            CPropertyIndexVariantMap vm(CSimulatedAircraft::IndexFastPositionUpdates, CVariant::fromValue(enableFastPositionUpdates));
            const int n = this->m_aircraft.applyIfCallsign(callsign, vm);
            return n > 0;
        }

        bool CRemoteAircraftProviderDummy::updateAircraftRendered(const CCallsign &callsign, bool rendered)
        {
            CPropertyIndexVariantMap vm(CSimulatedAircraft::IndexRendered, CVariant::fromValue(rendered));
            const int n = this->m_aircraft.applyIfCallsign(callsign, vm);
            return n > 0;
        }

        bool CRemoteAircraftProviderDummy::updateAircraftGroundElevation(const CCallsign &callsign, const CElevationPlane &elevation)
        {
            CPropertyIndexVariantMap vm({ CSimulatedAircraft::IndexSituation, CAircraftSituation::IndexGroundElevationPlane }, CVariant::fromValue(elevation));
            const int c = this->m_aircraft.applyIfCallsign(callsign, vm);
            return c > 0;
        }

        void CRemoteAircraftProviderDummy::updateMarkAllAsNotRendered()
        {
            this->m_aircraft.markAllAsNotRendered();
        }

        CStatusMessageList CRemoteAircraftProviderDummy::getReverseLookupMessages(const CCallsign &callsign) const
        {
            Q_UNUSED(callsign);
            return CStatusMessageList();
        }

        bool CRemoteAircraftProviderDummy::isReverseLookupMessagesEnabled() const
        {
            return false;
        }

        void CRemoteAircraftProviderDummy::enableReverseLookupMessages(bool enabled)
        {
            Q_UNUSED(enabled);
        }

        CStatusMessageList CRemoteAircraftProviderDummy::getAircraftPartsHistory(const CCallsign &callsign) const
        {
            Q_UNUSED(callsign);
            return CStatusMessageList();
        }

        bool CRemoteAircraftProviderDummy::isAircraftPartsHistoryEnabled() const
        {
            return false;
        }

        void CRemoteAircraftProviderDummy::enableAircraftPartsHistory(bool enabled)
        {
            Q_UNUSED(enabled);
        }

        void CRemoteAircraftProviderDummy::insertNewSituation(const CAircraftSituation &situation)
        {
            this->m_situations.push_front(situation);
            emit addedRemoteAircraftSituation(situation);
        }

        void CRemoteAircraftProviderDummy::insertNewAircraftParts(const CCallsign &callsign, const CAircraftParts &parts)
        {
            this->m_parts[callsign].push_front(parts);
            emit addedRemoteAircraftParts(callsign, parts);
        }

        void CRemoteAircraftProviderDummy::clear()
        {
            m_situations.clear();
            m_parts.clear();
            m_aircraft.clear();
        }

    } // namespace
} // namespace
