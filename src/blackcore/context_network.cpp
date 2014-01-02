/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "context_network.h"
#include "coreruntime.h"
#include "blackmisc/avatcstationlist.h"
#include <QtXml/QDomElement>

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Geo;

namespace BlackCore
{

    /*
     * Init this context
     */
    CContextNetwork::CContextNetwork(CCoreRuntime *parent) :
        IContextNetwork(parent),
        m_atcStationsOnline(), m_atcStationsBooked(), m_aircraftsInRange(),
        m_network(nullptr), m_ownAircraft(),
        m_metarCache()
    {

        // 1. Init by "network driver"
        this->m_network = new NetworkVatlib(this);

        // 2. Init own aircraft
        this->initOwnAircraft();

        // 3. Init network access driver for XML data (bookings)
        this->m_networkManager = new QNetworkAccessManager(this);
        this->connect(this->m_networkManager, SIGNAL(finished(QNetworkReply *)), this, SLOT(psAtcBookingsRead(QNetworkReply *)));
        this->m_atcBookingTimer = new QTimer(this);
        this->connect(this->m_atcBookingTimer, SIGNAL(timeout()), this, SLOT(readAtcBookingsFromSource()));
        this->m_atcBookingTimer->start(15 * 1000);

        // 4. connect signals and slots
        bool connect = this->connect(this->m_network, SIGNAL(connectionStatusChanged(Cvatlib_Network::connStatus, Cvatlib_Network::connStatus)),
                                     this, SLOT(psFsdConnectionStatusChanged(Cvatlib_Network::connStatus, Cvatlib_Network::connStatus)));
        Q_ASSERT_X(connect, "CContextNetwork", "Cannot connect connectionStatusChanged");

        connect = this->connect(this->m_network, SIGNAL(terminate()),
                                this, SLOT(psFsdConnectionTerminated()));
        Q_ASSERT_X(connect, "CContextNetwork", "Cannot connect terminate");

        connect = this->connect(this->m_network, SIGNAL(atcPositionUpdate(BlackMisc::Aviation::CCallsign, BlackMisc::PhysicalQuantities::CFrequency, BlackMisc::Geo::CCoordinateGeodetic, BlackMisc::PhysicalQuantities::CLength)),
                                this, SLOT(psFsdAtcPositionUpdate(BlackMisc::Aviation::CCallsign, BlackMisc::PhysicalQuantities::CFrequency, BlackMisc::Geo::CCoordinateGeodetic, BlackMisc::PhysicalQuantities::CLength)));
        Q_ASSERT_X(connect, "CContextNetwork", "Cannot connect atcPositionUpdate");

        connect = this->connect(this->m_network, SIGNAL(atisQueryReplyReceived(BlackMisc::Aviation::CCallsign, QString)),
                                this, SLOT(psFsdAtisQueryReceived(BlackMisc::Aviation::CCallsign, QString)));
        Q_ASSERT_X(connect, "CContextNetwork", "Cannot connect atis");

        connect = this->connect(this->m_network, SIGNAL(metarReceived(QString)),
                                this, SLOT(psFsdMetarReceived(QString)));
        Q_ASSERT_X(connect, "CContextNetwork", "Cannot connect metar");

        connect = this->connect(this->m_network, SIGNAL(nameQueryReplyReceived(BlackMisc::Aviation::CCallsign, QString)),
                                this, SLOT(psFsdNameQueryReplyReceived(BlackMisc::Aviation::CCallsign, QString)));
        Q_ASSERT_X(connect, "CContextNetwork", "Cannot connect name reply");

        connect = this->connect(this->m_network, SIGNAL(exception(QString, bool)),
                                this, SLOT(psVatlibExceptionMessage(QString, bool)));
        Q_ASSERT_X(connect, "CContextNetwork", "Cannot connect exception");

        connect = this->connect(this->m_network, SIGNAL(aircraftInfoReceived(BlackMisc::Aviation::CCallsign, BlackMisc::Aviation::CAircraftIcao)),
                                this, SLOT(psFsdAircraftInfoReceived(BlackMisc::Aviation::CCallsign, BlackMisc::Aviation::CAircraftIcao)));
        Q_ASSERT_X(connect, "CContextNetwork", "Cannot connect aircraft info");

        connect = this->connect(this->m_network, SIGNAL(pilotDisconnected(BlackMisc::Aviation::CCallsign)),
                                this, SLOT(psFsdPilotDisconnected(BlackMisc::Aviation::CCallsign)));
        Q_ASSERT_X(connect, "CContextNetwork", "Cannot connect pilot disconnected");

        connect = this->connect(this->m_network, SIGNAL(aircraftPositionUpdate(BlackMisc::Aviation::CCallsign, BlackMisc::Aviation::CAircraftSituation, BlackMisc::Aviation::CTransponder)),
                                this, SLOT(psFsdAircraftPositionUpdate(BlackMisc::Aviation::CCallsign, BlackMisc::Aviation::CAircraftSituation, BlackMisc::Aviation::CTransponder)));
        Q_ASSERT_X(connect, "CContextNetwork", "Cannot connect aircraft position update");

        connect = this->connect(this->m_network, SIGNAL(frequencyQueryReplyReceived(BlackMisc::Aviation::CCallsign, BlackMisc::PhysicalQuantities::CFrequency)),
                                this, SLOT(psFsdFrequencyReceived(BlackMisc::Aviation::CCallsign, BlackMisc::PhysicalQuantities::CFrequency)));
        Q_ASSERT_X(connect, "CContextNetwork", "Cannot connect frequency update");

        connect = this->connect(this->m_network, SIGNAL(textMessagesReceived(BlackMisc::Network::CTextMessageList)),
                                this, SLOT(psFsdTextMessageReceived(BlackMisc::Network::CTextMessageList)));
        Q_ASSERT_X(connect, "CContextNetwork", "Cannot connect text message");


        // relay status message
        connect = this->connect(this->m_network, SIGNAL(statusMessage(BlackMisc::CStatusMessage)),
                                this, SIGNAL(statusMessage(BlackMisc::CStatusMessage)));
        Q_ASSERT_X(connect, "CContextNetwork", "Cannot connect status message");
    }

