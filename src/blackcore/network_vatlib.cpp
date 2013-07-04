/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "network_vatlib.h"
#include <vector>
#include <exception>
#include <cassert>

/*
Macro to convert a QString to a C string (const char*).
The C string will cease to be valid at the end of the current statement, so it can't be saved for later without making a copy.
Something like this is valid:
  myPrint(C_STR(myQString));
But this is not:
  const char *s = C_STR(myQString); myPrint(s);
*/
#define C_STR(s) ((s).toUtf8().constData()) //TODO ask Gary UTF-8 or ASCII for text that will be sent over FSD?

namespace BlackCore
{

    using namespace BlackMisc::PhysicalQuantities;
    using namespace BlackMisc::Geo;

    NetworkVatlib::NetworkVatlib()
        : m_net(Create_Cvatlib_Network())
    {
        try
        {
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

            QString capabilities;
            capabilities += m_net->capability_AtcInfo;
            capabilities += "=1:";
            capabilities += m_net->capability_InterimPos;
            capabilities += "=1:";
            capabilities += m_net->capability_ModelDesc;
            capabilities += "=1";

            m_net->CreateNetworkSession(CLIENT_NAME_VERSION, CLIENT_VERSION_MAJOR, CLIENT_VERSION_MINOR,
                CLIENT_SIMULATOR_NAME, CLIENT_PUBLIC_ID, CLIENT_PRIVATE_KEY, C_STR(capabilities));

            m_timer.start(c_updateIntervalMillisecs, this);
        }
        catch (...) { exceptionDispatcher(); }
    }

    NetworkVatlib::~NetworkVatlib()
    {
        try
        {
            m_timer.stop();

            if (m_net->IsNetworkConnected())
            {
                m_net->LogoffAndDisconnect(0);
            }

            m_net->DestroyNetworkSession();
        }
        catch (...) { exceptionDispatcher(); }
    }

    void NetworkVatlib::timerEvent(QTimerEvent*)
    {
        try
        {
            if (m_net->IsValid() && m_net->IsSessionExists())
            {
                m_net->DoProcessing();
            }
        }
        catch (...) { exceptionDispatcher(); }
    }

    void NetworkVatlib::exceptionDispatcher()
    {
        try
        {
            throw;
        }
        //catch (InvalidObjectException& e) //TODO ask Gary to confirm that trying to use an invalid Cvatlib_Network is always the result of a programmer error
        //catch (ObjectNotSetupException& e) //TODO ask Gary what is "the setup function" mentioned in the ObjectNotSetupException documentation?
        catch (NetworkNotConnectedException& e)
        {
            qDebug() << "NetworkNotConnectedException: " << e.what();
        }
        catch (VatlibException& e)
        {
            Q_ASSERT_X(false, "NetworkVatlib", e.what());
            assert(false);
            qDebug() << "VatlibException: " << e.what();
        }
        catch (std::exception& e)
        {
            Q_ASSERT_X(false, "NetworkVatlib", e.what());
            assert(false);
            qDebug() << "NetworkVatlib: std::exception: " << e.what();
        }
        catch (...)
        {
            Q_ASSERT_X(false, "NetworkVatlib", "Unknown exception");
            assert(false);
            qDebug() << "NetworkVatlib: unknown exception";
        }
    }

    /********************************** * * * * * * * * * * * * * * * * * * * ************************************/
    /**********************************             INetwork slots            ************************************/
    /********************************** * * * * * * * * * * * * * * * * * * * ************************************/

    void NetworkVatlib::setServerDetails(const QString& host, quint16 port)
    {
        m_serverHost = host;
        m_serverPort = port;
    }

    void NetworkVatlib::setUserCredentials(const QString& username, const QString& password)
    {
        m_username = username;
        m_password = password;
    }

    void NetworkVatlib::initiateConnection()
    {
        try
        {
            m_net->ConnectAndLogon();
        }
        catch (...) { exceptionDispatcher(); }
    }

    void NetworkVatlib::terminateConnection()
    {
        try
        {
            m_net->LogoffAndDisconnect(c_logoffTimeoutSeconds); //TODO ask Gary will this block?
        }
        catch (...) { exceptionDispatcher(); }
    }

