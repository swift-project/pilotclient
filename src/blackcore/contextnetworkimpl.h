/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_CONTEXTNETWORK_IMPL_H
#define BLACKCORE_CONTEXTNETWORK_IMPL_H

#include "blackcoreexport.h"
#include "blackcore/contextnetwork.h"
#include "blackcore/contextruntime.h"
#include "blackcore/dbusserver.h"
#include "blackcore/network.h"
#include "blackmisc/simulation/remoteaircraftprovider.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/aircraftsituationlist.h"
#include "blackmisc/weather/metarset.h"
#include "blackmisc/network/clientlist.h"
#include "blackmisc/digestsignal.h"
#include "blackmisc/logmessage.h"

#include <QMap>
#include <QTimer>

namespace BlackCore
{
    class CAirspaceMonitor;
    class CVatsimMetarReader;
    class CWebDataServices;

    //! Network context implementation
    class BLACKCORE_EXPORT CContextNetwork :
        public IContextNetwork,
        public BlackMisc::Simulation::IRemoteAircraftProvider
    {
        Q_OBJECT
        Q_INTERFACES(BlackMisc::Simulation::IRemoteAircraftProvider)
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTNETWORK_INTERFACENAME)

        friend class IContextNetwork;
        friend class CRuntime;

    public:
        //! Destructor
        virtual ~CContextNetwork();

        //! \copydoc IRemoteAircraftProvider::remoteAircraftSituations
        //! \ingroup remoteaircraftprovider
        virtual BlackMisc::Aviation::CAircraftSituationList remoteAircraftSituations(const BlackMisc::Aviation::CCallsign &callsign) const override;

        //! \copydoc IRemoteAircraftProvider::remoteAircraftSituationsCount
        //! \ingroup remoteaircraftprovider
        virtual int remoteAircraftSituationsCount(const BlackMisc::Aviation::CCallsign &callsign) const override;

        //! \copydoc IRemoteAircraftProvider::remoteAircraftParts
        //! \ingroup remoteaircraftprovider
        virtual BlackMisc::Aviation::CAircraftPartsList remoteAircraftParts(const BlackMisc::Aviation::CCallsign &callsign, qint64 cutoffTimeBefore = -1) const override;

        //! \copydoc IRemoteAircraftProvider::isRemoteAircraftSupportingParts
        //! \ingroup remoteaircraftprovider
        virtual bool isRemoteAircraftSupportingParts(const BlackMisc::Aviation::CCallsign &callsign) const;

        //! \copydoc IRemoteAircraftProvider::remoteAircraftSupportingParts
        //! \ingroup remoteaircraftprovider
        virtual BlackMisc::Aviation::CCallsignSet remoteAircraftSupportingParts() const override;

        //! \copydoc IRemoteAircraftProvider::connectSignals
        //! \ingroup remoteaircraftprovider
        virtual QList<QMetaObject::Connection> connectRemoteAircraftProviderSignals(
            QObject *receiver,
            std::function<void(const BlackMisc::Aviation::CAircraftSituation &)>          addedSituationSlot,
            std::function<void(const BlackMisc::Aviation::CCallsign &, const BlackMisc::Aviation::CAircraftParts &)>    addedPartsSlot,
            std::function<void(const BlackMisc::Aviation::CCallsign &)>                   removedAircraftSlot,
            std::function<void(const BlackMisc::Simulation::CAirspaceAircraftSnapshot &)> aircraftSnapshotSlot
        ) override;

        //! \copydoc IRemoteAircraftProvider::updateAircraftRendered
        //! \ingroup remoteaircraftprovider
        virtual bool updateAircraftRendered(const BlackMisc::Aviation::CCallsign &callsign, bool rendered, const BlackMisc::CIdentifier &originator) override;

        //! \copydoc IRemoteAircraftProvider::updateMarkAllAsNotRendered
        //! \ingroup remoteaircraftprovider
        virtual void updateMarkAllAsNotRendered(const BlackMisc::CIdentifier &originator) override;

        //! \copydoc IRemoteAircraftProvider::getLatestAirspaceAircraftSnapshot
        //! \ingroup remoteaircraftprovider
        virtual BlackMisc::Simulation::CAirspaceAircraftSnapshot getLatestAirspaceAircraftSnapshot() const override;

        //! Network library
        INetwork *network() const { return m_network; }

    public slots:
        //! \copydoc IContextNetwork::updateAircraftEnabled
        //! \ingroup remoteaircraftprovider
        virtual bool updateAircraftEnabled(const BlackMisc::Aviation::CCallsign &callsign, bool enabledForRedering, const BlackMisc::CIdentifier &originator) override;

