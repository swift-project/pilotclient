// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_CONTEXT_CONTEXTNETWORK_PROXY_H
#define SWIFT_CORE_CONTEXT_CONTEXTNETWORK_PROXY_H

#include <stdbool.h>

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
            virtual ~CContextNetworkProxy() override {}

            //! Unit test relay signals
            //! \private
            static void unitTestRelaySignals();

        public slots:
            //! \name Interface overrides
            //! @{

            //! \copydoc swift::core::context::IContextNetwork::getAtcStationsOnline
            virtual swift::misc::aviation::CAtcStationList
            getAtcStationsOnline(bool recalculateDistance) const override;

            //! \copydoc swift::core::context::IContextNetwork::getClosestAtcStationsOnline
            virtual swift::misc::aviation::CAtcStationList getClosestAtcStationsOnline(int number) const override;

            //! \copydoc swift::core::context::IContextNetwork::getAircraftInRange
            virtual swift::misc::simulation::CSimulatedAircraftList getAircraftInRange() const override;

            //! \copydoc swift::core::context::IContextNetwork::getAircraftInRangeCallsigns
            virtual swift::misc::aviation::CCallsignSet getAircraftInRangeCallsigns() const override;

            //! \copydoc swift::core::context::IContextNetwork::getAircraftInRangeCount
            virtual int getAircraftInRangeCount() const override;

            //! \copydoc swift::core::context::IContextNetwork::isAircraftInRange
            virtual bool isAircraftInRange(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc swift::core::context::IContextNetwork::getAircraftInRangeForCallsign
            virtual swift::misc::simulation::CSimulatedAircraft
            getAircraftInRangeForCallsign(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc swift::core::context::IContextNetwork::getOnlineStationsForFrequency
            virtual swift::misc::aviation::CAtcStationList
            getOnlineStationsForFrequency(const swift::misc::physical_quantities::CFrequency &frequency) const override;

            //! \copydoc swift::core::context::IContextNetwork::getOnlineStationForCallsign
            virtual swift::misc::aviation::CAtcStation
            getOnlineStationForCallsign(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc swift::core::context::IContextNetwork::isOnlineStation
            virtual bool isOnlineStation(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc swift::core::context::IContextNetwork::connectToNetwork
            virtual swift::misc::CStatusMessage
            connectToNetwork(const swift::misc::network::CServer &server, const QString &extraLiveryString,
                             bool sendLivery, const QString &extraModelString, bool sendModelString,
                             const swift::misc::aviation::CCallsign &partnerCallsign,
                             swift::misc::network::CLoginMode mode) override;

            //! \copydoc swift::core::context::IContextNetwork::disconnectFromNetwork
            virtual swift::misc::CStatusMessage disconnectFromNetwork() override;

            //! \copydoc swift::core::context::IContextNetwork::isConnected
            virtual bool isConnected() const override;

            //! \copydoc swift::core::context::IContextNetwork::getConnectedServer
            virtual swift::misc::network::CServer getConnectedServer() const override;

            //! \copydoc swift::core::context::IContextNetwork::getLoginMode
            virtual swift::misc::network::CLoginMode getLoginMode() const override;

            //! \copydoc swift::core::context::IContextNetwork::parseCommandLine
            virtual bool parseCommandLine(const QString &commandLine,
                                          const swift::misc::CIdentifier &originator) override;

            //! \copydoc swift::core::context::IContextNetwork::sendTextMessages
            virtual void sendTextMessages(const swift::misc::network::CTextMessageList &textMessages) override;

            //! \copydoc swift::core::context::IContextNetwork::sendFlightPlan
            virtual void sendFlightPlan(const swift::misc::aviation::CFlightPlan &flightPlan) override;

            //! \copydoc swift::core::context::IContextNetwork::loadFlightPlanFromNetwork
            virtual swift::misc::aviation::CFlightPlan
            loadFlightPlanFromNetwork(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc swift::core::context::IContextNetwork::getMetarForAirport
            swift::misc::weather::CMetar
            getMetarForAirport(const swift::misc::aviation::CAirportIcaoCode &airportIcaoCode) const override;

            //! \copydoc swift::core::context::IContextNetwork::getUsers
            virtual swift::misc::network::CUserList getUsers() const override;

            //! \copydoc swift::core::context::IContextNetwork::getUsersForCallsigns
            virtual swift::misc::network::CUserList
            getUsersForCallsigns(const swift::misc::aviation::CCallsignSet &callsigns) const override;

            //! \copydoc swift::core::context::IContextNetwork::getUserForCallsign
            virtual swift::misc::network::CUser
            getUserForCallsign(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc swift::core::context::IContextNetwork::getClients
            virtual swift::misc::network::CClientList getClients() const override;

            //! \copydoc swift::core::context::IContextNetwork::getVatsimFsdServers
            virtual swift::misc::network::CServerList getVatsimFsdServers() const override;

            //! \copydoc swift::core::context::IContextNetwork::getClientsForCallsigns
            virtual swift::misc::network::CClientList
            getClientsForCallsigns(const swift::misc::aviation::CCallsignSet &callsigns) const override;

            //! \copydoc swift::core::context::IContextNetwork::setOtherClient
            virtual bool setOtherClient(const swift::misc::network::CClient &client) override;

            //! \copydoc swift::core::context::IContextNetwork::requestAircraftDataUpdates
            virtual void requestAircraftDataUpdates() override;

            //! \copydoc swift::core::context::IContextNetwork::requestAtisUpdates
            virtual void requestAtisUpdates() override;

            //! \copydoc swift::core::context::IContextNetwork::updateAircraftEnabled
            virtual bool updateAircraftEnabled(const swift::misc::aviation::CCallsign &callsign,
                                               bool enabledForRendering) override;

            //! \copydoc swift::core::context::IContextNetwork::setAircraftEnabledFlag
            virtual bool setAircraftEnabledFlag(const swift::misc::aviation::CCallsign &callsign,
                                                bool enabledForRendering) override;

            //! \copydoc swift::core::context::IContextNetwork::updateAircraftModel
            virtual bool updateAircraftModel(const swift::misc::aviation::CCallsign &callsign,
                                             const swift::misc::simulation::CAircraftModel &model,
                                             const swift::misc::CIdentifier &originator) override;

            //! \copydoc swift::core::context::IContextNetwork::updateAircraftNetworkModel
            virtual bool updateAircraftNetworkModel(const swift::misc::aviation::CCallsign &callsign,
                                                    const swift::misc::simulation::CAircraftModel &model,
                                                    const swift::misc::CIdentifier &originator) override;

            //! \copydoc swift::core::context::IContextNetwork::updateFastPositionEnabled
            virtual bool updateFastPositionEnabled(const swift::misc::aviation::CCallsign &callsign,
                                                   bool enableFastPositionSending) override;

            //! \copydoc swift::core::context::IContextNetwork::updateAircraftSupportingGndFLag
            virtual bool updateAircraftSupportingGndFLag(const swift::misc::aviation::CCallsign &callsign,
                                                         bool supportGndFlag) override;

            //! \copydoc swift::core::context::IContextNetwork::reInitializeAllAircraft
            virtual int reInitializeAllAircraft() override;

            //! \copydoc swift::core::context::IContextNetwork::setFastPositionEnabledCallsigns
            virtual void setFastPositionEnabledCallsigns(swift::misc::aviation::CCallsignSet &callsigns) override;

            //! \copydoc swift::core::context::IContextNetwork::getFastPositionEnabledCallsigns
            virtual swift::misc::aviation::CCallsignSet getFastPositionEnabledCallsigns() const override;

            //! \copydoc swift::core::context::IContextNetwork::getReverseLookupMessages
            virtual swift::misc::CStatusMessageList
            getReverseLookupMessages(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc swift::core::context::IContextNetwork::isReverseLookupMessagesEnabled
            virtual swift::misc::simulation::ReverseLookupLogging isReverseLookupMessagesEnabled() const override;

            //! \copydoc swift::core::context::IContextNetwork::enableReverseLookupMessages
            virtual void enableReverseLookupMessages(swift::misc::simulation::ReverseLookupLogging enable) override;

            //! \copydoc swift::core::context::IContextNetwork::getAircraftPartsHistory
            virtual swift::misc::CStatusMessageList
            getAircraftPartsHistory(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc swift::core::context::IContextNetwork::getRemoteAircraftParts
            virtual swift::misc::aviation::CAircraftPartsList
            getRemoteAircraftParts(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc swift::core::context::IContextNetwork::getLibraryInfo
            virtual QString getLibraryInfo(bool detailed) const override;

            //! \copydoc swift::core::context::IContextNetwork::isAircraftPartsHistoryEnabled
            virtual bool isAircraftPartsHistoryEnabled() const override;

            //! \copydoc swift::core::context::IContextNetwork::isRemoteAircraftSupportingParts
            virtual bool
            isRemoteAircraftSupportingParts(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc swift::core::context::IContextNetwork::enableAircraftPartsHistory
            virtual void enableAircraftPartsHistory(bool enabled) override;

            //! \copydoc swift::core::context::IContextNetwork::getNetworkStatistics
            virtual QString getNetworkStatistics(bool reset, const QString &separator) override;

            //! \copydoc swift::core::context::IContextNetwork::setNetworkStatisticsEnable
            virtual bool setNetworkStatisticsEnable(bool enabled) override;

            //! \copydoc swift::core::context::IContextNetwork::getNetworkPresetValues
            virtual QStringList getNetworkPresetValues() const override;

            //! \copydoc swift::core::context::IContextNetwork::getPartnerCallsign
            virtual swift::misc::aviation::CCallsign getPartnerCallsign() const override;

            //! \copydoc swift::core::context::IContextNetwork::testCreateDummyOnlineAtcStations
            virtual void testCreateDummyOnlineAtcStations(int number) override;

            //! \copydoc swift::core::context::IContextNetwork::testAddAircraftParts
            virtual void testAddAircraftParts(const swift::misc::aviation::CCallsign &callsign,
                                              const swift::misc::aviation::CAircraftParts &parts,
                                              bool incremental) override;

            //! \copydoc swift::core::context::IContextNetwork::testReceivedTextMessages
            virtual void testReceivedTextMessages(const swift::misc::network::CTextMessageList &textMessages) override;

            //! \copydoc swift::core::context::IContextNetwork::testReceivedAtisMessage
            virtual void testReceivedAtisMessage(const swift::misc::aviation::CCallsign &callsign,
                                                 const swift::misc::aviation::CInformationMessage &msg) override;

            //! \copydoc swift::core::context::IContextNetwork::testRequestAircraftConfig
            virtual void testRequestAircraftConfig(const swift::misc::aviation::CCallsign &callsign) override;

            //! \copydoc swift::core::context::IContextNetwork::testAddAltitudeOffset
            virtual bool testAddAltitudeOffset(const swift::misc::aviation::CCallsign &callsign,
                                               const swift::misc::physical_quantities::CLength &offset =
                                                   swift::misc::physical_quantities::CLength::null()) override;
            //! @}

        public:
            //! \copydoc IContextNetwork::connectRawFsdMessageSignal
            virtual QMetaObject::Connection
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
#endif // guard
