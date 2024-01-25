// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_CONTEXT_CONTEXTNETWORK_PROXY_H
#define BLACKCORE_CONTEXT_CONTEXTNETWORK_PROXY_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/context/contextnetwork.h"
#include "blackcore/corefacadeconfig.h"
#include "blackmisc/aviation/airporticaocode.h"
#include "blackmisc/aviation/atcstation.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/callsignset.h"
#include "blackmisc/aviation/flightplan.h"
#include "blackmisc/identifier.h"
#include "blackmisc/network/clientlist.h"
#include "blackmisc/network/loginmode.h"
#include "blackmisc/network/server.h"
#include "blackmisc/network/serverlist.h"
#include "blackmisc/network/textmessagelist.h"
#include "blackmisc/network/user.h"
#include "blackmisc/network/userlist.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/weather/metar.h"

#include <stdbool.h>
#include <QObject>
#include <QString>

// clazy:excludeall=const-signal-or-slot

class QDBusConnection;

namespace BlackMisc
{
    class CGenericDBusInterface;
    namespace Aviation
    {
        class CAircraftParts;
        class CCallsign;
    }
    namespace Simulation
    {
        class CAircraftModel;
    }
}

namespace BlackCore
{
    class CCoreFacade;
    namespace Context
    {
        //! Network context proxy
        //! \ingroup dbus
        class BLACKCORE_EXPORT CContextNetworkProxy : public IContextNetwork
        {
            Q_OBJECT
            friend class IContextNetwork;

        public:
            //! Destructor
            virtual ~CContextNetworkProxy() override {}

            //! Unit test relay signals
            //! \private
            static void unitTestRelaySignals();

        public slots:
            //! \name Interface overrides
            //! @{

            //! \copydoc BlackCore::Context::IContextNetwork::getAtcStationsOnline
            virtual BlackMisc::Aviation::CAtcStationList getAtcStationsOnline(bool recalculateDistance) const override;

            //! \copydoc BlackCore::Context::IContextNetwork::getClosestAtcStationsOnline
            virtual BlackMisc::Aviation::CAtcStationList getClosestAtcStationsOnline(int number) const override;

            //! \copydoc BlackCore::Context::IContextNetwork::getAircraftInRange
            virtual BlackMisc::Simulation::CSimulatedAircraftList getAircraftInRange() const override;

            //! \copydoc BlackCore::Context::IContextNetwork::getAircraftInRangeCallsigns
            virtual BlackMisc::Aviation::CCallsignSet getAircraftInRangeCallsigns() const override;

            //! \copydoc BlackCore::Context::IContextNetwork::getAircraftInRangeCount
            virtual int getAircraftInRangeCount() const override;

            //! \copydoc BlackCore::Context::IContextNetwork::isAircraftInRange
            virtual bool isAircraftInRange(const BlackMisc::Aviation::CCallsign &callsign) const override;

            //! \copydoc BlackCore::Context::IContextNetwork::getAircraftInRangeForCallsign
            virtual BlackMisc::Simulation::CSimulatedAircraft getAircraftInRangeForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const override;

            //! \copydoc BlackCore::Context::IContextNetwork::getOnlineStationsForFrequency
            virtual BlackMisc::Aviation::CAtcStationList getOnlineStationsForFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency) const override;

            //! \copydoc BlackCore::Context::IContextNetwork::getOnlineStationForCallsign
            virtual BlackMisc::Aviation::CAtcStation getOnlineStationForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const override;

            //! \copydoc BlackCore::Context::IContextNetwork::isOnlineStation
            virtual bool isOnlineStation(const BlackMisc::Aviation::CCallsign &callsign) const override;

            //! \copydoc BlackCore::Context::IContextNetwork::connectToNetwork
            virtual BlackMisc::CStatusMessage connectToNetwork(const BlackMisc::Network::CServer &server, const QString &extraLiveryString, bool sendLivery, const QString &extraModelString, bool sendModelString, const BlackMisc::Aviation::CCallsign &partnerCallsign, BlackMisc::Network::CLoginMode mode) override;

            //! \copydoc BlackCore::Context::IContextNetwork::disconnectFromNetwork
            virtual BlackMisc::CStatusMessage disconnectFromNetwork() override;

            //! \copydoc BlackCore::Context::IContextNetwork::isConnected
            virtual bool isConnected() const override;

            //! \copydoc BlackCore::Context::IContextNetwork::getConnectedServer
            virtual BlackMisc::Network::CServer getConnectedServer() const override;

            //! \copydoc BlackCore::Context::IContextNetwork::getLoginMode
            virtual BlackMisc::Network::CLoginMode getLoginMode() const override;

