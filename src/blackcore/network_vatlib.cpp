/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "network_vatlib.h"
#include "blackmisc/project.h"
#include <vector>
#include <exception>
#include <type_traits>

static_assert(! std::is_abstract<BlackCore::CNetworkVatlib>::value, "Must implement all pure virtuals");

//TODO just placeholders to allow this to compile
#define CLIENT_PUBLIC_ID 0
#define CLIENT_PRIVATE_KEY ""

namespace BlackCore
{

    using namespace BlackMisc::PhysicalQuantities;
    using namespace BlackMisc::Geo;
    using namespace BlackMisc::Aviation;
    using namespace BlackMisc::Network;
    using namespace BlackMisc;


    void exceptionDispatcher(const char *caller);

    CNetworkVatlib::CNetworkVatlib(QObject *parent)
        : INetwork(parent),
          m_loginMode(LoginNormal),
          m_status(Cvatlib_Network::connStatus_Idle),
          m_fsdTextCodec(QTextCodec::codecForName("latin1"))
    {
        connect(this, &CNetworkVatlib::terminate, this, &INetwork::terminateConnection, Qt::QueuedConnection);

        Q_ASSERT_X(m_fsdTextCodec, "CNetworkVatlib", "Missing default wire text encoding");
        //TODO reinit m_fsdTextCodec from WireTextEncoding config setting if present

        connect(&m_processingTimer, SIGNAL(timeout()), this, SLOT(process()));
        connect(&m_updateTimer, SIGNAL(timeout()), this, SLOT(update()));
        m_processingTimer.start(c_processingIntervalMsec);
    }

    void CNetworkVatlib::initializeSession()
    {
        Q_ASSERT_X(isDisconnected(), "CNetworkVatlib", "attempted to reinitialize session while still connected");

        try
        {
            m_net.reset(Cvatlib_Network::Create());

            QString capabilities;
            capabilities += m_net->capability_AtcInfo;
            capabilities += "=1:";
            capabilities += m_net->capability_InterimPos;
            capabilities += "=1:";
            capabilities += m_net->capability_ModelDesc;
            capabilities += "=1";
            if (m_loginMode == LoginStealth)
            {
                capabilities += ":";
                capabilities += m_net->capability_Stealth;
                capabilities += "=1";
            }

            m_net->CreateNetworkSession(CProject::systemNameAndVersionChar(), CProject::versionMajor(), CProject::versionMinor(),
                                        CProject::simulatorsChar(), CLIENT_PUBLIC_ID, CLIENT_PRIVATE_KEY, toFSD(capabilities));

            m_net->InstallOnConnectionStatusChangedEvent(onConnectionStatusChanged, this);
            m_net->InstallOnTextMessageReceivedEvent(onTextMessageReceived, this);
            m_net->InstallOnRadioMessageReceivedEvent(onRadioMessageReceived, this);
            m_net->InstallOnPilotDisconnectedEvent(onPilotDisconnected, this);
            m_net->InstallOnControllerDisconnectedEvent(onControllerDisconnected, this);
            m_net->InstallOnPilotPositionUpdateEvent(onPilotPositionUpdate, this);
            m_net->InstallOnInterimPilotPositionUpdateEvent(onInterimPilotPositionUpdate, this);
            m_net->InstallOnAtcPositionUpdateEvent(onAtcPositionUpdate, this);
            m_net->InstallOnKickedEvent(onKicked, this);
            m_net->InstallOnPongEvent(onPong, this);
            m_net->InstallOnMetarReceivedEvent(onMetarReceived, this);
            m_net->InstallOnInfoQueryRequestReceivedEvent(onInfoQueryRequestReceived, this);
            m_net->InstallOnInfoQueryReplyReceivedEvent(onInfoQueryReplyReceived, this);
            m_net->InstallOnCapabilitiesReplyReceivedEvent(onCapabilitiesReplyReceived, this);
            m_net->InstallOnAtisReplyReceivedEvent(onAtisReplyReceived, this);
            m_net->InstallOnTemperatureDataReceivedEvent(onTemperatureDataReceived, this);
            m_net->InstallOnErrorReceivedEvent(onErrorReceived, this);
            m_net->InstallOnWindDataReceivedEvent(onWindDataReceived, this);
            m_net->InstallOnCloudDataReceivedEvent(onCloudDataReceived, this);
            m_net->InstallOnPilotInfoRequestReceivedEvent(onPilotInfoRequestReceived, this);
            m_net->InstallOnPilotInfoReceivedEvent(onPilotInfoReceived, this);
            m_net->InstallOnCustomPilotPacketReceivedEvent(onCustomPacketReceived, this);
        }
        catch (...) { exceptionDispatcher(Q_FUNC_INFO); }
    }

