/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_CONTEXTNETWORK_PROXY_H
#define BLACKCORE_CONTEXTNETWORK_PROXY_H

#include "context_network.h"
#include "network_vatlib.h"

#include "blackmisc/avallclasses.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/nwtextmessagelist.h"
#include "blackmisc/nwuserlist.h"
#include "blackmisc/genericdbusinterface.h"
#include "blackmisc/voiceroomlist.h"

namespace BlackCore
{

    //! \brief Network context proxy
    class CContextNetworkProxy : public IContextNetwork
    {
        Q_OBJECT

    public:

        /*!
         * \brief DBus version constructor
         * \param serviceName
         * \param connection
         * \param parent
         */
        CContextNetworkProxy(const QString &serviceName, QDBusConnection &connection, QObject *parent = nullptr);

        //! Destructor
        virtual ~CContextNetworkProxy() {}

        //! \copydoc IContextNetwork::usingLocalObjects()
        virtual bool usingLocalObjects() const override { return false; }

    private:
        BlackMisc::CGenericDBusInterface *m_dBusInterface; /*!< DBus interface */

        //! \brief Relay connection signals to local signals.
        void relaySignals(const QString &serviceName, QDBusConnection &connection);

    protected:
        //! \brief Constructor
        CContextNetworkProxy(QObject *parent = nullptr) : IContextNetwork(parent), m_dBusInterface(nullptr) {}

        /*!
         * \brief Helper for logging, likely to be removed / changed
         * \param method
         * \param m1
         * \param m2
         * \param m3
         * \param m4
         */
        void log(const QString &method, const QString &m1 = "", const QString &m2 = "", const QString &m3 = "", const QString &m4 = "") const;

    public slots: // IContextNetwork overrides

        //! \copydoc IContextNetwork::readAtcBookingsFromSource()
        virtual void readAtcBookingsFromSource() const override;

        /*!
         * \copydoc IContextNetwork::getAtcStationsOnline()
         * \todo If I make this &getAtcStations XML is not generated correctly, needs to be crosschecked with the latest version of Qt
         */
        virtual const BlackMisc::Aviation::CAtcStationList getAtcStationsOnline() const override;

        /*!
         * \copydoc IContextNetwork::getAtcStationsBooked()
         * \todo If I make this &getAtcStations XML is not generated correctly
         */
        virtual const BlackMisc::Aviation::CAtcStationList getAtcStationsBooked() const override;

        //! \copydoc IContextNetwork::getAircraftsInRange()
        virtual const BlackMisc::Aviation::CAircraftList getAircraftsInRange() const override;

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
    };
}

#endif // guard