        //! \copydoc IContextNetwork::updateAircraftModel
        //! \ingroup remoteaircraftprovider
        virtual bool updateAircraftModel(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Simulation::CAircraftModel &model, const BlackMisc::CIdentifier &originator) override;

        //! \copydoc IContextNetwork::updateFastPositionEnabled
        //! \ingroup remoteaircraftprovider
        virtual bool updateFastPositionEnabled(const BlackMisc::Aviation::CCallsign &callsign, bool enableFastPositonUpdates, const BlackMisc::CIdentifier &originator) override;

        //! \copydoc IContextNetwork::readAtcBookingsFromSource()
        virtual void readAtcBookingsFromSource() const override;

        //! \copydoc IContextNetwork::getAtcStationsOnline()
        virtual BlackMisc::Aviation::CAtcStationList getAtcStationsOnline() const override;

        //! \copydoc IContextNetwork::getAtcStationsBooked()
        virtual BlackMisc::Aviation::CAtcStationList getAtcStationsBooked() const override;

        //! \copydoc IContextNetwork::getAircraftInRange
        //! \ingroup remoteaircraftprovider
        virtual BlackMisc::Simulation::CSimulatedAircraftList getAircraftInRange() const override;

        //! \copydoc IRemoteAircraftProvider::getAircraftInRangeCount
        //! \ingroup remoteaircraftprovider
        virtual int getAircraftInRangeCount() const override;

        //! \copydoc IContextNetwork::getAircraftInRangeForCallsign
        //! \ingroup remoteaircraftprovider
        virtual BlackMisc::Simulation::CSimulatedAircraft getAircraftInRangeForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const override;

        //! \copydoc IContextNetwork::getAircraftinRangeModelForCallsign
        //! \ingroup remoteaircraftprovider
        virtual BlackMisc::Simulation::CAircraftModel getAircraftInRangeModelForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const override;

        //! \copydoc IContextNetwork::getOnlineStationForCallsign
        virtual BlackMisc::Aviation::CAtcStation getOnlineStationForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const override;

        //! \copydoc IContextNetwork::connectToNetwork()
        virtual BlackMisc::CStatusMessage connectToNetwork(const BlackMisc::Network::CServer &server, BlackCore::INetwork::LoginMode mode) override;

        //! \copydoc IContextNetwork::getConnectedServer
        virtual BlackMisc::Network::CServer getConnectedServer() const override;

        //! \copydoc IContextNetwork::disconnectFromNetwork()
        virtual BlackMisc::CStatusMessage disconnectFromNetwork() override;

        //! \copydoc IContextNetwork::isConnected()
        virtual bool isConnected() const override;

        //! In transition state, e.g. connecting, disconnecting.
        //! \details In such a state it is advisable to wait until an end state (connected/disconnected) is reached
        //! \remarks Intentionally only running locally, not in interface
        bool isPendingConnection() const;

        //! \ingroup commandline
        //! @{
        //! <pre>
        //! .m  .msg   message text
        //! </pre>
        //! @}
        //! \copydoc IContextNetwork::parseCommandLine
        virtual bool parseCommandLine(const QString &commandLine, const BlackMisc::CIdentifier &originator) override;

        //! \copydoc IContextNetwork::sendTextMessages()
        virtual void sendTextMessages(const BlackMisc::Network::CTextMessageList &textMessages) override;

        //! \copydoc IContextNetwork::sendFlightPlan()
        virtual void sendFlightPlan(const BlackMisc::Aviation::CFlightPlan &flightPlan) override;

        //! \copydoc IContextNetwork::loadFlightPlanFromNetwork()
        virtual BlackMisc::Aviation::CFlightPlan loadFlightPlanFromNetwork(const BlackMisc::Aviation::CCallsign &callsign) const override;

        //! \copydoc IContextNetwork::getMetarForAirport
        BlackMisc::Weather::CMetar getMetarForAirport(const BlackMisc::Aviation::CAirportIcaoCode &airportIcaoCode) const override;

        //! \copydoc IContextNetwork::getSelectedVoiceRooms()
        virtual BlackMisc::Audio::CVoiceRoomList getSelectedVoiceRooms() const override;

        //! \copydoc IContextNetwork::getSelectedAtcStations
        virtual BlackMisc::Aviation::CAtcStationList getSelectedAtcStations() const override;

        //! \copydoc IContextNetwork::getUsers()
        virtual BlackMisc::Network::CUserList getUsers() const override;

        //! \copydoc IContextNetwork::getUsersForCallsigns
        virtual BlackMisc::Network::CUserList getUsersForCallsigns(const BlackMisc::Aviation::CCallsignSet &callsigns) const override;