    void NetworkVatlib::sendPrivateTextMessage(const QString& callsign, const QString& msg)
    {
        try
        {
            m_net->SendPrivateTextMessage(C_STR(callsign), C_STR(msg));
        }
        catch (...) { exceptionDispatcher(); }
    }

    void NetworkVatlib::sendRadioTextMessage(const QVector<CFrequency>& freqs, const QString& msg)
    {
        try
        {
            std::vector<INT> freqsVec;
            for (int i = 0; i < freqs.size(); ++i)
            {
                freqsVec.push_back(freqs[i].value(CFrequencyUnit::kHz()));
            }
            m_net->SendRadioTextMessage(freqsVec.size(), freqsVec.data(), C_STR(msg));
        }
        catch (...) { exceptionDispatcher(); }
    }

    void NetworkVatlib::requestPlaneInfo(const QString& callsign)
    {
        try
        {
            m_net->RequestPlaneInfo(C_STR(callsign));
        }
        catch (...) { exceptionDispatcher(); }
    }

    void NetworkVatlib::sendPlaneInfo(const QString& callsign, const QString& acTypeICAO, const QString& airlineICAO, const QString& livery)
    {
        try
        {
            const QByteArray acTypeICAObytes = acTypeICAO.toUtf8(); //TODO ask Gary UTF-8 or ASCII?
            const QByteArray airlineICAObytes = airlineICAO.toUtf8(); //TODO ask Gary UTF-8 or ASCII?
            const QByteArray liverybytes = livery.toUtf8(); //TODO ask Gary UTF-8 or ASCII?
            std::vector<const char*> keysValues;
            if (! acTypeICAO.isEmpty())
            {
                keysValues.push_back(m_net->acinfo_Equipment);
                keysValues.push_back(acTypeICAObytes.data());
            }
            if (! airlineICAO.isEmpty())
            {
                keysValues.push_back(m_net->acinfo_Airline);
                keysValues.push_back(airlineICAObytes.data());
            }
            if (! livery.isEmpty())
            {
                keysValues.push_back(m_net->acinfo_Livery);
                keysValues.push_back(liverybytes.data());
            }
            keysValues.push_back(0);
            m_net->SendPlaneInfo(C_STR(callsign), keysValues.data());
        }
        catch (...) { exceptionDispatcher(); }
    }

    void NetworkVatlib::ping(const QString& callsign)
    {
        try
        {
            m_net->PingUser(C_STR(callsign));
        }
        catch (...) { exceptionDispatcher(); }
    }

    void NetworkVatlib::requestMetar(const QString& airportICAO)
    {
        try
        {
            m_net->RequestMetar(C_STR(airportICAO));
        }
        catch (...) { exceptionDispatcher(); }
    }

    void NetworkVatlib::requestWeatherData(const QString& airportICAO)
    {
        try
        {
            m_net->RequestWeatherData(C_STR(airportICAO));
        }
        catch (...) { exceptionDispatcher(); }
    }

    /********************************** * * * * * * * * * * * * * * * * * * * ************************************/
    /**********************************           shimlib callbacks           ************************************/
    /********************************** * * * * * * * * * * * * * * * * * * * ************************************/

    NetworkVatlib* cbvar_cast(void* cbvar)
    {
        return static_cast<NetworkVatlib*>(cbvar);
    }

    void NetworkVatlib::onConnectionStatusChanged(Cvatlib_Network*, Cvatlib_Network::connStatus, Cvatlib_Network::connStatus newStatus, void* cbvar)
    {
        switch (newStatus)
        {
        case Cvatlib_Network::connStatus_Idle:          emit cbvar_cast(cbvar)->connectionStatusIdle(); break;
        case Cvatlib_Network::connStatus_Connecting:    emit cbvar_cast(cbvar)->connectionStatusConnecting(); break;
        case Cvatlib_Network::connStatus_Connected:     emit cbvar_cast(cbvar)->connectionStatusConnected(); break;
        case Cvatlib_Network::connStatus_Disconnected:  emit cbvar_cast(cbvar)->connectionStatusDisconnected(); break;
        case Cvatlib_Network::connStatus_Error:         emit cbvar_cast(cbvar)->connectionStatusError(); break;
        }
    }

