/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_CONTEXT_CONTEXTNETWORK_EMPTY_H
#define BLACKCORE_CONTEXT_CONTEXTNETWORK_EMPTY_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/context/contextnetwork.h"
#include "blackmisc/logmessage.h"

// clazy:excludeall=const-signal-or-slot

namespace BlackCore::Context
{
    //! Empty context, used during shutdown/initialization
    class BLACKCORE_EXPORT CContextNetworkEmpty : public IContextNetwork
    {
        Q_OBJECT

    public:
        //! Constructor
        CContextNetworkEmpty(CCoreFacade *runtime) : IContextNetwork(CCoreFacadeConfig::NotUsed, runtime) {}

    public slots:
        //! \copydoc IContextNetwork::requestAtcBookingsUpdate()
        virtual void requestAtcBookingsUpdate() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextNetwork::getAtcStationsOnline()
        virtual BlackMisc::Aviation::CAtcStationList getAtcStationsOnline(bool recalculateDistance) const override
        {
            Q_UNUSED(recalculateDistance)
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Aviation::CAtcStationList();
        }

        //! \copydoc IContextNetwork::getClosestAtcStationsOnline()
        virtual BlackMisc::Aviation::CAtcStationList getClosestAtcStationsOnline(int number) const override
        {
            Q_UNUSED(number)
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Aviation::CAtcStationList();
        }

        //! \copydoc IContextNetwork::getAtcStationsBooked()
        virtual BlackMisc::Aviation::CAtcStationList getAtcStationsBooked(bool recalculateDistance) const override
        {
            Q_UNUSED(recalculateDistance)
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Aviation::CAtcStationList();
        }

        //! \copydoc IContextNetwork::getAircraftInRange()
        virtual BlackMisc::Simulation::CSimulatedAircraftList getAircraftInRange() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Simulation::CSimulatedAircraftList();
        }

        //! \copydoc IContextNetwork::getAircraftInRangeForCallsign
        virtual BlackMisc::Simulation::CSimulatedAircraft getAircraftInRangeForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const override
        {
            Q_UNUSED(callsign)
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Simulation::CSimulatedAircraft();
        }

