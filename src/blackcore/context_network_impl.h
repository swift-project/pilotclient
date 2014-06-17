/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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

    public slots: // IContextNetwork overrides

        //! \copydoc IContextNetwork::readAtcBookingsFromSource()
        virtual void readAtcBookingsFromSource() const override;

        //! \copydoc IContextNetwork::getAtcStationsOnline()
        virtual const BlackMisc::Aviation::CAtcStationList getAtcStationsOnline() const override
        {
            this->getRuntime()->logSlot(c_logContext, Q_FUNC_INFO);
            return this->m_airspace->getAtcStationsOnline();
        }

        //! \copydoc IContextNetwork::getAtcStationsBooked()
        virtual const BlackMisc::Aviation::CAtcStationList getAtcStationsBooked() const override
        {
            this->getRuntime()->logSlot(c_logContext, Q_FUNC_INFO);
            return this->m_airspace->getAtcStationsBooked();
        }

        //! \copydoc IContextNetwork::getAircraftsInRange()
        virtual const BlackMisc::Aviation::CAircraftList getAircraftsInRange() const override
        {
            this->getRuntime()->logSlot(c_logContext, Q_FUNC_INFO);
            return this->m_airspace->getAircraftInRange();
        }

        //! \copydoc IContextNetwork::connectToNetwork()
        virtual BlackMisc::CStatusMessageList connectToNetwork(uint mode) override;

        //! \copydoc IContextNetwork::disconnectFromNetwork()
        virtual BlackMisc::CStatusMessageList disconnectFromNetwork() override;

        //! \copydoc IContextNetwork::isConnected()
        virtual bool isConnected() const override;

        /*!
         * In transition state, e.g. connecting, disconnecting.
         * \details In such a state it is advisable to wait until an end state (connected/disconnected) is reached
         * \remarks Intentionally only running locally, not in interface
         */
        bool isPendingConnection() const;

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

        //! \copydoc IContextNetwork::requestDataUpdates
        virtual void requestDataUpdates()override;

        //! \copydoc IContextNetwork::requestAtisUpdates
        virtual void requestAtisUpdates() override;

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
        static const auto c_logContext = CRuntime::LogForNetwork;

        CAirspaceMonitor *m_airspace;
        BlackCore::INetwork *m_network;
        INetwork::ConnectionStatus m_currentStatus; //!< used to detect pending connections

        // for reading XML and VATSIM data files
        CVatsimBookingReader *m_vatsimBookingReader;
        CVatsimDataFileReader *m_vatsimDataFileReader;
        QTimer *m_dataUpdateTimer; //!< general updates such as ATIS, frequencies, see requestDataUpdates()

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
        //! Own aircraft was updated
        void psChangedOwnAircraft(const BlackMisc::Aviation::CAircraft &aircraft, const QString &originator);

        //! ATC bookings received
        void psReceivedBookings(const BlackMisc::Aviation::CAtcStationList &bookedStations);

        //! Data file has been read
        void psDataFileRead();

        /*!
         * \brief Connection status changed?
         * \param from  old status
         * \param to    new status
         */
        void psFsdConnectionStatusChanged(INetwork::ConnectionStatus from, INetwork::ConnectionStatus to, const QString &message);

        //! Radio text messages received
        void psFsdTextMessageReceived(const BlackMisc::Network::CTextMessageList &messages);
    };
}

#endif // guard
