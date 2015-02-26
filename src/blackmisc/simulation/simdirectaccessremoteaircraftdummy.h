/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMDIRECTACCESSREMOTEAIRCRAFTDUMMY_H
#define BLACKMISC_SIMDIRECTACCESSREMOTEAIRCRAFTDUMMY_H

#include "blackmisc/simulation/simdirectaccessremoteaircraft.h"

namespace BlackMisc
{
    namespace Simulation
    {

        //! Dummy implementation for testing purpose
        class CRemoteAircraftProviderDummy :
            public QObject,
            public IRemoteAircraftProvider
        {
            Q_OBJECT
            Q_INTERFACES(BlackMisc::Simulation::IRemoteAircraftProvider)

        public:
            //! Constructor
            CRemoteAircraftProviderDummy(QObject *parent = nullptr);

            //! IRemoteAircraftProviderReadOnly::remoteAircraft
            virtual const CSimulatedAircraftList &remoteAircraft() const;

            //! IRemoteAircraftProvider::remoteAircraft
            virtual CSimulatedAircraftList &remoteAircraft();

            //! \copydoc IRemoteAircraftProviderReadOnly::remoteAircraftParts
            virtual const BlackMisc::Aviation::CAircraftPartsList &remoteAircraftParts() const override;

            //! \copydoc IRemoteAircraftProvider::remoteAircraftParts
            virtual BlackMisc::Aviation::CAircraftPartsList &remoteAircraftParts() override;

            //! \copydoc IRemoteAircraftProviderReadOnly::remoteAircraftSituations
            virtual const BlackMisc::Aviation::CAircraftSituationList &remoteAircraftSituations() const override;

            //! \copydoc IRemoteAircraftProvider::remoteAircraftSituations
            virtual BlackMisc::Aviation::CAircraftSituationList &remoteAircraftSituations() override;

            //! \copydoc IRemoteAircraftProviderReadOnly::connectRemoteAircraftProviderSignals
            virtual bool connectRemoteAircraftProviderSignals(
                std::function<void(const BlackMisc::Aviation::CAircraftSituation &)> situationSlot,
                std::function<void(const BlackMisc::Aviation::CAircraftParts &)> partsSlot,
                std::function<void(const BlackMisc::Aviation::CCallsign &)> removedAircraftSlot
            ) override;

            //! \copydoc IRemoteAircraftProvider::updateAircraftEnabled
            virtual bool updateAircraftEnabled(const BlackMisc::Aviation::CCallsign &callsign, bool enabledForRendering, const QString &originator) override;

            //! \copydoc IRemoteAircraftProvider::updateAircraftModel
            virtual bool updateAircraftModel(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Simulation::CAircraftModel &model, const QString &originator) override;

            //! For testing, add new situation and fire signals
            void insertNewSituation(const BlackMisc::Aviation::CAircraftSituation &situation);

            // clear all data
            void clear();

        signals:
            //! \copydoc IRemoteAircraftProviderReadOnly::addedRemoteAircraftSituation
            void addedRemoteAircraftSituation(const BlackMisc::Aviation::CAircraftSituation &situation);

            //! \copydoc IRemoteAircraftProviderReadOnly::addedRemoteAircraftParts
            void addedRemoteAircraftParts(const BlackMisc::Aviation::CAircraftParts &parts);

            //! \copydoc IRemoteAircraftProviderReadOnly::removedRemoteAircraft
            void removedRemoteAircraft(const BlackMisc::Aviation::CCallsign &callsign);

        private:
            BlackMisc::Simulation::CSimulatedAircraftList m_aircraft;
            BlackMisc::Aviation::CAircraftSituationList m_situations;
            BlackMisc::Aviation::CAircraftPartsList m_parts;
        };

    } // namespace
} // namespace

#endif // guard