        //! \copydoc IContextNetwork::getOnlineStationsForFrequency
        virtual BlackMisc::Aviation::CAtcStationList getOnlineStationsForFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency) const override
        {
            Q_UNUSED(frequency)
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Aviation::CAtcStationList();
        }

        //! \copydoc IContextNetwork::getAircraftInRangeCallsigns()
        virtual BlackMisc::Aviation::CCallsignSet getAircraftInRangeCallsigns() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Aviation::CCallsignSet();
        }

        //! \copydoc IContextNetwork::getAircraftInRangeCount
        virtual int getAircraftInRangeCount() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return 0;
        }

        //! \copydoc IContextNetwork::isAircraftInRange
        virtual bool isAircraftInRange(const BlackMisc::Aviation::CCallsign &callsign) const override
        {
            Q_UNUSED(callsign)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextNetwork::getOnlineStationForCallsign
        virtual BlackMisc::Aviation::CAtcStation getOnlineStationForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const override
        {
            Q_UNUSED(callsign)
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Aviation::CAtcStation();
        }

        //! \copydoc IContextNetwork::isOnlineStation
        virtual bool isOnlineStation(const BlackMisc::Aviation::CCallsign &callsign) const override
        {
            Q_UNUSED(callsign)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextNetwork::connectToNetwork
        virtual BlackMisc::CStatusMessage connectToNetwork(const BlackMisc::Network::CServer &server, const QString &extraLiveryString, bool sendLivery, const QString &extraModelString, bool sendModelString, const BlackMisc::Aviation::CCallsign &partnerCallsign, BlackMisc::Network::CLoginMode mode) override
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
        virtual BlackMisc::Aviation::CCallsign getPartnerCallsign() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return {};
        }

        //! \copydoc IContextNetwork::disconnectFromNetwork()
        virtual BlackMisc::CStatusMessage disconnectFromNetwork() override
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
        virtual BlackMisc::Network::CServer getConnectedServer() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Network::CServer();
        }

        //! \copydoc IContextNetwork::getLoginMode
        virtual BlackMisc::Network::CLoginMode getLoginMode() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Network::CLoginMode::Pilot;
        }

        //! \copydoc IContextNetwork::sendTextMessages()
        virtual void sendTextMessages(const BlackMisc::Network::CTextMessageList &textMessages) override
        {
            Q_UNUSED(textMessages)
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextNetwork::sendFlightPlan()
        virtual void sendFlightPlan(const BlackMisc::Aviation::CFlightPlan &flightPlan) override
        {
            Q_UNUSED(flightPlan)
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextNetwork::loadFlightPlanFromNetwork()
        virtual BlackMisc::Aviation::CFlightPlan loadFlightPlanFromNetwork(const BlackMisc::Aviation::CCallsign &callsign) const override
        {
            Q_UNUSED(callsign)
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Aviation::CFlightPlan();
        }

        //! \copydoc IContextNetwork::getMetarForAirport
        BlackMisc::Weather::CMetar getMetarForAirport(const BlackMisc::Aviation::CAirportIcaoCode &airportIcaoCode) const override
        {
            Q_UNUSED(airportIcaoCode)
            logEmptyContextWarning(Q_FUNC_INFO);
            return {};
        }

        //! \copydoc IContextNetwork::getSelectedAtcStations
        virtual BlackMisc::Aviation::CAtcStationList getSelectedAtcStations() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            // normally 2 entries
            return BlackMisc::Aviation::CAtcStationList({ BlackMisc::Aviation::CAtcStation(), BlackMisc::Aviation::CAtcStation()});
        }

        //! \copydoc IContextNetwork::getUsers()
        virtual BlackMisc::Network::CUserList getUsers() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Network::CUserList();
        }

        //! \copydoc IContextNetwork::getUsersForCallsigns
        virtual BlackMisc::Network::CUserList getUsersForCallsigns(const BlackMisc::Aviation::CCallsignSet &callsigns) const override
        {
            Q_UNUSED(callsigns)
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Network::CUserList();
        }

        //! \copydoc IContextNetwork::getUserForCallsign
        virtual BlackMisc::Network::CUser getUserForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const override
        {
            Q_UNUSED(callsign)
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Network::CUser();
        }

        //! \copydoc IContextNetwork::getClients
        virtual BlackMisc::Network::CClientList getClients() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Network::CClientList();
        }

        //! \copydoc IContextNetwork::getClientsForCallsigns
        virtual BlackMisc::Network::CClientList getClientsForCallsigns(const BlackMisc::Aviation::CCallsignSet &callsigns) const override
        {
            Q_UNUSED(callsigns)
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Network::CClientList();
        }

        //! \copydoc IContextNetwork::setOtherClient
        virtual bool setOtherClient(const BlackMisc::Network::CClient &client) override
        {
            Q_UNUSED(client)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextNetwork::requestAircraftDataUpdates
        virtual void requestAircraftDataUpdates()override
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
        virtual void testAddAircraftParts(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftParts &parts, bool incremental) override
        {
            Q_UNUSED(callsign)
            Q_UNUSED(parts)
            Q_UNUSED(incremental)
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextNetwork::testReceivedTextMessages
        virtual void testReceivedTextMessages(const BlackMisc::Network::CTextMessageList &textMessages) override
        {
            Q_UNUSED(textMessages)
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextNetwork::testRequestAircraftConfig
        virtual void testRequestAircraftConfig(const BlackMisc::Aviation::CCallsign &callsign) override
        {
            Q_UNUSED(callsign)
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextNetwork::testAddAltitudeOffset
        virtual bool testAddAltitudeOffset(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CLength &offset = BlackMisc::PhysicalQuantities::CLength::null()) override
        {
            Q_UNUSED(callsign)
            Q_UNUSED(offset)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }


        //! \copydoc IContextNetwork::testReceivedAtisMessage
        virtual void testReceivedAtisMessage(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CInformationMessage &msg) override
        {
            Q_UNUSED(callsign)
            Q_UNUSED(msg)
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextNetwork::parseCommandLine
        virtual bool parseCommandLine(const QString &commandLine, const BlackMisc::CIdentifier &originator) override
        {
            Q_UNUSED(commandLine)
            Q_UNUSED(originator)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextNetwork::getVatsimVoiceServers
        virtual BlackMisc::Network::CServerList getVatsimVoiceServers() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Network::CServerList();
        }

        //! \copydoc IContextNetwork::getVatsimFsdServers
        virtual BlackMisc::Network::CServerList getVatsimFsdServers() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Network::CServerList();
        }

        //! \copydoc IContextNetwork::updateAircraftEnabled
        virtual bool updateAircraftEnabled(const BlackMisc::Aviation::CCallsign &callsign, bool enabledForRendering) override
        {
            Q_UNUSED(callsign)
            Q_UNUSED(enabledForRendering)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextNetwork::updateAircraftSupportingGndFLag
        virtual bool updateAircraftSupportingGndFLag(const BlackMisc::Aviation::CCallsign &callsign, bool supportGndFlag) override
        {
            Q_UNUSED(callsign)
            Q_UNUSED(supportGndFlag)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextNetwork::updateAircraftModel
        virtual bool updateAircraftModel(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Simulation::CAircraftModel &model, const BlackMisc::CIdentifier &originator) override
        {
            Q_UNUSED(callsign)
            Q_UNUSED(model)
            Q_UNUSED(originator)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextNetwork::updateAircraftNetworkModel
        virtual bool updateAircraftNetworkModel(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Simulation::CAircraftModel &model, const BlackMisc::CIdentifier &originator) override
        {
            Q_UNUSED(callsign)
            Q_UNUSED(model)
            Q_UNUSED(originator)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextNetwork::updateFastPositionEnabled
        virtual bool updateFastPositionEnabled(const BlackMisc::Aviation::CCallsign &callsign, bool enableFastPositionSending) override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            Q_UNUSED(callsign)
            Q_UNUSED(enableFastPositionSending)
            return false;
        }

        //! \copydoc IContextNetwork::setAircraftEnabledFlag
        virtual bool setAircraftEnabledFlag(const BlackMisc::Aviation::CCallsign &callsign, bool enabledForRendering) override
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
        virtual void setFastPositionEnabledCallsigns(BlackMisc::Aviation::CCallsignSet &callsigns) override
        {
            Q_UNUSED(callsigns)
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextNetwork::getFastPositionEnabledCallsigns
        virtual BlackMisc::Aviation::CCallsignSet getFastPositionEnabledCallsigns() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Aviation::CCallsignSet();
        }

        //! \copydoc IContextNetwork::getReverseLookupMessages
        virtual BlackMisc::CStatusMessageList getReverseLookupMessages(const BlackMisc::Aviation::CCallsign &callsign) const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            Q_UNUSED(callsign)
            return BlackMisc::CStatusMessageList();
        }

        //! \copydoc IContextNetwork::isReverseLookupMessagesEnabled
        virtual BlackMisc::Simulation::ReverseLookupLogging isReverseLookupMessagesEnabled() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Simulation::RevLogDisabled;
        }

        //! \copydoc IContextNetwork::enableReverseLookupMessages
        virtual void enableReverseLookupMessages(BlackMisc::Simulation::ReverseLookupLogging enable) override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            Q_UNUSED(enable)
        }

        //! \copydoc IContextNetwork::getAircraftPartsHistory
        virtual BlackMisc::CStatusMessageList getAircraftPartsHistory(const BlackMisc::Aviation::CCallsign &callsign) const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            Q_UNUSED(callsign)
            return BlackMisc::CStatusMessageList();
        }

        //! \copydoc IContextNetwork::getRemoteAircraftParts
        virtual BlackMisc::Aviation::CAircraftPartsList getRemoteAircraftParts(const BlackMisc::Aviation::CCallsign &callsign) const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            Q_UNUSED(callsign)
            return BlackMisc::Aviation::CAircraftPartsList();
        }

        //! \copydoc IContextNetwork::isAircraftPartsHistoryEnabled
        virtual bool isAircraftPartsHistoryEnabled() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextNetwork::isRemoteAircraftSupportingParts
        virtual bool isRemoteAircraftSupportingParts(const BlackMisc::Aviation::CCallsign &callsign) const override
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
