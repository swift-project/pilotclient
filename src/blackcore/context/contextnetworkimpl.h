/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_CONTEXT_CONTEXTNETWORK_IMPL_H
#define BLACKCORE_CONTEXT_CONTEXTNETWORK_IMPL_H

#include <QList>
#include <QMetaObject>
#include <QObject>
#include <QString>
#include <QtGlobal>
#include <functional>

#include "blackcore/blackcoreexport.h"
#include "blackcore/context/contextnetwork.h"
#include "blackcore/corefacadeconfig.h"
#include "blackcore/network.h"
#include "blackmisc/audio/voiceroomlist.h"
#include "blackmisc/aviation/aircraftpartslist.h"
#include "blackmisc/aviation/aircraftsituationlist.h"
#include "blackmisc/aviation/airporticaocode.h"
#include "blackmisc/aviation/atcstation.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/callsignset.h"
#include "blackmisc/aviation/flightplan.h"
#include "blackmisc/network/clientlist.h"
#include "blackmisc/network/server.h"
#include "blackmisc/network/serverlist.h"
#include "blackmisc/network/textmessagelist.h"
#include "blackmisc/network/user.h"
#include "blackmisc/network/userlist.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/airspaceaircraftsnapshot.h"
#include "blackmisc/simulation/remoteaircraftprovider.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/weather/metar.h"
#include "blackmisc/weather/metarlist.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/digestsignal.h"
#include "blackmisc/identifier.h"
#include "blackmisc/simplecommandparser.h"

class QTimer;

namespace BlackMisc
{
    class CDBusServer;
    namespace Aviation
    {
        class CAircraftParts;
        class CAircraftSituation;
        class CCallsign;
    }
}

namespace BlackCore
{
    class CAirspaceMonitor;
    class CCoreFacade;
    namespace Context
    {
        //! Network context implementation
        class BLACKCORE_EXPORT CContextNetwork :
            public IContextNetwork,
            public BlackMisc::Simulation::IRemoteAircraftProvider
        {
            Q_OBJECT
            Q_INTERFACES(BlackMisc::Simulation::IRemoteAircraftProvider)
            Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTNETWORK_INTERFACENAME)

            friend class IContextNetwork;
            friend class BlackCore::CCoreFacade;

        public:
            //! Destructor
            virtual ~CContextNetwork();

            //! \ingroup remoteaircraftprovider
            //! @{
            virtual BlackMisc::Aviation::CAircraftSituationList remoteAircraftSituations(const BlackMisc::Aviation::CCallsign &callsign) const override;
            virtual int remoteAircraftSituationsCount(const BlackMisc::Aviation::CCallsign &callsign) const override;
            virtual BlackMisc::Aviation::CAircraftPartsList remoteAircraftParts(const BlackMisc::Aviation::CCallsign &callsign, qint64 cutoffTimeBefore = -1) const override;
            virtual bool isRemoteAircraftSupportingParts(const BlackMisc::Aviation::CCallsign &callsign) const override;
            virtual BlackMisc::Aviation::CCallsignSet remoteAircraftSupportingParts() const override;
            virtual QList<QMetaObject::Connection> connectRemoteAircraftProviderSignals(
                QObject *receiver,
                std::function<void(const BlackMisc::Aviation::CAircraftSituation &)>          addedSituationSlot,
                std::function<void(const BlackMisc::Aviation::CCallsign &, const BlackMisc::Aviation::CAircraftParts &)>    addedPartsSlot,
                std::function<void(const BlackMisc::Aviation::CCallsign &)>                   removedAircraftSlot,
                std::function<void(const BlackMisc::Simulation::CAirspaceAircraftSnapshot &)> aircraftSnapshotSlot
            ) override;
            virtual bool updateAircraftRendered(const BlackMisc::Aviation::CCallsign &callsign, bool rendered) override;
            virtual bool updateAircraftGroundElevation(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Geo::CElevationPlane &elevation) override;
            virtual void updateMarkAllAsNotRendered() override;
            virtual BlackMisc::Simulation::CAirspaceAircraftSnapshot getLatestAirspaceAircraftSnapshot() const override;
            //! @}

