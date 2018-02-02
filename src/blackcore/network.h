/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_NETWORK_H
#define BLACKCORE_NETWORK_H

#include "blackcoreexport.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/network/rawfsdmessage.h"
#include "blackmisc/network/serverlist.h"
#include "blackmisc/network/textmessagelist.h"
#include "blackmisc/aviation/informationmessage.h"
#include "blackmisc/aviation/flightplan.h"
#include "blackmisc/aviation/callsignset.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/time.h"

#include <QObject>
#include <QString>
#include <QMap>
#include <QVector>
#include <QMetaEnum>
#include <QList>
#include <QUrl>

namespace BlackCore
{
    /*!
     * Interface for a connection to a multi-user flight simulation and ATC network.
     *
     * The connection can be in one of three essential states: disconnected, connecting, and
     * connected. (There is a fourth state, disconnected due to error, which is a substate of
     * disconnected.) Some slots may only be called when connected, and some may only be called
     * when disconnected; there is a naming convention to highlight this fact using prefixes:
     * "preset" slots are only callable when disconnected, "send" slots are only callable when
     * connected, and "set" slots are callable in any state.
     *
     * Slots with the word "query" in their names are handled asynchronously, with one or more
     * "reply" signals being sent in response to each invokation of a query slot.
     *
     * \warning If an INetwork signal is connected to a slot, and that slot emits a signal
     *          which is connected to an INetwork slot, then at least one of those connections
     *          must be a Qt::QueuedConnection.
     */
    class BLACKCORE_EXPORT INetwork : public QObject
    {
        Q_OBJECT

    protected:
        /*!
         * Constructor
         */
        INetwork(QObject *parent = nullptr) : QObject(parent) {}

    public:
        /*!
         * Destructor.
         */
        virtual ~INetwork() {}

        /*!
         * Underlying library info.
         */
        virtual const QString &getLibraryInfo(bool detailed) const = 0;

        /*!
         * Flags for capabilities bitfield.
         */
        enum
        {
            AcceptsAtisResponses        = 1 << 0,
            SupportsInterimPosUpdates   = 1 << 1,
            SupportsIcaoCodes           = 1 << 2,
            SupportsAircraftConfigs     = 1 << 3
        };

        /*!
         * Login modes
         */
        enum LoginMode
        {
            LoginNormal = 0,    //!< Normal login
            LoginAsObserver,    //!< Login as observer
            LoginStealth        //!< Login stealth mode
        };

        /*!
         * Status of the connection.
         */
        enum ConnectionStatus
        {
            Disconnected = 0,   //!< Not connected
            Disconnecting,      //!< In transition to disconnected
            DisconnectedError,  //!< Disconnected due to socket error
            DisconnectedFailed, //!< A connection was not established due to socket error
            DisconnectedLost,   //!< Connection lost due to socket error
            Connecting,         //!< Connection initiated but not established
            Connected           //!< Connection established
        };
        Q_ENUM(ConnectionStatus)

        /*!
         * Convert a ConnectionStatus to a string.
         */
        static QString connectionStatusToString(ConnectionStatus status)
        {
            const int index = staticMetaObject.indexOfEnumerator("ConnectionStatus");
            const QMetaEnum metaEnum = staticMetaObject.enumerator(index);
            return metaEnum.valueToKey(status);
        }

        /*!
         * Returns true if the given ConnectionStatus represents an error state.
         */
        static bool isErrorStatus(ConnectionStatus status)
        {
            return status == DisconnectedError;
        }

        /*!
         * Returns true if the given ConnectionStatus represents a pending state.
         */
        static bool isPendingStatus(ConnectionStatus status)
        {
            return status == Disconnecting || status == Connecting;
        }

        /*!
         * Returns true if the given ConnectionStatus represents a disconnected state.
         */
        static bool isDisconnectedStatus(ConnectionStatus status)
        {
            return status == Disconnected || status == DisconnectedError ||
                   status == DisconnectedFailed || status == DisconnectedLost;
        }

        /*!
         * Returns true if the given ConnectionStatus represents a connected state.
         */
        static bool isConnectedStatus(ConnectionStatus status)
        {
            return status == Connected;
        }

        ////////////////////////////////////////////////////////////////
        //! \name Network slots
        //! @{
        ////////////////////////////////////////////////////////////////

        /*!
         * Returns true if the current ConnectionStatus is a connected state.
         */
        virtual bool isConnected() const = 0;

