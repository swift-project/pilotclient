/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "network_vatlib.h"
#include <vector>
#include <exception>
#include <type_traits>

static_assert(! std::is_abstract<BlackCore::CNetworkVatlib>::value, "Must implement all pure virtuals");

//TODO just placeholders to allow this to compile
#define CLIENT_NAME_VERSION "BlackBox 0.3"
#define CLIENT_VERSION_MAJOR 0
#define CLIENT_VERSION_MINOR 3
#define CLIENT_SIMULATOR_NAME "None"
#define CLIENT_PUBLIC_ID 0
#define CLIENT_PRIVATE_KEY ""

namespace BlackCore
{

    using namespace BlackMisc::PhysicalQuantities;
    using namespace BlackMisc::Geo;
    using namespace BlackMisc::Aviation;
    using namespace BlackMisc::Network;

    void exceptionDispatcher(const char *caller);

    CNetworkVatlib::CNetworkVatlib(CNetworkVatlib::LoginMode loginMode, QObject *parent)
        : INetwork(parent),
          m_net(Cvatlib_Network::Create()),
          m_loginMode(loginMode),
          m_status(Cvatlib_Network::connStatus_Idle),
          m_fsdTextCodec(QTextCodec::codecForName("latin1"))
    {
        try
        {
            connect(this, SIGNAL(terminate()), this, SLOT(terminateConnection()), Qt::QueuedConnection);

            Q_ASSERT_X(m_fsdTextCodec, "CNetworkVatlib", "Missing default wire text encoding");
            //TODO reinit m_fsdTextCodec from WireTextEncoding config setting if present

            QString capabilities;
            capabilities += m_net->capability_AtcInfo;
            capabilities += "=1:";
            capabilities += m_net->capability_InterimPos;
            capabilities += "=1:";
            capabilities += m_net->capability_ModelDesc;
            capabilities += "=1";
            if (loginMode == LoginStealth)
            {
                capabilities += "STEALTH"; // TODO  m_net->capability_Stealth
                capabilities += "=1";
            }

            m_net->CreateNetworkSession(CLIENT_NAME_VERSION, CLIENT_VERSION_MAJOR, CLIENT_VERSION_MINOR,
                                        CLIENT_SIMULATOR_NAME, CLIENT_PUBLIC_ID, CLIENT_PRIVATE_KEY, toFSD(capabilities));

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

            connect(&m_processingTimer, SIGNAL(timeout()), this, SLOT(process()));
            connect(&m_updateTimer, SIGNAL(timeout()), this, SLOT(update()));
            m_processingTimer.start(c_processingIntervalMsec);
        }
        catch (...) { exceptionDispatcher(Q_FUNC_INFO); }
    }

