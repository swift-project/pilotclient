/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_CONTEXTNETWORK_IMPL_H
#define BLACKCORE_CONTEXTNETWORK_IMPL_H

#include "blackcore/context_network.h"
#include "blackcore/context_settings.h"
#include "blackcore/context_runtime.h"
#include "blackcore/dbus_server.h"
#include "blackcore/network.h"
#include "blackcore/airspace_monitor.h"
#include "blackmisc/avatcstationlist.h"
#include "blackmisc/setnetwork.h"
#include "blackmisc/nwclientlist.h"
#include "blackmisc/digestsignal.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/avaircraftsituationlist.h"

#include <QMap>

class QTimer;

namespace BlackCore
{
    class CVatsimBookingReader;
    class CVatsimDataFileReader;

    //! Network context implementation
    class CContextNetwork : public IContextNetwork
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTNETWORK_INTERFACENAME)
        friend class IContextNetwork;
        friend class CRuntime;

    public:
        //! Destructor
        virtual ~CContextNetwork();

    public slots:

        //! \copydoc IContextNetwork::readAtcBookingsFromSource()
        virtual void readAtcBookingsFromSource() const override;

        //! \copydoc IContextNetwork::getAtcStationsOnline()
        virtual BlackMisc::Aviation::CAtcStationList getAtcStationsOnline() const override
        {
            BlackMisc::CLogMessage(this, BlackMisc::CLogCategory::contextSlot()).debug() << Q_FUNC_INFO;
            return this->m_airspace->getAtcStationsOnline();
        }

        //! \copydoc IContextNetwork::getAtcStationsBooked()
        virtual BlackMisc::Aviation::CAtcStationList getAtcStationsBooked() const override
        {
            BlackMisc::CLogMessage(this, BlackMisc::CLogCategory::contextSlot()).debug() << Q_FUNC_INFO;
            return this->m_airspace->getAtcStationsBooked();
        }

        //! \copydoc IContextNetwork::getAircraftInRange()
        virtual BlackMisc::Aviation::CAircraftList getAircraftInRange() const override
        {
            BlackMisc::CLogMessage(this, BlackMisc::CLogCategory::contextSlot()).debug() << Q_FUNC_INFO;
            return this->m_airspace->getAircraftInRange();
        }

        //! \copydoc IContextNetwork::connectToNetwork()
        virtual BlackMisc::CStatusMessage connectToNetwork(const BlackMisc::Network::CServer &server, uint mode) override;

        //! \copydoc IContextNetwork::disconnectFromNetwork()
        virtual BlackMisc::CStatusMessage disconnectFromNetwork() override;

        //! \copydoc IContextNetwork::isConnected()
        virtual bool isConnected() const override;

        /*!
         * In transition state, e.g. connecting, disconnecting.
         * \details In such a state it is advisable to wait until an end state (connected/disconnected) is reached
         * \remarks Intentionally only running locally, not in interface
         */
        bool isPendingConnection() const;

        //! \copydoc IContextNetwork::parseCommandLine
        virtual bool parseCommandLine(const QString &commandLine) override;

        //! \copydoc IContextNetwork::sendTextMessages()
        virtual void sendTextMessages(const BlackMisc::Network::CTextMessageList &textMessages) override;

        //! \copydoc IContextNetwork::sendFlightPlan()
        virtual void sendFlightPlan(const BlackMisc::Aviation::CFlightPlan &flightPlan) override;

        //! \copydoc IContextNetwork::loadFlightPlanFromNetwork()
        virtual BlackMisc::Aviation::CFlightPlan loadFlightPlanFromNetwork(const BlackMisc::Aviation::CCallsign &callsign) const override;

        //! \copydoc IContextNetwork::getMetar
        virtual BlackMisc::Aviation::CInformationMessage getMetar(const BlackMisc::Aviation::CAirportIcao &airportIcaoCode) override;

        //! \copydoc IContextNetwork::getSelectedVoiceRooms()
        virtual BlackMisc::Audio::CVoiceRoomList getSelectedVoiceRooms() const override;

        //! \copydoc IContextNetwork::getSelectedAtcStations
        virtual BlackMisc::Aviation::CAtcStationList getSelectedAtcStations() const override;

        //! \copydoc IContextNetwork::getUsers()
        virtual BlackMisc::Network::CUserList getUsers() const override;

        //! \copydoc IContextNetwork::getUsersForCallsigns
        virtual BlackMisc::Network::CUserList getUsersForCallsigns(const BlackMisc::Aviation::CCallsignList &callsigns) const override;

        //! \copydoc IContextNetwork::getUsersForCallsign
        virtual BlackMisc::Network::CUser getUserForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const override;

        //! \copydoc IContextNetwork::getOtherClients
        virtual BlackMisc::Network::CClientList getOtherClients() const override;

        //! \copydoc IContextNetwork::getOtherClientForCallsigns
        virtual BlackMisc::Network::CClientList getOtherClientsForCallsigns(const BlackMisc::Aviation::CCallsignList &callsigns) const override;

        //! \copydoc IContextNetwork::getVatsimFsdServers
        virtual BlackMisc::Network::CServerList getVatsimFsdServers() const override;

        //! \copydoc IContextNetwork::getVatsimVoiceServers
        virtual BlackMisc::Network::CServerList getVatsimVoiceServers() const override;

        //! \copydoc IContextNetwork::requestDataUpdates
        virtual void requestDataUpdates()override;

        //! \copydoc IContextNetwork::requestAtisUpdates
        virtual void requestAtisUpdates() override;

        //! \copydoc IContextNetwork::testCreateDummyOnlineAtcStations
        virtual void testCreateDummyOnlineAtcStations(int number) override;

        //! Gracefully shut down, e.g. for thread safety
        void gracefulShutdown();

    protected:
        //! Constructor, with link to runtime
        CContextNetwork(CRuntimeConfig::ContextMode, CRuntime *runtime);

        //! Register myself in DBus
        CContextNetwork *registerWithDBus(CDBusServer *server)
        {
            if (!server || this->m_mode != CRuntimeConfig::LocalInDbusServer) return this;
            server->addObject(IContextNetwork::ObjectPath(), this);
            return this;
        }

    private:
        CAirspaceMonitor    *m_airspace = nullptr;
        BlackCore::INetwork *m_network  = nullptr;
        INetwork::ConnectionStatus m_currentStatus = INetwork::Disconnected; //!< used to detect pending connections

        BlackMisc::CDigestSignal m_dsAtcStationsBookedChanged { this, &IContextNetwork::changedAtcStationsBooked, &IContextNetwork::changedAtcStationsBookedDigest, 750, 2 };
        BlackMisc::CDigestSignal m_dsAtcStationsOnlineChanged { this, &IContextNetwork::changedAtcStationsOnline, &IContextNetwork::changedAtcStationsOnlineDigest, 750, 4 };
        BlackMisc::CDigestSignal m_dsAircraftsInRangeChanged  { this, &IContextNetwork::changedAircraftInRange, &IContextNetwork::changedAircraftInRangeDigest, 750, 4 };

        // for reading XML and VATSIM data files
        CVatsimBookingReader  *m_vatsimBookingReader  = nullptr;
        CVatsimDataFileReader *m_vatsimDataFileReader = nullptr;
        QTimer *m_dataUpdateTimer = nullptr; //!< general updates such as ATIS, frequencies, see requestDataUpdates()

        //! Get network settings
        BlackMisc::Settings::CSettingsNetwork getNetworkSettings() const
        {
            Q_ASSERT(this->getRuntime());
            Q_ASSERT(this->getRuntime()->getIContextSettings());
            return this->getRuntime()->getIContextSettings()->getNetworkSettings();
        }

        //! Own aircraft
        const BlackMisc::Aviation::CAircraft &ownAircraft() const;

    private slots:
        //! ATC bookings received
        void ps_receivedBookings(const BlackMisc::Aviation::CAtcStationList &bookedStations);

        //! Data file has been read
        void ps_dataFileRead();

        /*!
         * \brief Connection status changed?
         * \param from  old status
         * \param to    new status
         */
        void ps_fsdConnectionStatusChanged(INetwork::ConnectionStatus from, INetwork::ConnectionStatus to);

        //! Radio text messages received
        void ps_fsdTextMessageReceived(const BlackMisc::Network::CTextMessageList &messages);
    };
}

#endif // guard
