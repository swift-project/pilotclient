// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_CONTEXT_CONTEXTNETWORK_EMPTY_H
#define SWIFT_CORE_CONTEXT_CONTEXTNETWORK_EMPTY_H

#include "core/swiftcoreexport.h"
#include "core/context/contextnetwork.h"
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
        virtual swift::misc::aviation::CAtcStationList getAtcStationsOnline(bool recalculateDistance) const override
        {
            Q_UNUSED(recalculateDistance)
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::aviation::CAtcStationList();
        }

        //! \copydoc IContextNetwork::getClosestAtcStationsOnline()
        virtual swift::misc::aviation::CAtcStationList getClosestAtcStationsOnline(int number) const override
        {
            Q_UNUSED(number)
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::aviation::CAtcStationList();
        }

        //! \copydoc IContextNetwork::getAircraftInRange()
        virtual swift::misc::simulation::CSimulatedAircraftList getAircraftInRange() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::simulation::CSimulatedAircraftList();
        }

        //! \copydoc IContextNetwork::getAircraftInRangeForCallsign
        virtual swift::misc::simulation::CSimulatedAircraft getAircraftInRangeForCallsign(const swift::misc::aviation::CCallsign &callsign) const override
        {
            Q_UNUSED(callsign)
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::simulation::CSimulatedAircraft();
        }

        //! \copydoc IContextNetwork::getOnlineStationsForFrequency
        virtual swift::misc::aviation::CAtcStationList getOnlineStationsForFrequency(const swift::misc::physical_quantities::CFrequency &frequency) const override
        {
            Q_UNUSED(frequency)
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::aviation::CAtcStationList();
        }

        //! \copydoc IContextNetwork::getAircraftInRangeCallsigns()
        virtual swift::misc::aviation::CCallsignSet getAircraftInRangeCallsigns() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::aviation::CCallsignSet();
        }

        //! \copydoc IContextNetwork::getAircraftInRangeCount
        virtual int getAircraftInRangeCount() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return 0;
        }

        //! \copydoc IContextNetwork::isAircraftInRange
        virtual bool isAircraftInRange(const swift::misc::aviation::CCallsign &callsign) const override
        {
            Q_UNUSED(callsign)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextNetwork::getOnlineStationForCallsign
        virtual swift::misc::aviation::CAtcStation getOnlineStationForCallsign(const swift::misc::aviation::CCallsign &callsign) const override
        {
            Q_UNUSED(callsign)
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::aviation::CAtcStation();
        }

        //! \copydoc IContextNetwork::isOnlineStation
        virtual bool isOnlineStation(const swift::misc::aviation::CCallsign &callsign) const override
        {
            Q_UNUSED(callsign)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextNetwork::connectToNetwork
        virtual swift::misc::CStatusMessage connectToNetwork(const swift::misc::network::CServer &server, const QString &extraLiveryString, bool sendLivery, const QString &extraModelString, bool sendModelString, const swift::misc::aviation::CCallsign &partnerCallsign, swift::misc::network::CLoginMode mode) override
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
        virtual QStringList getNetworkPresetValues() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return {};
        }

        //! \copydoc IContextNetwork::getPartnerCallsign
        virtual swift::misc::aviation::CCallsign getPartnerCallsign() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return {};
        }

        //! \copydoc IContextNetwork::disconnectFromNetwork()
        virtual swift::misc::CStatusMessage disconnectFromNetwork() override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return statusMessageEmptyContext();
        }

        //! \copydoc IContextNetwork::isConnected()
        virtual bool isConnected() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextNetwork::getConnectedServer
        virtual swift::misc::network::CServer getConnectedServer() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::network::CServer();
        }

        //! \copydoc IContextNetwork::getLoginMode
        virtual swift::misc::network::CLoginMode getLoginMode() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::network::CLoginMode::Pilot;
        }

        //! \copydoc IContextNetwork::sendTextMessages()
        virtual void sendTextMessages(const swift::misc::network::CTextMessageList &textMessages) override
        {
            Q_UNUSED(textMessages)
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextNetwork::sendFlightPlan()
        virtual void sendFlightPlan(const swift::misc::aviation::CFlightPlan &flightPlan) override
        {
            Q_UNUSED(flightPlan)
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextNetwork::loadFlightPlanFromNetwork()
        virtual swift::misc::aviation::CFlightPlan loadFlightPlanFromNetwork(const swift::misc::aviation::CCallsign &callsign) const override
        {
            Q_UNUSED(callsign)
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::aviation::CFlightPlan();
        }

        //! \copydoc IContextNetwork::getMetarForAirport
        swift::misc::weather::CMetar getMetarForAirport(const swift::misc::aviation::CAirportIcaoCode &airportIcaoCode) const override
        {
            Q_UNUSED(airportIcaoCode)
            logEmptyContextWarning(Q_FUNC_INFO);
            return {};
        }

        //! \copydoc IContextNetwork::getUsers()
        virtual swift::misc::network::CUserList getUsers() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::network::CUserList();
        }

        //! \copydoc IContextNetwork::getUsersForCallsigns
        virtual swift::misc::network::CUserList getUsersForCallsigns(const swift::misc::aviation::CCallsignSet &callsigns) const override
        {
            Q_UNUSED(callsigns)
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::network::CUserList();
        }

        //! \copydoc IContextNetwork::getUserForCallsign
        virtual swift::misc::network::CUser getUserForCallsign(const swift::misc::aviation::CCallsign &callsign) const override
        {
            Q_UNUSED(callsign)
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::network::CUser();
        }

        //! \copydoc IContextNetwork::getClients
        virtual swift::misc::network::CClientList getClients() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::network::CClientList();
        }

        //! \copydoc IContextNetwork::getClientsForCallsigns
        virtual swift::misc::network::CClientList getClientsForCallsigns(const swift::misc::aviation::CCallsignSet &callsigns) const override
        {
            Q_UNUSED(callsigns)
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::network::CClientList();
        }

        //! \copydoc IContextNetwork::setOtherClient
        virtual bool setOtherClient(const swift::misc::network::CClient &client) override
        {
            Q_UNUSED(client)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextNetwork::requestAircraftDataUpdates
        virtual void requestAircraftDataUpdates() override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextNetwork::requestAtisUpdates
        virtual void requestAtisUpdates() override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextNetwork::getLibraryInfo
        virtual QString getLibraryInfo(bool detailed) const override
        {
            Q_UNUSED(detailed)
            logEmptyContextWarning(Q_FUNC_INFO);
            return QString("N/A");
        }

        //! \copydoc IContextNetwork::testCreateDummyOnlineAtcStations
        virtual void testCreateDummyOnlineAtcStations(int number) override
        {
            Q_UNUSED(number)
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextNetwork::testAddAircraftParts
        virtual void testAddAircraftParts(const swift::misc::aviation::CCallsign &callsign, const swift::misc::aviation::CAircraftParts &parts, bool incremental) override
        {
            Q_UNUSED(callsign)
            Q_UNUSED(parts)
            Q_UNUSED(incremental)
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextNetwork::testReceivedTextMessages
        virtual void testReceivedTextMessages(const swift::misc::network::CTextMessageList &textMessages) override
        {
            Q_UNUSED(textMessages)
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextNetwork::testRequestAircraftConfig
        virtual void testRequestAircraftConfig(const swift::misc::aviation::CCallsign &callsign) override
        {
            Q_UNUSED(callsign)
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextNetwork::testAddAltitudeOffset
        virtual bool testAddAltitudeOffset(const swift::misc::aviation::CCallsign &callsign, const swift::misc::physical_quantities::CLength &offset = swift::misc::physical_quantities::CLength::null()) override
        {
            Q_UNUSED(callsign)
            Q_UNUSED(offset)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextNetwork::testReceivedAtisMessage
        virtual void testReceivedAtisMessage(const swift::misc::aviation::CCallsign &callsign, const swift::misc::aviation::CInformationMessage &msg) override
        {
            Q_UNUSED(callsign)
            Q_UNUSED(msg)
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextNetwork::parseCommandLine
        virtual bool parseCommandLine(const QString &commandLine, const swift::misc::CIdentifier &originator) override
        {
            Q_UNUSED(commandLine)
            Q_UNUSED(originator)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextNetwork::getVatsimFsdServers
        virtual swift::misc::network::CServerList getVatsimFsdServers() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::network::CServerList();
        }

        //! \copydoc IContextNetwork::updateAircraftEnabled
        virtual bool updateAircraftEnabled(const swift::misc::aviation::CCallsign &callsign, bool enabledForRendering) override
        {
            Q_UNUSED(callsign)
            Q_UNUSED(enabledForRendering)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextNetwork::updateAircraftSupportingGndFLag
        virtual bool updateAircraftSupportingGndFLag(const swift::misc::aviation::CCallsign &callsign, bool supportGndFlag) override
        {
            Q_UNUSED(callsign)
            Q_UNUSED(supportGndFlag)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextNetwork::updateAircraftModel
        virtual bool updateAircraftModel(const swift::misc::aviation::CCallsign &callsign, const swift::misc::simulation::CAircraftModel &model, const swift::misc::CIdentifier &originator) override
        {
            Q_UNUSED(callsign)
            Q_UNUSED(model)
            Q_UNUSED(originator)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextNetwork::updateAircraftNetworkModel
        virtual bool updateAircraftNetworkModel(const swift::misc::aviation::CCallsign &callsign, const swift::misc::simulation::CAircraftModel &model, const swift::misc::CIdentifier &originator) override
        {
            Q_UNUSED(callsign)
            Q_UNUSED(model)
            Q_UNUSED(originator)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextNetwork::updateFastPositionEnabled
        virtual bool updateFastPositionEnabled(const swift::misc::aviation::CCallsign &callsign, bool enableFastPositionSending) override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            Q_UNUSED(callsign)
            Q_UNUSED(enableFastPositionSending)
            return false;
        }

        //! \copydoc IContextNetwork::setAircraftEnabledFlag
        virtual bool setAircraftEnabledFlag(const swift::misc::aviation::CCallsign &callsign, bool enabledForRendering) override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            Q_UNUSED(callsign)
            Q_UNUSED(enabledForRendering)
            return false;
        }

        //! \copydoc IContextNetwork::reInitializeAllAircraft
        virtual int reInitializeAllAircraft() override
        {
            return 0;
        }

        //! \copydoc IContextNetwork::setFastPositionEnabledCallsigns
        virtual void setFastPositionEnabledCallsigns(swift::misc::aviation::CCallsignSet &callsigns) override
        {
            Q_UNUSED(callsigns)
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextNetwork::getFastPositionEnabledCallsigns
        virtual swift::misc::aviation::CCallsignSet getFastPositionEnabledCallsigns() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::aviation::CCallsignSet();
        }

        //! \copydoc IContextNetwork::getReverseLookupMessages
        virtual swift::misc::CStatusMessageList getReverseLookupMessages(const swift::misc::aviation::CCallsign &callsign) const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            Q_UNUSED(callsign)
            return swift::misc::CStatusMessageList();
        }

        //! \copydoc IContextNetwork::isReverseLookupMessagesEnabled
        virtual swift::misc::simulation::ReverseLookupLogging isReverseLookupMessagesEnabled() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::simulation::RevLogDisabled;
        }

        //! \copydoc IContextNetwork::enableReverseLookupMessages
        virtual void enableReverseLookupMessages(swift::misc::simulation::ReverseLookupLogging enable) override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            Q_UNUSED(enable)
        }

        //! \copydoc IContextNetwork::getAircraftPartsHistory
        virtual swift::misc::CStatusMessageList getAircraftPartsHistory(const swift::misc::aviation::CCallsign &callsign) const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            Q_UNUSED(callsign)
            return swift::misc::CStatusMessageList();
        }

        //! \copydoc IContextNetwork::getRemoteAircraftParts
        virtual swift::misc::aviation::CAircraftPartsList getRemoteAircraftParts(const swift::misc::aviation::CCallsign &callsign) const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            Q_UNUSED(callsign)
            return swift::misc::aviation::CAircraftPartsList();
        }

        //! \copydoc IContextNetwork::isAircraftPartsHistoryEnabled
        virtual bool isAircraftPartsHistoryEnabled() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextNetwork::isRemoteAircraftSupportingParts
        virtual bool isRemoteAircraftSupportingParts(const swift::misc::aviation::CCallsign &callsign) const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            Q_UNUSED(callsign)
            return false;
        }

        //! \copydoc IContextNetwork::enableAircraftPartsHistory
        virtual void enableAircraftPartsHistory(bool enabled) override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            Q_UNUSED(enabled)
        }

        //! \copydoc IContextNetwork::getNetworkStatistics
        virtual QString getNetworkStatistics(bool reset, const QString &separator) override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            Q_UNUSED(reset)
            Q_UNUSED(separator)
            return {};
        }

        //! \copydoc IContextNetwork::setNetworkStatisticsEnable
        virtual bool setNetworkStatisticsEnable(bool enabled) override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            Q_UNUSED(enabled;)
            return false;
        }

    public:
        //! \copydoc IContextNetwork::connectRawFsdMessageSignal
        virtual QMetaObject::Connection connectRawFsdMessageSignal(QObject *receiver, RawFsdMessageReceivedSlot rawFsdMessageReceivedSlot) override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            Q_UNUSED(receiver)
            Q_UNUSED(rawFsdMessageReceivedSlot)
            return {};
        }
    };
} // namespace
#endif // guard
