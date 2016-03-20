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
#include "blackmisc/identifier.h"
#include "blackmisc/simulation/airspaceaircraftsnapshot.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/aviation/aircraftsituationlist.h"
#include "blackmisc/aviation/aircraftpartslist.h"

#include <functional>

namespace BlackMisc
{
    namespace Simulation
    {
        //! Direct thread safe in memory access to remote aircraft
        //! \note Can not be derived from QObject (as for the signals), as this would create multiple
        //!       inheritance. Hence Q_DECLARE_INTERFACE is used.
        //! \ingroup remoteaircraftprovider
        class BLACKMISC_EXPORT IRemoteAircraftProvider
        {
        public:
            static constexpr int MaxSituationsPerCallsign = 6; //!< How many situations per callsign
            static constexpr int MaxPartsPerCallsign = 3;      //!< How many parts per callsign

            //! Situations per callsign
            using CSituationsPerCallsign = QHash<BlackMisc::Aviation::CCallsign, BlackMisc::Aviation::CAircraftSituationList>;

            //! Parts per callsign
            using CPartsPerCallsign = QHash<BlackMisc::Aviation::CCallsign, BlackMisc::Aviation::CAircraftPartsList>;

            //! All remote aircraft
            //! \threadsafe
            virtual BlackMisc::Simulation::CSimulatedAircraftList getAircraftInRange() const = 0;

            //! Count remote aircraft
            //! \threadsafe
            virtual int getAircraftInRangeCount() const = 0;

            //! Current snapshot
            //! \threadsafe
            virtual BlackMisc::Simulation::CAirspaceAircraftSnapshot getLatestAirspaceAircraftSnapshot() const = 0;

            //! Aircraft for callsign
            //! \threadsafe
            virtual BlackMisc::Simulation::CSimulatedAircraft getAircraftInRangeForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const = 0;

            //! Aircraft model for callsign
            //! \threadsafe
            virtual BlackMisc::Simulation::CAircraftModel getAircraftInRangeModelForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const = 0;

            //! Rendered aircraft situations (per callsign, time history)
            //! \threadsafe
            virtual BlackMisc::Aviation::CAircraftSituationList remoteAircraftSituations(const BlackMisc::Aviation::CCallsign &callsign) const = 0;

            //! Number of remote aircraft situations for callsign
            //! \threadsafe
            virtual int remoteAircraftSituationsCount(const BlackMisc::Aviation::CCallsign &callsign) const = 0;

            //! All parts (per callsign, time history)
            //! \threadsafe
            virtual BlackMisc::Aviation::CAircraftPartsList remoteAircraftParts(const BlackMisc::Aviation::CCallsign &callsign, qint64 cutoffTimeBefore) const = 0;

            //! Is remote aircraft supporting parts?
            //! \threadsafe
            virtual bool isRemoteAircraftSupportingParts(const BlackMisc::Aviation::CCallsign &callsign) const = 0;

            //! Remote aircraft supporting parts.
            //! \threadsafe
            virtual BlackMisc::Aviation::CCallsignSet remoteAircraftSupportingParts() const = 0;

            //! Enable/disable rendering
            //! \threadsafe
            virtual bool updateAircraftEnabled(const BlackMisc::Aviation::CCallsign &callsign, bool enabledForRendering, const BlackMisc::CIdentifier &originator) = 0;

            //! Rendered?
            //! \threadsafe
            virtual bool updateAircraftRendered(const BlackMisc::Aviation::CCallsign &callsign, bool rendered, const BlackMisc::CIdentifier &originator) = 0;

            //! Mark all as not rendered
            //! \threadsafe
            virtual void updateMarkAllAsNotRendered(const BlackMisc::CIdentifier &originator) = 0;

            //! Change model string
            //! \threadsafe
            virtual bool updateAircraftModel(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Simulation::CAircraftModel &model, const BlackMisc::CIdentifier &originator) = 0;

            //! Change fast position updates
            //! \threadsafe
            virtual bool updateFastPositionEnabled(const BlackMisc::Aviation::CCallsign &callsign, bool enableFastPositonUpdates, const BlackMisc::CIdentifier &originator) = 0;

            //! Destructor
            virtual ~IRemoteAircraftProvider() {}

