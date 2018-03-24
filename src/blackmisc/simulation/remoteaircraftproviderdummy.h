/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_REMOTEAIRCRAFTPROVIDERDUMMY_H
#define BLACKMISC_SIMULATION_REMOTEAIRCRAFTPROVIDERDUMMY_H

#include "blackmisc/aviation/aircraftpartslist.h"
#include "blackmisc/aviation/aircraftsituationlist.h"
#include "blackmisc/aviation/callsignset.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/identifiable.h"
#include "blackmisc/identifier.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/airspaceaircraftsnapshot.h"
#include "blackmisc/simulation/remoteaircraftprovider.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"

#include <QList>
#include <QMetaObject>
#include <QObject>
#include <QtGlobal>
#include <functional>

namespace BlackMisc
{
    namespace Simulation
    {
        //! Dummy implementation for testing purpose, not thread safe
        class BLACKMISC_EXPORT CRemoteAircraftProviderDummy :
            public QObject,
            public IRemoteAircraftProvider
        {
            Q_OBJECT
            Q_INTERFACES(BlackMisc::Simulation::IRemoteAircraftProvider)

        public:
            //! Constructor
            CRemoteAircraftProviderDummy(QObject *parent = nullptr);

            //! For testing, add new situation and fire signals
            void insertNewSituation(const BlackMisc::Aviation::CAircraftSituation &situation);

            //! For testing, add new situation and fire signals
            void insertNewSituations(const BlackMisc::Aviation::CAircraftSituationList &situations);

            //! For testing, add new parts and fire signals
            void insertNewAircraftParts(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftParts &parts);

            //! For testing, add new parts and fire signals
            void insertNewAircraftParts(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftPartsList &partsList);

            //! Clear all data
            void clear();

            // Interface overrides
            virtual CSimulatedAircraftList getAircraftInRange() const override;
            virtual int getAircraftInRangeCount() const override;
            virtual Aviation::CCallsignSet getAircraftInRangeCallsigns() const override;
            virtual BlackMisc::Simulation::CSimulatedAircraft getAircraftInRangeForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const override;
            virtual BlackMisc::Simulation::CAircraftModel getAircraftInRangeModelForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const override;
            virtual bool isAircraftInRange(const Aviation::CCallsign &callsign) const override;
            virtual bool isVtolAircraft(const Aviation::CCallsign &callsign) const override;
            virtual BlackMisc::Simulation::CAirspaceAircraftSnapshot getLatestAirspaceAircraftSnapshot() const override;
            virtual BlackMisc::Aviation::CAircraftPartsList remoteAircraftParts(const Aviation::CCallsign &callsign, qint64 cutoffTimeBefore = -1) const override;
            virtual int remoteAircraftPartsCount(const Aviation::CCallsign &callsign, qint64 cutoffTimeBefore) const override;
            virtual BlackMisc::Aviation::CAircraftSituationList remoteAircraftSituations(const Aviation::CCallsign &callsign) const override;
            virtual int remoteAircraftSituationsCount(const Aviation::CCallsign &callsign) const override;
            virtual int getRemoteAircraftSupportingPartsCount() const override;
            virtual BlackMisc::Aviation::CCallsignSet remoteAircraftSupportingParts() const override;
            virtual bool isRemoteAircraftSupportingParts(const Aviation::CCallsign &callsign) const override;
            virtual QList<QMetaObject::Connection> connectRemoteAircraftProviderSignals(
                QObject *receiver,
                std::function<void(const BlackMisc::Aviation::CAircraftSituation &)> addedSituationSlot,
                std::function<void(const BlackMisc::Aviation::CCallsign &, const BlackMisc::Aviation::CAircraftParts &)>    addedPartsSlot,
                std::function<void(const BlackMisc::Aviation::CCallsign &)> removedAircraftSlot,
                std::function<void(const BlackMisc::Simulation::CAirspaceAircraftSnapshot &)> aircraftSnapshotSlot
            ) override;
            virtual bool updateAircraftEnabled(const BlackMisc::Aviation::CCallsign &callsign, bool enabledForRendering) override;
            virtual bool updateAircraftModel(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Simulation::CAircraftModel &model, const BlackMisc::CIdentifier &originator) override;
            virtual bool updateAircraftNetworkModel(const Aviation::CCallsign &callsign, const BlackMisc::Simulation::CAircraftModel &model, const BlackMisc::CIdentifier &originator) override;
            virtual bool updateFastPositionEnabled(const Aviation::CCallsign &callsign, bool enableFastPositionUpdates) override;
            virtual bool updateAircraftRendered(const Aviation::CCallsign &callsign, bool rendered) override;
            virtual bool updateAircraftGroundElevation(const Aviation::CCallsign &callsign, const BlackMisc::Geo::CElevationPlane &elevation) override;
            virtual void updateMarkAllAsNotRendered() override;
            virtual BlackMisc::CStatusMessageList getReverseLookupMessages(const BlackMisc::Aviation::CCallsign &callsign) const override;
            virtual bool isReverseLookupMessagesEnabled() const override;
            virtual void enableReverseLookupMessages(bool enabled) override;
            virtual BlackMisc::CStatusMessageList getAircraftPartsHistory(const BlackMisc::Aviation::CCallsign &callsign) const override;
            virtual bool isAircraftPartsHistoryEnabled() const override;
            virtual void enableAircraftPartsHistory(bool enabled) override;

        signals:
            //! Added situation
            void addedRemoteAircraftSituation(const BlackMisc::Aviation::CAircraftSituation &situation);

            //! Added parts
            void addedRemoteAircraftParts(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftParts &parts);

            //! Added aircraft
            void removedRemoteAircraft(const BlackMisc::Aviation::CCallsign &callsign);

            //! New aircraft snapshot
            void airspaceAircraftSnapshot(const BlackMisc::Simulation::CAirspaceAircraftSnapshot &snapshot);

        private:
            BlackMisc::Simulation::CSimulatedAircraftList m_aircraft;
            BlackMisc::Aviation::CAircraftSituationList m_situations;
            CPartsPerCallsign m_parts;
        };
    } // namespace
} // namespace

#endif // guard