        /*!
         * Get preset server.
         */
        virtual BlackMisc::Network::CServer getPresetServer() const = 0;

        /*!
         * Returns true if the current ConnectionStatus is in transition, e.g. connecting.
         */
        virtual bool isPendingConnection() const = 0;

        /*!
         * Set the server which will be connected to.
         * \pre Network must be disconnected when calling this function.
         */
        virtual void presetServer(const BlackMisc::Network::CServer &server) = 0;

        /*!
         * Set our own callsign before connecting.
         * \pre Network must be disconnected when calling this function.
         */
        virtual void presetCallsign(const BlackMisc::Aviation::CCallsign &callsign) = 0;

        /*!
         * Set our own aircraft ICAO codes before connecting.
         * \pre Network must be disconnected when calling this function.
         */
        virtual void presetIcaoCodes(const BlackMisc::Simulation::CSimulatedAircraft &ownAircraft) = 0;

        /*!
         * Select a login mode before connecting.
         * \pre Network must be disconnected when calling this function.
         */
        virtual void presetLoginMode(BlackCore::INetwork::LoginMode mode) = 0;

        /*!
         * Set simulator info before connecting.
         * \pre Network must be disconnected when calling this function.
         */
        virtual void presetSimulatorInfo(const BlackMisc::Simulation::CSimulatorPluginInfo &simInfo) = 0;

        /*!
         * Initiate a connection to the network server.
         * \pre Network must be disconnected when calling this function.
         * \post Connection status changes from Disconnected to either Connecting or DisconnectedError.
         */
        virtual void initiateConnection() = 0;

        /*!
         * Ask the connection to the network server to terminate itself.
         * \pre It is not legal to call this function when already disconnected.
         * \post Connection status changes to Disconnected, but maybe not immediately.
         */
        virtual void terminateConnection() = 0;

        /*!
         * Send a ping message to a user with a specific callsign.
         * \pre Network must be connected when calling this function.
         * \sa pongReceived
         */
        virtual void sendPing(const BlackMisc::Aviation::CCallsign &callsign) = 0;

        /*!
         * Send a message querying the real name of the user with a specific callsign.
         * \pre Network must be connected when calling this function.
         * \sa realNameReplyReceived
         */
        virtual void sendRealNameQuery(const BlackMisc::Aviation::CCallsign &callsign) = 0;

        /*!
         * Send a message querying our own IP address as reported by the server.
         * \pre Network must be connected when calling this function.
         * \sa ipReplyReceived
         */
        virtual void sendIpQuery() = 0;

        /*!
         * Send a message querying which server the user with a specific callsign is connected to.
         * \pre Network must be connected when calling this function.
         * \sa serverReplyReceived
         */
        virtual void sendServerQuery(const BlackMisc::Aviation::CCallsign &callsign) = 0;

        /*!
         * Send one or more text messages.
         * \pre Network must be connected when calling this function.
         */
        virtual void sendTextMessages(const BlackMisc::Network::CTextMessageList &messages) = 0;

        //! @}
        ////////////////////////////////////////////////////////////////
        //! \name Custom packets
        //! @{
        ////////////////////////////////////////////////////////////////

        /*!
         * Send a FSInn custom packet.
         * \details FSIPI(R) queries, some example data below:
         * <BLOCKQUOTE>
         * index 0 .. 0/1 ???
         *       1 .. MQT, GEC, DLH -> Airline ICAO, most of the time empty
         *       2 .. AIRCRAFT ICAO "B747"
         *       3 .. 10.64195
         *       4 .. 1.06080:
         *       5 .. 5825.00000
         *       6 .. 4.DD89CCB6.EC9BB7D7
         *       7 .. 3-letter combined L2J
         *       8 .. Model string
         * </BLOCKQUOTE>
         */
        //! @{
        virtual void sendCustomFsinnQuery(const BlackMisc::Aviation::CCallsign &callsign) = 0;
        virtual void sendCustomFsinnReponse(const BlackMisc::Aviation::CCallsign &callsign) = 0;
        //! @}

        //! Broadcast an incremental aircraft config
        virtual void broadcastAircraftConfig(const QJsonObject &config) = 0;

        //! Query callsign for its current full aircraft config
        virtual void sendAircraftConfigQuery(const BlackMisc::Aviation::CCallsign &callsign) = 0;

