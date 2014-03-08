/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "context_network_impl.h"
#include "coreruntime.h"
#include "context_settings.h"
#include "network_vatlib.h"
#include "vatsimbookingreader.h"
#include "vatsimdatafilereader.h"

#include "blackmisc/networkchecks.h"
#include "blackmisc/avatcstationlist.h"

#include <QtXml/QDomElement>
#include <QNetworkReply>

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
    CContextNetwork::CContextNetwork(QObject *parent) :
        IContextNetwork(parent), m_network(nullptr), m_vatsimBookingReader(nullptr), m_vatsimDataFileReader(nullptr), m_dataUpdateTimer(nullptr)
    {
        Q_ASSERT(this->getRuntime());
        Q_ASSERT(this->getRuntime()->getIContextSettings());

        // 1. Init by "network driver"
        this->m_network = new CNetworkVatlib(this);

        // 2. Init own aircraft
        this->initOwnAircraft();

        // 3. Init VATSIM bookings
        this->m_vatsimBookingReader = new CVatsimBookingReader(this->getRuntime()->getIContextSettings()->getNetworkSettings().getBookingServiceUrl(), this);
        this->connect(this->m_vatsimBookingReader, &CVatsimBookingReader::dataRead, this, &CContextNetwork::psReceivedBookings);
        this->m_vatsimBookingReader->setInterval(10 * 1000); // first read

        // 4. VATSIM data file
        QStringList dataFileUrls;
        dataFileUrls << "http://info.vroute.net/vatsim-data.txt";
        this->m_vatsimDataFileReader = new CVatsimDataFileReader(dataFileUrls, this);
        this->connect(this->m_vatsimDataFileReader, &CVatsimDataFileReader::dataRead, this, &CContextNetwork::psDataFileRead);
        this->m_vatsimDataFileReader->setInterval(5 * 1000); // first read

        // 5. Update timer for data
        this->m_dataUpdateTimer = new QTimer(this);
        this->connect(this->m_dataUpdateTimer, &QTimer::timeout, this, &CContextNetwork::requestDataUpdates);
        this->m_dataUpdateTimer->start(30 * 1000);

        // 6. connect signals and slots
        this->connect(this->m_network, &INetwork::connectionStatusChanged, this, &CContextNetwork::psFsdConnectionStatusChanged);
        this->connect(this->m_network, &INetwork::atcPositionUpdate, this, &CContextNetwork::psFsdAtcPositionUpdate);
        this->connect(this->m_network, &INetwork::atisReplyReceived, this, &CContextNetwork::psFsdAtisQueryReceived);
        this->connect(this->m_network, &INetwork::atisVoiceRoomReplyReceived, this, &CContextNetwork::psFsdAtisVoiceRoomQueryReceived);
        this->connect(this->m_network, &INetwork::atisLogoffTimeReplyReceived, this, &CContextNetwork::psFsdAtisLogoffTimeQueryReceived);
        this->connect(this->m_network, &INetwork::metarReplyReceived, this, &CContextNetwork::psFsdMetarReceived);
        this->connect(this->m_network, &INetwork::realNameReplyReceived, this, &CContextNetwork::psFsdRealNameReplyReceived);
        this->connect(this->m_network, &INetwork::icaoCodesReplyReceived, this, &CContextNetwork::psFsdIcaoCodesReceived);
        this->connect(this->m_network, &INetwork::pilotDisconnected, this, &CContextNetwork::psFsdPilotDisconnected);
        this->connect(this->m_network, &INetwork::atcDisconnected, this, &CContextNetwork::psFsdAtcControllerDisconnected);
        this->connect(this->m_network, &INetwork::aircraftPositionUpdate, this, &CContextNetwork::psFsdAircraftUpdateReceived);
        this->connect(this->m_network, &INetwork::frequencyReplyReceived, this, &CContextNetwork::psFsdFrequencyReceived);
        this->connect(this->m_network, &INetwork::textMessagesReceived, this, &CContextNetwork::psFsdTextMessageReceived);
        this->connect(this->m_network, &INetwork::statusMessage, this, &CContextNetwork::statusMessage);
    }

    /*
     * Cleanup
     */
    CContextNetwork::~CContextNetwork()
    {
        if (this->isConnected()) this->disconnectFromNetwork();
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
    CStatusMessageList CContextNetwork::connectToNetwork(uint loginMode)
    {
        // this->log(Q_FUNC_INFO);
        CStatusMessageList msgs;
        CServer currentServer = this->getRuntime()->getIContextSettings()->getNetworkSettings().getCurrentNetworkServer();

        if (!currentServer.getUser().isValid())
        {
            msgs.push_back(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityWarning, "Invalid user credentials"));
        }
        else if (this->m_network->isConnected())
        {
            msgs.push_back(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityWarning, "Already connected"));
        }
        else
        {
            QString msg;
            if (CNetworkChecks::canConnect(currentServer, msg, 2000))
            {
                INetwork::LoginMode mode = static_cast<INetwork::LoginMode>(loginMode);
                this->m_ownAircraft.setPilot(currentServer.getUser()); // still needed?
                this->m_network->presetServer(currentServer);
                this->m_network->presetLoginMode(mode);
                this->m_network->presetCallsign(this->m_ownAircraft.getCallsign());
                this->m_network->presetIcaoCodes(this->m_ownAircraft.getIcaoInfo());
                this->m_network->setOwnAircraft(this->m_ownAircraft);
                this->m_network->initiateConnection();
                msg = "Connection pending ";
                msg.append(" ").append(currentServer.getAddress()).append(" ").append(QString::number(currentServer.getPort()));
                msgs.push_back(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityInfo, msg));
            }
            else
            {
                // I cannot connect, add error message
                msgs.push_back(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityError, msg));
            }
        }
        return msgs;
    }

    /*
     * Disconnect from network
     */
    CStatusMessageList CContextNetwork::disconnectFromNetwork()
    {
        // this->log(Q_FUNC_INFO);
        CStatusMessageList msgs;
        if (this->m_network->isConnected())
        {
            this->m_network->terminateConnection();
            this->m_aircraftsInRange.clear();
            this->m_atcStationsBooked.clear();
            this->m_atcStationsOnline.clear();
            this->m_metarCache.clear();
            msgs.push_back(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityInfo, "Connection terminating"));
        }
        else
        {
            msgs.push_back(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityWarning, "Already disconnected"));
        }
        return msgs;
    }

    /*
     * Connected
     */
    bool CContextNetwork::isConnected() const
    {
        return this->m_network->isConnected();
    }

    /*
     * Own Aircraft
     */
    CStatusMessageList CContextNetwork::setOwnAircraft(const BlackMisc::Aviation::CAircraft &aircraft)
    {
        // this->log(Q_FUNC_INFO, aircraft.toQString());
        CStatusMessageList msgs;
        if (this->m_network->isConnected())
        {
            msgs.push_back(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityError, "Cannot set aircraft info, network already connected"));
        }
        else
        {
            this->m_ownAircraft = aircraft;
        }
        return msgs;
    }

    /*
     * Own position
     */
    void CContextNetwork::updateOwnPosition(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude)
    {
        this->m_ownAircraft.setPosition(position);
        this->m_ownAircraft.setAltitude(altitude);
        this->m_network->setOwnAircraftPosition(position, altitude);
    }

    /*
     * Update own situation
     */
    void CContextNetwork::updateOwnSituation(const BlackMisc::Aviation::CAircraftSituation &situation)
    {
        // TODO: Do I really need own member?
        this->m_ownAircraft.setSituation(situation);
        this->m_network->setOwnAircraftSituation(situation);
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
        this->m_network->setOwnAircraftAvionics(com1, com2, transponder);
    }

    /*
     * Own aircraft
     */
    CAircraft CContextNetwork::getOwnAircraft() const
    {
        // this->log(Q_FUNC_INFO, this->m_ownAircraft.toQString());
        return this->m_ownAircraft;
    }

    /*
     * Send text messages
     */
    void CContextNetwork::sendTextMessages(const CTextMessageList &textMessages)
    {
        // this->log(Q_FUNC_INFO, textMessages.toQString());
        this->m_network->sendTextMessages(textMessages);
    }

    /*
     * Connection status changed
     */
    void CContextNetwork::psFsdConnectionStatusChanged(INetwork::ConnectionStatus from, INetwork::ConnectionStatus to)
    {
        CStatusMessageList msgs;
        // send 1st position
        if (to == INetwork::Connected)
        {
            QString m("Connected, own aircraft ");
            m.append(this->m_ownAircraft.toQString(true));
            msgs.push_back(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityInfo, m));
        }

        // send as message
        QString m("connection status changed from %1 to %2");
        m = m.arg(INetwork::connectionStatusToString(from), INetwork::connectionStatusToString(to));
        msgs.push_back(CStatusMessage(CStatusMessage::TypeTrafficNetwork,
                                      to == INetwork::DisconnectedError ? CStatusMessage::SeverityError : CStatusMessage::SeverityInfo, m));
        emit this->statusMessages(msgs);

        // send as own signal
        emit this->connectionStatusChanged(from, to);
    }

    /*
     * Name query
     */
    void CContextNetwork::psFsdRealNameReplyReceived(const CCallsign &callsign, const QString &realname)
    {
        // this->log(Q_FUNC_INFO, callsign.toQString(), realname);
        if (realname.isEmpty()) return;
        CValueMap vm(CAtcStation::IndexControllerRealName, realname);
        this->m_atcStationsOnline.applyIf(&CAtcStation::getCallsign, callsign, vm);
        this->m_atcStationsBooked.applyIf(&CAtcStation::getCallsign, callsign, vm);

        vm = CValueMap(CAircraft::IndexPilotRealName, realname);
        this->m_aircraftsInRange.applyIf(&CAircraft::getCallsign, callsign, vm);
    }

    /*
     * Data file has been read
     */
    void CContextNetwork::psDataFileRead()
    {
        const int interval = 60 * 1000;
        if (this->m_vatsimDataFileReader->interval() < interval) this->m_vatsimDataFileReader->setInterval(interval);
    }

} // namespace