        public slots:
            // from context and provider interface
            //! \ingroup remoteaircraftprovider
            //! @{
            virtual bool updateAircraftEnabled(const BlackMisc::Aviation::CCallsign &callsign, bool enabledForRendering) override;
            virtual bool updateAircraftModel(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Simulation::CAircraftModel &model, const BlackMisc::CIdentifier &originator) override;
            virtual bool updateAircraftNetworkModel(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Simulation::CAircraftModel &model, const BlackMisc::CIdentifier &originator) override;
            virtual bool updateFastPositionEnabled(const BlackMisc::Aviation::CCallsign &callsign, bool enableFastPositonUpdates) override;
            virtual BlackMisc::Simulation::CSimulatedAircraftList getAircraftInRange() const override;
            virtual BlackMisc::Aviation::CCallsignSet getAircraftInRangeCallsigns() const override;
            virtual int getAircraftInRangeCount() const override;
            virtual bool isAircraftInRange(const BlackMisc::Aviation::CCallsign &callsign) const override;
            virtual BlackMisc::Simulation::CSimulatedAircraft getAircraftInRangeForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const override;
            virtual BlackMisc::Simulation::CAircraftModel getAircraftInRangeModelForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const override;
            virtual BlackMisc::CStatusMessageList getReverseLookupMessages(const BlackMisc::Aviation::CCallsign &callsign) const override;
            virtual bool isReverseLookupMessagesEnabled() const override;
            virtual void enableReverseLookupMessages(bool enabled) override;
            virtual BlackMisc::CStatusMessageList getAircraftPartsHistory(const BlackMisc::Aviation::CCallsign &callsign) const override;
            virtual BlackMisc::Aviation::CAircraftPartsList getRemoteAircraftParts(const BlackMisc::Aviation::CCallsign &callsign, qint64 cutoffTimeValuesBefore) const override;
            virtual int getRemoteAircraftSupportingPartsCount() const override;
            virtual bool isAircraftPartsHistoryEnabled() const override;
            virtual void enableAircraftPartsHistory(bool enabled) override;
            //! @}

            //! In transition state, e.g. connecting, disconnecting.
            //! \details In such a state it is advisable to wait until an end state (connected/disconnected) is reached
            //! \remarks Intentionally only running locally, not in interface
            bool isPendingConnection() const;

            // --------------------- IContextNetwork implementations ---------------------
            //! \addtogroup swiftdotcommands
            //! @{
            //! <pre>
            //! .m .msg   message text
            //! </pre>
            //! @}
            //! \copydoc IContextNetwork::parseCommandLine
            virtual bool parseCommandLine(const QString &commandLine, const BlackMisc::CIdentifier &originator) override;

            //! Register help
            static void registerHelp()
            {
                if (BlackMisc::CSimpleCommandParser::registered("BlackCore::CContextNetwork")) { return; }
                BlackMisc::CSimpleCommandParser::registerCommand({".m", "alias: .msg"});
                BlackMisc::CSimpleCommandParser::registerCommand({".m message <text>", "send text message"});
                BlackMisc::CSimpleCommandParser::registerCommand({".m callsign message <text>", "send text message"});
            }

