/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_REMOTEAIRCRAFTPROVIDER_H
#define BLACKMISC_SIMULATION_REMOTEAIRCRAFTPROVIDER_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/aviation/aircraftsituationlist.h"
#include "blackmisc/aviation/aircraftpartslist.h"
#include <functional>

namespace BlackMisc
{
    namespace Simulation
    {
        //! Direct in memory access to remote aircraft
        //! \note Can not be derived from QObject (as for the signals), as this would create multiple
        //!       inheritance. Hence Q_DECLARE_INTERFACE is used.
        class BLACKMISC_EXPORT IRemoteAircraftProviderReadOnly
        {
        public:
            //! Situations per callsign
            typedef QHash<BlackMisc::Aviation::CCallsign, BlackMisc::Aviation::CAircraftSituationList> CSituationsPerCallsign;

            //! Parts per callsign
            typedef QHash<BlackMisc::Aviation::CCallsign, BlackMisc::Aviation::CAircraftPartsList> CPartsPerCallsign;

            //! All rendered aircraft
            virtual const CSimulatedAircraftList &remoteAircraft() const = 0;

            //! Rendered aircraft situations (per callsign, time history)
            //! \threadsafe
            virtual BlackMisc::Aviation::CAircraftSituationList remoteAircraftSituations(const BlackMisc::Aviation::CCallsign &callsign) const = 0;

            //! Number of remote aircraft situations for callsign
            //! \threadsafe
            virtual int remoteAircraftSituationsCount(const BlackMisc::Aviation::CCallsign &callsign) const = 0;

            //! All parts (per callsign, time history)
            //! \threadsafe
            virtual BlackMisc::Aviation::CAircraftPartsList remoteAircraftParts(const BlackMisc::Aviation::CCallsign &callsign, qint64 cutoffTimeBefore = -1) const = 0;

            //! Is remote aircraft supporting parts?
            //! \threadsafe
            virtual bool isRemoteAircraftSupportingParts(const BlackMisc::Aviation::CCallsign &callsign) const = 0;

            //! Remote aircraft supporting parts.
            //! \threadsafe
            virtual BlackMisc::Aviation::CCallsignSet remoteAircraftSupportingParts() const = 0;

            //! Destructor
            virtual ~IRemoteAircraftProviderReadOnly() {}

            //! Connect signals to slot receiver. As the interface is no QObject, slots can not be connected directly.
            virtual bool connectRemoteAircraftProviderSignals(
                std::function<void(const BlackMisc::Aviation::CAircraftSituation &)> situationSlot,
                std::function<void(const BlackMisc::Aviation::CAircraftParts &)> partsSlot,
                std::function<void(const BlackMisc::Aviation::CCallsign &)> removedAircraftSlot
            ) = 0;

            static const int MaxSituationsPerCallsign = 6; //!< How many situations per callsign
            static const int MaxPartsPerCallsign = 3;      //!< How many parts per callsign

            // those signals have to be implemented by classes using the interface.
        signals:
            //! New parts got added
            void addedRemoteAircraftParts(const BlackMisc::Aviation::CAircraftParts &parts);

            //! Aircraft was removed
            void removedRemoteAircraft(const BlackMisc::Aviation::CCallsign &callsign);

        };

        //! Direct in memory access to own aircraft
        class BLACKMISC_EXPORT IRemoteAircraftProvider : public IRemoteAircraftProviderReadOnly
        {
        public:

            //! The read only /sa IRemoteAircraftProviderReadOnly::remoteAircraft
            using IRemoteAircraftProviderReadOnly::remoteAircraft;

            //! All rendered aircraft
            //! \note in memory reference, not thread safe
            virtual CSimulatedAircraftList &remoteAircraft() = 0;

            //! Enable/disable rendering
            virtual bool updateAircraftEnabled(const BlackMisc::Aviation::CCallsign &callsign, bool enabledForRendering, const QString &originator) = 0;

            //! Change model string
            virtual bool updateAircraftModel(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Simulation::CAircraftModel &model, const QString &originator) = 0;

            //! Change fast position updates
            virtual bool updateFastPositionEnabled(const BlackMisc::Aviation::CCallsign &callsign, bool enableFastPositonUpdates, const QString &originator) = 0;
        };

        //! Class which can be directly used to access an \sa IRemoteAircraftProviderReadOnly object
        class BLACKMISC_EXPORT CRemoteAircraftAwareReadOnly
        {
        public:
            //! \copydoc IRemoteAircraftProviderReadOnly::renderedAircraft
            virtual const CSimulatedAircraftList &remoteAircraft() const;

