/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_CONTEXTNETWORK_H
#define BLACKCORE_CONTEXTNETWORK_H

#include "blackcore/dbus_server.h"
#include "blackcore/network_vatlib.h"
#include "blackcore/coreruntime.h"
#include "blackcore/context_network_interface.h"
#include "blackcore/context_settings_interface.h"
#include "blackmisc/avcallsignlist.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/nwuserlist.h"

#include <QTimer>
#include <QNetworkAccessManager>

#define BLACKCORE_CONTEXTNETWORK_INTERFACENAME "blackcore.contextnetwork"

namespace BlackCore
{
    /*!
     * \brief Network context
     */
    class CContextNetwork : public IContextNetwork
    {
        // Register by same name, make signals sender independent
        // http://dbus.freedesktop.org/doc/dbus-faq.html#idp48032144
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTNETWORK_INTERFACENAME)
        Q_OBJECT

    public:
        /*!
         * \brief With link to server
         */
        CContextNetwork(CCoreRuntime *runtime);

        /*!
         * \brief Destructor
         */
        virtual ~CContextNetwork();

        /*!
         * \brief Register myself in DBus
         * \param server    DBus server
         */
        void registerWithDBus(CDBusServer *server)
        {
            Q_ASSERT(server);
            server->addObject(IContextNetwork::ServicePath(), this);
        }

        /*!
         * \brief Runtime
         * \return
         */
        const CCoreRuntime *getRuntime() const
        {
            return static_cast<CCoreRuntime *>(this->parent());
        }

        /*!
         * \brief Using local objects?
         */
        virtual bool usingLocalObjects() const { return true; }

    public slots:

        /*!
         * \brief Read ATC bookings
         */
        virtual void readAtcBookingsFromSource() const;

        /*!
         * \brief The "central" ATC list with online ATC controllers
         * \todo If I make this &getAtcStations XML is not generated correctly, needs to be crosschecked with the latest version of Qt
         */
        virtual const BlackMisc::Aviation::CAtcStationList getAtcStationsOnline() const
        {
            // this->log(Q_FUNC_INFO);
            return m_atcStationsOnline;
        }

        /*!
         * \brief ATC list, with booked controllers
         * \todo If I make this &getAtcStations XML is not generated correctly
         */
        virtual const BlackMisc::Aviation::CAtcStationList getAtcStationsBooked() const
        {
            // this->log(Q_FUNC_INFO);
            return m_atcStationsBooked;
        }

        /*!
         * \brief Aircraft list
         * \return
         */
        virtual const BlackMisc::Aviation::CAircraftList getAircraftsInRange() const
        {
            // this->log(Q_FUNC_INFO);
            return m_aircraftsInRange;
        }

        /*!
         * \brief Connect to Network
         * \return  a message list showing the connection situation
         */
        virtual BlackMisc::CStatusMessageList connectToNetwork();

        /*!
         * \brief Disconnect from network
         * \return  a message list showing the disconnection situation
         */
        virtual BlackMisc::CStatusMessageList disconnectFromNetwork();

        /*!
         * \brief Network connected?
         */
        virtual bool isConnected() const;

        /*!
         * \copydoc IContextNetwork::setOwnAircraft()
         */
        virtual BlackMisc::CStatusMessageList setOwnAircraft(const BlackMisc::Aviation::CAircraft &aircraft);

        /*!
         * \copydoc IContextNetwork::updateOwnPosition()
         */
        virtual void updateOwnPosition(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude);

        /*!
         * \copydoc IContextNetwork::updateOwnSituation()
         */
        virtual void updateOwnSituation(const BlackMisc::Aviation::CAircraftSituation &situation);

