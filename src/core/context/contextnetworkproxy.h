// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_CONTEXT_CONTEXTNETWORK_PROXY_H
#define SWIFT_CORE_CONTEXT_CONTEXTNETWORK_PROXY_H

#include <QObject>
#include <QString>

#include "core/context/contextnetwork.h"
#include "core/corefacadeconfig.h"
#include "core/swiftcoreexport.h"
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

// clazy:excludeall=const-signal-or-slot

class QDBusConnection;

namespace swift::misc
{
    class CGenericDBusInterface;
    namespace aviation
    {
        class CAircraftParts;
        class CCallsign;
    } // namespace aviation
    namespace simulation
    {
        class CAircraftModel;
    }
} // namespace swift::misc

namespace swift::core
{
    class CCoreFacade;
    namespace context
    {
        //! Network context proxy
        //! \ingroup dbus
        class SWIFT_CORE_EXPORT CContextNetworkProxy : public IContextNetwork
        {
            Q_OBJECT
            friend class IContextNetwork;

        public:
            //! Destructor
            ~CContextNetworkProxy() override = default;

            //! Unit test relay signals
            //! \private
            static void unitTestRelaySignals();

        public slots:
            //! \name Interface overrides
            //! @{

            //! \copydoc swift::core::context::IContextNetwork::getAtcStationsOnline
            swift::misc::aviation::CAtcStationList getAtcStationsOnline(bool recalculateDistance) const override;

            //! \copydoc swift::core::context::IContextNetwork::getClosestAtcStationsOnline
            swift::misc::aviation::CAtcStationList getClosestAtcStationsOnline(int number) const override;

            //! \copydoc swift::core::context::IContextNetwork::getAircraftInRange
            swift::misc::simulation::CSimulatedAircraftList getAircraftInRange() const override;

            //! \copydoc swift::core::context::IContextNetwork::getAircraftInRangeCallsigns
            swift::misc::aviation::CCallsignSet getAircraftInRangeCallsigns() const override;

            //! \copydoc swift::core::context::IContextNetwork::getAircraftInRangeCount
            int getAircraftInRangeCount() const override;