        //! @}
        ////////////////////////////////////////////////////////////////
        //! \name ATC slots
        //! @{
        ////////////////////////////////////////////////////////////////

        /*!
         * Send a message querying whether or not the user with a specific callsign is an ATC station.
         * \pre Network must be connected when calling this function.
         * \sa atcReplyReceived
         */
        virtual void sendAtcQuery(const BlackMisc::Aviation::CCallsign &callsign) = 0;

        /*!
         * Send a message querying the ATIS for the ATC station with a specific callsign.
         * \pre Network must be connected when calling this function.
         * \sa atisReplyReceived
         * \sa atisVoiceRoomReplyReceived
         * \sa atisLogoffTimeReplyReceived
         */
        virtual void sendAtisQuery(const BlackMisc::Aviation::CCallsign &callsign) = 0;

        /*!
         * Send a message to file a flight plan.
         * \pre Network must be connected when calling this function.
         */
        virtual void sendFlightPlan(const BlackMisc::Aviation::CFlightPlan &flightPlan) = 0;

        /*!
         * Send a message querying our currently filed flight plan, which may have been amended by a controller.
         * \pre Network must be connected when calling this function.
         * \sa flightPlanReplyReceived
         */
        virtual void sendFlightPlanQuery(const BlackMisc::Aviation::CCallsign &callsign) = 0;

        //! @}
        ////////////////////////////////////////////////////////////////
        //! \name Aircraft slots
        //! @{
        ////////////////////////////////////////////////////////////////

        /*!
         * Send a message querying the capabilities of the client software of the user with a specific callsign.
         * \pre Network must be connected when calling this function.
         * \sa capabilitiesReplyReceived
         */
        virtual void sendCapabilitiesQuery(const BlackMisc::Aviation::CCallsign &callsign) = 0;

        /*!
         * Send a message querying the ICAO codes of the aircraft of the user with a specific callsign.
         * \pre Network must be connected when calling this function.
         * \sa icaoCodesReplyReceived
         */
        virtual void sendIcaoCodesQuery(const BlackMisc::Aviation::CCallsign &callsign) = 0;

        /*!
         * Send a message querying the COM frequency of the user with a specific callsign.
         * \pre Network must be connected when calling this function.
         * \sa frequencyReplyReceived
         */
        virtual void sendFrequencyQuery(const BlackMisc::Aviation::CCallsign &callsign) = 0;

        /*!
         * Send a user info query, which only shall be sent by supervisiors. Reply is received as
         * text message
         * \pre Network must be connected when calling this function.
         */
        virtual void sendUserInfoQuery(const BlackMisc::Aviation::CCallsign &callsign) = 0;

        /*!
         * Set the group of callsigns receiving regular interim position updates.
         */
        virtual void setInterimPositionReceivers(const BlackMisc::Aviation::CCallsignSet &receiver) = 0;

        /*!
         * Get the group of callsigns receiving regular interim position updates.
         */
        virtual const BlackMisc::Aviation::CCallsignSet &getInterimPositionReceivers() const = 0;

        //! @}
        ////////////////////////////////////////////////////////////////
        //! \name Weather slots
        //! @{
        ////////////////////////////////////////////////////////////////

        /*!
         * Send a message querying the METAR for the airport with a specific ICAO code.
         * \pre Network must be connected when calling this function.
         * \sa metarReplyReceived
         */
        virtual void sendMetarQuery(const BlackMisc::Aviation::CAirportIcaoCode &airportIcao) = 0;

    signals:
        //! @}
        ////////////////////////////////////////////////////////////////
        //! \name ATC signals
        //! @{
        ////////////////////////////////////////////////////////////////

        /*!
         * We received a notification of the state of an ATC station on the network.
         */
        void atcPositionUpdate(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CFrequency &freq,
                               const BlackMisc::Geo::CCoordinateGeodetic &pos, const BlackMisc::PhysicalQuantities::CLength &range);

        /*!
         * We received a notification that an ATC station has disconnected from the network.
         */
        void atcDisconnected(const BlackMisc::Aviation::CCallsign &callsign);

        /*!
         * We received a reply to one of our queries.
         * \sa sendAtcQuery
         */
        void atcReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, bool isATC);