            //! \publicsection
            //! @{
            virtual BlackMisc::Aviation::CAtcStationList getAtcStationsBooked() const override;
            virtual void requestAtcBookingsUpdate() const override;
            virtual BlackMisc::Aviation::CAtcStationList getAtcStationsOnline() const override;
            virtual BlackMisc::Aviation::CAtcStation getOnlineStationForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const override;
            virtual BlackMisc::CStatusMessage connectToNetwork(const BlackMisc::Network::CServer &server, BlackCore::INetwork::LoginMode mode) override;
            virtual BlackMisc::Network::CServer getConnectedServer() const override;
            virtual BlackMisc::CStatusMessage disconnectFromNetwork() override;
            virtual bool isConnected() const override;
            virtual void sendTextMessages(const BlackMisc::Network::CTextMessageList &textMessages) override;
            virtual void sendFlightPlan(const BlackMisc::Aviation::CFlightPlan &flightPlan) override;
            virtual BlackMisc::Aviation::CFlightPlan loadFlightPlanFromNetwork(const BlackMisc::Aviation::CCallsign &callsign) const override;
            BlackMisc::Weather::CMetar getMetarForAirport(const BlackMisc::Aviation::CAirportIcaoCode &airportIcaoCode) const override;
            virtual BlackMisc::Audio::CVoiceRoomList getSelectedVoiceRooms() const override;
            virtual BlackMisc::Aviation::CAtcStationList getSelectedAtcStations() const override;
            virtual BlackMisc::Network::CUserList getUsers() const override;
            virtual BlackMisc::Network::CUserList getUsersForCallsigns(const BlackMisc::Aviation::CCallsignSet &callsigns) const override;
            virtual BlackMisc::Network::CUser getUserForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const override;
            virtual BlackMisc::Network::CClientList getOtherClients() const override;
            virtual BlackMisc::Network::CClientList getOtherClientsForCallsigns(const BlackMisc::Aviation::CCallsignSet &callsigns) const override;
            virtual BlackMisc::Network::CServerList getVatsimFsdServers() const override;
            virtual BlackMisc::Network::CServerList getVatsimVoiceServers() const override;
            virtual void requestDataUpdates()override;
            virtual void requestAtisUpdates() override;
            virtual void setFastPositionEnabledCallsigns(BlackMisc::Aviation::CCallsignSet &callsigns) override;
            virtual BlackMisc::Aviation::CCallsignSet getFastPositionEnabledCallsigns() const override;
            virtual void testCreateDummyOnlineAtcStations(int number) override;
            virtual void testAddAircraftParts(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftParts &parts, bool incremental) override;
            virtual void testReceivedTextMessages(const BlackMisc::Network::CTextMessageList &textMessages) override;
            virtual void testRequestAircraftConfig(const BlackMisc::Aviation::CCallsign &callsign) override;
            //! @}
            // --------------------- IContextNetwork implementations ---------------------

            //! Gracefully shut down, e.g. for thread safety
            void gracefulShutdown();

        protected:
            //! Constructor, with link to runtime
            CContextNetwork(CCoreFacadeConfig::ContextMode, CCoreFacade *runtime);

            //! Register myself in DBus
            CContextNetwork *registerWithDBus(BlackMisc::CDBusServer *server);

            //! Network library
            //! \remarks normally only for core facade internal usage
            INetwork *network() const { return m_network; }

            //! Airspace
            //! \remarks normally only for core facade internal usage
            CAirspaceMonitor *airspace() const { return m_airspace; }

        private:
            CAirspaceMonitor           *m_airspace = nullptr;
            INetwork                   *m_network  = nullptr;
            INetwork::ConnectionStatus  m_currentStatus = INetwork::Disconnected; //!< used to detect pending connections
            QTimer                     *m_networkDataUpdateTimer = nullptr;       //!< general updates such as ATIS, frequencies, see requestDataUpdates()

            // Digest signals, only sending after some time
            BlackMisc::CDigestSignal m_dsAtcStationsBookedChanged { this, &IContextNetwork::changedAtcStationsBooked, &IContextNetwork::changedAtcStationsBookedDigest, 750, 2 };
            BlackMisc::CDigestSignal m_dsAtcStationsOnlineChanged { this, &IContextNetwork::changedAtcStationsOnline, &IContextNetwork::changedAtcStationsOnlineDigest, 750, 4 };
            BlackMisc::CDigestSignal m_dsAircraftsInRangeChanged  { this, &IContextNetwork::changedAircraftInRange, &IContextNetwork::changedAircraftInRangeDigest, 750, 4 };

            //! Own aircraft from \sa CContextOwnAircraft
            const BlackMisc::Simulation::CSimulatedAircraft ownAircraft() const;

        private slots:
            //! Update METAR collection
            void ps_updateMetars(const BlackMisc::Weather::CMetarList &metars);

            //! Check if a supervisor message was received
            void ps_checkForSupervisiorTextMessage(const BlackMisc::Network::CTextMessageList &messages);

            //! Connection status changed
            void ps_fsdConnectionStatusChanged(INetwork::ConnectionStatus from, INetwork::ConnectionStatus to);

            //! Render restrictions have been changed, used with analyzer
            //! \sa CAirspaceAnalyzer
            void ps_simulatorRenderRestrictionsChanged(bool restricted, bool enabled, int maxAircraft, const BlackMisc::PhysicalQuantities::CLength &maxRenderedDistance);
        };
    } // ns
} // ns

#endif // guard
