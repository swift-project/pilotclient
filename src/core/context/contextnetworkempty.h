// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_CONTEXT_CONTEXTNETWORK_EMPTY_H
#define SWIFT_CORE_CONTEXT_CONTEXTNETWORK_EMPTY_H

#include "core/context/contextnetwork.h"
#include "core/swiftcoreexport.h"
#include "misc/logmessage.h"

// clazy:excludeall=const-signal-or-slot

namespace swift::core::context
{
    //! Empty context, used during shutdown/initialization
    class SWIFT_CORE_EXPORT CContextNetworkEmpty : public IContextNetwork
    {
        Q_OBJECT

    public:
        //! Constructor
        CContextNetworkEmpty(CCoreFacade *runtime) : IContextNetwork(CCoreFacadeConfig::NotUsed, runtime) {}

    public slots:
        //! \copydoc IContextNetwork::getAtcStationsOnline()
        swift::misc::aviation::CAtcStationList getAtcStationsOnline(bool recalculateDistance) const override
        {
            Q_UNUSED(recalculateDistance)
            logEmptyContextWarning(Q_FUNC_INFO);
            return {};
        }

        //! \copydoc IContextNetwork::getClosestAtcStationsOnline()
        swift::misc::aviation::CAtcStationList getClosestAtcStationsOnline(int number) const override
        {
            Q_UNUSED(number)
            logEmptyContextWarning(Q_FUNC_INFO);
            return {};
        }

        //! \copydoc IContextNetwork::getAircraftInRange()
        swift::misc::simulation::CSimulatedAircraftList getAircraftInRange() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return {};
        }

        //! \copydoc IContextNetwork::getAircraftInRangeForCallsign
        swift::misc::simulation::CSimulatedAircraft
        getAircraftInRangeForCallsign(const swift::misc::aviation::CCallsign &callsign) const override
        {
            Q_UNUSED(callsign)
            logEmptyContextWarning(Q_FUNC_INFO);
            return {};
        }

        //! \copydoc IContextNetwork::getOnlineStationsForFrequency
        swift::misc::aviation::CAtcStationList
        getOnlineStationsForFrequency(const swift::misc::physical_quantities::CFrequency &frequency) const override
        {
            Q_UNUSED(frequency)
            logEmptyContextWarning(Q_FUNC_INFO);
            return {};
        }