    /*
     * Cleanup
     */
    CContextNetwork::~CContextNetwork()
    {
        if (this->isConnected()) this->disconnectFromNetwork();
        this->disconnect(this);
        this->disconnect(this->m_network);
    }

    /*
     * Init own aircraft
     */
    void CContextNetwork::initOwnAircraft()
    {
        Q_ASSERT(this->getRuntime());
        Q_ASSERT(this->getRuntime()->getIContextSettings());
        this->m_ownAircraft.initComSystems();
        this->m_ownAircraft.initTransponder();
        CAircraftSituation situation(
            CCoordinateGeodetic(
                CLatitude::fromWgs84("N 049° 18' 17"),
                CLongitude::fromWgs84("E 008° 27' 05"),
                CLength(0, CLengthUnit::m())),
            CAltitude(312, CAltitude::MeanSeaLevel, CLengthUnit::ft())
        );
        this->m_ownAircraft.setSituation(situation);
        this->m_ownAircraft.setPilot(this->getRuntime()->getIContextSettings()->getNetworkSettings().getCurrentNetworkServer().getUser());

        // TODO: This would need to come from somewhere (mappings)
        // Own callsign, plane ICAO status, model used
        this->m_ownAircraft.setCallsign(CCallsign("BLACK"));
        this->m_ownAircraft.setIcaoInfo(CAircraftIcao("C172", "L1P", "GA", "GA", "0000ff"));
    }

