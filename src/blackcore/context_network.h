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
#include "blackmisc/avallclasses.h"
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
         * \param server
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
         * \return
         */
        // If I make this &getAtcStations XML is not generated correctly
        // needs to be crosschecked with the latest version of Qt
        virtual const BlackMisc::Aviation::CAtcStationList getAtcStationsOnline() const
        {
            // this->log(Q_FUNC_INFO);
            return m_atcStationsOnline;
        }

        /*!
         * \brief ATC list, with booked controllers
         * \return
         */
        // If I make this &getAtcStations XML is not generated correctly
        virtual const BlackMisc::Aviation::CAtcStationList getAtcStationsBooked() const
        {
            // this->log(Q_FUNC_INFO);
            return m_atcStationsBooked;
        }

        /*!
         * \brief Aircraft list
         * \return
         */
        // If I make this &getAtcStations XML is not generated correctly
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
         * \brief Set own aircraft
         * \param aircraft my own aircraft
         * \return
         */
        virtual BlackMisc::CStatusMessageList setOwnAircraft(const BlackMisc::Aviation::CAircraft &aircraft);

        /*!
         * \brief Update own position
         * \param position  own position
         * \param altitude  own altitude
         */
        virtual void updateOwnPosition(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude);

        /*!
         * \brief Update own situation
         * \param situation own situation
         */
        virtual void updateOwnSituation(const BlackMisc::Aviation::CAircraftSituation &situation);

        /*!
         * \brief Update own cockpit
         * \param com1  my COM1 unit
         * \param com2  my COM2 unit
         * \param transponder   my transponder
         */
        virtual void updateOwnCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder);

        /*!
         * \brief Get own aircraft
         */
        virtual BlackMisc::Aviation::CAircraft getOwnAircraft() const;

        /*!
         * \brief Text messages (also private chat messages)
         */
        virtual void sendTextMessages(const BlackMisc::Network::CTextMessageList &textMessages);

        /*!
         * \brief Request METAR
         * \param airportIcaoCode   e.g. EDDF, KLAX
         */
        virtual BlackMisc::Aviation::CInformationMessage getMetar(const QString &airportIcaoCode);

        /*!
         * \brief Selected COM1/2 frequencies as voice rooms
         * \return  COM1/2 voice rooms
         */
        virtual BlackMisc::Voice::CVoiceRoomList getSelectedVoiceRooms() const;

        /*!
         * \brief Get all users
         */
        virtual BlackMisc::Network::CUserList getUsers() const;

        /*!
         * \brief All users with callsign, e.g. for voice room resolution
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