        /*!
         * We received a reply to one of our ATIS queries.
         * \sa sendAtisQuery
         */
        void atisReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CInformationMessage &atis);

        /*!
         * We received a reply to one of our ATIS queries, containing the controller's voice room URL.
         * \sa sendAtisQuery
         */
        void atisVoiceRoomReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &url);

        /*!
         * We received a reply to one of our ATIS queries, containing the controller's planned logoff time.
         * \sa sendAtisQuery
         */
        void atisLogoffTimeReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &zuluTime);

        /*!
         * We received a reply to one of our flight plan queries, containing our up-to-date flight plan.
         * \sa sendFlightPlanQuery
         */
        void flightPlanReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CFlightPlan &flightPlan);

        //! @}
        ////////////////////////////////////////////////////////////////
        //! \name Aircraft signals
        //! @{
        ////////////////////////////////////////////////////////////////

        /*!
         * We received a notification that a pilot has disconnected from the network.
         */
        void pilotDisconnected(const BlackMisc::Aviation::CCallsign &callsign);

        /*!
         * We received a reply to one of our queries.
         * \sa sendIcaoCodesQuery
         */
        void icaoCodesReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &aircraftIcaoDesignator, const QString &airlineIcaoDesignator, const QString &livery);

        /*!
         * We received a notification of the state of another aircraft on the network.
         * Corresponding callsign in \sa CAircraftSituation::getCallsign .
         */
        void aircraftPositionUpdate(const BlackMisc::Aviation::CAircraftSituation &situation,
                                    const BlackMisc::Aviation::CTransponder &transponder);

        /*!
         * We received a interim notification of the state of another aircraft on the network.
         * Corresponding callsign in \sa CAircraftSituation::getCallsign .
         */
        void aircraftInterimPositionUpdate(const BlackMisc::Aviation::CAircraftSituation &situation);

        /*!
         * We received a reply to one of our queries.
         * \sa sendFrequencyQuery
         */
        void frequencyReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CFrequency &freq);

        //! @}
        ////////////////////////////////////////////////////////////////
        //! \name Network signals
        //! @{
        ////////////////////////////////////////////////////////////////

        /*!
         * We were kicked from the network. (A connectionStatusChanged signal will also be sent.)
         */
        void kicked(const QString &msg);

        /*!
         * The status of our connection has changed.
         */
        void connectionStatusChanged(BlackCore::INetwork::ConnectionStatus oldStatus, BlackCore::INetwork::ConnectionStatus newStatus);

        /*!
         * We received a reply to one of our pings.
         * \sa sendPing
         */
        void pongReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CTime &elapsedTime);

        /*!
         * We received a reply to one of our queries.
         * \sa sendCapabilitiesQuery
         */
        void capabilitiesReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, int capabilitiesFlags);

        /*!
         * We received a reply to one of our queries.
         * \param ip Our IP address, as seen by the server.
         * \sa sendIpQuery
         */
        void ipReplyReceived(const QString &ip);

        /*!
         * We received a reply to one of our queries.
         * \sa sendServerQuery
         */
        void serverReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &server);

        /*!
         * We received a reply to one of our queries.
         * \sa sendRealNameQuery
         */
        void realNameReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &realname);

        /*!
         * We received one or more text messages from another user.
         */
        void textMessagesReceived(const BlackMisc::Network::CTextMessageList &messages);

        /*!
         * We have sent a text message.
         */
        void textMessageSent(const BlackMisc::Network::CTextMessage &sentMessage);

        /*!
         * We received a FSInn custom packet.
         */
        void customFSInnPacketReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &airlineDesignator,
                                       const QString &aircraftDesignator, const QString &combinedType, const QString &modelString);

        /*!
         * We received an aircraft config packet.
         */
        void aircraftConfigPacketReceived(const BlackMisc::Aviation::CCallsign &callsign, const QJsonObject &incremental, int currentOffsetTime);

        /*!
         * We received a raw message for debugging purposes
         */
        void rawFsdMessageReceived(const BlackMisc::Network::CRawFsdMessage &rawFsdMessage);

        //! @}
        ////////////////////////////////////////////////////////////////
        //! \name Weather signals
        //! @{
        ////////////////////////////////////////////////////////////////

        /*!
         * We received a reply to one of our METAR queries.
         * \sa sendMetarQuery
         */
        void metarReplyReceived(const QString &data);

        //! @}
    };
} // namespace

Q_DECLARE_METATYPE(BlackCore::INetwork::ConnectionStatus)
Q_DECLARE_METATYPE(BlackCore::INetwork::LoginMode)

#endif // guard