    void NetworkVatlib::onTextMessageReceived(Cvatlib_Network*, const char* from, const char* to, const char* msg, void* cbvar)
    {
        emit cbvar_cast(cbvar)->privateTextMessageReceived(from, to, msg);
    }

    void NetworkVatlib::onRadioMessageReceived(Cvatlib_Network*, const char* from, INT numFreq, INT* freqList, const char* msg, void* cbvar)
    {
        QVector<CFrequency> freqs;
        for (int i = 0; i < numFreq; ++i)
        {
            freqs.push_back(CFrequency(freqList[i], CFrequencyUnit::kHz()));
        }
        emit cbvar_cast(cbvar)->radioTextMessageReceived(from, msg, freqs);
    }

    void NetworkVatlib::onPilotDisconnected(Cvatlib_Network*, const char* callsign, void* cbvar)
    {
        emit cbvar_cast(cbvar)->pilotDisconnected(callsign);
    }

    void NetworkVatlib::onControllerDisconnected(Cvatlib_Network*, const char* callsign, void* cbvar)
    {
        emit cbvar_cast(cbvar)->atcDisconnected(callsign);
    }

    void NetworkVatlib::onPilotPositionUpdate(Cvatlib_Network*, const char* callsign, Cvatlib_Network::PilotPosUpdate pos, void* cbvar)
    {
        //TODO
    }

    void NetworkVatlib::onInterimPilotPositionUpdate(Cvatlib_Network*, const char* callsign, Cvatlib_Network::PilotPosUpdate pos, void* cbvar)
    {
        //TODO
    }

    void NetworkVatlib::onAtcPositionUpdate(Cvatlib_Network*, const char* callsign, Cvatlib_Network::ATCPosUpdate pos, void* cbvar)
    {
        emit cbvar_cast(cbvar)->atcPositionUpdate(callsign, CFrequency(pos.frequency, CFrequencyUnit::kHz()),
            CCoordinateGeodetic(pos.lat, pos.lon, 0), CLength(pos.visibleRange, CLengthUnit::NM()));
    }

    void NetworkVatlib::onKicked(Cvatlib_Network*, const char* reason, void* cbvar)
    {
        emit cbvar_cast(cbvar)->kicked(reason);
    }

    void NetworkVatlib::onPong(Cvatlib_Network*, const char* callsign, INT elapsedTime, void* cbvar)
    {
        emit cbvar_cast(cbvar)->pong(callsign, CTime(elapsedTime, CTimeUnit::ms())); //TODO ask Gary to confirm time unit
    }

    void NetworkVatlib::onMetarReceived(Cvatlib_Network*, const char* data, void* cbvar)
    {
        emit cbvar_cast(cbvar)->metarReceived(data);
    }

    void NetworkVatlib::onInfoQueryRequestReceived(Cvatlib_Network*, const char* callsign, Cvatlib_Network::infoQuery type, const char* data, void* cbvar)
    {
        switch (type)
        {
        case Cvatlib_Network::infoQuery_FP:     emit cbvar_cast(cbvar)->fpQueryRequestReceived(callsign); break;
        case Cvatlib_Network::infoQuery_Freq:   emit cbvar_cast(cbvar)->freqQueryRequestReceived(callsign); break;
            //TODO ask Gary whether we need to reply to UserInfo, Server, Name, Capabilities, or IP queries
        }
    }

    void NetworkVatlib::onInfoQueryReplyReceived(Cvatlib_Network*, const char* callsign, Cvatlib_Network::infoQuery type, const char* data, const char* data2, void* cbvar)
    {
        switch (type)
        {
        case Cvatlib_Network::infoQuery_ATIS:           break; //TODO ask Gary do we handle this here or in onAtisReplyReceived or both?
        case Cvatlib_Network::infoQuery_Name:           break; //TODO ask Gary what are the meanings of data and data2 in this context?
        case Cvatlib_Network::infoQuery_Capabilities:   break; //TODO ask Gary do we handle this here or in onCapabilitiesReplyReceived or both?
        case Cvatlib_Network::infoQuery_IP:             emit cbvar_cast(cbvar)->ipQueryReplyReceived(data); break;
        }
    }

    void NetworkVatlib::onCapabilitiesReplyReceived(Cvatlib_Network*, const char* callsign, const char** keysValues, void* cbvar)
    {
        //TODO
    }

