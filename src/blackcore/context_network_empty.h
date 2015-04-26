/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_CONTEXTNETWORK_EMPTY_H
#define BLACKCORE_CONTEXTNETWORK_EMPTY_H

#include "blackcoreexport.h"
#include "context_network.h"
#include "blackmisc/logmessage.h"

namespace BlackCore
{

    //! Empty context, used during shutdown/initialization
    class BLACKCORE_EXPORT CContextNetworkEmpty : public IContextNetwork
    {
        Q_OBJECT

    public:

        //! Constructor
        CContextNetworkEmpty(CRuntime *runtime) : IContextNetwork(CRuntimeConfig::NotUsed, runtime) {}

    public slots: // IContextNetwork overrides

        //! \copydoc IContextNetwork::readAtcBookingsFromSource()
        virtual void readAtcBookingsFromSource() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextNetwork::getAtcStationsOnline()
        virtual BlackMisc::Aviation::CAtcStationList getAtcStationsOnline() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Aviation::CAtcStationList();
        }

        //! \copydoc IContextNetwork::getAtcStationsBooked()
        virtual BlackMisc::Aviation::CAtcStationList getAtcStationsBooked() const override
        {
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
            Q_UNUSED(callsign);
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Simulation::CSimulatedAircraft();
        }

        //! \copydoc IRemoteAircraftProvider::getAircraftInRangeCount
        virtual int getAircraftInRangeCount() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return 0;
        }

        //! \copydoc IContextNetwork::getOnlineStationForCallsign
        virtual BlackMisc::Aviation::CAtcStation getOnlineStationForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const override
        {
            Q_UNUSED(callsign);
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Aviation::CAtcStation();
        }

        //! \copydoc IContextNetwork::connectToNetwork
        virtual BlackMisc::CStatusMessage connectToNetwork(const BlackMisc::Network::CServer &server, uint mode) override
        {
            Q_UNUSED(mode);
            Q_UNUSED(server);
            logEmptyContextWarning(Q_FUNC_INFO);
            return statusMessageEmptyContext();
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

        //! \copydoc IContextNetwork::sendTextMessages()
        virtual void sendTextMessages(const BlackMisc::Network::CTextMessageList &textMessages) override
        {
            Q_UNUSED(textMessages);
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextNetwork::sendFlightPlan()
        virtual void sendFlightPlan(const BlackMisc::Aviation::CFlightPlan &flightPlan) override
        {
            Q_UNUSED(flightPlan);
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextNetwork::loadFlightPlanFromNetwork()
        virtual BlackMisc::Aviation::CFlightPlan loadFlightPlanFromNetwork(const BlackMisc::Aviation::CCallsign &callsign) const override
        {
            Q_UNUSED(callsign);
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Aviation::CFlightPlan();
        }

        //! \copydoc IContextNetwork::getMetar
        virtual BlackMisc::Aviation::CInformationMessage getMetar(const BlackMisc::Aviation::CAirportIcao &airportIcaoCode) override
        {
            Q_UNUSED(airportIcaoCode);
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Aviation::CInformationMessage();
        }

        //! \copydoc IContextNetwork::getSelectedVoiceRooms()
        virtual BlackMisc::Audio::CVoiceRoomList getSelectedVoiceRooms() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Audio::CVoiceRoomList();
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
            Q_UNUSED(callsigns);
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Network::CUserList();
        }

        //! \copydoc IContextNetwork::getUserForCallsign
        virtual BlackMisc::Network::CUser getUserForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const override
        {
            Q_UNUSED(callsign);
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Network::CUser();
        }

        //! \copydoc IContextNetwork::getOtherClients
        virtual BlackMisc::Network::CClientList getOtherClients() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Network::CClientList();
        }

        //! \copydoc IContextNetwork::getOtherClientForCallsigns
        virtual BlackMisc::Network::CClientList getOtherClientsForCallsigns(const BlackMisc::Aviation::CCallsignSet &callsigns) const override
        {
            Q_UNUSED(callsigns);
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Network::CClientList();
        }

        //! \copydoc IContextNetwork::requestDataUpdates
        virtual void requestDataUpdates()override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextNetwork::requestAtisUpdates
        virtual void requestAtisUpdates() override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextNetwork::testCreateDummyOnlineAtcStations
        virtual void testCreateDummyOnlineAtcStations(int number) override
        {
            Q_UNUSED(number);
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextNetwork::testAddAircraftParts
        virtual void testAddAircraftParts(const BlackMisc::Aviation::CAircraftParts &parts, bool incremental) override
        {
            Q_UNUSED(parts);
            Q_UNUSED(incremental);
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextNetwork::parseCommandLine
        virtual bool parseCommandLine(const QString &commandLine, const QString &originator) override
        {
            Q_UNUSED(commandLine);
            Q_UNUSED(originator);
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
        virtual bool updateAircraftEnabled(const BlackMisc::Aviation::CCallsign &callsign, bool enabledForRedering, const QString &originator) override
        {
            Q_UNUSED(callsign);
            Q_UNUSED(originator);
            Q_UNUSED(enabledForRedering);
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextNetwork::updateAircraftModel
        virtual bool updateAircraftModel(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Simulation::CAircraftModel &model, const QString &originator) override
        {
            Q_UNUSED(callsign);
            Q_UNUSED(originator);
            Q_UNUSED(model);
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextNetwork::updateFastPositionEnabled
        virtual bool updateFastPositionEnabled(const BlackMisc::Aviation::CCallsign &callsign, bool enableFastPositionSending, const QString &originator) override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            Q_UNUSED(callsign);
            Q_UNUSED(enableFastPositionSending);
            Q_UNUSED(originator);
            return false;
        }

        //! \copydoc IContextNetwork::isFastPositionSendingEnabled
        virtual bool isFastPositionSendingEnabled() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextNetwork::enableFastPositionSending
        virtual void enableFastPositionSending(bool enable) override
        {
            Q_UNUSED(enable);
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextNetwork::setFastPositionEnabledCallsigns
        virtual void setFastPositionEnabledCallsigns(BlackMisc::Aviation::CCallsignSet &callsigns) override
        {
            Q_UNUSED(callsigns);
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextNetwork::getFastPositionEnabledCallsigns
        virtual BlackMisc::Aviation::CCallsignSet getFastPositionEnabledCallsigns() override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Aviation::CCallsignSet();
        }

    };
} // namespace

#endif // guard
