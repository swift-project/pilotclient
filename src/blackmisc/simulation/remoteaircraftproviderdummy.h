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

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/simulation/remoteaircraftprovider.h"

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

            //! IRemoteAircraftProvider::getAircraftInRange
            virtual CSimulatedAircraftList getAircraftInRange() const override;

            //! IRemoteAircraftProvider::getAircraftInRangeCount
            virtual int getAircraftInRangeCount() const override;

            //! IRemoteAircraftProvider::getAircraftInRangeForCallsign
            virtual BlackMisc::Simulation::CSimulatedAircraft getAircraftInRangeForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const override;

            //! \copydoc IRemoteAircraftProvider::remoteAircraftParts
            virtual BlackMisc::Aviation::CAircraftPartsList remoteAircraftParts(const Aviation::CCallsign &callsign, qint64 cutoffTimeBefore = -1) const override;

            //! \copydoc IRemoteAircraftProvider::remoteAircraftSituations
            virtual BlackMisc::Aviation::CAircraftSituationList remoteAircraftSituations(const Aviation::CCallsign &callsign) const override;

            //! \copydoc IRemoteAircraftProvider::remoteAircraftSituationsCount
            virtual int remoteAircraftSituationsCount(const Aviation::CCallsign &callsign) const override;

            //! \copydoc IRemoteAircraftProvider::remoteAircraftSupportingParts
            virtual BlackMisc::Aviation::CCallsignSet remoteAircraftSupportingParts() const override;

            //! \copydoc IRemoteAircraftProvider::isRemoteAircraftSupportingParts
            virtual bool isRemoteAircraftSupportingParts(const Aviation::CCallsign &callsign) const override;

            //! \copydoc IRemoteAircraftProvider::connectRemoteAircraftProviderSignals
            virtual bool connectRemoteAircraftProviderSignals(
                std::function<void(const BlackMisc::Aviation::CAircraftSituation &)> situationSlot,
                std::function<void(const BlackMisc::Aviation::CAircraftParts &)> partsSlot,
                std::function<void(const BlackMisc::Aviation::CCallsign &)> removedAircraftSlot
            ) override;

            //! \copydoc IRemoteAircraftProvider::updateAircraftEnabled
            virtual bool updateAircraftEnabled(const BlackMisc::Aviation::CCallsign &callsign, bool enabledForRendering, const QString &originator) override;

            //! \copydoc IRemoteAircraftProvider::updateAircraftModel
            virtual bool updateAircraftModel(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Simulation::CAircraftModel &model, const QString &originator) override;

            //! \copydoc IRemoteAircraftProvider::updateFastPositionEnabled
            virtual bool updateFastPositionEnabled(const Aviation::CCallsign &callsign, bool enableFastPositionUpdates, const QString &originator) override;

            //! \copydoc IRemoteAircraftProvider::updateAircraftRendered
            virtual bool updateAircraftRendered(const Aviation::CCallsign &callsign, bool rendered, const QString &originator) override;

            //! \copydoc IRemoteAircraftProvider::updateMarkAllAsNotRendered
            virtual void updateMarkAllAsNotRendered(const QString &originator) override;

            //! For testing, add new situation and fire signals
            void insertNewSituation(const BlackMisc::Aviation::CAircraftSituation &situation);

            //! For testing, add new parts and fire signals
            void insertNewAircraftParts(const BlackMisc::Aviation::CAircraftParts &parts);

            //! Clear all data
            void clear();

        signals:
            //! Added situation
            void addedRemoteAircraftSituation(const BlackMisc::Aviation::CAircraftSituation &situation);

            //! Added parts
            void addedRemoteAircraftParts(const BlackMisc::Aviation::CAircraftParts &parts);

            //! Added aircraft
            void removedRemoteAircraft(const BlackMisc::Aviation::CCallsign &callsign);

        private:
            BlackMisc::Simulation::CSimulatedAircraftList m_aircraft;
            BlackMisc::Aviation::CAircraftSituationList m_situations;
            BlackMisc::Aviation::CAircraftPartsList m_parts;
        };

    } // namespace
} // namespace

#endif // guard