            //! \copydoc IRemoteAircraftProviderReadOnly::remoteAircraftSituations
            virtual BlackMisc::Aviation::CAircraftSituationList remoteAircraftSituations(const BlackMisc::Aviation::CCallsign &callsign) const;

            //! \copydoc IRemoteAircraftProviderReadOnly::remoteAircraftSituationsCount
            virtual int remoteAircraftSituationsCount(const BlackMisc::Aviation::CCallsign &callsign) const;

            //! \copydoc IRemoteAircraftProviderReadOnly::remoteAircraftParts
            virtual BlackMisc::Aviation::CAircraftPartsList remoteAircraftParts(const BlackMisc::Aviation::CCallsign &callsign, qint64 cutoffTimeBefore = -1) const;

            //! \copydoc IRemoteAircraftProviderReadOnly::remoteAircraftSupportingParts
            virtual BlackMisc::Aviation::CCallsignSet remoteAircraftSupportingParts() const;

            //! \copydoc IRemoteAircraftProviderReadOnly::isRemoteAircraftSupportingParts
            virtual bool isRemoteAircraftSupportingParts(const BlackMisc::Aviation::CCallsign &callsign) const;

            //! Destructor
            virtual ~CRemoteAircraftAwareReadOnly() {}

        protected:
            //! Constructor
            CRemoteAircraftAwareReadOnly(const IRemoteAircraftProviderReadOnly *remoteAircraftProvider) : m_remoteAircraftProvider(remoteAircraftProvider) {}
            const IRemoteAircraftProviderReadOnly *m_remoteAircraftProvider = nullptr; //!< access to object
        };

        //! Class which can be directly used to access an \sa IRemoteAircraftProvider object
        class BLACKMISC_EXPORT CRemoteAircraftAware
        {
        public:
            //! \copydoc IRemoteAircraftProviderReadOnly::remoteAircraft
            virtual const CSimulatedAircraftList &remoteAircraft() const;

            //! \copydoc IRemoteAircraftProvider::remoteAircraft
            virtual CSimulatedAircraftList &remoteAircraft();

            //! \copydoc IRemoteAircraftProviderReadOnly::remoteAircraftSituations
            virtual BlackMisc::Aviation::CAircraftSituationList remoteAircraftSituations(const BlackMisc::Aviation::CCallsign &callsign) const;

            //! \copydoc IRemoteAircraftProviderReadOnly::remoteAircraftParts
            virtual BlackMisc::Aviation::CAircraftPartsList remoteAircraftParts(const BlackMisc::Aviation::CCallsign &callsign) const;

            //! \copydoc IRemoteAircraftProviderReadOnly::remoteAircraftSupportingParts
            virtual BlackMisc::Aviation::CCallsignSet remoteAircraftSupportingParts() const;

            //! \copydoc IRemoteAircraftProviderReadOnly::remoteAircraftSituationsCount
            virtual int remoteAircraftSituationsCount(const BlackMisc::Aviation::CCallsign &callsign) const;

            //! \copydoc IRemoteAircraftProvider::updateAircraftEnabled
            virtual bool providerUpdateAircraftEnabled(const BlackMisc::Aviation::CCallsign &callsign, bool enabledForRedering, const QString &originator);

            //! \copydoc IRemoteAircraftProvider::updateAircraftModel
            virtual bool providerUpdateAircraftModel(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Simulation::CAircraftModel &model, const QString &originator);

            //! \copydoc IRemoteAircraftProviderReadOnly::isRemoteAircraftSupportingParts
            virtual bool isRemoteAircraftSupportingParts(const BlackMisc::Aviation::CCallsign &callsign) const;

            //! Destructor
            virtual ~CRemoteAircraftAware() {}

        protected:
            //! Constructor
            CRemoteAircraftAware(IRemoteAircraftProvider *remoteAircraftProvider) : m_remoteAircraftProvider(remoteAircraftProvider) { Q_ASSERT(remoteAircraftProvider); }
            IRemoteAircraftProvider *m_remoteAircraftProvider = nullptr; //!< access to object
        };

    } // namespace
} // namespace

Q_DECLARE_INTERFACE(BlackMisc::Simulation::IRemoteAircraftProviderReadOnly, "IRemoteAircraftProviderReadOnly")
Q_DECLARE_INTERFACE(BlackMisc::Simulation::IRemoteAircraftProvider, "IRemoteAircraftProvider")

#endif // guard