            //! \copydoc BlackCore::Context::IContextNetwork::parseCommandLine
            virtual bool parseCommandLine(const QString &commandLine, const BlackMisc::CIdentifier &originator) override;

            //! \copydoc BlackCore::Context::IContextNetwork::sendTextMessages
            virtual void sendTextMessages(const BlackMisc::Network::CTextMessageList &textMessages) override;

            //! \copydoc BlackCore::Context::IContextNetwork::sendFlightPlan
            virtual void sendFlightPlan(const BlackMisc::Aviation::CFlightPlan &flightPlan) override;

            //! \copydoc BlackCore::Context::IContextNetwork::loadFlightPlanFromNetwork
            virtual BlackMisc::Aviation::CFlightPlan loadFlightPlanFromNetwork(const BlackMisc::Aviation::CCallsign &callsign) const override;

            //! \copydoc BlackCore::Context::IContextNetwork::getMetarForAirport
            BlackMisc::Weather::CMetar getMetarForAirport(const BlackMisc::Aviation::CAirportIcaoCode &airportIcaoCode) const override;

            //! \copydoc BlackCore::Context::IContextNetwork::getSelectedAtcStations
            virtual BlackMisc::Aviation::CAtcStationList getSelectedAtcStations() const override;

            //! \copydoc BlackCore::Context::IContextNetwork::getUsers
            virtual BlackMisc::Network::CUserList getUsers() const override;

            //! \copydoc BlackCore::Context::IContextNetwork::getUsersForCallsigns
            virtual BlackMisc::Network::CUserList getUsersForCallsigns(const BlackMisc::Aviation::CCallsignSet &callsigns) const override;

            //! \copydoc BlackCore::Context::IContextNetwork::getUserForCallsign
            virtual BlackMisc::Network::CUser getUserForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const override;

            //! \copydoc BlackCore::Context::IContextNetwork::getClients
            virtual BlackMisc::Network::CClientList getClients() const override;

            //! \copydoc BlackCore::Context::IContextNetwork::getVatsimVoiceServers
            virtual BlackMisc::Network::CServerList getVatsimVoiceServers() const override;

            //! \copydoc BlackCore::Context::IContextNetwork::getVatsimFsdServers
            virtual BlackMisc::Network::CServerList getVatsimFsdServers() const override;

            //! \copydoc BlackCore::Context::IContextNetwork::getClientsForCallsigns
            virtual BlackMisc::Network::CClientList getClientsForCallsigns(const BlackMisc::Aviation::CCallsignSet &callsigns) const override;

            //! \copydoc BlackCore::Context::IContextNetwork::setOtherClient
            virtual bool setOtherClient(const BlackMisc::Network::CClient &client) override;

            //! \copydoc BlackCore::Context::IContextNetwork::requestAircraftDataUpdates
            virtual void requestAircraftDataUpdates() override;

            //! \copydoc BlackCore::Context::IContextNetwork::requestAtisUpdates
            virtual void requestAtisUpdates() override;

            //! \copydoc BlackCore::Context::IContextNetwork::updateAircraftEnabled
            virtual bool updateAircraftEnabled(const BlackMisc::Aviation::CCallsign &callsign, bool enabledForRendering) override;

            //! \copydoc BlackCore::Context::IContextNetwork::setAircraftEnabledFlag
            virtual bool setAircraftEnabledFlag(const BlackMisc::Aviation::CCallsign &callsign, bool enabledForRendering) override;

            //! \copydoc BlackCore::Context::IContextNetwork::updateAircraftModel
            virtual bool updateAircraftModel(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Simulation::CAircraftModel &model, const BlackMisc::CIdentifier &originator) override;

            //! \copydoc BlackCore::Context::IContextNetwork::updateAircraftNetworkModel
            virtual bool updateAircraftNetworkModel(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Simulation::CAircraftModel &model, const BlackMisc::CIdentifier &originator) override;

            //! \copydoc BlackCore::Context::IContextNetwork::updateFastPositionEnabled
            virtual bool updateFastPositionEnabled(const BlackMisc::Aviation::CCallsign &callsign, bool enableFastPositionSending) override;

            //! \copydoc BlackCore::Context::IContextNetwork::updateAircraftSupportingGndFLag
            virtual bool updateAircraftSupportingGndFLag(const BlackMisc::Aviation::CCallsign &callsign, bool supportGndFlag) override;

            //! \copydoc BlackCore::Context::IContextNetwork::reInitializeAllAircraft
            virtual int reInitializeAllAircraft() override;

            //! \copydoc BlackCore::Context::IContextNetwork::setFastPositionEnabledCallsigns
            virtual void setFastPositionEnabledCallsigns(BlackMisc::Aviation::CCallsignSet &callsigns) override;