        //! \copydoc IContextNetwork::getUsersForCallsign
        virtual BlackMisc::Network::CUser getUserForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const override;

        //! \copydoc IContextNetwork::getOtherClients
        virtual BlackMisc::Network::CClientList getOtherClients() const override;

        //! \copydoc IContextNetwork::getOtherClientForCallsigns
        virtual BlackMisc::Network::CClientList getOtherClientsForCallsigns(const BlackMisc::Aviation::CCallsignSet &callsigns) const override;

        //! \copydoc IContextNetwork::getVatsimFsdServers
        virtual BlackMisc::Network::CServerList getVatsimFsdServers() const override;

        //! \copydoc IContextNetwork::getVatsimVoiceServers
        virtual BlackMisc::Network::CServerList getVatsimVoiceServers() const override;

        //! \copydoc IContextNetwork::requestDataUpdates
        virtual void requestDataUpdates()override;

        //! \copydoc IContextNetwork::requestAtisUpdates
        virtual void requestAtisUpdates() override;

        //! \copydoc IContextNetwork::isFastPositionSendingEnabled
        virtual bool isFastPositionSendingEnabled() const override;

        //! \copydoc IContextNetwork::enableFastPositionSending
        virtual void enableFastPositionSending(bool enable) override;

        //! \copydoc IContextNetwork::setFastPositionEnabledCallsigns
        virtual void setFastPositionEnabledCallsigns(BlackMisc::Aviation::CCallsignSet &callsigns) override;

        //! \copydoc IContextNetwork::getFastPositionEnabledCallsigns
        virtual BlackMisc::Aviation::CCallsignSet getFastPositionEnabledCallsigns() override;

        //! \copydoc IContextNetwork::testCreateDummyOnlineAtcStations
        virtual void testCreateDummyOnlineAtcStations(int number) override;

        //! \copydoc IContextNetwork::testAddAircraftParts
        virtual void testAddAircraftParts(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftParts &parts, bool incremental) override;

        //! Gracefully shut down, e.g. for thread safety
        void gracefulShutdown();

    protected:
        //! Constructor, with link to runtime
        CContextNetwork(CRuntimeConfig::ContextMode, CRuntime *runtime);

        //! Register myself in DBus
        CContextNetwork *registerWithDBus(CDBusServer *server);

    private:
        CAirspaceMonitor              *m_airspace = nullptr;
        INetwork                      *m_network  = nullptr;
        CWebDataServices              *m_webDataReader = nullptr;                //!< web service readers
        QList<QMetaObject::Connection> m_webReaderSignalConnections;
        INetwork::ConnectionStatus     m_currentStatus = INetwork::Disconnected; //!< used to detect pending connections
        QTimer                        *m_dataUpdateTimer = nullptr;              //!< general updates such as ATIS, frequencies, see requestDataUpdates()

        // Digest signals, only sending after some time
        BlackMisc::CDigestSignal m_dsAtcStationsBookedChanged { this, &IContextNetwork::changedAtcStationsBooked, &IContextNetwork::changedAtcStationsBookedDigest, 750, 2 };
        BlackMisc::CDigestSignal m_dsAtcStationsOnlineChanged { this, &IContextNetwork::changedAtcStationsOnline, &IContextNetwork::changedAtcStationsOnlineDigest, 750, 4 };
        BlackMisc::CDigestSignal m_dsAircraftsInRangeChanged  { this, &IContextNetwork::changedAircraftInRange, &IContextNetwork::changedAircraftInRangeDigest, 750, 4 };

        //! Own aircraft from \sa CContextOwnAircraft
        const BlackMisc::Simulation::CSimulatedAircraft ownAircraft() const;

        //! Disconnect all signals from swift DB readers
        void disconnectReaderSignals();

    private slots:
        //! Update METAR collection
        void ps_updateMetars(const BlackMisc::Weather::CMetarSet &metars);

        //! Check if a supervisor message was received
        void ps_checkForSupervisiorTextMessage(const BlackMisc::Network::CTextMessageList &messages);

        //! Connection status changed
        void ps_fsdConnectionStatusChanged(INetwork::ConnectionStatus from, INetwork::ConnectionStatus to);

        //! Render restrictions have been changed, used with analyzer
        //! \sa CAirspaceAnalyzer
        void ps_simulatorRenderRestrictionsChanged(bool restricted, bool enabled, int maxAircraft, const BlackMisc::PhysicalQuantities::CLength &maxRenderedDistance, const BlackMisc::PhysicalQuantities::CLength &maxRenderedBoundary);
    };
} // ns

#endif // guard