    /*
     * Connect to network
     */
    CStatusMessages CContextNetwork::connectToNetwork()
    {
        qDebug() << Q_FUNC_INFO;

        CStatusMessages msgs;
        CServer currentServer = this->getRuntime()->getIContextSettings()->getNetworkSettings().getCurrentNetworkServer();

        if (!currentServer.getUser().isValid())
        {
            msgs.append(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityWarning, "Invalid user credentials"));
        }
        //else if (!this->m_network->isDisconnected())
        //{
        //    msgs.append(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityWarning, "Already connected"));
        //}
        else
        {
            this->m_ownAircraft.setPilot(currentServer.getUser());
            this->m_network->setServer(currentServer);
            //this->m_network->setOwnAircraft(this->m_ownAircraft);
            this->m_network->initiateConnection();
            QString msg = "Connection pending ";
            msg.append(" ").append(currentServer.getAddress()).append(" ").append(QString::number(currentServer.getPort()));
            msgs.append(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityInfo, msg));
        }
        return msgs;
    }

    /*
     * Disconnect from network
     */
    CStatusMessages CContextNetwork::disconnectFromNetwork()
    {
        qDebug() << Q_FUNC_INFO;

        CStatusMessages msgs;
        //if (this->m_network->isDisconnected())
        //{
        //    msgs.append(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityWarning, "Already disconnected"));
        //}
        //else
        {
            this->m_network->terminateConnection();
            this->m_aircraftsInRange.clear();
            this->m_atcStationsBooked.clear();
            this->m_atcStationsOnline.clear();
            this->m_metarCache.clear();
            msgs.append(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityInfo, "Connection terminating"));
        }
        return msgs;
    }

    /*
     * Connected
     */
    bool CContextNetwork::isConnected() const
    {
        return false;
    }

    /*
     * Own's plane position update
     */
    void CContextNetwork::sendOwnAircraftCheckedPositionUpdateToNetwork() const
    {
        // TODO: Would this logic go into network_vatlib?
        // 1. Check not sending to many updates
        // 2. Send as pull update to intermediate update
        // 3. Same position, no update?
        // ...

        // this->log(Q_FUNC_INFO, this->m_ownAircraft.toQString());
        // this->m_network->sendAircraftUpdate(this->m_ownAircraft);
    }

    /*
     * Own Aircraft
     */
    CStatusMessages CContextNetwork::setOwnAircraft(const BlackMisc::Aviation::CAircraft &aircraft)
    {
        this->log(Q_FUNC_INFO, aircraft.toQString());
        CStatusMessages msgs;
        //if (this->m_network->isDisconnected())
        //{
        //    this->m_ownAircraft = aircraft;
        //}
        //else
        {
            msgs.append(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityError, "Cannot set plane info, network already connected"));
        }
        return msgs;
    }

    /*
     * Own Aircraft details
     */
    CStatusMessages CContextNetwork::sendOwnAircraftDetails()
    {
#if 0
        CStatusMessages msgs;
        if (this->m_network->isDisconnected() && false)
        {
            msgs.append(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityError, "Cannot send plane info, network not connected"));

        }
        else if (!this->m_ownAircraft.isValidForLogin())
        {
            msgs.append(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityError, "Cannot send plane info, own aircraft is invalid"));
        }
        else
        {
            // TODO: really sending plane info ??
            // NOPE: this->m_network->sendAircraftInfo(this->m_ownAircraft);
            this->sendOwnAircraftCheckedPositionUpdateToNetwork();
            QString m("Aircraft send ");
            m.append(this->m_ownAircraft.toQString(true));
            msgs.append(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityInfo, m));
        }
        return msgs;
