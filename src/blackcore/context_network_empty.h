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

#include "context_network.h"

namespace BlackCore
{

    //! Empty context, used during shutdown/initialization
    class CContextNetworkEmpty : public IContextNetwork
    {
        Q_OBJECT

    public:

        //! Constructor
        CContextNetworkEmpty(bool log, CRuntime *runtime) : IContextNetwork(CRuntimeConfig::NotUsed, runtime), m_log(log) {}

        //! Destructor
        virtual ~CContextNetworkEmpty() {}

    public slots: // IContextNetwork overrides

        //! \copydoc IContextNetwork::readAtcBookingsFromSource()
        virtual void readAtcBookingsFromSource() const override
        {
            if (m_log) { qDebug() << Q_FUNC_INFO ; }
        }

        //! \copydoc IContextNetwork::getAtcStationsOnline()
        virtual const BlackMisc::Aviation::CAtcStationList getAtcStationsOnline() const override
        {
            if (m_log) { qDebug() << Q_FUNC_INFO ; }
            return BlackMisc::Aviation::CAtcStationList();
        }

        //! \copydoc IContextNetwork::getAtcStationsBooked()
        virtual const BlackMisc::Aviation::CAtcStationList getAtcStationsBooked() const override
        {
            if (m_log) { qDebug() << Q_FUNC_INFO ; }
            return BlackMisc::Aviation::CAtcStationList();
        }

        //! \copydoc IContextNetwork::getAircraftsInRange()
        virtual const BlackMisc::Aviation::CAircraftList getAircraftsInRange() const override
        {
            if (m_log) { qDebug() << Q_FUNC_INFO ; }
            return BlackMisc::Aviation::CAircraftList();
        }

        //! \copydoc IContextNetwork::connectToNetwork()
        virtual BlackMisc::CStatusMessage connectToNetwork(uint mode) override
        {
            if (m_log) { qDebug() << Q_FUNC_INFO << mode; }
            return statusMessageEmptyContext();
        }

        //! \copydoc IContextNetwork::disconnectFromNetwork()
        virtual BlackMisc::CStatusMessage disconnectFromNetwork() override
        {
            if (m_log) { qDebug() << Q_FUNC_INFO ; }
            return statusMessageEmptyContext();
        }

        //! \copydoc IContextNetwork::isConnected()
        virtual bool isConnected() const override
        {
            if (m_log) { qDebug() << Q_FUNC_INFO ; }
            return false;
        }

        //! \copydoc IContextNetwork::sendTextMessages()
        virtual void sendTextMessages(const BlackMisc::Network::CTextMessageList &textMessages) override
        {
            if (m_log) { qDebug() << Q_FUNC_INFO << textMessages; }
        }

        //! \copydoc IContextNetwork::sendFlightPlan()
        virtual void sendFlightPlan(const BlackMisc::Aviation::CFlightPlan &flightPlan) override
        {
            if (m_log) { qDebug() << Q_FUNC_INFO << flightPlan; }
        }

        //! \copydoc IContextNetwork::loadFlightPlanFromNetwork()
        virtual BlackMisc::Aviation::CFlightPlan loadFlightPlanFromNetwork(const BlackMisc::Aviation::CCallsign &callsign) const override
        {
            if (m_log) { qDebug() << Q_FUNC_INFO << callsign; }
            return BlackMisc::Aviation::CFlightPlan();
        }

        //! \copydoc IContextNetwork::getMetar
        virtual BlackMisc::Aviation::CInformationMessage getMetar(const BlackMisc::Aviation::CAirportIcao &airportIcaoCode) override
        {
            if (m_log) { qDebug() << Q_FUNC_INFO << airportIcaoCode; }
            return BlackMisc::Aviation::CInformationMessage();
        }

        //! \copydoc IContextNetwork::getSelectedVoiceRooms()
        virtual BlackMisc::Audio::CVoiceRoomList getSelectedVoiceRooms() const override
        {
            if (m_log) { qDebug() << Q_FUNC_INFO ; }
            return BlackMisc::Audio::CVoiceRoomList();
        }

        //! \copydoc IContextNetwork::getSelectedAtcStations
        virtual BlackMisc::Aviation::CAtcStationList getSelectedAtcStations() const override
        {
            if (m_log) { qDebug() << Q_FUNC_INFO ; }
            return BlackMisc::Aviation::CAtcStationList();
        }

        //! \copydoc IContextNetwork::getUsers()
        virtual BlackMisc::Network::CUserList getUsers() const override
        {
            if (m_log) { qDebug() << Q_FUNC_INFO ; }
            return BlackMisc::Network::CUserList();
        }

        //! \copydoc IContextNetwork::getUsersForCallsigns
        virtual BlackMisc::Network::CUserList getUsersForCallsigns(const BlackMisc::Aviation::CCallsignList &callsigns) const override
        {
            if (m_log) { qDebug() << Q_FUNC_INFO << callsigns; }
            return BlackMisc::Network::CUserList();
        }

        //! \copydoc IContextNetwork::getUserForCallsign
        virtual BlackMisc::Network::CUser getUserForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const override
        {
            if (m_log) { qDebug() << Q_FUNC_INFO << callsign; }
            return BlackMisc::Network::CUser();
        }

        //! \copydoc IContextNetwork::getOtherClients
        virtual BlackMisc::Network::CClientList getOtherClients() const override
        {
            if (m_log) { qDebug() << Q_FUNC_INFO ; }
            return BlackMisc::Network::CClientList();
        }

        //! \copydoc IContextNetwork::getOtherClientForCallsigns
        virtual BlackMisc::Network::CClientList getOtherClientsForCallsigns(const BlackMisc::Aviation::CCallsignList &callsigns) const override
        {
            if (m_log) { qDebug() << Q_FUNC_INFO << callsigns; }
            return BlackMisc::Network::CClientList();
        }

        //! \copydoc IContextNetwork::requestDataUpdates
        virtual void requestDataUpdates()override
        {
            if (m_log) { qDebug() << Q_FUNC_INFO ; }
        }

        //! \copydoc IContextNetwork::requestAtisUpdates
        virtual void requestAtisUpdates() override
        {
            if (m_log) { qDebug() << Q_FUNC_INFO ; }
        }

        //! \copydoc IContextNetwork::testCreateDummyOnlineAtcStations
        virtual void testCreateDummyOnlineAtcStations(int number) override
        {
            if (m_log) { qDebug() << Q_FUNC_INFO << number; }
        }

        //! \copydoc IContextNetwork::parseCommandLine
        virtual bool parseCommandLine(const QString &commandLine) override
        {
            if (m_log) { qDebug() << Q_FUNC_INFO << commandLine; }
            return false;
        }


    private:
        bool m_log = true;

    };
} // namespace

#endif // guard
