// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_CONTEXT_CONTEXTNETWORK_PROXY_H
#define BLACKCORE_CONTEXT_CONTEXTNETWORK_PROXY_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/context/contextnetwork.h"
#include "blackcore/corefacadeconfig.h"
#include "misc/aviation/airporticaocode.h"
#include "misc/aviation/atcstation.h"
#include "misc/aviation/atcstationlist.h"
#include "misc/aviation/callsignset.h"
#include "misc/aviation/flightplan.h"
#include "misc/identifier.h"
#include "misc/network/clientlist.h"
#include "misc/network/loginmode.h"
#include "misc/network/server.h"
#include "misc/network/serverlist.h"
#include "misc/network/textmessagelist.h"
#include "misc/network/user.h"
#include "misc/network/userlist.h"
#include "misc/simulation/simulatedaircraft.h"
#include "misc/simulation/simulatedaircraftlist.h"
#include "misc/statusmessage.h"
#include "misc/weather/metar.h"

#include <stdbool.h>
#include <QObject>
#include <QString>

// clazy:excludeall=const-signal-or-slot

class QDBusConnection;

namespace swift::misc
{
    class CGenericDBusInterface;
    namespace aviation
    {
        class CAircraftParts;
        class CCallsign;
    }
    namespace simulation
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
            virtual swift::misc::aviation::CAtcStationList getAtcStationsOnline(bool recalculateDistance) const override;

            //! \copydoc BlackCore::Context::IContextNetwork::getClosestAtcStationsOnline
            virtual swift::misc::aviation::CAtcStationList getClosestAtcStationsOnline(int number) const override;

            //! \copydoc BlackCore::Context::IContextNetwork::getAircraftInRange
            virtual swift::misc::simulation::CSimulatedAircraftList getAircraftInRange() const override;

            //! \copydoc BlackCore::Context::IContextNetwork::getAircraftInRangeCallsigns
            virtual swift::misc::aviation::CCallsignSet getAircraftInRangeCallsigns() const override;

            //! \copydoc BlackCore::Context::IContextNetwork::getAircraftInRangeCount
            virtual int getAircraftInRangeCount() const override;