#endif // 0
        return CStatusMessages();
    }

    /*
     * Own position
     */
    void CContextNetwork::updateOwnPosition(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude)
    {
        if (position == this->m_ownAircraft.getPosition() && altitude == this->m_ownAircraft.getAltitude()) return;
        this->m_ownAircraft.setPosition(position);
        this->m_ownAircraft.setAltitude(altitude);
        this->sendOwnAircraftCheckedPositionUpdateToNetwork();
    }

    /*
     * Update own situation
     */
    void CContextNetwork::updateOwnSituation(const BlackMisc::Aviation::CAircraftSituation &situation)
    {
        if (situation == this->m_ownAircraft.getSituation()) return;
        this->m_ownAircraft.setSituation(situation);
        this->sendOwnAircraftCheckedPositionUpdateToNetwork();
    }

    /*
     * Own cockpit data
     */
    void CContextNetwork::updateOwnCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder)
    {
        bool changed = false;
        if (com1 != this->m_ownAircraft.getCom1System())
        {
            this->m_ownAircraft.setCom1System(com1);
            changed = true;
        }
        if (com2 != this->m_ownAircraft.getCom2System())
        {
            this->m_ownAircraft.setCom2System(com2);
            changed = true;
        }
        if (transponder != this->m_ownAircraft.getTransponder())
        {
            this->m_ownAircraft.setTransponder(transponder);
            changed = true;
        }

        if (!changed) return;
        //this->m_network->updateOwnCockpit(com1, com2, transponder);
    }

    /*
     * Own aircraft
     */
    CAircraft CContextNetwork::getOwnAircraft() const
    {
        this->log(Q_FUNC_INFO, this->m_ownAircraft.toQString());
        return this->m_ownAircraft;
    }

    /*
     * Send text messages
     */
    void CContextNetwork::sendTextMessages(const CTextMessageList &textMessages)
    {
        this->log(Q_FUNC_INFO, textMessages.toQString());
        this->m_network->sendTextMessages(textMessages);
    }

    /*
     * Connection terminated
     */
    void CContextNetwork::psFsdConnectionTerminated()
    {
        emit this->statusMessage(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityInfo, "connection terminated"));
        emit this->connectionTerminated();
    }

    /*
     * Connection status changed
     */
    void CContextNetwork::psFsdConnectionStatusChanged(Cvatlib_Network::connStatus from, Cvatlib_Network::connStatus to)
    {
        ConnectionStatus fromCs = static_cast<ConnectionStatus>(from);
        ConnectionStatus toCs = static_cast<ConnectionStatus>(to);
        CStatusMessages msgs;


        // send 1st position
        if (toCs == CContextNetwork::ConnectionStatusConnected)
        {
            msgs = this->sendOwnAircraftDetails();
        }

        // send as message
        QString m("connection status changed ");
        m.append(this->connectionStatusToString(fromCs)).
        append(" ").
        append(this->connectionStatusToString(toCs));
        qDebug() << m;
        msgs.append(CStatusMessage(CStatusMessage::TypeTrafficNetwork,
                                   toCs == ConnectionStatusError ? CStatusMessage::SeverityError : CStatusMessage::SeverityInfo, m));
        emit this->statusMessage(msgs.at(0));

        // send as own signal
        emit this->connectionStatusChanged(fromCs, toCs);
    }

    /*
     * Name query
     */
    void CContextNetwork::psFsdNameQueryReplyReceived(const CCallsign &callsign, const QString &realname)
    {
        this->log(Q_FUNC_INFO, callsign.toQString(), realname);
        if (realname.isEmpty()) return;
        CValueMap vm(CAtcStation::IndexControllerRealname, realname);
        this->m_atcStationsOnline.applyIf(&CAtcStation::getCallsign, callsign, vm);
        this->m_atcStationsBooked.applyIf(&CAtcStation::getCallsign, callsign, vm);

        vm = CValueMap(CAircraft::IndexPilotRealname, realname);
        this->m_aircraftsInRange.applyIf(&CAircraft::getCallsign, callsign, vm);
    }


    /*
     * Ping, is DBus alive?
     */
    qint64 CContextNetwork::ping(qint64 token) const
    {
        return token;
    }

    /*
     * Exception to status message
     */
    void CContextNetwork::psVatlibExceptionMessage(const QString &message, bool fatal)
    {
        CStatusMessage msg(CStatusMessage::TypeTrafficNetwork,
                           fatal ? CStatusMessage::SeverityError : CStatusMessage::SeverityWarning, message);
        emit this->statusMessage(msg);
    }

} // namespace