    void NetworkVatlib::onAtisReplyReceived(Cvatlib_Network*, const char* callsign, Cvatlib_Network::atisLineType type, const char* data, void* cbvar)
    {
        emit cbvar_cast(cbvar)->atisReplyReceived(callsign, data);
    }

    void NetworkVatlib::onTemperatureDataReceived(Cvatlib_Network*, Cvatlib_Network::TempLayer layers[4], INT pressure, void* cbvar)
    {
        //TODO
    }

    void NetworkVatlib::onErrorReceived(Cvatlib_Network*, Cvatlib_Network::error type, const char* msg, const char* data, void* cbvar)
    {
        switch (type)
        {
        case Cvatlib_Network::error_Ok:                     return;
        case Cvatlib_Network::error_CallsignTaken:          qCritical() << "The requested callsign is already taken"; break;
        case Cvatlib_Network::error_CallsignInvalid:        qCritical() << "The requested callsign is not valid"; break;
        case Cvatlib_Network::error_Registered:             break; //TODO ask Gary to explain meaning
        case Cvatlib_Network::error_Syntax:                 assert(false); qWarning() << "VATSIM shim library: Syntax error: " << data; break;
        case Cvatlib_Network::error_SourceInvalid:          break; //TODO ask Gary to explain meaning
        case Cvatlib_Network::error_CIDPasswdInvalid:       qCritical() << "Wrong user ID or password"; break;
        case Cvatlib_Network::error_CallsignNotExists:      qDebug() << "Shim lib: " << msg << " (" << data << ")"; break;
        case Cvatlib_Network::error_NoFP:                   break; //TODO ask Gary under what circumstance this can happen
        case Cvatlib_Network::error_NoWeather:              break; //TODO ask Gary to explain meaning
        case Cvatlib_Network::error_ProtoVersion:           qCritical() << "This server uses an unsupported protocol version"; break;
        case Cvatlib_Network::error_LevelTooHigh:           qCritical() << "You are not authorized to use the requested pilot rating"; break; //TODO ask Gary to confirm meaning
        case Cvatlib_Network::error_ServerFull:             qCritical() << "The server is full"; break;
        case Cvatlib_Network::error_CIDSuspended:           qCritical() << "Your user account is suspended"; break;
        case Cvatlib_Network::error_InvalidControl:         break; //TODO ask Gary to explain meaning
        case Cvatlib_Network::error_InvalidPosition:        qCritical() << "You are not authorized to use the requested pilot rating"; break;
        case Cvatlib_Network::error_SoftwareNotAuthorized:  qCritical() << "This client software has not been authorized for use on this network"; break;
        default:                                            assert(false); qWarning() << "VATSIM shim library: " << msg << "(error " << type << ")"; break;
        }
    }

    void NetworkVatlib::onWindDataReceived(Cvatlib_Network*, Cvatlib_Network::WindLayer layers[4], void* cbvar)
    {
        //TODO
    }

    void NetworkVatlib::onCloudDataReceived(Cvatlib_Network*, Cvatlib_Network::CloudLayer layers[2], Cvatlib_Network::StormLayer storm, float vis, void* cbvar)
    {
        //TODO
    }

    void NetworkVatlib::onPilotInfoRequestReceived(Cvatlib_Network*, const char* callsign, void* cbvar)
    {
        emit cbvar_cast(cbvar)->planeInfoRequestReceived(callsign);
    }

    void NetworkVatlib::onPilotInfoReceived(Cvatlib_Network* net, const char* callsign, const char** keysValues, void* cbvar)
    {
        QString acTypeICAO;
        QString airlineICAO;
        QString livery;
        while (*keysValues)
        {
            QString key (*keysValues);
            keysValues++;
                 if (key == net->acinfo_Equipment)  { acTypeICAO = *keysValues; }
            else if (key == net->acinfo_Airline)    { airlineICAO = *keysValues; }
            else if (key == net->acinfo_Livery)     { livery = *keysValues; }
            keysValues++;
        }
        emit cbvar_cast(cbvar)->planeInfoReceived(callsign, acTypeICAO, airlineICAO, livery);
    }

} //namespace BlackCore