            //! \copydoc BlackCore::Context::IContextNetwork::getFastPositionEnabledCallsigns
            virtual BlackMisc::Aviation::CCallsignSet getFastPositionEnabledCallsigns() const override;

            //! \copydoc BlackCore::Context::IContextNetwork::getReverseLookupMessages
            virtual BlackMisc::CStatusMessageList getReverseLookupMessages(const BlackMisc::Aviation::CCallsign &callsign) const override;

            //! \copydoc BlackCore::Context::IContextNetwork::isReverseLookupMessagesEnabled
            virtual BlackMisc::Simulation::ReverseLookupLogging isReverseLookupMessagesEnabled() const override;

            //! \copydoc BlackCore::Context::IContextNetwork::enableReverseLookupMessages
            virtual void enableReverseLookupMessages(BlackMisc::Simulation::ReverseLookupLogging enable) override;

            //! \copydoc BlackCore::Context::IContextNetwork::getAircraftPartsHistory
            virtual BlackMisc::CStatusMessageList getAircraftPartsHistory(const BlackMisc::Aviation::CCallsign &callsign) const override;

            //! \copydoc BlackCore::Context::IContextNetwork::getRemoteAircraftParts
            virtual BlackMisc::Aviation::CAircraftPartsList getRemoteAircraftParts(const BlackMisc::Aviation::CCallsign &callsign) const override;

            //! \copydoc BlackCore::Context::IContextNetwork::getLibraryInfo
            virtual QString getLibraryInfo(bool detailed) const override;

            //! \copydoc BlackCore::Context::IContextNetwork::isAircraftPartsHistoryEnabled
            virtual bool isAircraftPartsHistoryEnabled() const override;

            //! \copydoc BlackCore::Context::IContextNetwork::isRemoteAircraftSupportingParts
            virtual bool isRemoteAircraftSupportingParts(const BlackMisc::Aviation::CCallsign &callsign) const override;

            //! \copydoc BlackCore::Context::IContextNetwork::enableAircraftPartsHistory
            virtual void enableAircraftPartsHistory(bool enabled) override;

            //! \copydoc BlackCore::Context::IContextNetwork::getNetworkStatistics
            virtual QString getNetworkStatistics(bool reset, const QString &separator) override;

            //! \copydoc BlackCore::Context::IContextNetwork::setNetworkStatisticsEnable
            virtual bool setNetworkStatisticsEnable(bool enabled) override;

            //! \copydoc BlackCore::Context::IContextNetwork::getNetworkPresetValues
            virtual QStringList getNetworkPresetValues() const override;

            //! \copydoc BlackCore::Context::IContextNetwork::getPartnerCallsign
            virtual BlackMisc::Aviation::CCallsign getPartnerCallsign() const override;

            //! \copydoc BlackCore::Context::IContextNetwork::testCreateDummyOnlineAtcStations
            virtual void testCreateDummyOnlineAtcStations(int number) override;

            //! \copydoc BlackCore::Context::IContextNetwork::testAddAircraftParts
            virtual void testAddAircraftParts(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftParts &parts, bool incremental) override;

            //! \copydoc BlackCore::Context::IContextNetwork::testReceivedTextMessages
            virtual void testReceivedTextMessages(const BlackMisc::Network::CTextMessageList &textMessages) override;

            //! \copydoc BlackCore::Context::IContextNetwork::testReceivedAtisMessage
            virtual void testReceivedAtisMessage(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CInformationMessage &msg) override;

            //! \copydoc BlackCore::Context::IContextNetwork::testRequestAircraftConfig
            virtual void testRequestAircraftConfig(const BlackMisc::Aviation::CCallsign &callsign) override;

            //! \copydoc BlackCore::Context::IContextNetwork::testAddAltitudeOffset
            virtual bool testAddAltitudeOffset(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CLength &offset = BlackMisc::PhysicalQuantities::CLength::null()) override;
            //! @}

        public:
            //! \copydoc IContextNetwork::connectRawFsdMessageSignal
            virtual QMetaObject::Connection connectRawFsdMessageSignal(QObject *receiver, RawFsdMessageReceivedSlot rawFsdMessageReceivedSlot) override;

        private:
            BlackMisc::CGenericDBusInterface *m_dBusInterface; /*!< DBus interface */

            //! Relay connection signals to local signals.
            void relaySignals(const QString &serviceName, QDBusConnection &connection);

        protected:
            //! Constructor
            CContextNetworkProxy(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime) : IContextNetwork(mode, runtime), m_dBusInterface(nullptr) {}

            //! DBus version constructor
            CContextNetworkProxy(const QString &serviceName, QDBusConnection &connection, CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime);
        };
    } // ns
} // ns
#endif // guard