        /*!
         * \copydoc IContextNetwork::updateOwnCockpit()
         */
        virtual void updateOwnCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder);

        /*!
         * \copydoc IContextNetwork::getOwnAircraft()
         */
        virtual BlackMisc::Aviation::CAircraft getOwnAircraft() const;

        /*!
         * \copydoc IContextNetwork::sendTextMessages()
         */
        virtual void sendTextMessages(const BlackMisc::Network::CTextMessageList &textMessages);

        /*!
         * \copydoc IContextNetwork::getMetar()
         */
        virtual BlackMisc::Aviation::CInformationMessage getMetar(const QString &airportIcaoCode);

        /*!
         * \copydoc IContextNetwork::getSelectedVoiceRooms()
         */
        virtual BlackMisc::Voice::CVoiceRoomList getSelectedVoiceRooms() const;

        /*!
         * \copydoc IContextNetwork::getUsers()
         */
        virtual BlackMisc::Network::CUserList getUsers() const;

        /*!
         * \copydoc IContextNetwork::getUsersForCallsigns
         */
        virtual BlackMisc::Network::CUserList getUsersForCallsigns(const BlackMisc::Aviation::CCallsignList &callsigns) const;

    private:
        BlackMisc::Aviation::CAtcStationList m_atcStationsOnline;
        BlackMisc::Aviation::CAtcStationList m_atcStationsBooked;
        BlackMisc::Aviation::CAircraftList m_aircraftsInRange;
        BlackCore::INetwork *m_network;
        BlackMisc::Aviation::CAircraft m_ownAircraft;
        QMap<QString, BlackMisc::Aviation::CInformationMessage> m_metarCache /*!< Keep METARs for a while */;

        // for reading XML
        QNetworkAccessManager *m_networkManager;
        QTimer *m_atcBookingTimer;
        QDateTime m_atcBookingsUpdateTimestamp;

        /*!
         * \brief Replace value by new values
         */
        void setAtcStationsBooked(const BlackMisc::Aviation::CAtcStationList &newStations);

        /*!
         * \brief Replace value by new values
         */
        void setAtcStationsOnline(const BlackMisc::Aviation::CAtcStationList &newStations);

        /*!
         * \brief The "central" ATC list with online ATC controllers
         */
        BlackMisc::Aviation::CAtcStationList &atcStationsOnline()
        {
            return m_atcStationsOnline;
        }

        /*!
         * \brief ATC list, with booked controllers
         */
        BlackMisc::Aviation::CAtcStationList &atcStationsBooked()
        {
            return m_atcStationsBooked;
        }

        /*!
         * \brief Init my very onw aircraft
         */
        void initOwnAircraft();

        /*!
         * \brief Get network settings
         */
        BlackMisc::Settings::CSettingsNetwork getNetworkSettings() const
        {
            Q_ASSERT(this->getRuntime());
            Q_ASSERT(this->getRuntime()->getIContextSettings());
            return this->getRuntime()->getIContextSettings()->getNetworkSettings();
        }

    private slots:
        /*!
         * \brief Connection status changed
         * \param from  old status
         * \param to    new status
         */
        void psFsdConnectionStatusChanged(INetwork::ConnectionStatus from, INetwork::ConnectionStatus to);

        /*!
         * \brief ATC position update
         */
        void psFsdAtcPositionUpdate(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CFrequency &frequency, const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::PhysicalQuantities::CLength &range);

        /*!
         * \brief Controller disconnected
         * \param callsign  callsign of controller
         */
        void psFsdAtcControllerDisconnected(const BlackMisc::Aviation::CCallsign &callsign);

        /*!
         * \brief ATIS received
         */
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

        /*!
         * \brief METAR received
         */
        void psFsdMetarReceived(const QString &metarMessage);

        /*!
         * \brief Realname recevied
         */
        void psFsdNameQueryReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &realname);

        /*!
         * \brief Plane info received
         */
        void psFsdAircraftInfoReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftIcao &icaoData);

        /*!
         * \brief Aircraft position update received
         */
        void psFsdAircraftUpdateReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftSituation &situation, const BlackMisc::Aviation::CTransponder &transponder);

        /*!
         * \brief Pilot disconnected
         */
        void psFsdPilotDisconnected(const BlackMisc::Aviation::CCallsign &callsign);

        /*!
         * \brief Frequency received
         */
        void psFsdFrequencyReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CFrequency &frequency);

        /*!
         * \brief Radio text messages received
         */
        void psFsdTextMessageReceived(const BlackMisc::Network::CTextMessageList &messages);

        /*!
         * \brief Bookings via XML read
         * \todo encapsulate reading from WWW in some class
         */
        void psAtcBookingsRead(QNetworkReply *nwReply);
    };

}

#endif // guard