        //! \copydoc IContextNetwork::getAircraftInRangeCallsigns()
        swift::misc::aviation::CCallsignSet getAircraftInRangeCallsigns() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return {};
        }

        //! \copydoc IContextNetwork::getAircraftInRangeCount
        int getAircraftInRangeCount() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return 0;
        }

        //! \copydoc IContextNetwork::isAircraftInRange
        bool isAircraftInRange(const swift::misc::aviation::CCallsign &callsign) const override
        {
            Q_UNUSED(callsign)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextNetwork::getOnlineStationForCallsign
        swift::misc::aviation::CAtcStation
        getOnlineStationForCallsign(const swift::misc::aviation::CCallsign &callsign) const override
        {
            Q_UNUSED(callsign)
            logEmptyContextWarning(Q_FUNC_INFO);
            return {};
        }

        //! \copydoc IContextNetwork::isOnlineStation
        bool isOnlineStation(const swift::misc::aviation::CCallsign &callsign) const override
        {
            Q_UNUSED(callsign)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextNetwork::connectToNetwork
        swift::misc::CStatusMessage connectToNetwork(const swift::misc::network::CServer &server,
                                                     const QString &extraLiveryString, bool sendLivery,
                                                     const QString &extraModelString, bool sendModelString,
                                                     const swift::misc::aviation::CCallsign &partnerCallsign,
                                                     swift::misc::network::CLoginMode mode) override
        {
            Q_UNUSED(mode)
            Q_UNUSED(server)
            Q_UNUSED(extraModelString)
            Q_UNUSED(extraLiveryString)
            Q_UNUSED(sendLivery)
            Q_UNUSED(sendModelString)
            Q_UNUSED(partnerCallsign)
            logEmptyContextWarning(Q_FUNC_INFO);
            return statusMessageEmptyContext();
        }

        //! \copydoc IContextNetwork::getNetworkPresetValues
        QStringList getNetworkPresetValues() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return {};
        }

        //! \copydoc IContextNetwork::getPartnerCallsign
        swift::misc::aviation::CCallsign getPartnerCallsign() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return {};
        }

        //! \copydoc IContextNetwork::disconnectFromNetwork()
        swift::misc::CStatusMessage disconnectFromNetwork() override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return statusMessageEmptyContext();
        }

        //! \copydoc IContextNetwork::isConnected()
        bool isConnected() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextNetwork::getConnectedServer
        swift::misc::network::CServer getConnectedServer() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return {};
        }

        //! \copydoc IContextNetwork::getLoginMode
        swift::misc::network::CLoginMode getLoginMode() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::network::CLoginMode::Pilot;
        }

        //! \copydoc IContextNetwork::sendTextMessages()
        void sendTextMessages(const swift::misc::network::CTextMessageList &textMessages) override
        {
            Q_UNUSED(textMessages)
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextNetwork::sendFlightPlan()
        void sendFlightPlan(const swift::misc::aviation::CFlightPlan &flightPlan) override
        {
            Q_UNUSED(flightPlan)
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextNetwork::loadFlightPlanFromNetwork()
        swift::misc::aviation::CFlightPlan
        loadFlightPlanFromNetwork(const swift::misc::aviation::CCallsign &callsign) const override
        {
            Q_UNUSED(callsign)
            logEmptyContextWarning(Q_FUNC_INFO);
            return {};
        }

        //! \copydoc IContextNetwork::getMetarForAirport
        swift::misc::weather::CMetar
        getMetarForAirport(const swift::misc::aviation::CAirportIcaoCode &airportIcaoCode) const override
        {
            Q_UNUSED(airportIcaoCode)
            logEmptyContextWarning(Q_FUNC_INFO);
            return {};
        }

        //! \copydoc IContextNetwork::getUsers()
        swift::misc::network::CUserList getUsers() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return {};
        }

        //! \copydoc IContextNetwork::getUsersForCallsigns
        swift::misc::network::CUserList
        getUsersForCallsigns(const swift::misc::aviation::CCallsignSet &callsigns) const override
        {
            Q_UNUSED(callsigns)
            logEmptyContextWarning(Q_FUNC_INFO);
            return {};
        }

        //! \copydoc IContextNetwork::getUserForCallsign
        swift::misc::network::CUser getUserForCallsign(const swift::misc::aviation::CCallsign &callsign) const override
        {
            Q_UNUSED(callsign)
            logEmptyContextWarning(Q_FUNC_INFO);
            return {};
        }

        //! \copydoc IContextNetwork::getClients
        swift::misc::network::CClientList getClients() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return {};
        }

        //! \copydoc IContextNetwork::getClientsForCallsigns
        swift::misc::network::CClientList
        getClientsForCallsigns(const swift::misc::aviation::CCallsignSet &callsigns) const override
        {
            Q_UNUSED(callsigns)
            logEmptyContextWarning(Q_FUNC_INFO);
            return {};
        }

        //! \copydoc IContextNetwork::setOtherClient
        bool setOtherClient(const swift::misc::network::CClient &client) override
        {
            Q_UNUSED(client)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextNetwork::requestAircraftDataUpdates
        void requestAircraftDataUpdates() override { logEmptyContextWarning(Q_FUNC_INFO); }

        //! \copydoc IContextNetwork::requestAtisUpdates
        void requestAtisUpdates() override { logEmptyContextWarning(Q_FUNC_INFO); }

        //! \copydoc IContextNetwork::testCreateDummyOnlineAtcStations
        void testCreateDummyOnlineAtcStations(int number) override
        {
            Q_UNUSED(number)
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextNetwork::testAddAircraftParts
        void testAddAircraftParts(const swift::misc::aviation::CCallsign &callsign,
                                  const swift::misc::aviation::CAircraftParts &parts, bool incremental) override
        {
            Q_UNUSED(callsign)
            Q_UNUSED(parts)
            Q_UNUSED(incremental)
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextNetwork::testReceivedTextMessages
        void testReceivedTextMessages(const swift::misc::network::CTextMessageList &textMessages) override
        {
            Q_UNUSED(textMessages)
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextNetwork::testRequestAircraftConfig
        void testRequestAircraftConfig(const swift::misc::aviation::CCallsign &callsign) override
        {
            Q_UNUSED(callsign)
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextNetwork::testAddAltitudeOffset
        bool testAddAltitudeOffset(const swift::misc::aviation::CCallsign &callsign,
                                   const swift::misc::physical_quantities::CLength &offset =
                                       swift::misc::physical_quantities::CLength::null()) override
        {
            Q_UNUSED(callsign)
            Q_UNUSED(offset)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextNetwork::testReceivedAtisMessage
        void testReceivedAtisMessage(const swift::misc::aviation::CCallsign &callsign,
                                     const swift::misc::aviation::CInformationMessage &msg) override
        {
            Q_UNUSED(callsign)
            Q_UNUSED(msg)
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextNetwork::parseCommandLine
        bool parseCommandLine(const QString &commandLine, const swift::misc::CIdentifier &originator) override
        {
            Q_UNUSED(commandLine)
            Q_UNUSED(originator)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextNetwork::getVatsimFsdServers
        swift::misc::network::CServerList getVatsimFsdServers() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return {};
        }

        //! \copydoc IContextNetwork::updateAircraftEnabled
        bool updateAircraftEnabled(const swift::misc::aviation::CCallsign &callsign, bool enabledForRendering) override
        {
            Q_UNUSED(callsign)
            Q_UNUSED(enabledForRendering)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextNetwork::updateAircraftSupportingGndFLag
        bool updateAircraftSupportingGndFLag(const swift::misc::aviation::CCallsign &callsign,
                                             bool supportGndFlag) override
        {
            Q_UNUSED(callsign)
            Q_UNUSED(supportGndFlag)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextNetwork::updateAircraftModel
        bool updateAircraftModel(const swift::misc::aviation::CCallsign &callsign,
                                 const swift::misc::simulation::CAircraftModel &model,
                                 const swift::misc::CIdentifier &originator) override
        {
            Q_UNUSED(callsign)
            Q_UNUSED(model)
            Q_UNUSED(originator)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextNetwork::updateAircraftNetworkModel
        bool updateAircraftNetworkModel(const swift::misc::aviation::CCallsign &callsign,
                                        const swift::misc::simulation::CAircraftModel &model,
                                        const swift::misc::CIdentifier &originator) override
        {
            Q_UNUSED(callsign)
            Q_UNUSED(model)
            Q_UNUSED(originator)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextNetwork::updateFastPositionEnabled
        bool updateFastPositionEnabled(const swift::misc::aviation::CCallsign &callsign,
                                       bool enableFastPositionSending) override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            Q_UNUSED(callsign)
            Q_UNUSED(enableFastPositionSending)
            return false;
        }

        //! \copydoc IContextNetwork::setAircraftEnabledFlag
        bool setAircraftEnabledFlag(const swift::misc::aviation::CCallsign &callsign, bool enabledForRendering) override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            Q_UNUSED(callsign)
            Q_UNUSED(enabledForRendering)
            return false;
        }

        //! \copydoc IContextNetwork::reInitializeAllAircraft
        int reInitializeAllAircraft() override { return 0; }

        //! \copydoc IContextNetwork::setFastPositionEnabledCallsigns
        void setFastPositionEnabledCallsigns(swift::misc::aviation::CCallsignSet &callsigns) override
        {
            Q_UNUSED(callsigns)
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextNetwork::getFastPositionEnabledCallsigns
        swift::misc::aviation::CCallsignSet getFastPositionEnabledCallsigns() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return {};
        }

        //! \copydoc IContextNetwork::getReverseLookupMessages
        swift::misc::CStatusMessageList
        getReverseLookupMessages(const swift::misc::aviation::CCallsign &callsign) const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            Q_UNUSED(callsign)
            return {};
        }

        //! \copydoc IContextNetwork::isReverseLookupMessagesEnabled
        swift::misc::simulation::ReverseLookupLogging isReverseLookupMessagesEnabled() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::simulation::RevLogDisabled;
        }

        //! \copydoc IContextNetwork::enableReverseLookupMessages
        void enableReverseLookupMessages(swift::misc::simulation::ReverseLookupLogging enable) override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            Q_UNUSED(enable)
        }

        //! \copydoc IContextNetwork::getAircraftPartsHistory
        swift::misc::CStatusMessageList
        getAircraftPartsHistory(const swift::misc::aviation::CCallsign &callsign) const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            Q_UNUSED(callsign)
            return {};
        }

        //! \copydoc IContextNetwork::getRemoteAircraftParts
        swift::misc::aviation::CAircraftPartsList
        getRemoteAircraftParts(const swift::misc::aviation::CCallsign &callsign) const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            Q_UNUSED(callsign)
            return {};
        }

        //! \copydoc IContextNetwork::isAircraftPartsHistoryEnabled
        bool isAircraftPartsHistoryEnabled() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextNetwork::isRemoteAircraftSupportingParts
        bool isRemoteAircraftSupportingParts(const swift::misc::aviation::CCallsign &callsign) const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            Q_UNUSED(callsign)
            return false;
        }

        //! \copydoc IContextNetwork::enableAircraftPartsHistory
        void enableAircraftPartsHistory(bool enabled) override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            Q_UNUSED(enabled)
        }

        //! \copydoc IContextNetwork::getNetworkStatistics
        QString getNetworkStatistics(bool reset, const QString &separator) override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            Q_UNUSED(reset)
            Q_UNUSED(separator)
            return {};
        }

        //! \copydoc IContextNetwork::setNetworkStatisticsEnable
        bool setNetworkStatisticsEnable(bool enabled) override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            Q_UNUSED(enabled;)
            return false;
        }

    public:
        //! \copydoc IContextNetwork::connectRawFsdMessageSignal
        QMetaObject::Connection connectRawFsdMessageSignal(QObject *receiver,
                                                           RawFsdMessageReceivedSlot rawFsdMessageReceivedSlot) override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            Q_UNUSED(receiver)
            Q_UNUSED(rawFsdMessageReceivedSlot)
            return {};
        }
    };
} // namespace swift::core::context
#endif // SWIFT_CORE_CONTEXT_CONTEXTNETWORK_EMPTY_H