    CNetworkVatlib::~CNetworkVatlib()
    {
        try
        {
            if (m_net->IsNetworkConnected())
            {
                m_net->LogoffAndDisconnect(0);
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

    QByteArray CNetworkVatlib::toFSD(QString qstr) const
    {
        return m_fsdTextCodec->fromUnicode(qstr);
    }

    QByteArray CNetworkVatlib::toFSD(const BlackMisc::Aviation::CCallsign &callsign) const
    {
        return toFSD(callsign.getStringAsSet());
    }

    QString CNetworkVatlib::fromFSD(const char *cstr) const
    {
        return m_fsdTextCodec->toUnicode(cstr);
    }

    bool CNetworkVatlib::isConnected() const
    {
        return m_status == Cvatlib_Network::connStatus_Connected;
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

    /********************************** * * * * * * * * * * * * * * * * * * * ************************************/
    /**********************************             INetwork slots            ************************************/
    /********************************** * * * * * * * * * * * * * * * * * * * ************************************/

    void CNetworkVatlib::setServer(const CServer &server)
    {
        Q_ASSERT_X(isDisconnected(), "CNetworkVatlib", "Can't change server details while still connected");
        this->m_server = server;
    }

    void CNetworkVatlib::setCallsign(const BlackMisc::Aviation::CCallsign &callsign)
    {
        Q_ASSERT_X(isDisconnected(), "CNetworkVatlib", "Can't change callsign while still connected");

        m_ownAircraft.setCallsign(callsign);
    }

    void CNetworkVatlib::setRealName(const QString &name)
    {
        Q_ASSERT_X(isDisconnected(), "CNetworkVatlib", "Can't change name while still connected");

        auto pilot = m_ownAircraft.getPilot();
        pilot.setRealName(name);
        m_ownAircraft.setPilot(pilot);
    }

    void CNetworkVatlib::initiateConnection()
    {
        Q_ASSERT_X(isDisconnected(), "CNetworkVatlib", "Can't connect while still connected");

        try
        {
            m_status = Cvatlib_Network::connStatus_Connecting; // paranoia

            QByteArray callsign = toFSD(m_loginMode == LoginAsObserver ?
                                        m_ownAircraft.getCallsign().getAsObserverCallsignString() :
                                        m_ownAircraft.getCallsign().asString());
            QByteArray name = toFSD(m_ownAircraft.getPilot().getRealName());

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
            m_net->ConnectAndLogon();
            if (! m_updateTimer.isActive())
            {
                m_updateTimer.start(c_updateIntervalMsec);
            }
        }
        catch (...)
        {
            m_status = Cvatlib_Network::connStatus_Idle;
            exceptionDispatcher(Q_FUNC_INFO);
        }
    }

    void CNetworkVatlib::terminateConnection()
    {
        try
        {
            m_updateTimer.stop();
            m_net->LogoffAndDisconnect(c_logoffTimeoutSec);
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
                std::vector<INT> freqsVec;
                freqsVec.push_back(message.getFrequency().value(CFrequencyUnit::kHz()));
                m_net->SendRadioTextMessage(freqsVec.size(), freqsVec.data(), toFSD(message.getMessage()));
            }
        }
        catch (...) { exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CNetworkVatlib::sendIpQuery()
    {
        try
        {
            m_net->SendInfoQuery(Cvatlib_Network::infoQuery_IP, "SERVER");
        }
        catch (...) { exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CNetworkVatlib::sendFrequencyQuery(const BlackMisc::Aviation::CCallsign &callsign)
    {
        try
        {
            m_net->SendInfoQuery(Cvatlib_Network::infoQuery_Freq, toFSD(callsign));
        }
        catch (...) { exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CNetworkVatlib::sendServerQuery(const BlackMisc::Aviation::CCallsign &callsign)
    {
        try
        {
            m_net->SendInfoQuery(Cvatlib_Network::infoQuery_Server, toFSD(callsign));
        }
        catch (...) { exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CNetworkVatlib::sendAtcQuery(const BlackMisc::Aviation::CCallsign &callsign)
    {
        try
        {
            m_net->SendInfoQuery(Cvatlib_Network::infoQuery_ATC, toFSD(callsign));
        }
        catch (...) { exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CNetworkVatlib::sendAtisQuery(const BlackMisc::Aviation::CCallsign &callsign)
    {
        try
        {
            m_net->SendInfoQuery(Cvatlib_Network::infoQuery_ATIS, toFSD(callsign));
        }
        catch (...) { exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CNetworkVatlib::sendNameQuery(const BlackMisc::Aviation::CCallsign &callsign)
    {
        try
        {
            m_net->SendInfoQuery(Cvatlib_Network::infoQuery_Name, toFSD(callsign));
        }
        catch (...) { exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CNetworkVatlib::sendCapabilitiesQuery(const BlackMisc::Aviation::CCallsign &callsign)
    {
        try
        {
            m_net->SendInfoQuery(Cvatlib_Network::infoQuery_Capabilities, toFSD(callsign));
        }
        catch (...) { exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CNetworkVatlib::replyToFrequencyQuery(const BlackMisc::Aviation::CCallsign &callsign)
    {
        try
        {
            m_net->ReplyToInfoQuery(Cvatlib_Network::infoQuery_Freq, toFSD(callsign),
                                    toFSD(QString::number(m_ownAircraft.getCom1System().getFrequencyActive().value(CFrequencyUnit::MHz()), 'f', 3)));
        }
        catch (...) { exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CNetworkVatlib::replyToNameQuery(const BlackMisc::Aviation::CCallsign &callsign)
    {
        try
        {
            m_net->ReplyToInfoQuery(Cvatlib_Network::infoQuery_Name, toFSD(callsign), toFSD(m_server.getUser().getRealName()));
        }
        catch (...) { exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CNetworkVatlib::requestAircraftInfo(const BlackMisc::Aviation::CCallsign &callsign)
    {
        try
        {
            m_net->RequestPlaneInfo(toFSD(callsign));
        }
        catch (...) { exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CNetworkVatlib::sendAircraftInfo(const BlackMisc::Aviation::CCallsign &callsign)
    {
        try
        {
            const QByteArray acTypeICAObytes = toFSD(m_ownAircraft.getIcaoInfo().getDesignator());
            const QByteArray airlineICAObytes = toFSD(m_ownAircraft.getIcaoInfo().getAirline());
            const QByteArray liverybytes = toFSD(m_ownAircraft.getIcaoInfo().getLivery());
            std::vector<const char *> keysValues;
            if (!m_ownAircraft.getIcaoInfo().getDesignator().isEmpty())
            {
                keysValues.push_back(m_net->acinfo_Equipment);
                keysValues.push_back(acTypeICAObytes);
            }
            if (m_ownAircraft.getIcaoInfo().hasAirline())
            {
                keysValues.push_back(m_net->acinfo_Airline);
                keysValues.push_back(airlineICAObytes);
            }
            if (m_ownAircraft.getIcaoInfo().hasLivery())
            {
                keysValues.push_back(m_net->acinfo_Livery);
                keysValues.push_back(liverybytes);
            }
            keysValues.push_back(nullptr);
            m_net->SendPlaneInfo(toFSD(callsign), keysValues.data());
        }
        catch (...) { exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CNetworkVatlib::ping(const BlackMisc::Aviation::CCallsign &callsign)
    {
        try
        {
            m_net->PingUser(toFSD(callsign));
        }
        catch (...) { exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CNetworkVatlib::requestMetar(const QString &airportICAO)
    {
        try
        {
            m_net->RequestMetar(toFSD(airportICAO));
        }
        catch (...) { exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CNetworkVatlib::requestWeatherData(const QString &airportICAO)
    {
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

    CNetworkVatlib::ConnectionStatus convertConnectionStatus(Cvatlib_Network::connStatus status)
    {
        switch (status)
        {
        case Cvatlib_Network::connStatus_Idle:          return CNetworkVatlib::Disconnected;
        case Cvatlib_Network::connStatus_Connecting:    return CNetworkVatlib::Connecting;
        case Cvatlib_Network::connStatus_Connected:     return CNetworkVatlib::Connected;
        case Cvatlib_Network::connStatus_Disconnected:  return CNetworkVatlib::Disconnected;
        case Cvatlib_Network::connStatus_Error:         return CNetworkVatlib::DisconnectedError;
        }
        qFatal("unrecognised connection status");
        return CNetworkVatlib::DisconnectedError;
    }

    void CNetworkVatlib::onConnectionStatusChanged(Cvatlib_Network *, Cvatlib_Network::connStatus oldStatus, Cvatlib_Network::connStatus newStatus, void *cbvar)
    {
        cbvar_cast(cbvar)->m_status = newStatus;
        emit cbvar_cast(cbvar)->connectionStatusChanged(convertConnectionStatus(oldStatus), convertConnectionStatus(newStatus));
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
        CTransponder::TransponderMode mode = CTransponder::ModeS;
        switch (pos.xpdrMode)
        {
        case Cvatlib_Network::xpndrMode_Normal:
            mode = CTransponder::ModeC;
            break;
        case Cvatlib_Network::xpndrMode_Standby:
            mode = CTransponder::ModeS;
            break;
        case Cvatlib_Network::xpndrMode_Ident:
            mode = CTransponder::StateIdent;
            break;
        default:
            mode = CTransponder::ModeC;
            break;
        }
        CTransponder transponder(tn, pos.xpdrCode, mode);
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
        emit cbvar_cast(cbvar)->pong(cbvar_cast(cbvar)->fromFSD(callsign), CTime(elapsedTime, CTimeUnit::s()));
    }

    void CNetworkVatlib::onMetarReceived(Cvatlib_Network *, const char *data, void *cbvar)
    {
        emit cbvar_cast(cbvar)->metarReceived(cbvar_cast(cbvar)->fromFSD(data));
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
        case Cvatlib_Network::infoQuery_Freq:   emit cbvar_cast(cbvar)->frequencyQueryReplyReceived(cbvar_cast(cbvar)->fromFSD(callsign), CFrequency(cbvar_cast(cbvar)->fromFSD(data).toFloat(), CFrequencyUnit::MHz())); break;
        case Cvatlib_Network::infoQuery_Server: emit cbvar_cast(cbvar)->serverQueryReplyReceived(cbvar_cast(cbvar)->fromFSD(callsign), cbvar_cast(cbvar)->fromFSD(data)); break;
        case Cvatlib_Network::infoQuery_ATC:    emit cbvar_cast(cbvar)->atcQueryReplyReceived(cbvar_cast(cbvar)->fromFSD(data2), *data == 'Y'); break;
        case Cvatlib_Network::infoQuery_Name:   emit cbvar_cast(cbvar)->nameQueryReplyReceived(cbvar_cast(cbvar)->fromFSD(callsign), cbvar_cast(cbvar)->fromFSD(data)); break;
        case Cvatlib_Network::infoQuery_IP:     emit cbvar_cast(cbvar)->ipQueryReplyReceived(cbvar_cast(cbvar)->fromFSD(data)); break;
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
        emit cbvar_cast(cbvar)->capabilitiesQueryReplyReceived(cbvar_cast(cbvar)->fromFSD(callsign), flags);
    }

    void CNetworkVatlib::onAtisReplyReceived(Cvatlib_Network *, const char *callsign, Cvatlib_Network::atisLineType lineType, const char *data, void *cbvar)
    {
        auto &atis = cbvar_cast(cbvar)->m_atisParts[cbvar_cast(cbvar)->fromFSD(callsign)]; // also inserts in map if not already in

        if (lineType == Cvatlib_Network::atisLineType_VoiceRoom)
        {
            emit cbvar_cast(cbvar)->atisQueryVoiceRoomReplyReceived(cbvar_cast(cbvar)->fromFSD(callsign), cbvar_cast(cbvar)->fromFSD(data));
        }
        if (lineType == Cvatlib_Network::atisLineType_ZuluLogoff)
        {
            emit cbvar_cast(cbvar)->atisQueryLogoffTimeReplyReceived(cbvar_cast(cbvar)->fromFSD(callsign), cbvar_cast(cbvar)->fromFSD(data));
        }

        if (lineType == Cvatlib_Network::atisLineType_LineCount)
        {
            atis.setType(CInformationMessage::ATIS);
            emit cbvar_cast(cbvar)->atisQueryReplyReceived(cbvar_cast(cbvar)->fromFSD(callsign), atis);
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
        emit cbvar_cast(cbvar)->terminate();
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
            if (key == net->acinfo_Equipment)       { icao.setDesignator(*keysValues); }
            else if (key == net->acinfo_Airline)    { icao.setAirline(*keysValues); }
            else if (key == net->acinfo_Livery)     { icao.setLivery(*keysValues); }
            keysValues++;
        }
        emit cbvar_cast(cbvar)->aircraftInfoReceived(cbvar_cast(cbvar)->fromFSD(callsign), icao);
    }

} // namespace
