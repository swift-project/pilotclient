/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMDIRECTACCESSREMOTEAIRCRAFT_H
#define BLACKMISC_SIMDIRECTACCESSREMOTEAIRCRAFT_H

#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/avaircraftsituationlist.h"
#include "blackmisc/aviation/aircraftpartslist.h"
#include <functional>

namespace BlackMisc
{
    namespace Simulation
    {
        //! Direct in memory access to remote aircraft
        //! \note Can not be derived from QObject (as for the signals), as this would create multiple
        //!       inheritance. Hence Q_DECLARE_INTERFACE is used.
        class IRemoteAircraftProviderReadOnly
        {
        public:
            //! All rendered aircraft
            virtual const CSimulatedAircraftList &remoteAircraft() const = 0;

            //! All situation (per callsign, time history)
            virtual const BlackMisc::Aviation::CAircraftSituationList &remoteAircraftSituations() const = 0;

            //! All parts (per callsign, time history)
            virtual const BlackMisc::Aviation::CAircraftPartsList &remoteAircraftParts() const = 0;

            //! Destructor
            virtual ~IRemoteAircraftProviderReadOnly() {}

            //! Connect signals to slot receiver. As the interface is no QObject, slots can not be connected directly.
            virtual bool connectRemoteAircraftProviderSignals(
                std::function<void(const BlackMisc::Aviation::CAircraftSituation &)> situationSlot,
                std::function<void(const BlackMisc::Aviation::CAircraftParts &)> partsSlot,
                std::function<void(const BlackMisc::Aviation::CCallsign &)> removedAircraftSlot
            ) = 0;

            // those signals have to be implemented by classes using the interface.
        signals:
            //! A new situation got added
            void addedRemoteAircraftSituation(const BlackMisc::Aviation::CAircraftSituation &situation);

            //! New parts got added
            void addedRemoteAircraftParts(const BlackMisc::Aviation::CAircraftParts &parts);

            //! Aircraft was removed
            void removedRemoteAircraft(const BlackMisc::Aviation::CCallsign &callsign);

        };

        //! Direct in memory access to own aircraft
        class IRemoteAircraftProvider : public IRemoteAircraftProviderReadOnly
        {
        public:

            using IRemoteAircraftProviderReadOnly::remoteAircraft;
            using IRemoteAircraftProviderReadOnly::remoteAircraftParts;
            using IRemoteAircraftProviderReadOnly::remoteAircraftSituations;

            //! All rendered aircraft
            //! \note in memory reference, not thread safe
            virtual CSimulatedAircraftList &remoteAircraft() = 0;

            //! Rendered aircraft situations (history)
            virtual BlackMisc::Aviation::CAircraftSituationList &remoteAircraftSituations() = 0;

            //! All parts (per callsign, time history)
            //! \note in memory reference, not thread safe
            virtual BlackMisc::Aviation::CAircraftPartsList &remoteAircraftParts() = 0;

            //! Enable/disable rendering
            virtual bool updateAircraftEnabled(const BlackMisc::Aviation::CCallsign &callsign, bool enabledForRendering, const QString &originator) = 0;

            //! Change model string
            virtual bool updateAircraftModel(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Simulation::CAircraftModel &model, const QString &originator) = 0;

            //! Change fast position updates
            virtual bool updateFastPositionUpdates(const BlackMisc::Aviation::CCallsign &callsign, bool enableFastPositonUpdates, const QString &originator) = 0;
        };

        //! Class which can be directly used providing access to such an interface
        class CRemoteAircraftProviderSupportReadOnly
        {
        public:
            //! \copydoc IRemoteAircraftProviderReadOnly::renderedAircraft
            virtual const CSimulatedAircraftList &remoteAircraft() const;

            //!\copydoc IRemoteAircraftProviderReadOnly::renderedAircraftSituations
            virtual const BlackMisc::Aviation::CAircraftSituationList &remoteAircraftSituations() const;

            //!\copydoc IRemoteAircraftProviderReadOnly::renderedAircraftParts
            virtual const BlackMisc::Aviation::CAircraftPartsList &remoteAircraftParts() const;

            //! Destructor
            virtual ~CRemoteAircraftProviderSupportReadOnly() {}

        protected:
            //! Constructor
            CRemoteAircraftProviderSupportReadOnly(const IRemoteAircraftProviderReadOnly *remoteAircraftProvider) : m_remoteAircraftProvider(remoteAircraftProvider) {}
            const IRemoteAircraftProviderReadOnly *m_remoteAircraftProvider = nullptr; //!< access to object
        };

        //! Delegating class which can be directly used providing access to such an interface
        class CRemoteAircraftProviderSupport
        {
        public:
            //! \copydoc IRemoteAircraftProviderReadOnly::renderedAircraft
            virtual const CSimulatedAircraftList &remoteAircraft() const;

            //! \copydoc IRemoteAircraftProvider::renderedAircraft
            virtual CSimulatedAircraftList &remoteAircraft();

            //!\copydoc IRemoteAircraftProviderReadOnly::renderedAircraftSituations
            virtual const BlackMisc::Aviation::CAircraftSituationList &remoteAircraftSituations() const;

            //!\copydoc IRemoteAircraftProvider::remoteAircraftSituations
            virtual BlackMisc::Aviation::CAircraftSituationList &remoteAircraftSituations();

            //!\copydoc IRemoteAircraftProviderReadOnly::renderedAircraftParts
            virtual const BlackMisc::Aviation::CAircraftPartsList &remoteAircraftParts() const;

            //!\copydoc IRemoteAircraftProvider::remoteAircraftParts
            virtual BlackMisc::Aviation::CAircraftPartsList &remoteAircraftParts();

            //! \copydoc IRemoteAircraftProvider::updateAircraftEnabled
            virtual bool providerUpdateAircraftEnabled(const BlackMisc::Aviation::CCallsign &callsign, bool enabledForRedering, const QString &originator);

            //! \copydoc IRemoteAircraftProvider::updateAircraftModel
            virtual bool providerUpdateAircraftModel(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Simulation::CAircraftModel &model, const QString &originator);

            //! Destructor
            virtual ~CRemoteAircraftProviderSupport() {}

        protected:
            //! Constructor
            CRemoteAircraftProviderSupport(IRemoteAircraftProvider *remoteAircraftProvider) : m_remoteAircraftProvider(remoteAircraftProvider) { Q_ASSERT(remoteAircraftProvider); }
            IRemoteAircraftProvider *m_remoteAircraftProvider = nullptr; //!< access to object
        };

    } // namespace
} // namespace

Q_DECLARE_INTERFACE(BlackMisc::Simulation::IRemoteAircraftProviderReadOnly, "IRemoteAircraftProviderReadOnly")
Q_DECLARE_INTERFACE(BlackMisc::Simulation::IRemoteAircraftProvider, "IRemoteAircraftProvider")

#endif // guard
