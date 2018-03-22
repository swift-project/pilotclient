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

#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/airspaceaircraftsnapshot.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/aviation/aircraftpartslist.h"
#include "blackmisc/aviation/aircraftsituationlist.h"
#include "blackmisc/aviation/callsignset.h"
#include "blackmisc/provider.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/identifier.h"

#include <QHash>
#include <QList>
#include <QMetaObject>
#include <QObject>
#include <QtGlobal>
#include <functional>

namespace BlackMisc
{
    namespace Aviation
    {
        class CAircraftParts;
        class CAircraftSituation;
        class CCallsign;
    }
    namespace Geo { class CElevationPlane; }
    namespace Simulation
    {
        //! Direct thread safe in memory access to remote aircraft
        //! \note Can not be derived from QObject (as for the signals), as this would create multiple
        //!       inheritance. Hence Q_DECLARE_INTERFACE is used.
        //! \ingroup remoteaircraftprovider
        class BLACKMISC_EXPORT IRemoteAircraftProvider : public IProvider
        {
        public:
            static constexpr int MaxSituationsPerCallsign = 6;    //!< How many situations we keep per callsign
            static constexpr int MaxPartsPerCallsign = 20;        //!< How many parts we keep per callsign (we keep more parts than situations as parts can just come in)
            static constexpr int MaxPartsAgePerCallsignSecs = 60; //!< How many seconds to keep parts for interpolation
            static constexpr int DefaultOffsetTimeMs = 6000;      //!< \fixme copied from CNetworkVatlib::c_positionTimeOffsetMsec

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

            //! Unique callsigns for aircraft in range
            //! \threadsafe
            virtual BlackMisc::Aviation::CCallsignSet getAircraftInRangeCallsigns() const = 0;

            //! Is aircraft in range?
            //! \threadsafe
            virtual bool isAircraftInRange(const BlackMisc::Aviation::CCallsign &callsign) const = 0;

            //! Is VTOL aircraft?
            //! \threadsafe
            virtual bool isVtolAircraft(const BlackMisc::Aviation::CCallsign &callsign) const = 0;

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

            //! Get the latest aircraft parts (if any, otherwise default)
            //! \threadsafe
            BlackMisc::Aviation::CAircraftParts getLatestAircraftParts(const BlackMisc::Aviation::CCallsign &callsign) const;

            //! Number of aircraft supporting parts
            //! \threadsafe
            virtual int getRemoteAircraftSupportingPartsCount() const = 0;

            //! Remote aircraft supporting parts.
            //! \threadsafe
            virtual BlackMisc::Aviation::CCallsignSet remoteAircraftSupportingParts() const = 0;

            //! Enable/disable rendering
            //! \threadsafe
            virtual bool updateAircraftEnabled(const BlackMisc::Aviation::CCallsign &callsign, bool enabledForRendering) = 0;

            //! Rendered?
            //! \threadsafe
            virtual bool updateAircraftRendered(const BlackMisc::Aviation::CCallsign &callsign, bool rendered) = 0;

            //! Mark all as not rendered
            //! \threadsafe
            virtual void updateMarkAllAsNotRendered() = 0;

            //! Change model
            //! \threadsafe
            virtual bool updateAircraftModel(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Simulation::CAircraftModel &model, const BlackMisc::CIdentifier &originator) = 0;

            //! Change network model
            //! \threadsafe
            virtual bool updateAircraftNetworkModel(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Simulation::CAircraftModel &model, const BlackMisc::CIdentifier &originator) = 0;

            //! Change fast position updates
            //! \threadsafe
            virtual bool updateFastPositionEnabled(const BlackMisc::Aviation::CCallsign &callsign, bool enableFastPositonUpdates) = 0;

            //! Ground elevation of aircraft
            //! \threadsafe
            virtual bool updateAircraftGroundElevation(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Geo::CElevationPlane &elevation) = 0;

            //! Get reverse lookup meesages
            //! \threadsafe
            virtual BlackMisc::CStatusMessageList getReverseLookupMessages(const BlackMisc::Aviation::CCallsign &callsign) const = 0;

            //! Enabled reverse lookup logging?
            //! \threadsafe
            virtual bool isReverseLookupMessagesEnabled() const = 0;

            //! Enable reverse lookup logging
            //! \threadsafe
            virtual void enableReverseLookupMessages(bool enabled) = 0;

            //! Get aircraft parts history
            //! \threadsafe
            virtual BlackMisc::CStatusMessageList getAircraftPartsHistory(const BlackMisc::Aviation::CCallsign &callsign) const = 0;

            //! Is storing aircraft parts history enabled?
            //! \threadsafe
            virtual bool isAircraftPartsHistoryEnabled() const = 0;

            //! Enable storing of aircraft parts history
            //! \threadsafe
            virtual void enableAircraftPartsHistory(bool enabled) = 0;

            //! Destructor
            virtual ~IRemoteAircraftProvider() {}

