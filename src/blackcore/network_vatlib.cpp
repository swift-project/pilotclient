/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "network_vatlib.h"
#include "blackmisc/project.h"
#include "blackmisc/logmessage.h"
#include <vector>
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
          m_status(vatStatusIdle),
          m_fsdTextCodec(QTextCodec::codecForName("latin1"))
    {
        connect(this, &CNetworkVatlib::terminate, this, &INetwork::terminateConnection, Qt::QueuedConnection);
        connect(this, &INetwork::customPacketReceived, this, &CNetworkVatlib::customPacketDispatcher);

        Q_ASSERT_X(m_fsdTextCodec, "CNetworkVatlib", "Missing default wire text encoding");
        //TODO reinit m_fsdTextCodec from WireTextEncoding config setting if present

        Vat_SetNetworkErrorHandler(CNetworkVatlib::networkErrorHandler);

        connect(&m_processingTimer, SIGNAL(timeout()), this, SLOT(process()));
        connect(&m_updateTimer, SIGNAL(timeout()), this, SLOT(update()));
        m_processingTimer.start(c_processingIntervalMsec);
    }

    void CNetworkVatlib::initializeSession()
    {
        Q_ASSERT_X(isDisconnected(), "CNetworkVatlib", "attempted to reinitialize session while still connected");

        int clientCapabilities = vatCapsModelDesc | vatCapsInterminPos | vatCapsAtcInfo;
        if (m_loginMode == LoginStealth)
        {
            clientCapabilities |= vatCapsStealth;
        }

        m_net.reset(Vat_CreateNetworkSession(vatServerLegacyFsd, CProject::systemNameAndVersionChar(),
                                             CProject::versionMajor(), CProject::versionMinor(),
                                             CProject::simulatorsChar(), CLIENT_PUBLIC_ID, CLIENT_PRIVATE_KEY,
                                             clientCapabilities));

        Vat_SetStateChangeHandler(m_net.data(), onConnectionStatusChanged, this);
        Vat_SetMessageHandler(m_net.data(), onTextMessageReceived, this);
        Vat_SetRadioHandler(m_net.data(), onRadioMessageReceived, this);
        Vat_SetDeletePilotHandler(m_net.data(), onPilotDisconnected, this);
        Vat_SetDeleteAtcHandler(m_net.data(), onControllerDisconnected, this);
        Vat_SetPilotPositionHandler(m_net.data(), onPilotPositionUpdate, this);
        Vat_SetInterimPilotPositionHandler(m_net.data(), onInterimPilotPositionUpdate, this);
        Vat_SetAtcPositionHandler(m_net.data(), onAtcPositionUpdate, this);
        Vat_SetKillHandler(m_net.data(), onKicked, this);
        Vat_SetPongHandler(m_net.data(), onPong, this);
        Vat_SetACARSDataHandler(m_net.data(), onMetarReceived, this);
        Vat_SetInfoRequestHandler(m_net.data(), onInfoQueryRequestReceived, this);
        Vat_SetInfoReplyHandler(m_net.data(), onInfoQueryReplyReceived, this);
        Vat_SetInfoCAPSReplyHandler(m_net.data(), onCapabilitiesReplyReceived, this);
        Vat_SetControllerAtisHandler(m_net.data(), onAtisReplyReceived, this);
        Vat_SetFlightPlanHandler(m_net.data(), onFlightPlanReceived, this);
        Vat_SetServerErrorHandler(m_net.data(), onErrorReceived, this);
        Vat_SetTemperatureDataHandler(m_net.data(), onTemperatureDataReceived, this);
        Vat_SetWindDataHandler(m_net.data(), onWindDataReceived, this);
        Vat_SetCloudDataHandler(m_net.data(), onCloudDataReceived, this);
        Vat_SetAircraftInfoRequestHandler(m_net.data(), onPilotInfoRequestReceived, this);
        Vat_SetAircraftInfoHandler(m_net.data(), onPilotInfoReceived, this);
        Vat_SetCustomPilotPacketHandler(m_net.data(), onCustomPacketReceived, this);
    }

    CNetworkVatlib::~CNetworkVatlib()
    {
        Q_ASSERT_X(isDisconnected(), "CNetworkVatlib", "CNetworkVatlib destoyed while still connected.");
    }

    void CNetworkVatlib::process()
    {
        if (!m_net) { return; }
        Vat_ExecuteNetworkTasks(m_net.data());
    }

    void CNetworkVatlib::update()
    {
        if (!m_net) { return; }

        if (isConnected())
        {
            if (this->m_loginMode == LoginAsObserver)
            {
                // Observer
                VatAtcPosition pos;
                pos.facility = vatFacilityTypeUnknown;
                pos.visibleRange = 10; // NM
                pos.latitude = m_ownAircraft.latitude().value(CAngleUnit::deg());
                pos.longitude = m_ownAircraft.longitude().value(CAngleUnit::deg());
                pos.elevation = 0;
                Vat_SendATCUpdate(m_net.data(), &pos);
            }
            else
            {
                // Normal / Stealth mode
                VatPilotPosition pos;
                pos.altitudeAdjust = 0; // TODO: this needs to be calculated
                pos.altitudeTrue = m_ownAircraft.getAltitude().value(CLengthUnit::ft());
                pos.heading = m_ownAircraft.getHeading().value(CAngleUnit::deg());
                pos.pitch = m_ownAircraft.getPitch().value(CAngleUnit::deg());
                pos.bank = m_ownAircraft.getBank().value(CAngleUnit::deg());
                pos.latitude = m_ownAircraft.latitude().value(CAngleUnit::deg());
                pos.longitude = m_ownAircraft.longitude().value(CAngleUnit::deg());
                pos.groundSpeed = m_ownAircraft.getGroundSpeed().value(CSpeedUnit::kts());
                pos.rating = vatPilotRatingUnknown;
                pos.transponderCode = static_cast<qint16>(m_ownAircraft.getTransponderCode());
                pos.transponderMode = vatTransponderModeStandby;
                switch (m_ownAircraft.getTransponderMode())
                {
                case CTransponder::ModeC: pos.transponderMode = vatTransponderModeCharlie; break;
                case CTransponder::StateIdent: pos.transponderMode = vatTransponderModeIdent; break;
                default: pos.transponderMode = vatTransponderModeStandby; break;
                }
                Vat_SendPilotUpdate(m_net.data(), &pos);
            }
        }
    }

    INetwork::ConnectionStatus convertConnectionStatus(VatConnectionStatus status)
    {
        switch (status)
        {
        case vatStatusIdle:              return INetwork::Disconnected;
        case vatStatusConnecting:        return INetwork::Connecting;
        case vatStatusConnected:         return INetwork::Connected;
        case vatStatusDisconnected:      return INetwork::Disconnected;
        case vatStatusDisconnecting:     return INetwork::Disconnecting;
        case vatStatusError:             return INetwork::DisconnectedError;
        }
        qFatal("unrecognised connection status");
        return INetwork::DisconnectedError;
    }

    void CNetworkVatlib::changeConnectionStatus(VatConnectionStatus status)
    {
        if (m_status != status)
        {
            qSwap(m_status, status);
            emit connectionStatusChanged(convertConnectionStatus(status), convertConnectionStatus(m_status));

            if (isDisconnected())
            {
                m_updateTimer.stop();
            }
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
        m_icaoCode = icao;
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

        if (!m_net)
        {
            initializeSession();
        }

        QByteArray callsign = toFSD(m_loginMode == LoginAsObserver ?
                                    m_callsign.getAsObserverCallsignString() :
                                    m_callsign.asString());
        QByteArray name = toFSD(m_server.getUser().getRealName());

        if (this->m_loginMode == LoginAsObserver)
        {
            // Observer mode
            VatAtcConnection info;
            info.name = name.data();
            info.rating = vatAtcRatingObserver;
            info.callsign = callsign.data();
            Vat_SpecifyATCLogon(m_net.data(), toFSD(m_server.getAddress()), m_server.getPort(),
                                toFSD(m_server.getUser().getId()),
                                toFSD(m_server.getUser().getPassword()),
                                &info);
        }
        else
        {
            // normal scenario, also used in STEALTH
            VatPilotConnection info;
            info.callsign = callsign.data();
            info.name = name.data();
            info.rating = vatPilotRatingStudent; //TODO
            info.simType = vatSimTypeMSFS98; //TODO
            Vat_SpecifyPilotLogon(m_net.data(), toFSD(m_server.getAddress()), m_server.getPort(),
                                  toFSD(m_server.getUser().getId()),
                                  toFSD(m_server.getUser().getPassword()),
                                  &info);
        }

        Vat_Logon(m_net.data());

        if (! m_updateTimer.isActive())
        {
            m_updateTimer.start(c_updateIntervalMsec);
        }
    }

    void CNetworkVatlib::terminateConnection()
    {
        m_updateTimer.stop();

        if (m_net && isConnected())
        {
            Vat_Logoff(m_net.data());
        }
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

    void CNetworkVatlib::setOwnCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2,
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
        CTextMessageList privateMessages = messages.getPrivateMessages();
        for(const auto &message : privateMessages)
        {
            if (message.getRecipientCallsign().isEmpty()) continue;
            Vat_SendTextMessage(m_net.data(), toFSD(message.getRecipientCallsign()), toFSD(message.getMessage()));
        }
        CTextMessageList radioMessages = messages.getRadioMessages();
        if (radioMessages.isEmpty()) return;
        for(const auto &message : radioMessages)
        {
            // I could send the same message to n frequencies in one step
            // if this is really required, I need to group by message
            // currently I send individual messages
            QVector<int> freqsVec;
            freqsVec.push_back(message.getFrequency().valueRounded(CFrequencyUnit::kHz(), 0));
            Vat_SendRadioMessage(m_net.data(), freqsVec.data(), freqsVec.size(), toFSD(message.getMessage()));
        }
    }

    void CNetworkVatlib::sendCustomPacket(const BlackMisc::Aviation::CCallsign &callsign, const QString &packetId, const QStringList &data)
    {
        Q_ASSERT_X(isConnected(), "CNetworkVatlib", "Can't send to server when disconnected");
        Vat_SendCustomPilotPacket(m_net.data(), toFSD(callsign), toFSD(packetId), toFSD(data)(), data.size());
    }

    void CNetworkVatlib::sendIpQuery()
    {
        Q_ASSERT_X(isConnected(), "CNetworkVatlib", "Can't send to server when disconnected");
        Vat_RequestInformation(m_net.data(), vatInfoQueryTypeIP, nullptr);
    }

    void CNetworkVatlib::sendFrequencyQuery(const BlackMisc::Aviation::CCallsign &callsign)
    {
        Q_ASSERT_X(isConnected(), "CNetworkVatlib", "Can't send to server when disconnected");
        Vat_RequestInformation(m_net.data(), vatInfoQueryTypeFreq, toFSD(callsign));
    }

    void CNetworkVatlib::sendUserInfoQuery(const BlackMisc::Aviation::CCallsign &callsign)
    {
        Q_ASSERT_X(isConnected(), "CNetworkVatlib", "Can't send to server when disconnected");
        Vat_RequestInformation(m_net.data(), vatInfoQueryTypeInfo, toFSD(callsign));
    }

    void CNetworkVatlib::sendServerQuery(const BlackMisc::Aviation::CCallsign &callsign)
    {
        Q_ASSERT_X(isConnected(), "CNetworkVatlib", "Can't send to server when disconnected");
        Vat_RequestInformation(m_net.data(), vatInfoQueryTypeServer, toFSD(callsign));
    }

    void CNetworkVatlib::sendAtcQuery(const BlackMisc::Aviation::CCallsign &callsign)
    {
        Q_ASSERT_X(isConnected(), "CNetworkVatlib", "Can't send to server when disconnected");
        Vat_RequestInformation(m_net.data(), vatInfoQueryTypeAtc, toFSD(callsign));
    }

    void CNetworkVatlib::sendAtisQuery(const BlackMisc::Aviation::CCallsign &callsign)
    {
        Q_ASSERT_X(isConnected(), "CNetworkVatlib", "Can't send to server when disconnected");
        Vat_RequestInformation(m_net.data(), vatInfoQueryTypeAtis, toFSD(callsign));
    }

    void CNetworkVatlib::sendFlightPlan(const CFlightPlan &flightPlan)
    {
        Q_ASSERT_X(isConnected(), "CNetworkVatlib", "Can't send to server when disconnected");

        VatFlightPlan vatlibFP;
        QString route = QString(flightPlan.getRoute()).replace(" ", ".");
        QString remarks = QString(flightPlan.getRemarks()).replace(":", ";").trimmed();
        QString alt = flightPlan.getCruiseAltitude().isFlightLevel() ?
                      flightPlan.getCruiseAltitude().toQString() :
                      flightPlan.getCruiseAltitude().valueRoundedWithUnit(0);
        alt = alt.remove('.').remove(','); // remove any separators

        QByteArray acTypeTemp, altAptTemp, cruiseAltTemp, depAptTemp, destAptTemp, routeTemp, remarksTemp;
        vatlibFP.aircraftType = acTypeTemp = toFSD(flightPlan.getEquipmentIcao());
        vatlibFP.alternateAirport = altAptTemp = toFSD(flightPlan.getAlternateAirportIcao().asString());
        vatlibFP.cruiseAltitude = cruiseAltTemp = toFSD(alt);
        vatlibFP.departAirport = depAptTemp = toFSD(flightPlan.getOriginAirportIcao().asString());
        vatlibFP.departTimeActual = flightPlan.getTakeoffTimeActual().toUTC().toString("hhmm").toInt();
        vatlibFP.departTime = flightPlan.getTakeoffTimePlanned().toUTC().toString("hhmm").toInt();
        vatlibFP.destAirport = destAptTemp = toFSD(flightPlan.getDestinationAirportIcao().asString());

        QList<int> timeParts = flightPlan.getEnrouteTime().getHrsMinSecParts();
        vatlibFP.enrouteHrs = timeParts[CTime::Hours];
        vatlibFP.enrouteMins = timeParts[CTime::Minutes];

        timeParts = flightPlan.getFuelTime().getHrsMinSecParts();
        vatlibFP.fuelHrs = timeParts[CTime::Hours];
        vatlibFP.fuelMins = timeParts[CTime::Minutes];
        vatlibFP.remarks = remarksTemp = toFSD(remarks);
        vatlibFP.route = routeTemp = toFSD(route);
        vatlibFP.trueCruisingSpeed = flightPlan.getCruiseTrueAirspeed().valueRounded(CSpeedUnit::kts());
        switch (flightPlan.getFlightRules())
        {
        default:
        case CFlightPlan::IFR:  vatlibFP.flightType = vatFlightTypeIFR; break;
        case CFlightPlan::VFR:  vatlibFP.flightType = vatFlightTypeVFR; break;
        case CFlightPlan::SVFR: vatlibFP.flightType = vatFlightTypeSVFR; break;
        case CFlightPlan::DVFR:  vatlibFP.flightType = vatFlightTypeDVFR; break;
        }
        Vat_SendFlightPlan(m_net.data(), &vatlibFP);
    }

    void CNetworkVatlib::sendFlightPlanQuery(const BlackMisc::Aviation::CCallsign &callsign)
    {
        Q_ASSERT_X(isConnected(), "CNetworkVatlib", "Can't send to server when disconnected");
        Vat_RequestInformation(m_net.data(), vatInfoQueryTypeFP, toFSD(callsign));
    }

    void CNetworkVatlib::sendRealNameQuery(const BlackMisc::Aviation::CCallsign &callsign)
    {
        Q_ASSERT_X(isConnected(), "CNetworkVatlib", "Can't send to server when disconnected");
        Vat_RequestInformation(m_net.data(), vatInfoQueryTypeName, toFSD(callsign));
    }

    void CNetworkVatlib::sendCapabilitiesQuery(const BlackMisc::Aviation::CCallsign &callsign)
    {
        Q_ASSERT_X(isConnected(), "CNetworkVatlib", "Can't send to server when disconnected");
        Vat_RequestInformation(m_net.data(), vatInfoQueryTypeCaps, toFSD(callsign));
    }

    void CNetworkVatlib::replyToFrequencyQuery(const BlackMisc::Aviation::CCallsign &callsign) // private
    {
        Vat_SendInformation(m_net.data(), vatInfoQueryTypeFreq, toFSD(callsign),
                            toFSD(QString::number(m_ownAircraft.getCom1System().getFrequencyActive().value(CFrequencyUnit::MHz()), 'f', 3)));
    }

    void CNetworkVatlib::replyToNameQuery(const BlackMisc::Aviation::CCallsign &callsign) // private
    {
        Vat_SendInformation(m_net.data(), vatInfoQueryTypeName, toFSD(callsign),toFSD(m_server.getUser().getRealName()));
    }

    void CNetworkVatlib::sendIcaoCodesQuery(const BlackMisc::Aviation::CCallsign &callsign)
    {
        Q_ASSERT_X(isConnected(), "CNetworkVatlib", "Can't send to server when disconnected");
        Vat_RequestAircraftInfo(m_net.data(), toFSD(callsign));
    }

    void CNetworkVatlib::sendAircraftInfo(const BlackMisc::Aviation::CCallsign &callsign) // private
    {
        const QByteArray acTypeICAObytes = toFSD(m_icaoCode.getAircraftDesignator());
        const QByteArray airlineICAObytes = toFSD(m_icaoCode.getAirlineDesignator());
        const QByteArray liverybytes = toFSD(m_icaoCode.getLivery());

        VatAircraftInfo aircraftInfo {acTypeICAObytes, airlineICAObytes, liverybytes};
        Vat_SendModernPlaneInfo(m_net.data(), toFSD(callsign), &aircraftInfo);
    }

    void CNetworkVatlib::sendPing(const BlackMisc::Aviation::CCallsign &callsign)
    {
        Q_ASSERT_X(isConnected(), "CNetworkVatlib", "Can't send to server when disconnected");
        Vat_SendPing(m_net.data(), toFSD(callsign));
    }

    void CNetworkVatlib::sendMetarQuery(const BlackMisc::Aviation::CAirportIcao &airportIcao)
    {
        Q_ASSERT_X(isConnected(), "CNetworkVatlib", "Can't send to server when disconnected");
        Vat_RequestACARS(m_net.data(), toFSD(airportIcao.asString()));
    }

    void CNetworkVatlib::sendWeatherDataQuery(const BlackMisc::Aviation::CAirportIcao &airportIcao)
    {
        Q_ASSERT_X(isConnected(), "CNetworkVatlib", "Can't send to server when disconnected");
        Vat_RequestWeather(m_net.data(), toFSD(airportIcao.asString()));
    }

    void CNetworkVatlib::sendFsipiCustomPacket(const BlackMisc::Aviation::CCallsign &callsign,
        const QString &airlineIcao, const QString &aircraftIcao, const QString &combinedType, const QString &modelString)
    {
        QStringList data { { "0" }, airlineIcao, aircraftIcao, { "" }, { "" }, { "" }, { "" }, combinedType, modelString };
        sendCustomPacket(callsign, "FSIPI", data);
    }

    void CNetworkVatlib::sendFsipirCustomPacket(const BlackMisc::Aviation::CCallsign &callsign,
        const QString &airlineIcao, const QString &aircraftIcao, const QString &combinedType, const QString &modelString)
    {
        QStringList data { { "0" }, airlineIcao, aircraftIcao, { "" }, { "" }, { "" }, { "" }, combinedType, modelString };
        sendCustomPacket(callsign, "FSIPIR", data);
    }

    /********************************** * * * * * * * * * * * * * * * * * * * ************************************/
    /**********************************           shimlib callbacks           ************************************/
    /********************************** * * * * * * * * * * * * * * * * * * * ************************************/

    CNetworkVatlib *cbvar_cast(void *cbvar)
    {
        return static_cast<CNetworkVatlib *>(cbvar);
    }

    void CNetworkVatlib::onConnectionStatusChanged(VatSessionID, VatConnectionStatus, VatConnectionStatus newStatus, void *cbvar)
    {
        if (newStatus == vatStatusError)
        {
            cbvar_cast(cbvar)->changeConnectionStatus(newStatus);
        }
        else
        {
            cbvar_cast(cbvar)->changeConnectionStatus(newStatus);
        }
    }

    void CNetworkVatlib::onTextMessageReceived(VatSessionID, const char *from, const char *to, const char *msg, void *cbvar)
    {
        BlackMisc::Network::CTextMessageList messages(cbvar_cast(cbvar)->fromFSD(msg), CCallsign(cbvar_cast(cbvar)->fromFSD(from)), CCallsign(cbvar_cast(cbvar)->fromFSD(to)));
        emit cbvar_cast(cbvar)->textMessagesReceived(messages);
    }

    void CNetworkVatlib::onRadioMessageReceived(VatSessionID, const char *from, int numFreq, int *freqList, const char *msg, void *cbvar)
    {
        // FIXME: This method forwards radio message for EVERY frequency. We should only forward those to which our COM's are listening to.
        QList<CFrequency> frequencies;
        for (int i = 0; i < numFreq; ++i)
        {
            frequencies.push_back(CFrequency(freqList[i], CFrequencyUnit::kHz()));
        }
        BlackMisc::Network::CTextMessageList messages(cbvar_cast(cbvar)->fromFSD(msg), frequencies, CCallsign(cbvar_cast(cbvar)->fromFSD(from)));
        emit cbvar_cast(cbvar)->textMessagesReceived(messages);
    }

    void CNetworkVatlib::onPilotDisconnected(VatSessionID, const char *callsign, void *cbvar)
    {
        emit cbvar_cast(cbvar)->pilotDisconnected(cbvar_cast(cbvar)->fromFSD(callsign));
    }

    void CNetworkVatlib::onControllerDisconnected(VatSessionID, const char *callsign, void *cbvar)
    {
        emit cbvar_cast(cbvar)->atcDisconnected(cbvar_cast(cbvar)->fromFSD(callsign));
    }

    void CNetworkVatlib::onPilotPositionUpdate(VatSessionID, const char *callsignChar , const VatPilotPosition *position, void *cbvar)
    {
        const CCallsign callsign(callsignChar);
        const CAircraftSituation situation(
            CCoordinateGeodetic(position->latitude, position->longitude, 0.0),
            CAltitude(position->altitudeTrue, CAltitude::AboveGround, CLengthUnit::ft()),
            CHeading(position->heading, CHeading::True, CAngleUnit::deg()),
            CAngle(position->pitch, CAngleUnit::deg()),
            CAngle(position->bank, CAngleUnit::deg()),
            CSpeed(position->groundSpeed, CSpeedUnit::kts())
        );

        QString tn("transponder ");
        tn.append(callsign.asString());
        CTransponder::TransponderMode mode = CTransponder::StateStandby;
        switch (position->transponderMode)
        {
        case vatTransponderModeCharlie:
            mode = CTransponder::ModeC;
            break;
        case vatTransponderModeStandby:
            mode = CTransponder::StateStandby;
            break;
        case vatTransponderModeIdent:
            mode = CTransponder::StateIdent;
            break;
        default:
            mode = CTransponder::StateStandby;
            break;
        }

        // I did have a situation where I got wrong transponger codes (KB)
        // So I now check for a valid code in order to detect such codes
        CTransponder transponder(tn, 0, mode);
        if (CTransponder::isValidTransponderCode(position->transponderCode))
        {
            transponder = CTransponder(tn, position->transponderCode, mode);
        }
        else
        {
            // TODO: how do with log this
            qDebug() << "Wrong transponder code" << position->transponderMode << callsign;
        }
        emit cbvar_cast(cbvar)->aircraftPositionUpdate(callsign, situation, transponder);
    }

    void CNetworkVatlib::onInterimPilotPositionUpdate(VatSessionID, const char * /** callsign **/, const VatPilotPosition * /** position **/, void * /** cbvar **/)
    {
        //TODO
    }

    void CNetworkVatlib::onAtcPositionUpdate(VatSessionID, const char *callsign, const VatAtcPosition *pos, void *cbvar)
    {
        CFrequency freq(pos->frequency, CFrequencyUnit::kHz());
        freq.switchUnit(CFrequencyUnit::MHz()); // we would not need to bother, but this makes it easier to identify
        emit cbvar_cast(cbvar)->atcPositionUpdate(cbvar_cast(cbvar)->fromFSD(callsign), freq,
                CCoordinateGeodetic(pos->latitude, pos->longitude, 0), CLength(pos->visibleRange, CLengthUnit::NM()));
    }

    void CNetworkVatlib::onKicked(VatSessionID, const char *reason, void *cbvar)
    {
        emit cbvar_cast(cbvar)->kicked(cbvar_cast(cbvar)->fromFSD(reason));
    }

    void CNetworkVatlib::onPong(VatSessionID, const char *sender, double elapsedTime, void *cbvar)
    {
        emit cbvar_cast(cbvar)->pongReceived(cbvar_cast(cbvar)->fromFSD(sender), CTime(elapsedTime, CTimeUnit::ms()));
    }

    void CNetworkVatlib::onCustomPacketReceived(VatSessionID, const char *callsign, const char *packetId, const char **data, int dataSize, void *cbvar)
    {
        emit cbvar_cast(cbvar)->customPacketReceived(cbvar_cast(cbvar)->fromFSD(callsign), cbvar_cast(cbvar)->fromFSD(packetId), cbvar_cast(cbvar)->fromFSD(data, dataSize));
    }

    void CNetworkVatlib::customPacketDispatcher(const BlackMisc::Aviation::CCallsign &callsign, const QString &packetId, const QStringList &data)
    {
        if (packetId.compare("FSIPI", Qt::CaseInsensitive) == 0)
        {
            if (data.size() < 9)
            {
                CLogMessage(this).warning("Malformed FSIPI packet");
            }
            else
            {
                emit fsipiCustomPacketReceived(callsign, data[1], data[2], data[7], data[8]);
            }
        }
        else if (packetId.compare("FSIPIR", Qt::CaseInsensitive) == 0)
        {
            if (data.size() < 9)
            {
                CLogMessage(this).warning("Malformed FSIPIR packet");
            }
            else
            {
                emit fsipirCustomPacketReceived(callsign, data[1], data[2], data[7], data[8]);
            }
        }
    }

    void CNetworkVatlib::onMetarReceived(VatSessionID, const char *data, void *cbvar)
    {
        emit cbvar_cast(cbvar)->metarReplyReceived(cbvar_cast(cbvar)->fromFSD(data));
    }

    void CNetworkVatlib::onInfoQueryRequestReceived(VatSessionID, const char *callsignString, VatInfoQueryType type, const char *, void *cbvar)
    {
        auto timer = new QTimer(cbvar_cast(cbvar));
        timer->setSingleShot(true);
        timer->start(0);

        BlackMisc::Aviation::CCallsign callsign(callsignString);
        switch (type)
        {
        case vatInfoQueryTypeFreq:
            connect(timer, &QTimer::timeout, [ = ]() { cbvar_cast(cbvar)->replyToFrequencyQuery(callsign); });
            break;
        case vatInfoQueryTypeName:
            connect(timer, &QTimer::timeout, [ = ]() { cbvar_cast(cbvar)->replyToNameQuery(callsign); });
            break;
        default:
            break;
        }
    }

    void CNetworkVatlib::onInfoQueryReplyReceived(VatSessionID, const char *callsign, VatInfoQueryType type, const char *data, const char *data2, void *cbvar)
    {
        switch (type)
        {
        case vatInfoQueryTypeFreq:   emit cbvar_cast(cbvar)->frequencyReplyReceived(cbvar_cast(cbvar)->fromFSD(callsign), CFrequency(cbvar_cast(cbvar)->fromFSD(data).toFloat(), CFrequencyUnit::MHz())); break;
        case vatInfoQueryTypeServer: emit cbvar_cast(cbvar)->serverReplyReceived(cbvar_cast(cbvar)->fromFSD(callsign), cbvar_cast(cbvar)->fromFSD(data)); break;
        case vatInfoQueryTypeAtc:    emit cbvar_cast(cbvar)->atcReplyReceived(cbvar_cast(cbvar)->fromFSD(data2), *data == 'Y'); break;
        case vatInfoQueryTypeName:   emit cbvar_cast(cbvar)->realNameReplyReceived(cbvar_cast(cbvar)->fromFSD(callsign), cbvar_cast(cbvar)->fromFSD(data)); break;
        case vatInfoQueryTypeIP:     emit cbvar_cast(cbvar)->ipReplyReceived(cbvar_cast(cbvar)->fromFSD(data)); break;
        default: break;
        }
    }

    void CNetworkVatlib::onCapabilitiesReplyReceived(VatSessionID, const char *callsign, int capabilityFlags, void *cbvar)
    {
        int flags = 0;
        if (capabilityFlags & vatCapsAtcInfo) { flags |= AcceptsAtisResponses; }
        else if (capabilityFlags & vatCapsInterminPos) { flags |= SupportsInterimPosUpdates; }
        else if (capabilityFlags & vatCapsModelDesc) { flags |= SupportsModelDescriptions; }
        emit cbvar_cast(cbvar)->capabilitiesReplyReceived(cbvar_cast(cbvar)->fromFSD(callsign), flags);
    }

    void CNetworkVatlib::onAtisReplyReceived(VatSessionID, const char *callsign, const VatControllerAtis *atis, void *cbvar)
    {
        emit cbvar_cast(cbvar)->atisVoiceRoomReplyReceived(cbvar_cast(cbvar)->fromFSD(callsign), cbvar_cast(cbvar)->fromFSD(atis->voiceRoom));
        emit cbvar_cast(cbvar)->atisLogoffTimeReplyReceived(cbvar_cast(cbvar)->fromFSD(callsign), cbvar_cast(cbvar)->fromFSD(atis->zuluLogoff));

        CInformationMessage atisMessage;
        atisMessage.setType(CInformationMessage::ATIS);
        for (int i = 0; i < atis->textLineCount; ++i)
        {
            const QString fixed = cbvar_cast(cbvar)->fromFSD(atis->textLines[i]).trimmed();
            if (! fixed.isEmpty())
            {
                // detect the stupid z1, z2, z3 placeholders
                // TODO: Anything better as this stupid code here?
                const QString test = fixed.toLower().remove(QRegExp("[\\n\\t\\r]"));
                if (test == "z") return;
                if (test.startsWith("z") && test.length() == 2) return; // z1, z2, ..
                if (test.length() == 1) return; // sometimes just z

                // append
                if (!atisMessage.isEmpty()) atisMessage.appendMessage("\n");
                atisMessage.appendMessage(fixed);
            }
        }

        emit cbvar_cast(cbvar)->atisReplyReceived(cbvar_cast(cbvar)->fromFSD(callsign), atisMessage);

    }

    void CNetworkVatlib::onFlightPlanReceived(VatSessionID, const char *callsign, const VatFlightPlan *fp, void *cbvar)
    {
        BlackMisc::Aviation::CFlightPlan::FlightRules rules = BlackMisc::Aviation::CFlightPlan::VFR;
        switch (fp->flightType)
        {
        default:
        case vatFlightTypeVFR:   rules = BlackMisc::Aviation::CFlightPlan::VFR;  break;
        case vatFlightTypeIFR:   rules = BlackMisc::Aviation::CFlightPlan::IFR;  break;
//        case Cvatlib_Network::fpRuleType_SVFR:  rules = BlackMisc::Aviation::CFlightPlan::SVFR; break;
        }

        auto cruiseAltString = cbvar_cast(cbvar)->fromFSD(fp->cruiseAltitude);
        static const QRegExp withUnit("\\D+");
        if (!cruiseAltString.isEmpty() && withUnit.indexIn(cruiseAltString) < 0)
        {
            cruiseAltString += "ft";
        }
        BlackMisc::Aviation::CAltitude cruiseAlt;
        cruiseAlt.parseFromString(cruiseAltString);


        QString depTimePlanned = QString("0000").append(QString::number(fp->departTime)).right(4);
        QString depTimeActual = QString("0000").append(QString::number(fp->departTimeActual)).right(4);

        BlackMisc::Aviation::CFlightPlan flightPlan(
            cbvar_cast(cbvar)->fromFSD(fp->aircraftType),
            cbvar_cast(cbvar)->fromFSD(fp->departAirport),
            cbvar_cast(cbvar)->fromFSD(fp->destAirport),
            cbvar_cast(cbvar)->fromFSD(fp->alternateAirport),
            QDateTime::fromString(depTimePlanned, "hhmm"),
            QDateTime::fromString(depTimeActual, "hhmm"),
            BlackMisc::PhysicalQuantities::CTime(fp->enrouteHrs * 60 + fp->enrouteMins, BlackMisc::PhysicalQuantities::CTimeUnit::min()),
            BlackMisc::PhysicalQuantities::CTime(fp->fuelHrs * 60 + fp->fuelMins, BlackMisc::PhysicalQuantities::CTimeUnit::min()),
            cruiseAlt,
            BlackMisc::PhysicalQuantities::CSpeed(fp->trueCruisingSpeed, BlackMisc::PhysicalQuantities::CSpeedUnit::kts()),
            rules,
            cbvar_cast(cbvar)->fromFSD(fp->route),
            cbvar_cast(cbvar)->fromFSD(fp->remarks)
        );

        emit cbvar_cast(cbvar)->flightPlanReplyReceived(callsign, flightPlan);
    }

    void CNetworkVatlib::onTemperatureDataReceived(VatSessionID, const VatTempLayer /** layer **/ [4], int /** pressure **/, void * /** cbvar **/)
    {
        //TODO
    }

    void CNetworkVatlib::onErrorReceived(VatSessionID, VatServerError error, const char *msg, const char *data, void *cbvar)
    {
        auto *self = cbvar_cast(cbvar);
        switch (error)
        {
        case vatServerErrorCsInUs:          CLogMessage(self).error("The requested callsign is already taken"); emit self->terminate(); break;
        case vatServerErrorCallsignInvalid: CLogMessage(self).error("The requested callsign is not valid"); emit self->terminate(); break;
        case vatServerErrorCidInvalid:      CLogMessage(self).error("Wrong user ID or password"); emit self->terminate(); break;
        case vatServerErrorRevision:        CLogMessage(self).error("This server does not support our protocol version"); emit self->terminate(); break;
        case vatServerErrorLevel:           CLogMessage(self).error("You are not authorized to use the requested pilot rating"); emit self->terminate(); break;
        case vatServerErrorServFull:        CLogMessage(self).error("The server is full"); emit self->terminate(); break;
        case vatServerErrorCsSuspended:     CLogMessage(self).error("Your user account is suspended"); emit self->terminate(); break;
        case vatServerErrorInvPos:          CLogMessage(self).error("You are not authorized to use the requested rating"); emit self->terminate(); break;
        case vatServerErrorUnAuth:          CLogMessage(self).error("This software is not authorized for use on this network"); emit self->terminate(); break;

        case vatServerErrorNone:            CLogMessage(self).info("OK"); break;
        case vatServerErrorSyntax:          CLogMessage(self).info("Malformed packet: Syntax error: %1") << self->fromFSD(data); break;
        case vatServerErrorSrcInvalid:      CLogMessage(self).info("Server: source invalid %1") << self->fromFSD(data); break;
        case vatServerErrorNoSuchCs:        CLogMessage(self).info("Shim lib: %1 (%2)") << self->fromFSD(msg) << self->fromFSD(data); break;
        case vatServerErrorNoFP:            CLogMessage(self).info("Server: no flight plan"); break;
        case vatServerErrorNoWeather:       CLogMessage(self).info("Server: requested weather profile does not exist"); break;

        // we have no idea what these mean
        case vatServerErrorRegistered:
        case vatServerErrorInvalidCtrl:     CLogMessage(self).info("Server: ") << self->fromFSD(msg); break;

        default:                            qFatal("vatlib: %s (error %d)", msg, error); emit self->terminate();
        }
    }

    void CNetworkVatlib::onWindDataReceived(VatSessionID, const VatWindLayer /** layer **/ [4], void * /** cbvar **/)
    {
        //TODO
    }

    void CNetworkVatlib::onCloudDataReceived(VatSessionID,const VatCloudLayer /** layers **/ [2], VatThunderStormLayer /** storm **/, float /** vis **/, void * /** cbvar **/)
    {
        //TODO
    }

    void CNetworkVatlib::onPilotInfoRequestReceived(VatSessionID, const char *callsignString, void *cbvar)
    {
        auto timer = new QTimer(cbvar_cast(cbvar));
        timer->setSingleShot(true);
        timer->start(0);

        BlackMisc::Aviation::CCallsign callsign(callsignString);
        connect(timer, &QTimer::timeout, [ = ]() { cbvar_cast(cbvar)->sendAircraftInfo(callsign); });
    }

    void CNetworkVatlib::onPilotInfoReceived(VatSessionID, const char *callsign, const VatAircraftInfo *aircraftInfo, void *cbvar)
    {
        BlackMisc::Aviation::CAircraftIcao icao;
        icao.setAircraftDesignator(aircraftInfo->aircraftType);
        icao.setAirlineDesignator(aircraftInfo->airline);
        icao.setLivery(aircraftInfo->livery);
        emit cbvar_cast(cbvar)->icaoCodesReplyReceived(cbvar_cast(cbvar)->fromFSD(callsign), icao);
    }

    void CNetworkVatlib::networkErrorHandler(const char *message)
    {
        CLogMessage(static_cast<CNetworkVatlib*>(nullptr)).error(message);
    }

} // namespace