            //! Connect signals to slot receiver. As the interface is no QObject, slots can not be connected directly.
            //! In order to disconnect a list of connections is provided, which have to be disconnected manually.
            //! \note receiver is required for connection type
            virtual QList<QMetaObject::Connection> connectRemoteAircraftProviderSignals(
                QObject *receiver,
                std::function<void(const BlackMisc::Aviation::CAircraftSituation &)>          addedSituationSlot,
                std::function<void(const BlackMisc::Aviation::CCallsign &, const BlackMisc::Aviation::CAircraftParts &)> addedPartsSlot,
                std::function<void(const BlackMisc::Aviation::CCallsign &)>                   removedAircraftSlot,
                std::function<void(const BlackMisc::Simulation::CAirspaceAircraftSnapshot &)> aircraftSnapshot
            ) = 0;

        };

        //! Class which can be directly used to access an \sa IRemoteAircraftProvider object
        class BLACKMISC_EXPORT CRemoteAircraftAware
        {
        public:
            //! \copydoc IRemoteAircraftProvider::getAircraftInRange
            virtual BlackMisc::Simulation::CSimulatedAircraftList getAircraftInRange() const;

            //! \copydoc IRemoteAircraftProvider::getAircraftInRangeCount
            int getAircraftInRangeCount() const;

            //! \copydoc IRemoteAircraftProvider::getAircraftInRangeForCallsign
            virtual BlackMisc::Simulation::CSimulatedAircraft getAircraftInRangeForCallsign(const Aviation::CCallsign &callsign) const;

            //! \copydoc IRemoteAircraftProvider::getAircraftInRangeModelForCallsign
            virtual BlackMisc::Simulation::CAircraftModel getAircraftInRangeModelForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const;

            //! \copydoc IRemoteAircraftProvider::getLatestAirspaceAircraftSnapshot
            virtual BlackMisc::Simulation::CAirspaceAircraftSnapshot getLatestAirspaceAircraftSnapshot() const;

            //! \copydoc IRemoteAircraftProvider::remoteAircraftSituations
            virtual BlackMisc::Aviation::CAircraftSituationList remoteAircraftSituations(const BlackMisc::Aviation::CCallsign &callsign) const;

            //! \copydoc IRemoteAircraftProvider::remoteAircraftParts
            virtual BlackMisc::Aviation::CAircraftPartsList remoteAircraftParts(const BlackMisc::Aviation::CCallsign &callsign, qint64 cutoffTimeBefore) const;

            //! \copydoc IRemoteAircraftProvider::remoteAircraftSupportingParts
            virtual BlackMisc::Aviation::CCallsignSet remoteAircraftSupportingParts() const;

            //! \copydoc IRemoteAircraftProvider::remoteAircraftSituationsCount
            virtual int remoteAircraftSituationsCount(const BlackMisc::Aviation::CCallsign &callsign) const;

            //! \copydoc IRemoteAircraftProvider::isRemoteAircraftSupportingParts
            virtual bool isRemoteAircraftSupportingParts(const BlackMisc::Aviation::CCallsign &callsign) const;

            //! \copydoc IRemoteAircraftProvider::updateAircraftEnabled
            virtual bool updateAircraftEnabled(const BlackMisc::Aviation::CCallsign &callsign, bool enabledForRedering, const BlackMisc::CIdentifier &originator);

            //! \copydoc IRemoteAircraftProvider::updateAircraftModel
            virtual bool updateAircraftModel(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Simulation::CAircraftModel &model, const BlackMisc::CIdentifier &originator);

            //! \copydoc IRemoteAircraftProvider::updateAircraftRendered
            virtual bool updateAircraftRendered(const BlackMisc::Aviation::CCallsign &callsign, bool rendered, const BlackMisc::CIdentifier &originator);

            //! \copydoc IRemoteAircraftProvider::updateMarkAllAsNotRendered
            virtual void updateMarkAllAsNotRendered(const CIdentifier &originator);

            //! Destructor
            virtual ~CRemoteAircraftAware() {}

        protected:
            //! Constructor
            CRemoteAircraftAware(IRemoteAircraftProvider *remoteAircraftProvider) : m_remoteAircraftProvider(remoteAircraftProvider) { Q_ASSERT(remoteAircraftProvider); }
            IRemoteAircraftProvider *m_remoteAircraftProvider = nullptr; //!< access to object
        };

    } // namespace
} // namespace

Q_DECLARE_INTERFACE(BlackMisc::Simulation::IRemoteAircraftProvider, "BlackMisc::Simulation::IRemoteAircraftProvider")

#endif // guard