            //! \copydoc BlackCore::Context::IContextNetwork::isAircraftInRange
            virtual bool isAircraftInRange(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc BlackCore::Context::IContextNetwork::getAircraftInRangeForCallsign
            virtual swift::misc::simulation::CSimulatedAircraft getAircraftInRangeForCallsign(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc BlackCore::Context::IContextNetwork::getOnlineStationsForFrequency
            virtual swift::misc::aviation::CAtcStationList getOnlineStationsForFrequency(const swift::misc::physical_quantities::CFrequency &frequency) const override;

            //! \copydoc BlackCore::Context::IContextNetwork::getOnlineStationForCallsign
            virtual swift::misc::aviation::CAtcStation getOnlineStationForCallsign(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc BlackCore::Context::IContextNetwork::isOnlineStation
            virtual bool isOnlineStation(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc BlackCore::Context::IContextNetwork::connectToNetwork
            virtual swift::misc::CStatusMessage connectToNetwork(const swift::misc::network::CServer &server, const QString &extraLiveryString, bool sendLivery, const QString &extraModelString, bool sendModelString, const swift::misc::aviation::CCallsign &partnerCallsign, swift::misc::network::CLoginMode mode) override;

            //! \copydoc BlackCore::Context::IContextNetwork::disconnectFromNetwork
            virtual swift::misc::CStatusMessage disconnectFromNetwork() override;

            //! \copydoc BlackCore::Context::IContextNetwork::isConnected
            virtual bool isConnected() const override;

            //! \copydoc BlackCore::Context::IContextNetwork::getConnectedServer
            virtual swift::misc::network::CServer getConnectedServer() const override;

            //! \copydoc BlackCore::Context::IContextNetwork::getLoginMode
            virtual swift::misc::network::CLoginMode getLoginMode() const override;

            //! \copydoc BlackCore::Context::IContextNetwork::parseCommandLine
            virtual bool parseCommandLine(const QString &commandLine, const swift::misc::CIdentifier &originator) override;

            //! \copydoc BlackCore::Context::IContextNetwork::sendTextMessages
            virtual void sendTextMessages(const swift::misc::network::CTextMessageList &textMessages) override;

            //! \copydoc BlackCore::Context::IContextNetwork::sendFlightPlan
            virtual void sendFlightPlan(const swift::misc::aviation::CFlightPlan &flightPlan) override;

            //! \copydoc BlackCore::Context::IContextNetwork::loadFlightPlanFromNetwork
            virtual swift::misc::aviation::CFlightPlan loadFlightPlanFromNetwork(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc BlackCore::Context::IContextNetwork::getMetarForAirport
            swift::misc::weather::CMetar getMetarForAirport(const swift::misc::aviation::CAirportIcaoCode &airportIcaoCode) const override;

            //! \copydoc BlackCore::Context::IContextNetwork::getUsers
            virtual swift::misc::network::CUserList getUsers() const override;

            //! \copydoc BlackCore::Context::IContextNetwork::getUsersForCallsigns
            virtual swift::misc::network::CUserList getUsersForCallsigns(const swift::misc::aviation::CCallsignSet &callsigns) const override;

            //! \copydoc BlackCore::Context::IContextNetwork::getUserForCallsign
            virtual swift::misc::network::CUser getUserForCallsign(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc BlackCore::Context::IContextNetwork::getClients
            virtual swift::misc::network::CClientList getClients() const override;

            //! \copydoc BlackCore::Context::IContextNetwork::getVatsimFsdServers
            virtual swift::misc::network::CServerList getVatsimFsdServers() const override;

            //! \copydoc BlackCore::Context::IContextNetwork::getClientsForCallsigns
            virtual swift::misc::network::CClientList getClientsForCallsigns(const swift::misc::aviation::CCallsignSet &callsigns) const override;

            //! \copydoc BlackCore::Context::IContextNetwork::setOtherClient
            virtual bool setOtherClient(const swift::misc::network::CClient &client) override;

            //! \copydoc BlackCore::Context::IContextNetwork::requestAircraftDataUpdates
            virtual void requestAircraftDataUpdates() override;

            //! \copydoc BlackCore::Context::IContextNetwork::requestAtisUpdates
            virtual void requestAtisUpdates() override;

            //! \copydoc BlackCore::Context::IContextNetwork::updateAircraftEnabled
            virtual bool updateAircraftEnabled(const swift::misc::aviation::CCallsign &callsign, bool enabledForRendering) override;

            //! \copydoc BlackCore::Context::IContextNetwork::setAircraftEnabledFlag
            virtual bool setAircraftEnabledFlag(const swift::misc::aviation::CCallsign &callsign, bool enabledForRendering) override;

            //! \copydoc BlackCore::Context::IContextNetwork::updateAircraftModel
            virtual bool updateAircraftModel(const swift::misc::aviation::CCallsign &callsign, const swift::misc::simulation::CAircraftModel &model, const swift::misc::CIdentifier &originator) override;

            //! \copydoc BlackCore::Context::IContextNetwork::updateAircraftNetworkModel
            virtual bool updateAircraftNetworkModel(const swift::misc::aviation::CCallsign &callsign, const swift::misc::simulation::CAircraftModel &model, const swift::misc::CIdentifier &originator) override;

            //! \copydoc BlackCore::Context::IContextNetwork::updateFastPositionEnabled
            virtual bool updateFastPositionEnabled(const swift::misc::aviation::CCallsign &callsign, bool enableFastPositionSending) override;

            //! \copydoc BlackCore::Context::IContextNetwork::updateAircraftSupportingGndFLag
            virtual bool updateAircraftSupportingGndFLag(const swift::misc::aviation::CCallsign &callsign, bool supportGndFlag) override;

            //! \copydoc BlackCore::Context::IContextNetwork::reInitializeAllAircraft
            virtual int reInitializeAllAircraft() override;

            //! \copydoc BlackCore::Context::IContextNetwork::setFastPositionEnabledCallsigns
            virtual void setFastPositionEnabledCallsigns(swift::misc::aviation::CCallsignSet &callsigns) override;

            //! \copydoc BlackCore::Context::IContextNetwork::getFastPositionEnabledCallsigns
            virtual swift::misc::aviation::CCallsignSet getFastPositionEnabledCallsigns() const override;

            //! \copydoc BlackCore::Context::IContextNetwork::getReverseLookupMessages
            virtual swift::misc::CStatusMessageList getReverseLookupMessages(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc BlackCore::Context::IContextNetwork::isReverseLookupMessagesEnabled
            virtual swift::misc::simulation::ReverseLookupLogging isReverseLookupMessagesEnabled() const override;

            //! \copydoc BlackCore::Context::IContextNetwork::enableReverseLookupMessages
            virtual void enableReverseLookupMessages(swift::misc::simulation::ReverseLookupLogging enable) override;

            //! \copydoc BlackCore::Context::IContextNetwork::getAircraftPartsHistory
            virtual swift::misc::CStatusMessageList getAircraftPartsHistory(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc BlackCore::Context::IContextNetwork::getRemoteAircraftParts
            virtual swift::misc::aviation::CAircraftPartsList getRemoteAircraftParts(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc BlackCore::Context::IContextNetwork::getLibraryInfo
            virtual QString getLibraryInfo(bool detailed) const override;

            //! \copydoc BlackCore::Context::IContextNetwork::isAircraftPartsHistoryEnabled
            virtual bool isAircraftPartsHistoryEnabled() const override;

            //! \copydoc BlackCore::Context::IContextNetwork::isRemoteAircraftSupportingParts
            virtual bool isRemoteAircraftSupportingParts(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc BlackCore::Context::IContextNetwork::enableAircraftPartsHistory
            virtual void enableAircraftPartsHistory(bool enabled) override;

            //! \copydoc BlackCore::Context::IContextNetwork::getNetworkStatistics
            virtual QString getNetworkStatistics(bool reset, const QString &separator) override;

            //! \copydoc BlackCore::Context::IContextNetwork::setNetworkStatisticsEnable
            virtual bool setNetworkStatisticsEnable(bool enabled) override;

            //! \copydoc BlackCore::Context::IContextNetwork::getNetworkPresetValues
            virtual QStringList getNetworkPresetValues() const override;

            //! \copydoc BlackCore::Context::IContextNetwork::getPartnerCallsign
            virtual swift::misc::aviation::CCallsign getPartnerCallsign() const override;

            //! \copydoc BlackCore::Context::IContextNetwork::testCreateDummyOnlineAtcStations
            virtual void testCreateDummyOnlineAtcStations(int number) override;

            //! \copydoc BlackCore::Context::IContextNetwork::testAddAircraftParts
            virtual void testAddAircraftParts(const swift::misc::aviation::CCallsign &callsign, const swift::misc::aviation::CAircraftParts &parts, bool incremental) override;

            //! \copydoc BlackCore::Context::IContextNetwork::testReceivedTextMessages
            virtual void testReceivedTextMessages(const swift::misc::network::CTextMessageList &textMessages) override;

            //! \copydoc BlackCore::Context::IContextNetwork::testReceivedAtisMessage
            virtual void testReceivedAtisMessage(const swift::misc::aviation::CCallsign &callsign, const swift::misc::aviation::CInformationMessage &msg) override;

            //! \copydoc BlackCore::Context::IContextNetwork::testRequestAircraftConfig
            virtual void testRequestAircraftConfig(const swift::misc::aviation::CCallsign &callsign) override;

            //! \copydoc BlackCore::Context::IContextNetwork::testAddAltitudeOffset
            virtual bool testAddAltitudeOffset(const swift::misc::aviation::CCallsign &callsign, const swift::misc::physical_quantities::CLength &offset = swift::misc::physical_quantities::CLength::null()) override;
            //! @}

        public:
            //! \copydoc IContextNetwork::connectRawFsdMessageSignal
            virtual QMetaObject::Connection connectRawFsdMessageSignal(QObject *receiver, RawFsdMessageReceivedSlot rawFsdMessageReceivedSlot) override;

        private:
            swift::misc::CGenericDBusInterface *m_dBusInterface; /*!< DBus interface */

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