            //! \copydoc swift::core::context::IContextNetwork::isAircraftInRange
            bool isAircraftInRange(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc swift::core::context::IContextNetwork::getAircraftInRangeForCallsign
            swift::misc::simulation::CSimulatedAircraft
            getAircraftInRangeForCallsign(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc swift::core::context::IContextNetwork::getOnlineStationsForFrequency
            swift::misc::aviation::CAtcStationList
            getOnlineStationsForFrequency(const swift::misc::physical_quantities::CFrequency &frequency) const override;

            //! \copydoc swift::core::context::IContextNetwork::getOnlineStationForCallsign
            swift::misc::aviation::CAtcStation
            getOnlineStationForCallsign(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc swift::core::context::IContextNetwork::isOnlineStation
            bool isOnlineStation(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc swift::core::context::IContextNetwork::connectToNetwork
            swift::misc::CStatusMessage connectToNetwork(const swift::misc::network::CServer &server,
                                                         const QString &extraLiveryString, bool sendLivery,
                                                         const QString &extraModelString, bool sendModelString,
                                                         const swift::misc::aviation::CCallsign &partnerCallsign,
                                                         swift::misc::network::CLoginMode mode) override;

            //! \copydoc swift::core::context::IContextNetwork::disconnectFromNetwork
            swift::misc::CStatusMessage disconnectFromNetwork() override;

            //! \copydoc swift::core::context::IContextNetwork::isConnected
            bool isConnected() const override;

            //! \copydoc swift::core::context::IContextNetwork::getConnectedServer
            swift::misc::network::CServer getConnectedServer() const override;

            //! \copydoc swift::core::context::IContextNetwork::getLoginMode
            swift::misc::network::CLoginMode getLoginMode() const override;

            //! \copydoc swift::core::context::IContextNetwork::parseCommandLine
            bool parseCommandLine(const QString &commandLine, const swift::misc::CIdentifier &originator) override;

            //! \copydoc swift::core::context::IContextNetwork::sendTextMessages
            void sendTextMessages(const swift::misc::network::CTextMessageList &textMessages) override;

            //! \copydoc swift::core::context::IContextNetwork::sendFlightPlan
            void sendFlightPlan(const swift::misc::aviation::CFlightPlan &flightPlan) override;

            //! \copydoc swift::core::context::IContextNetwork::loadFlightPlanFromNetwork
            swift::misc::aviation::CFlightPlan
            loadFlightPlanFromNetwork(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc swift::core::context::IContextNetwork::getMetarForAirport
            swift::misc::weather::CMetar
            getMetarForAirport(const swift::misc::aviation::CAirportIcaoCode &airportIcaoCode) const override;

            //! \copydoc swift::core::context::IContextNetwork::getUsers
            swift::misc::network::CUserList getUsers() const override;

            //! \copydoc swift::core::context::IContextNetwork::getUsersForCallsigns
            swift::misc::network::CUserList
            getUsersForCallsigns(const swift::misc::aviation::CCallsignSet &callsigns) const override;

            //! \copydoc swift::core::context::IContextNetwork::getUserForCallsign
            swift::misc::network::CUser
            getUserForCallsign(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc swift::core::context::IContextNetwork::getClients
            swift::misc::network::CClientList getClients() const override;

            //! \copydoc swift::core::context::IContextNetwork::getVatsimFsdServers
            swift::misc::network::CServerList getVatsimFsdServers() const override;

            //! \copydoc swift::core::context::IContextNetwork::getClientsForCallsigns
            swift::misc::network::CClientList
            getClientsForCallsigns(const swift::misc::aviation::CCallsignSet &callsigns) const override;

            //! \copydoc swift::core::context::IContextNetwork::setOtherClient
            bool setOtherClient(const swift::misc::network::CClient &client) override;

            //! \copydoc swift::core::context::IContextNetwork::requestAircraftDataUpdates
            void requestAircraftDataUpdates() override;

            //! \copydoc swift::core::context::IContextNetwork::requestAtisUpdates
            void requestAtisUpdates() override;

            //! \copydoc swift::core::context::IContextNetwork::updateAircraftEnabled
            bool updateAircraftEnabled(const swift::misc::aviation::CCallsign &callsign,
                                       bool enabledForRendering) override;

            //! \copydoc swift::core::context::IContextNetwork::setAircraftEnabledFlag
            bool setAircraftEnabledFlag(const swift::misc::aviation::CCallsign &callsign,
                                        bool enabledForRendering) override;

            //! \copydoc swift::core::context::IContextNetwork::updateAircraftModel
            bool updateAircraftModel(const swift::misc::aviation::CCallsign &callsign,
                                     const swift::misc::simulation::CAircraftModel &model,
                                     const swift::misc::CIdentifier &originator) override;

            //! \copydoc swift::core::context::IContextNetwork::updateAircraftNetworkModel
            bool updateAircraftNetworkModel(const swift::misc::aviation::CCallsign &callsign,
                                            const swift::misc::simulation::CAircraftModel &model,
                                            const swift::misc::CIdentifier &originator) override;

            //! \copydoc swift::core::context::IContextNetwork::updateFastPositionEnabled
            bool updateFastPositionEnabled(const swift::misc::aviation::CCallsign &callsign,
                                           bool enableFastPositionSending) override;

            //! \copydoc swift::core::context::IContextNetwork::updateAircraftSupportingGndFLag
            bool updateAircraftSupportingGndFLag(const swift::misc::aviation::CCallsign &callsign,
                                                 bool supportGndFlag) override;

            //! \copydoc swift::core::context::IContextNetwork::reInitializeAllAircraft
            int reInitializeAllAircraft() override;

            //! \copydoc swift::core::context::IContextNetwork::setFastPositionEnabledCallsigns
            void setFastPositionEnabledCallsigns(swift::misc::aviation::CCallsignSet &callsigns) override;

            //! \copydoc swift::core::context::IContextNetwork::getFastPositionEnabledCallsigns
            swift::misc::aviation::CCallsignSet getFastPositionEnabledCallsigns() const override;

            //! \copydoc swift::core::context::IContextNetwork::getReverseLookupMessages
            swift::misc::CStatusMessageList
            getReverseLookupMessages(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc swift::core::context::IContextNetwork::isReverseLookupMessagesEnabled
            swift::misc::simulation::ReverseLookupLogging isReverseLookupMessagesEnabled() const override;

            //! \copydoc swift::core::context::IContextNetwork::enableReverseLookupMessages
            void enableReverseLookupMessages(swift::misc::simulation::ReverseLookupLogging enable) override;

            //! \copydoc swift::core::context::IContextNetwork::getAircraftPartsHistory
            swift::misc::CStatusMessageList
            getAircraftPartsHistory(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc swift::core::context::IContextNetwork::getRemoteAircraftParts
            swift::misc::aviation::CAircraftPartsList
            getRemoteAircraftParts(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc swift::core::context::IContextNetwork::isAircraftPartsHistoryEnabled
            bool isAircraftPartsHistoryEnabled() const override;

            //! \copydoc swift::core::context::IContextNetwork::isRemoteAircraftSupportingParts
            bool isRemoteAircraftSupportingParts(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc swift::core::context::IContextNetwork::enableAircraftPartsHistory
            void enableAircraftPartsHistory(bool enabled) override;

            //! \copydoc swift::core::context::IContextNetwork::getNetworkStatistics
            QString getNetworkStatistics(bool reset, const QString &separator) override;

            //! \copydoc swift::core::context::IContextNetwork::setNetworkStatisticsEnable
            bool setNetworkStatisticsEnable(bool enabled) override;

            //! \copydoc swift::core::context::IContextNetwork::getNetworkPresetValues
            QStringList getNetworkPresetValues() const override;

            //! \copydoc swift::core::context::IContextNetwork::getPartnerCallsign
            swift::misc::aviation::CCallsign getPartnerCallsign() const override;

            //! \copydoc swift::core::context::IContextNetwork::testCreateDummyOnlineAtcStations
            void testCreateDummyOnlineAtcStations(int number) override;

            //! \copydoc swift::core::context::IContextNetwork::testAddAircraftParts
            void testAddAircraftParts(const swift::misc::aviation::CCallsign &callsign,
                                      const swift::misc::aviation::CAircraftParts &parts, bool incremental) override;

            //! \copydoc swift::core::context::IContextNetwork::testReceivedTextMessages
            void testReceivedTextMessages(const swift::misc::network::CTextMessageList &textMessages) override;

            //! \copydoc swift::core::context::IContextNetwork::testReceivedAtisMessage
            void testReceivedAtisMessage(const swift::misc::aviation::CCallsign &callsign,
                                         const swift::misc::aviation::CInformationMessage &msg) override;

            //! \copydoc swift::core::context::IContextNetwork::testRequestAircraftConfig
            void testRequestAircraftConfig(const swift::misc::aviation::CCallsign &callsign) override;

            //! \copydoc swift::core::context::IContextNetwork::testAddAltitudeOffset
            bool testAddAltitudeOffset(const swift::misc::aviation::CCallsign &callsign,
                                       const swift::misc::physical_quantities::CLength &offset =
                                           swift::misc::physical_quantities::CLength::null()) override;
            //! @}

        public:
            //! \copydoc IContextNetwork::connectRawFsdMessageSignal
            QMetaObject::Connection
            connectRawFsdMessageSignal(QObject *receiver, RawFsdMessageReceivedSlot rawFsdMessageReceivedSlot) override;

        private:
            swift::misc::CGenericDBusInterface *m_dBusInterface; /*!< DBus interface */

            //! Relay connection signals to local signals.
            void relaySignals(const QString &serviceName, QDBusConnection &connection);

        protected:
            //! Constructor
            CContextNetworkProxy(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime)
                : IContextNetwork(mode, runtime), m_dBusInterface(nullptr)
            {}

            //! DBus version constructor
            CContextNetworkProxy(const QString &serviceName, QDBusConnection &connection,
                                 CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime);
        };
    } // namespace context
} // namespace swift::core
#endif // SWIFT_CORE_CONTEXT_CONTEXTNETWORK_PROXY_H