    CNetworkVatlib::~CNetworkVatlib()
    {
        if (!m_net) { return; }

        try
        {
            if (m_net->IsValid() && m_net->IsNetworkConnected())
            {
                m_net->LogoffAndDisconnect(0); // emits a connectionStatusChanged signal
            }
        }
        catch (...) { exceptionDispatcher(Q_FUNC_INFO); }
        try
        {
            m_net->DestroyNetworkSession();
        }
        catch (...) { exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CNetworkVatlib::process()
    {
        if (!m_net) { return; }

        try
        {
            if (m_net->IsValid() && m_net->IsSessionExists())
            {
                m_net->DoProcessing();
            }
        }
        catch (...) { exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CNetworkVatlib::update()
    {
        if (!m_net) { return; }

        try
        {
            if (m_net->IsValid() && m_net->IsSessionExists() && isConnected())
            {
                if (this->m_loginMode == LoginAsObserver)
                {
                    // Observer
                    Cvatlib_Network::ATCPosUpdate pos;
                    pos.facility = Cvatlib_Network::facilityType_Unknown;
                    pos.visibleRange = 10; // NM
                    pos.lat = m_ownAircraft.latitude().value(CAngleUnit::deg());
                    pos.lon = m_ownAircraft.longitude().value(CAngleUnit::deg());
                    pos.elevation = 0;
                    m_net->SendATCUpdate(pos);
                }
                else
                {
                    // Normal / Stealth mode
                    Cvatlib_Network::PilotPosUpdate pos;
                    pos.altAdj = 0; // TODO: this needs to be calculated
                    pos.altTrue = m_ownAircraft.getAltitude().value(CLengthUnit::ft());
                    pos.heading = m_ownAircraft.getHeading().value(CAngleUnit::deg());
                    pos.pitch = m_ownAircraft.getPitch().value(CAngleUnit::deg());
                    pos.bank = m_ownAircraft.getBank().value(CAngleUnit::deg());
                    pos.lat = m_ownAircraft.latitude().value(CAngleUnit::deg());
                    pos.lon = m_ownAircraft.longitude().value(CAngleUnit::deg());
                    pos.groundSpeed = m_ownAircraft.getGroundSpeed().value(CSpeedUnit::kts());
                    pos.rating = Cvatlib_Network::pilotRating_Unknown;
                    pos.xpdrCode = static_cast<qint16>(m_ownAircraft.getTransponderCode());
                    pos.xpdrMode = Cvatlib_Network::xpndrMode_Standby;
                    switch (m_ownAircraft.getTransponderMode())
                    {
                    case CTransponder::ModeC: pos.xpdrMode = Cvatlib_Network::xpndrMode_Normal; break;
                    case CTransponder::StateIdent: pos.xpdrMode = Cvatlib_Network::xpndrMode_Ident; break;
                    default: pos.xpdrMode = Cvatlib_Network::xpndrMode_Standby; break;
                    }
                    m_net->SendPilotUpdate(pos);
                }
            }
        }
        catch (...) { exceptionDispatcher(Q_FUNC_INFO); }
    }

    INetwork::ConnectionStatus convertConnectionStatus(Cvatlib_Network::connStatus status)
    {
        switch (status)
        {
        case Cvatlib_Network::connStatus_Idle:              return INetwork::Disconnected;
        case Cvatlib_Network::connStatus_Connecting:        return INetwork::Connecting;
        case Cvatlib_Network::connStatus_Connected:         return INetwork::Connected;
        case Cvatlib_Network::connStatus_Disconnected:      return INetwork::Disconnected;
        case Cvatlib_Network::connStatus_Error:             return INetwork::DisconnectedError;
        case Cvatlib_Network::connStatus_ConnectionFailed:  return INetwork::DisconnectedFailed;
        case Cvatlib_Network::connStatus_ConnectionLost:    return INetwork::DisconnectedLost;
        }
        qFatal("unrecognised connection status");
        return INetwork::DisconnectedError;
    }

    void CNetworkVatlib::changeConnectionStatus(Cvatlib_Network::connStatus status, QString errorMessage)
    {
        if (m_status != status)
        {
            qSwap(m_status, status);
            emit connectionStatusChanged(convertConnectionStatus(status), convertConnectionStatus(m_status), errorMessage);

            if (isDisconnected())
            {
                m_updateTimer.stop();
            }
        }
    }

    QString CNetworkVatlib::getSocketError() const
    {
        static QMap<QString, QString> errorCodes;
        if (errorCodes.isEmpty())
        {
            errorCodes["ECONNABORTED"] = "Connection aborted";
            errorCodes["ECONNREFUSED"] = "Connection refused";
            errorCodes["ECONNRESET"] = "Connection reset";
            errorCodes["EHOSTDOWN"] = "Host is down";
            errorCodes["EHOSTUNREACH"] = "Host is unreachable";
            errorCodes["ENETDOWN"] = "Network is down";
            errorCodes["ENETRESET"] = "Connection aborted by network";
            errorCodes["ENETUNREACH"] = "Network unreachable";
            errorCodes["ENOTCONN"] = "The socket is not connected";
            errorCodes["ETIMEDOUT"] = "Connection timed out";
        }

        QString err(m_net->GetNetworkErrorCode());
        if (errorCodes.contains(err))
        {
            return errorCodes[err];
        }
        else if (err.startsWith("EUNKWN"))
        {
            return "Unknown error code " + err.section(' ', 1);
        }
        else if (err == "EOK" || err.isEmpty())
        {
            return "";
        }
        else
        {
            return "Unrecognized error code " + err;
        }
    }

    QByteArray CNetworkVatlib::toFSD(QString qstr) const
    {
        return m_fsdTextCodec->fromUnicode(qstr);
    }

    QByteArray CNetworkVatlib::toFSD(const BlackMisc::Aviation::CCallsign &callsign) const
    {
        return toFSD(callsign.getStringAsSet());
    }

    std::function<const char **()> CNetworkVatlib::toFSD(QStringList qstrList) const
    {
        struct Closure
        {
            QVector<QByteArray> m_bytesVec;
            QVector<const char *> m_cstrVec;
            Closure(QStringList qsl, const CNetworkVatlib *creator)
            {
                for (auto i = qsl.begin(); i != qsl.end(); ++i)
                {
                    m_bytesVec.push_back(creator->toFSD(*i));
                }
            }
            const char **operator()()
            {
                Q_ASSERT(m_cstrVec.isEmpty());
                for (auto i = m_bytesVec.begin(); i != m_bytesVec.end(); ++i)
                {
                    m_cstrVec.push_back(i->constData());
                }
                return const_cast<const char **>(m_cstrVec.constData());
            }
        };
        return Closure(qstrList, this);
    }

    QString CNetworkVatlib::fromFSD(const char *cstr) const
    {
        return m_fsdTextCodec->toUnicode(cstr);
    }

    QStringList CNetworkVatlib::fromFSD(const char **cstrArray, int size) const
    {
        QStringList qstrList;
        for (int i = 0; i < size; ++i)
        {
            qstrList.push_back(fromFSD(cstrArray[i]));
        }
        return qstrList;
    }

    void exceptionDispatcher(const char *caller)
    {
        try
        {
            throw;
        }
        catch (const NetworkNotConnectedException &e)
        {
            // this could be caused by a race condition during normal operation, so not an error
            qDebug() << "NetworkNotConnectedException caught in " << caller << "\n" << e.what();
        }
        catch (const VatlibException &e)
        {
            qFatal("VatlibException caught in %s\n%s", caller, e.what());
        }
        catch (const std::exception &e)
        {
            qFatal("std::exception caught in %s\n%s", caller, e.what());
        }
        catch (...)
        {
            qFatal("Unknown exception caught in %s", caller);
        }
    }

    QList<QUrl> CNetworkVatlib::getStatusUrls() const
    {
        QList<QUrl> result;
        try
        {
            Cvatlib_Network *net = m_net.data();
            decltype(m_net) netPtr;
            if (!net)
            {
                netPtr.reset(Cvatlib_Network::Create());
                net = netPtr.data();
            }

            auto urlsPtr = QSharedPointer<const char *const>(net->GetVatsimStatusUrls(), [ = ](const char *const * p) { net->GetVatsimStatusUrls_Free(p); });
            auto urls = urlsPtr.data();
            while (*urls)
            {
                result.push_back(QUrl(*urls++));
            }
        }
        catch (...) { exceptionDispatcher(Q_FUNC_INFO); }
        return result;
    }

    BlackMisc::Network::CServerList CNetworkVatlib::getKnownServers() const
    {
        BlackMisc::Network::CServerList result;
        try
        {
            Cvatlib_Network *net = m_net.data();
            decltype(m_net) netPtr;
            if (!net)
            {
                netPtr.reset(Cvatlib_Network::Create());
                net = netPtr.data();
            }

            auto namesPtr = QSharedPointer<const char *const>(net->GetVatsimFSDServerNames(), [ = ](const char *const * p) { net->GetVatsimFSDServerNames_Free(p); });
            auto ipsPtr = QSharedPointer<const char *const>(net->GetVatsimFSDServerIps(), [ = ](const char *const * p) { net->GetVatsimFSDServerIps_Free(p); });
            auto locationsPtr = QSharedPointer<const char *const>(net->GetVatsimFSDServerLocations(), [ = ](const char *const * p) { net->GetVatsimFSDServerLocations_Free(p); });
            auto acceptsPtr = QSharedPointer<const bool>(net->GetVatsimFSDServerAcceptingConnections(), [ = ](const bool * p) { net->GetVatsimFSDServerAcceptingConnections_Free(p); });
            auto names = namesPtr.data();
            auto ips = ipsPtr.data();
            auto locations = locationsPtr.data();
            auto accepts = acceptsPtr.data();
            int port = 6809; // TODO hard-coded number?
            while (*names)
            {
                result.push_back(BlackMisc::Network::CServer(*names++, *locations++, *ips++, port, BlackMisc::Network::CUser(), *accepts++));
            }
        }
        catch (...) { exceptionDispatcher(Q_FUNC_INFO); }
        return result;
    }

    /********************************** * * * * * * * * * * * * * * * * * * * ************************************/
    /**********************************             INetwork slots            ************************************/
    /********************************** * * * * * * * * * * * * * * * * * * * ************************************/

    void CNetworkVatlib::presetServer(const CServer &server)
    {
        Q_ASSERT_X(isDisconnected(), "CNetworkVatlib", "Can't change server details while still connected");
        m_server = server;
    }

    void CNetworkVatlib::presetCallsign(const BlackMisc::Aviation::CCallsign &callsign)
    {
        Q_ASSERT_X(isDisconnected(), "CNetworkVatlib", "Can't change callsign while still connected");
        m_callsign = callsign;
    }

    void CNetworkVatlib::presetIcaoCodes(const BlackMisc::Aviation::CAircraftIcao &icao)
    {
        Q_ASSERT_X(isDisconnected(), "CNetworkVatlib", "Can't change ICAO codes while still connected");
        m_icaoCodes = icao;
    }

    void CNetworkVatlib::presetLoginMode(LoginMode mode)
    {
        Q_ASSERT_X(isDisconnected(), "CNetworkVatlib", "Can't change login mode while still connected");
        m_loginMode = mode;

        m_net.reset(nullptr);
    }

    void CNetworkVatlib::initiateConnection()
    {
        Q_ASSERT_X(isDisconnected(), "CNetworkVatlib", "Can't connect while still connected");

        try
        {
            if (!m_net)
            {
                initializeSession();
            }

            changeConnectionStatus(Cvatlib_Network::connStatus_Connecting); // paranoia

            QByteArray callsign = toFSD(m_loginMode == LoginAsObserver ?
                                        m_callsign.getAsObserverCallsignString() :
                                        m_callsign.asString());
            QByteArray name = toFSD(m_server.getUser().getRealName());

            if (this->m_loginMode == LoginAsObserver)
            {
                // Observer mode
                Cvatlib_Network::ATCConnectionInfo info;
                info.name = name.data();
                info.rating = Cvatlib_Network::atcRating_Obs;
                info.callsign = callsign.data();
                m_net->SetATCLoginInfo(toFSD(m_server.getAddress()), m_server.getPort(),
                                       toFSD(m_server.getUser().getId()),
                                       toFSD(m_server.getUser().getPassword()),
                                       info);
            }
            else
            {
                // normal scenario, also used in STEALTH
                Cvatlib_Network::PilotConnectionInfo info;
                info.callsign = callsign.data();
                info.name = name.data();
                info.rating = Cvatlib_Network::pilotRating_Student; //TODO
                info.sim = Cvatlib_Network::simType_MSFS98; //TODO
                m_net->SetPilotLoginInfo(toFSD(m_server.getAddress()), m_server.getPort(),
                                         toFSD(m_server.getUser().getId()),
                                         toFSD(m_server.getUser().getPassword()),
                                         info);
            }

            if (m_net->ConnectAndLogon())
            {
                if (! m_updateTimer.isActive())
                {
                    m_updateTimer.start(c_updateIntervalMsec);
                }
            }
            else
            {
                changeConnectionStatus(Cvatlib_Network::connStatus_Error/*, getSocketError()*/);
            }
        }
        catch (...)
        {
            changeConnectionStatus(Cvatlib_Network::connStatus_Error/*, getSocketError()*/);
            exceptionDispatcher(Q_FUNC_INFO);
        }
    }

    void CNetworkVatlib::terminateConnection()
    {
        try
        {
            m_updateTimer.stop();

            if (m_net && m_net->IsValid() && m_net->IsNetworkConnected())
            {
                // emit signal directly because there is no Cvatlib_Network enum for Disconnecting
                emit this->connectionStatusChanged(convertConnectionStatus(m_status), Disconnecting);
                m_net->LogoffAndDisconnect(c_logoffTimeoutSec);
            }
        }
        catch (...) { exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CNetworkVatlib::setOwnAircraft(const BlackMisc::Aviation::CAircraft &aircraft)
    {
        m_ownAircraft = aircraft;
    }

    void CNetworkVatlib::setOwnAircraftPosition(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude)
    {
        m_ownAircraft.setPosition(position);
        m_ownAircraft.setAltitude(altitude);
    }

    void CNetworkVatlib::setOwnAircraftSituation(const BlackMisc::Aviation::CAircraftSituation &situation)
    {
        m_ownAircraft.setSituation(situation);
    }

    void CNetworkVatlib::setOwnAircraftAvionics(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2,
            const BlackMisc::Aviation::CTransponder &xpdr)
    {
        m_ownAircraft.setCom1System(com1);
        m_ownAircraft.setCom1System(com2);
        m_ownAircraft.setTransponder(xpdr);
    }

    void CNetworkVatlib::sendTextMessages(const BlackMisc::Network::CTextMessageList &messages)
    {
        Q_ASSERT_X(isConnected(), "CNetworkVatlib", "Can't send to server when disconnected");

        if (messages.isEmpty()) return;
        try
        {
            CTextMessageList privateMessages = messages.getPrivateMessages();
            foreach(BlackMisc::Network::CTextMessage message, privateMessages)
            {
                if (message.getRecipientCallsign().isEmpty()) continue;
                m_net->SendPrivateTextMessage(toFSD(message.getRecipientCallsign()), toFSD(message.getMessage()));
            }
            CTextMessageList radioMessages = messages.getRadioMessages();
            if (radioMessages.isEmpty()) return;
            foreach(BlackMisc::Network::CTextMessage message, radioMessages)
            {
                // I could send the same message to n frequencies in one step
                // if this is really required, I need to group by message
                // currently I send individual messages
                QVector<INT> freqsVec;
                freqsVec.push_back(message.getFrequency().value(CFrequencyUnit::kHz()));
                m_net->SendRadioTextMessage(freqsVec.size(), freqsVec.data(), toFSD(message.getMessage()));
            }
        }
        catch (...) { exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CNetworkVatlib::sendCustomPacket(const BlackMisc::Aviation::CCallsign &callsign, const QString &packetId, const QStringList &data)
    {
        Q_ASSERT_X(isConnected(), "CNetworkVatlib", "Can't send to server when disconnected");

        try
        {
            m_net->SendCustomPilotPacket(toFSD(callsign), toFSD(packetId), toFSD(data)(), data.size());
        }
        catch (...) { exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CNetworkVatlib::sendIpQuery()
    {
        Q_ASSERT_X(isConnected(), "CNetworkVatlib", "Can't send to server when disconnected");

        try
        {
            m_net->SendInfoQuery(Cvatlib_Network::infoQuery_IP, "SERVER");
        }
        catch (...) { exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CNetworkVatlib::sendFrequencyQuery(const BlackMisc::Aviation::CCallsign &callsign)
    {
        Q_ASSERT_X(isConnected(), "CNetworkVatlib", "Can't send to server when disconnected");

        try
        {
            m_net->SendInfoQuery(Cvatlib_Network::infoQuery_Freq, toFSD(callsign));
        }
        catch (...) { exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CNetworkVatlib::sendServerQuery(const BlackMisc::Aviation::CCallsign &callsign)
    {
        Q_ASSERT_X(isConnected(), "CNetworkVatlib", "Can't send to server when disconnected");

        try
        {
            m_net->SendInfoQuery(Cvatlib_Network::infoQuery_Server, toFSD(callsign));
        }
        catch (...) { exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CNetworkVatlib::sendAtcQuery(const BlackMisc::Aviation::CCallsign &callsign)
    {
        Q_ASSERT_X(isConnected(), "CNetworkVatlib", "Can't send to server when disconnected");

        try
        {
            m_net->SendInfoQuery(Cvatlib_Network::infoQuery_ATC, toFSD(callsign));
        }
        catch (...) { exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CNetworkVatlib::sendAtisQuery(const BlackMisc::Aviation::CCallsign &callsign)
    {
        Q_ASSERT_X(isConnected(), "CNetworkVatlib", "Can't send to server when disconnected");

        try
        {
            m_net->SendInfoQuery(Cvatlib_Network::infoQuery_ATIS, toFSD(callsign));
        }
        catch (...) { exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CNetworkVatlib::sendFlightPlan(const CFlightPlan &flightPlan)
    {
        Q_ASSERT_X(isConnected(), "CNetworkVatlib", "Can't send to server when disconnected");

        try
        {
            Cvatlib_Network::FlightPlan vatlibFP;
            QByteArray acType, altApt, cruiseAlt, depApt, destApt, remarks, route;
            vatlibFP.acType = acType = toFSD(flightPlan.getEquipmentIcao());
            vatlibFP.altApt = altApt = toFSD(flightPlan.getAlternateAirportIcao().asString());
            vatlibFP.cruiseAlt = cruiseAlt = toFSD(QByteArray::number(flightPlan.getCruiseAltitude().value(CLengthUnit::ft()), 'f', 0));
            vatlibFP.depApt = depApt = toFSD(flightPlan.getOriginAirportIcao().asString());
            vatlibFP.depTimeActual = flightPlan.getTakeoffTimeActual().toUTC().toString("hhmm").toInt();
            vatlibFP.depTimePlanned = flightPlan.getTakeoffTimePlanned().toUTC().toString("hhmm").toInt();
            vatlibFP.destApt = destApt = toFSD(flightPlan.getDestinationAirportIcao().asString());
            vatlibFP.enrouteHrs = flightPlan.getEnrouteTime().valueRounded(CTimeUnit::h(), 0);
            vatlibFP.enrouteMins = int(flightPlan.getEnrouteTime().valueRounded(CTimeUnit::hrmin(), 0)) % 60;
            vatlibFP.fuelHrs = flightPlan.getFuelTime().valueRounded(CTimeUnit::h(), 0);
            vatlibFP.fuelMins = int(flightPlan.getFuelTime().valueRounded(CTimeUnit::hrmin(), 0)) % 60;
            vatlibFP.remarks = remarks = toFSD(QString(flightPlan.getRemarks()).replace(":", ";"));
            vatlibFP.route = route = toFSD(QString(flightPlan.getRoute()).replace(" ", "."));
            vatlibFP.trueCruiseSpeed = flightPlan.getCruiseTrueAirspeed().valueRounded(CSpeedUnit::kts());
            switch (flightPlan.getFlightRules())
            {
            default:
            case CFlightPlan::IFR:  vatlibFP.fpRules = Cvatlib_Network::fpRuleType_IFR; break;
            case CFlightPlan::VFR:  vatlibFP.fpRules = Cvatlib_Network::fpRuleType_VFR; break;
            case CFlightPlan::SVFR: vatlibFP.fpRules = Cvatlib_Network::fpRuleType_SVFR; break;
            }
            m_net->SendFlightPlan(vatlibFP);
        }
        catch (...) { exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CNetworkVatlib::sendRealNameQuery(const BlackMisc::Aviation::CCallsign &callsign)
    {
        Q_ASSERT_X(isConnected(), "CNetworkVatlib", "Can't send to server when disconnected");

        try
        {
            m_net->SendInfoQuery(Cvatlib_Network::infoQuery_Name, toFSD(callsign));
        }
        catch (...) { exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CNetworkVatlib::sendCapabilitiesQuery(const BlackMisc::Aviation::CCallsign &callsign)
    {
        Q_ASSERT_X(isConnected(), "CNetworkVatlib", "Can't send to server when disconnected");

        try
        {
            m_net->SendInfoQuery(Cvatlib_Network::infoQuery_Capabilities, toFSD(callsign));
        }
        catch (...) { exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CNetworkVatlib::replyToFrequencyQuery(const BlackMisc::Aviation::CCallsign &callsign) // private
    {
        try
        {
            m_net->ReplyToInfoQuery(Cvatlib_Network::infoQuery_Freq, toFSD(callsign),
                                    toFSD(QString::number(m_ownAircraft.getCom1System().getFrequencyActive().value(CFrequencyUnit::MHz()), 'f', 3)));
        }
        catch (...) { exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CNetworkVatlib::replyToNameQuery(const BlackMisc::Aviation::CCallsign &callsign) // private
    {
        try
        {
            m_net->ReplyToInfoQuery(Cvatlib_Network::infoQuery_Name, toFSD(callsign), toFSD(m_server.getUser().getRealName()));
        }
        catch (...) { exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CNetworkVatlib::sendIcaoCodesQuery(const BlackMisc::Aviation::CCallsign &callsign)
    {
        Q_ASSERT_X(isConnected(), "CNetworkVatlib", "Can't send to server when disconnected");

        try
        {
            m_net->RequestPlaneInfo(toFSD(callsign));
        }
        catch (...) { exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CNetworkVatlib::sendAircraftInfo(const BlackMisc::Aviation::CCallsign &callsign) // private
    {
        try
        {
            const QByteArray acTypeICAObytes = toFSD(m_icaoCodes.getAircraftDesignator());
            const QByteArray airlineICAObytes = toFSD(m_icaoCodes.getAirlineDesignator());
            const QByteArray liverybytes = toFSD(m_icaoCodes.getLivery());
            std::vector<const char *> keysValues;
            if (!m_icaoCodes.getAircraftDesignator().isEmpty())
            {
                keysValues.push_back(m_net->acinfo_Equipment);
                keysValues.push_back(acTypeICAObytes);
            }
            if (m_icaoCodes.hasAirlineDesignator())
            {
                keysValues.push_back(m_net->acinfo_Airline);
                keysValues.push_back(airlineICAObytes);
            }
            if (m_icaoCodes.hasLivery())
            {
                keysValues.push_back(m_net->acinfo_Livery);
                keysValues.push_back(liverybytes);
            }
            keysValues.push_back(nullptr);
            m_net->SendPlaneInfo(toFSD(callsign), keysValues.data());
        }
        catch (...) { exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CNetworkVatlib::sendPing(const BlackMisc::Aviation::CCallsign &callsign)
    {
        Q_ASSERT_X(isConnected(), "CNetworkVatlib", "Can't send to server when disconnected");

        try
        {
            m_net->PingUser(toFSD(callsign));
        }
        catch (...) { exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CNetworkVatlib::sendMetarQuery(const QString &airportICAO)
    {
        Q_ASSERT_X(isConnected(), "CNetworkVatlib", "Can't send to server when disconnected");

        try
        {
            m_net->RequestMetar(toFSD(airportICAO));
        }
        catch (...) { exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CNetworkVatlib::sendWeatherDataQuery(const QString &airportICAO)
    {
        Q_ASSERT_X(isConnected(), "CNetworkVatlib", "Can't send to server when disconnected");

        try
        {
            m_net->RequestWeatherData(toFSD(airportICAO));
        }
        catch (...) { exceptionDispatcher(Q_FUNC_INFO); }
    }

    /********************************** * * * * * * * * * * * * * * * * * * * ************************************/
    /**********************************           shimlib callbacks           ************************************/
    /********************************** * * * * * * * * * * * * * * * * * * * ************************************/

    CNetworkVatlib *cbvar_cast(void *cbvar)
    {
        return static_cast<CNetworkVatlib *>(cbvar);
    }

    void CNetworkVatlib::onConnectionStatusChanged(Cvatlib_Network *, Cvatlib_Network::connStatus, Cvatlib_Network::connStatus newStatus, void *cbvar)
    {
        if (newStatus == Cvatlib_Network::connStatus_Error ||
                newStatus == Cvatlib_Network::connStatus_ConnectionFailed ||
                newStatus == Cvatlib_Network::connStatus_ConnectionLost)
        {
            cbvar_cast(cbvar)->changeConnectionStatus(newStatus, cbvar_cast(cbvar)->getSocketError());
        }
        else
        {
            cbvar_cast(cbvar)->changeConnectionStatus(newStatus);
        }
    }

    void CNetworkVatlib::onTextMessageReceived(Cvatlib_Network *, const char *from, const char *to, const char *msg, void *cbvar)
    {
        BlackMisc::Network::CTextMessageList messages(cbvar_cast(cbvar)->fromFSD(msg), CCallsign(cbvar_cast(cbvar)->fromFSD(from)), CCallsign(cbvar_cast(cbvar)->fromFSD(to)));
        emit cbvar_cast(cbvar)->textMessagesReceived(messages);
    }

    void CNetworkVatlib::onRadioMessageReceived(Cvatlib_Network *, const char *from, INT numFreq, INT *freqList, const char *msg, void *cbvar)
    {
        QList<CFrequency> frequencies;
        for (int i = 0; i < numFreq; ++i)
        {
            frequencies.push_back(CFrequency(freqList[i], CFrequencyUnit::kHz()));
        }
        BlackMisc::Network::CTextMessageList messages(cbvar_cast(cbvar)->fromFSD(msg), frequencies, CCallsign(cbvar_cast(cbvar)->fromFSD(from)));
        emit cbvar_cast(cbvar)->textMessagesReceived(messages);
    }

    void CNetworkVatlib::onPilotDisconnected(Cvatlib_Network *, const char *callsign, void *cbvar)
    {
        emit cbvar_cast(cbvar)->pilotDisconnected(cbvar_cast(cbvar)->fromFSD(callsign));
    }

    void CNetworkVatlib::onControllerDisconnected(Cvatlib_Network *, const char *callsign, void *cbvar)
    {
        emit cbvar_cast(cbvar)->atcDisconnected(cbvar_cast(cbvar)->fromFSD(callsign));
    }

    void CNetworkVatlib::onPilotPositionUpdate(Cvatlib_Network *, const char *callsignChar , Cvatlib_Network::PilotPosUpdate pos, void *cbvar)
    {
        const CCallsign callsign(callsignChar);
        const CAircraftSituation situation(
            CCoordinateGeodetic(pos.lat, pos.lon, 0.0),
            CAltitude(pos.altTrue, CAltitude::AboveGround, CLengthUnit::ft()),
            CHeading(pos.heading, CHeading::True, CAngleUnit::deg()),
            CAngle(pos.pitch, CAngleUnit::deg()),
            CAngle(pos.bank, CAngleUnit::deg()),
            CSpeed(pos.groundSpeed, CSpeedUnit::kts())
        );

        QString tn("transponder ");
        tn.append(callsign.asString());
        CTransponder::TransponderMode mode = CTransponder::StateStandby;
        switch (pos.xpdrMode)
        {
        case Cvatlib_Network::xpndrMode_Normal:
            mode = CTransponder::ModeC;
            break;
        case Cvatlib_Network::xpndrMode_Standby:
            mode = CTransponder::StateStandby;
            break;
        case Cvatlib_Network::xpndrMode_Ident:
            mode = CTransponder::StateIdent;
            break;
        default:
            mode = CTransponder::ModeC;
            break;
        }

        // I did have a situation where I got wrong transponger codes (KB)
        // So I now check for a valid code in order to detect such codes
        CTransponder transponder(tn, 0, mode);
        if (CTransponder::isValidTransponderCode(pos.xpdrCode))
        {
            transponder = CTransponder(tn, pos.xpdrCode, mode);
        }
        else
        {
            // TODO: how do with log this
            qDebug() << "Wrong transponder code" << pos.xpdrMode << callsign;
        }
        emit cbvar_cast(cbvar)->aircraftPositionUpdate(callsign, situation, transponder);
    }

    void CNetworkVatlib::onInterimPilotPositionUpdate(Cvatlib_Network *, const char * /** callsign **/, Cvatlib_Network::PilotPosUpdate /** pos **/, void * /** cbvar **/)
    {
        //TODO
    }

    void CNetworkVatlib::onAtcPositionUpdate(Cvatlib_Network *, const char *callsign, Cvatlib_Network::ATCPosUpdate pos, void *cbvar)
    {
        CFrequency freq(pos.frequency, CFrequencyUnit::kHz());
        freq.switchUnit(CFrequencyUnit::MHz()); // we would not need to bother, but this makes it easier to identify
        emit cbvar_cast(cbvar)->atcPositionUpdate(cbvar_cast(cbvar)->fromFSD(callsign), freq,
                CCoordinateGeodetic(pos.lat, pos.lon, 0), CLength(pos.visibleRange, CLengthUnit::NM()));
    }

    void CNetworkVatlib::onKicked(Cvatlib_Network *, const char *reason, void *cbvar)
    {
        emit cbvar_cast(cbvar)->kicked(cbvar_cast(cbvar)->fromFSD(reason));
    }

    void CNetworkVatlib::onPong(Cvatlib_Network *, const char *callsign, INT elapsedTime, void *cbvar)
    {
        emit cbvar_cast(cbvar)->pongReceived(cbvar_cast(cbvar)->fromFSD(callsign), CTime(elapsedTime, CTimeUnit::s()));
    }

    void CNetworkVatlib::onCustomPacketReceived(Cvatlib_Network *, const char *callsign, const char *packetId, const char **data, INT dataSize, void *cbvar)
    {
        emit cbvar_cast(cbvar)->customPacketReceived(cbvar_cast(cbvar)->fromFSD(callsign), cbvar_cast(cbvar)->fromFSD(packetId), cbvar_cast(cbvar)->fromFSD(data, dataSize));
    }

    void CNetworkVatlib::onMetarReceived(Cvatlib_Network *, const char *data, void *cbvar)
    {
        emit cbvar_cast(cbvar)->metarReplyReceived(cbvar_cast(cbvar)->fromFSD(data));
    }

    void CNetworkVatlib::onInfoQueryRequestReceived(Cvatlib_Network *, const char *callsignString, Cvatlib_Network::infoQuery type, const char *, void *cbvar)
    {
        auto timer = new QTimer(cbvar_cast(cbvar));
        timer->setSingleShot(true);
        timer->start(0);

        BlackMisc::Aviation::CCallsign callsign(callsignString);
        switch (type)
        {
        case Cvatlib_Network::infoQuery_Freq:
            connect(timer, &QTimer::timeout, [ = ]() { cbvar_cast(cbvar)->replyToFrequencyQuery(callsign); });
            break;
        case Cvatlib_Network::infoQuery_Name:
            connect(timer, &QTimer::timeout, [ = ]() { cbvar_cast(cbvar)->replyToNameQuery(callsign); });
            break;
        }
    }

    void CNetworkVatlib::onInfoQueryReplyReceived(Cvatlib_Network *, const char *callsign, Cvatlib_Network::infoQuery type, const char *data, const char *data2, void *cbvar)
    {
        switch (type)
        {
        case Cvatlib_Network::infoQuery_Freq:   emit cbvar_cast(cbvar)->frequencyReplyReceived(cbvar_cast(cbvar)->fromFSD(callsign), CFrequency(cbvar_cast(cbvar)->fromFSD(data).toFloat(), CFrequencyUnit::MHz())); break;
        case Cvatlib_Network::infoQuery_Server: emit cbvar_cast(cbvar)->serverReplyReceived(cbvar_cast(cbvar)->fromFSD(callsign), cbvar_cast(cbvar)->fromFSD(data)); break;
        case Cvatlib_Network::infoQuery_ATC:    emit cbvar_cast(cbvar)->atcReplyReceived(cbvar_cast(cbvar)->fromFSD(data2), *data == 'Y'); break;
        case Cvatlib_Network::infoQuery_Name:   emit cbvar_cast(cbvar)->realNameReplyReceived(cbvar_cast(cbvar)->fromFSD(callsign), cbvar_cast(cbvar)->fromFSD(data)); break;
        case Cvatlib_Network::infoQuery_IP:     emit cbvar_cast(cbvar)->ipReplyReceived(cbvar_cast(cbvar)->fromFSD(data)); break;
        }
    }

    void CNetworkVatlib::onCapabilitiesReplyReceived(Cvatlib_Network *net, const char *callsign, const char **keysValues, void *cbvar)
    {
        quint32 flags = 0;
        while (*keysValues)
        {
            const char *key = keysValues[0];
            const char *value = keysValues[1];
            if (*value == '1')
            {
                if (key == net->capability_AtcInfo)         { flags |= AcceptsAtisResponses; }
                else if (key == net->capability_InterimPos) { flags |= SupportsInterimPosUpdates; }
                else if (key == net->capability_ModelDesc)  { flags |= SupportsModelDescriptions; }
            }
            keysValues += 2;
        }
        emit cbvar_cast(cbvar)->capabilitiesReplyReceived(cbvar_cast(cbvar)->fromFSD(callsign), flags);
    }

    void CNetworkVatlib::onAtisReplyReceived(Cvatlib_Network *, const char *callsign, Cvatlib_Network::atisLineType lineType, const char *data, void *cbvar)
    {
        auto &atis = cbvar_cast(cbvar)->m_atisParts[cbvar_cast(cbvar)->fromFSD(callsign)]; // also inserts in map if not already in

        if (lineType == Cvatlib_Network::atisLineType_VoiceRoom)
        {
            emit cbvar_cast(cbvar)->atisVoiceRoomReplyReceived(cbvar_cast(cbvar)->fromFSD(callsign), cbvar_cast(cbvar)->fromFSD(data));
        }
        if (lineType == Cvatlib_Network::atisLineType_ZuluLogoff)
        {
            emit cbvar_cast(cbvar)->atisLogoffTimeReplyReceived(cbvar_cast(cbvar)->fromFSD(callsign), cbvar_cast(cbvar)->fromFSD(data));
        }

        if (lineType == Cvatlib_Network::atisLineType_LineCount)
        {
            atis.setType(CInformationMessage::ATIS);
            emit cbvar_cast(cbvar)->atisReplyReceived(cbvar_cast(cbvar)->fromFSD(callsign), atis);
            cbvar_cast(cbvar)->m_atisParts.remove(cbvar_cast(cbvar)->fromFSD(callsign));
        }
        else
        {
            const QString fixed = cbvar_cast(cbvar)->fromFSD(data).trimmed();
            if (! fixed.isEmpty())
            {
                // detect the stupid z1, z2, z3 placeholders
                // TODO: Anything better as this stupid code here?
                const QString test = fixed.toLower().remove(QRegExp("[\\n\\t\\r]"));
                if (test == "z") return;
                if (test.startsWith("z") && test.length() == 2) return; // z1, z2, ..
                if (test.length() == 1) return; // sometimes just z

                // append
                if (!atis.isEmpty()) atis.appendMessage("\n");
                atis.appendMessage(fixed);
            }
        }
    }

    void CNetworkVatlib::onTemperatureDataReceived(Cvatlib_Network *, Cvatlib_Network::TempLayer /** layers **/ [4], INT /** pressure **/, void * /** cbvar **/)
    {
        //TODO
    }

    void CNetworkVatlib::onErrorReceived(Cvatlib_Network *, Cvatlib_Network::error type, const char *msgData, const char *data, void *cbvar)
    {
        QString msg;

        switch (type)
        {
        case Cvatlib_Network::error_CallsignTaken:          msg = "The requested callsign is already taken"; goto terminate;
        case Cvatlib_Network::error_CallsignInvalid:        msg = "The requested callsign is not valid"; goto terminate;
        case Cvatlib_Network::error_CIDPasswdInvalid:       msg = "Wrong user ID or password"; goto terminate;
        case Cvatlib_Network::error_ProtoVersion:           msg = "This server does not support our protocol version"; goto terminate;
        case Cvatlib_Network::error_LevelTooHigh:           msg = "You are not authorized to use the requested pilot rating"; goto terminate;
        case Cvatlib_Network::error_ServerFull:             msg = "The server is full"; goto terminate;
        case Cvatlib_Network::error_CIDSuspended:           msg = "Your user account is suspended"; goto terminate;
        case Cvatlib_Network::error_InvalidPosition:        msg = "You are not authorized to use the requested pilot rating"; goto terminate;
        case Cvatlib_Network::error_SoftwareNotAuthorized:  msg = "This client software has not been authorized for use on this network"; goto terminate;

        case Cvatlib_Network::error_Ok:                     msg = "OK"; break;
        case Cvatlib_Network::error_Syntax:                 msg = "Malformed packet: Syntax error: "; msg.append(cbvar_cast(cbvar)->fromFSD(data)); break;
        case Cvatlib_Network::error_SourceInvalid:          msg = "Server: source invalid "; msg.append(cbvar_cast(cbvar)->fromFSD(data)); break;
        case Cvatlib_Network::error_CallsignNotExists:      msg = "Shim lib: "; msg.append(cbvar_cast(cbvar)->fromFSD(msgData)).append(" (").append(cbvar_cast(cbvar)->fromFSD(data)).append(")"); break;
        case Cvatlib_Network::error_NoFP:                   msg = "Server: no flight plan"; break;
        case Cvatlib_Network::error_NoWeather:              msg = "Server: requested weather profile does not exist"; break;

        // we have no idea what these mean
        case Cvatlib_Network::error_Registered:
        case Cvatlib_Network::error_InvalidControl:         msg = "Server: "; msg.append(cbvar_cast(cbvar)->fromFSD(msgData)); break;

        default:                                            qFatal("VATSIM shim library: %s (error %d)", qPrintable(msg), type); goto terminate;
        }

        emit cbvar_cast(cbvar)->statusMessage(BlackMisc::CStatusMessage(BlackMisc::CStatusMessage::TypeTrafficNetwork, BlackMisc::CStatusMessage::SeverityInfo, msg));
        return;

    terminate:
        emit cbvar_cast(cbvar)->statusMessage(BlackMisc::CStatusMessage(BlackMisc::CStatusMessage::TypeTrafficNetwork, BlackMisc::CStatusMessage::SeverityError, msg));
        emit cbvar_cast(cbvar)->terminate(); // private, will be handled during the next pass of the Qt event loop
    }

    void CNetworkVatlib::onWindDataReceived(Cvatlib_Network *, Cvatlib_Network::WindLayer /** layers **/[4], void * /** cbvar **/)
    {
        //TODO
    }

    void CNetworkVatlib::onCloudDataReceived(Cvatlib_Network *, Cvatlib_Network::CloudLayer /** layers **/ [2], Cvatlib_Network::StormLayer /** storm **/, float /** vis **/, void * /** cbvar **/)
    {
        //TODO
    }

    void CNetworkVatlib::onPilotInfoRequestReceived(Cvatlib_Network *, const char *callsignString, void *cbvar)
    {
        auto timer = new QTimer(cbvar_cast(cbvar));
        timer->setSingleShot(true);
        timer->start(0);

        BlackMisc::Aviation::CCallsign callsign(callsignString);
        connect(timer, &QTimer::timeout, [ = ]() { cbvar_cast(cbvar)->sendAircraftInfo(callsign); });
    }

    void CNetworkVatlib::onPilotInfoReceived(Cvatlib_Network *net, const char *callsign, const char **keysValues, void *cbvar)
    {
        BlackMisc::Aviation::CAircraftIcao icao;
        while (*keysValues)
        {
            QString key(*keysValues);
            keysValues++;
            if (key == net->acinfo_Equipment)       { icao.setAircraftDesignator(*keysValues); }
            else if (key == net->acinfo_Airline)    { icao.setAirlineDesignator(*keysValues); }
            else if (key == net->acinfo_Livery)     { icao.setLivery(*keysValues); }
            keysValues++;
        }
        emit cbvar_cast(cbvar)->icaoCodesReplyReceived(cbvar_cast(cbvar)->fromFSD(callsign), icao);
    }

} // namespace