            //! Connect signals to slot receiver. As the interface is no QObject, slots can not be connected directly.
            //! In order to disconnect a list of connections is provided, which have to be disconnected manually.
            //! \note receiver is required for connection type
            virtual QList<QMetaObject::Connection> connectRemoteAircraftProviderSignals(
                QObject *receiver,
                std::function<void(const BlackMisc::Aviation::CAircraftSituation &)> addedSituationSlot,
                std::function<void(const BlackMisc::Aviation::CCallsign &, const BlackMisc::Aviation::CAircraftParts &)> addedPartsSlot,
                std::function<void(const BlackMisc::Aviation::CCallsign &)> removedAircraftSlot,
                std::function<void(const BlackMisc::Simulation::CAirspaceAircraftSnapshot &)> aircraftSnapshot
            ) = 0;

            //! Remove outdated aircraft parts, but never the most recent one
            static void removeOutdatedParts(Aviation::CAircraftPartsList &partsList);
        };

        //! Class which can be directly used to access an \sa IRemoteAircraftProvider object
        class BLACKMISC_EXPORT CRemoteAircraftAware : public IProviderAware<IRemoteAircraftProvider>
        {
        public:
            //! \copydoc IRemoteAircraftProvider::getAircraftInRange
            BlackMisc::Simulation::CSimulatedAircraftList getAircraftInRange() const;

            //! \copydoc IRemoteAircraftProvider::isAircraftInRange
            bool isAircraftInRange(const BlackMisc::Aviation::CCallsign &callsign) const;

            //! \copydoc IRemoteAircraftProvider::isVtolAircraft
            bool isVtolAircraft(const BlackMisc::Aviation::CCallsign &callsign) const;

            //! \copydoc IRemoteAircraftProvider::getAircraftInRangeCount
            int getAircraftInRangeCount() const;

            //! \copydoc IRemoteAircraftProvider::getAircraftInRangeCallsigns
            BlackMisc::Aviation::CCallsignSet getAircraftInRangeCallsigns() const;

            //! \copydoc IRemoteAircraftProvider::getAircraftInRangeForCallsign
            BlackMisc::Simulation::CSimulatedAircraft getAircraftInRangeForCallsign(const Aviation::CCallsign &callsign) const;

            //! \copydoc IRemoteAircraftProvider::getAircraftInRangeModelForCallsign
            BlackMisc::Simulation::CAircraftModel getAircraftInRangeModelForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const;

            //! \copydoc IRemoteAircraftProvider::getLatestAirspaceAircraftSnapshot
            BlackMisc::Simulation::CAirspaceAircraftSnapshot getLatestAirspaceAircraftSnapshot() const;

            //! \copydoc IRemoteAircraftProvider::remoteAircraftSituations
            BlackMisc::Aviation::CAircraftSituationList remoteAircraftSituations(const BlackMisc::Aviation::CCallsign &callsign) const;

            //! \copydoc IRemoteAircraftProvider::remoteAircraftParts
            BlackMisc::Aviation::CAircraftPartsList remoteAircraftParts(const BlackMisc::Aviation::CCallsign &callsign, qint64 cutoffTimeBefore) const;

            //! \copydoc IRemoteAircraftProvider::remoteAircraftSupportingParts
            BlackMisc::Aviation::CCallsignSet remoteAircraftSupportingParts() const;

            //! \copydoc IRemoteAircraftProvider::remoteAircraftSituationsCount
            int remoteAircraftSituationsCount(const BlackMisc::Aviation::CCallsign &callsign) const;

            //! \copydoc IRemoteAircraftProvider::isRemoteAircraftSupportingParts
            bool isRemoteAircraftSupportingParts(const BlackMisc::Aviation::CCallsign &callsign) const;

            //! \copydoc IRemoteAircraftProvider::getRemoteAircraftSupportingPartsCount
            int getRemoteAircraftSupportingPartsCount() const;

            //! \copydoc IRemoteAircraftProvider::updateAircraftEnabled
            bool updateAircraftEnabled(const BlackMisc::Aviation::CCallsign &callsign, bool enabledForRedering);

            //! \copydoc IRemoteAircraftProvider::updateAircraftModel
            bool updateAircraftModel(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Simulation::CAircraftModel &model, const BlackMisc::CIdentifier &originator);

            //! \copydoc IRemoteAircraftProvider::updateAircraftNetworkModel
            bool updateAircraftNetworkModel(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Simulation::CAircraftModel &model, const BlackMisc::CIdentifier &originator);

            //! \copydoc IRemoteAircraftProvider::updateAircraftRendered
            bool updateAircraftRendered(const BlackMisc::Aviation::CCallsign &callsign, bool rendered);

            //! \copydoc IRemoteAircraftProvider::updateAircraftGroundElevation
            bool updateAircraftGroundElevation(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Geo::CElevationPlane &elevation);

            //! \copydoc IRemoteAircraftProvider::updateMarkAllAsNotRendered
            void updateMarkAllAsNotRendered();

            //! Set remote aircraft provider
            void setRemoteAircraftProvider(IRemoteAircraftProvider *remoteAircraftProvider) { this->setProvider(remoteAircraftProvider); }

        protected:
            //! Default constructor
            CRemoteAircraftAware() {}

            //! Constructor
            CRemoteAircraftAware(IRemoteAircraftProvider *remoteAircraftProvider) : IProviderAware(remoteAircraftProvider) { Q_ASSERT(remoteAircraftProvider); }
        };
    } // namespace
} // namespace

Q_DECLARE_INTERFACE(BlackMisc::Simulation::IRemoteAircraftProvider, "org.swift-project.blackmisc::simulation::iremoteaircraftprovider")

#endif // guard
