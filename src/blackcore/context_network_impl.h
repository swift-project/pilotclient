/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_CONTEXTNETWORK_IMPL_H
#define BLACKCORE_CONTEXTNETWORK_IMPL_H

#include "blackcore/context_network.h"
#include "blackcore/context_settings.h"
#include "blackcore/coreruntime.h"
#include "blackcore/dbus_server.h"
#include "blackcore/network.h"
#include "blackmisc/avatcstationlist.h"
#include "blackmisc/setnetwork.h"

#include <QMap>

class QTimer;

namespace BlackCore
{
    class CVatsimBookingReader;
    class CVatsimDataFileReader;

    //! \brief Network context implementation
    class CContextNetwork : public IContextNetwork
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTNETWORK_INTERFACENAME)

    public:

        //! \brief Constructor, with link to runtime
        CContextNetwork(QObject *parent = nullptr);

        //! \brief Destructor
        virtual ~CContextNetwork();

        /*!
         * \brief Register myself in DBus
         * \param server    DBus server
         */
        void registerWithDBus(CDBusServer *server)
        {
            Q_ASSERT(server);
            server->addObject(IContextNetwork::ObjectPath(), this);
        }

        //! \brief Runtime
        CCoreRuntime *getRuntime()
        {
            return static_cast<CCoreRuntime *>(this->parent());
        }

        //! \brief Const runtime
        const CCoreRuntime *getRuntime() const
        {
            return static_cast<CCoreRuntime *>(this->parent());
        }

        //! \copydoc IContextNetwork::usingLocalObjects()
        virtual bool usingLocalObjects() const override { return true; }

    public slots: // IContextNetwork overrides

        //! \copydoc IContextNetwork::readAtcBookingsFromSource()
        virtual void readAtcBookingsFromSource() const override;

        /*!
         * \copydoc IContextNetwork::getAtcStationsOnline()
         * \todo If I make this &getAtcStations XML is not generated correctly, needs to be crosschecked with the latest version of Qt
         */
        virtual const BlackMisc::Aviation::CAtcStationList getAtcStationsOnline() const override
        {
            // this->log(Q_FUNC_INFO);
            return m_atcStationsOnline;
        }

        /*!
         * \copydoc IContextNetwork::getAtcStationsBooked()
         * \todo If I make this &getAtcStations XML is not generated correctly
         */
        virtual const BlackMisc::Aviation::CAtcStationList getAtcStationsBooked() const override
        {
            // this->log(Q_FUNC_INFO);
            return m_atcStationsBooked;
        }

        //! \copydoc IContextNetwork::getAircraftsInRange()
        virtual const BlackMisc::Aviation::CAircraftList getAircraftsInRange() const override
        {
            // this->log(Q_FUNC_INFO);
            return m_aircraftsInRange;
        }

        //! \copydoc IContextNetwork::connectToNetwork()
        virtual BlackMisc::CStatusMessageList connectToNetwork(uint mode) override;

        //! \copydoc IContextNetwork::disconnectFromNetwork()
        virtual BlackMisc::CStatusMessageList disconnectFromNetwork() override;

        //! \copydoc IContextNetwork::isConnected()
        virtual bool isConnected() const override;

        //! \copydoc IContextNetwork::setOwnAircraft()
        virtual BlackMisc::CStatusMessageList setOwnAircraft(const BlackMisc::Aviation::CAircraft &aircraft) override;

        //! \copydoc IContextNetwork::updateOwnPosition()
        virtual void updateOwnPosition(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude) override;

        //! \copydoc IContextNetwork::updateOwnSituation()
        virtual void updateOwnSituation(const BlackMisc::Aviation::CAircraftSituation &situation) override;

        //! \copydoc IContextNetwork::updateOwnCockpit()
        virtual void updateOwnCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder) override;

        //! \copydoc IContextNetwork::getOwnAircraft()
        virtual BlackMisc::Aviation::CAircraft getOwnAircraft() const override;

        //! \copydoc IContextNetwork::sendTextMessages()
        virtual void sendTextMessages(const BlackMisc::Network::CTextMessageList &textMessages) override;

        //! \copydoc IContextNetwork::getMetar()
        virtual BlackMisc::Aviation::CInformationMessage getMetar(const QString &airportIcaoCode) override;

        //! \copydoc IContextNetwork::getSelectedVoiceRooms()
        virtual BlackMisc::Audio::CVoiceRoomList getSelectedVoiceRooms() const override;

        //! \copydoc IContextNetwork::getSelectedAtcStations
        virtual BlackMisc::Aviation::CAtcStationList getSelectedAtcStations() const override;

        //! \copydoc IContextNetwork::getUsers()
        virtual BlackMisc::Network::CUserList getUsers() const override;

        //! \copydoc IContextNetwork::getUsersForCallsigns
        virtual BlackMisc::Network::CUserList getUsersForCallsigns(const BlackMisc::Aviation::CCallsignList &callsigns) const override;

        //! \copydoc IContextNetwork::requestDataUpdates
        virtual void requestDataUpdates()override;

        //! \copydoc IContextNetwork::requestAtisUpdates
        virtual void requestAtisUpdates() override;

    private:
        BlackMisc::Aviation::CAtcStationList m_atcStationsOnline;
        BlackMisc::Aviation::CAtcStationList m_atcStationsBooked;
        BlackMisc::Aviation::CAircraftList m_aircraftsInRange;
        BlackCore::INetwork *m_network;
        BlackMisc::Aviation::CAircraft m_ownAircraft;
        QMap<QString, BlackMisc::Aviation::CInformationMessage> m_metarCache /*!< Keep METARs for a while */;

        // for reading XML and VATSIM data files
        CVatsimBookingReader *m_vatsimBookingReader;
        CVatsimDataFileReader *m_vatsimDataFileReader;
        QTimer *m_dataUpdateTimer; //!< general updates such as ATIS, frequencies, see requestDataUpdates()

        //! \brief Replace value by new values
        void setAtcStationsBooked(const BlackMisc::Aviation::CAtcStationList &newStations);

        //! \brief Replace value by new values
        void setAtcStationsOnline(const BlackMisc::Aviation::CAtcStationList &newStations);

        //! \brief The "central" ATC list with online ATC controllers
        BlackMisc::Aviation::CAtcStationList &atcStationsOnline()
        {
            return m_atcStationsOnline;
        }

        //! \brief ATC list, with booked controllers
        BlackMisc::Aviation::CAtcStationList &atcStationsBooked()
        {
            return m_atcStationsBooked;
        }

        //! \brief Init my very onw aircraft
        void initOwnAircraft();

        //! \brief Get network settings
        BlackMisc::Settings::CSettingsNetwork getNetworkSettings() const
        {
            Q_ASSERT(this->getRuntime());
            Q_ASSERT(this->getRuntime()->getIContextSettings());
            return this->getRuntime()->getIContextSettings()->getNetworkSettings();
        }

    private slots:
        //! \brief ATC bookings received
        void psReceivedBookings(BlackMisc::Aviation::CAtcStationList bookedStations);

        //! \brief Data file has been read
        void psDataFileRead();

        /*!
         * \brief Connection status changed?
         * \param from  old status
         * \param to    new status
         */
        void psFsdConnectionStatusChanged(INetwork::ConnectionStatus from, INetwork::ConnectionStatus to);

        //! \brief ATC position update
        void psFsdAtcPositionUpdate(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CFrequency &frequency, const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::PhysicalQuantities::CLength &range);

        /*!
         * \brief Controller disconnected
         * \param callsign  callsign of controller
         */
        void psFsdAtcControllerDisconnected(const BlackMisc::Aviation::CCallsign &callsign);

        //! \brief ATIS received
        void psFsdAtisQueryReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CInformationMessage &atisMessage);

        /*!
         * \brief ATIS received (voice room part)
         * \param callsign  station callsign
         * \param url       voice room's URL
         */
        void psFsdAtisVoiceRoomQueryReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &url);

        /*!
         * \brief ATIS received (logoff time part)
         * \param callsign  station callsign
         * \param zuluTime  UTC time, when controller will logoff
         */
        void psFsdAtisLogoffTimeQueryReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &zuluTime);

        //! \brief METAR received
        void psFsdMetarReceived(const QString &metarMessage);

        //! \brief Realname recevied
        void psFsdRealNameReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &realname);

        //! \brief Plane ICAO codes received
        void psFsdIcaoCodesReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftIcao &icaoData);

        //! \brief Aircraft position update received
        void psFsdAircraftUpdateReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftSituation &situation, const BlackMisc::Aviation::CTransponder &transponder);

        //! \brief Pilot disconnected
        void psFsdPilotDisconnected(const BlackMisc::Aviation::CCallsign &callsign);

        //! \brief Frequency received
        void psFsdFrequencyReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CFrequency &frequency);

        //! \brief Radio text messages received
        void psFsdTextMessageReceived(const BlackMisc::Network::CTextMessageList &messages);
    };
}

#endif // guard
