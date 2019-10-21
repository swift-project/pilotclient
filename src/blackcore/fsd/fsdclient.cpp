/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "fsdclient.h"

#include "blackcore/application.h"
#include "blackcore/fsd/addatc.h"
#include "blackcore/fsd/addpilot.h"
#include "blackcore/fsd/atcdataupdate.h"
#include "blackcore/fsd/authchallenge.h"
#include "blackcore/fsd/authresponse.h"
#include "blackcore/fsd/clientidentification.h"
#include "blackcore/fsd/deleteatc.h"
#include "blackcore/fsd/deletepilot.h"
#include "blackcore/fsd/pilotdataupdate.h"
#include "blackcore/fsd/ping.h"
#include "blackcore/fsd/pong.h"
#include "blackcore/fsd/killrequest.h"
#include "blackcore/fsd/textmessage.h"
#include "blackcore/fsd/clientquery.h"
#include "blackcore/fsd/clientresponse.h"
#include "blackcore/fsd/flightplan.h"
#include "blackcore/fsd/fsdidentification.h"
#include "blackcore/fsd/serializer.h"
#include "blackcore/fsd/servererror.h"
#include "blackcore/fsd/interimpilotdataupdate.h"
#include "blackcore/fsd/planeinforequest.h"
#include "blackcore/fsd/planeinformation.h"
#include "blackcore/fsd/planeinforequestfsinn.h"
#include "blackcore/fsd/planeinformationfsinn.h"

#include "blackmisc/aviation/flightplan.h"
#include "blackmisc/network/rawfsdmessage.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/range.h"
#include "blackmisc/verify.h"

#include "blackconfig/buildconfig.h"

#include <QHostAddress>
#include <QStringBuilder>
#include <QStringView>

using namespace BlackConfig;
using namespace BlackCore::Vatsim;
using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Json;
using namespace BlackMisc::Network;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;

namespace BlackCore
{
    namespace Fsd
    {
        QString convertToUnicodeEscaped(const QString &str)
        {
            QString escaped;
            for (const auto &ch : str)
            {
                const ushort code = ch.unicode();
                if (code < 0x80)
                {
                    escaped += ch;
                }
                else
                {
                    escaped += "\\u" % QString::number(code, 16).rightJustified(4, '0');
                }
            }
            return escaped;
        }

        CFSDClient::CFSDClient(IClientProvider *clientProvider,
                               IOwnAircraftProvider *ownAircraftProvider,
                               IRemoteAircraftProvider *remoteAircraftProvider,
                               QObject *parent)
            : QObject(parent),
              CClientAware(clientProvider),
              COwnAircraftAware(ownAircraftProvider),
              CRemoteAircraftAware(remoteAircraftProvider),
              m_tokenBucket(10, CTime(5, CTimeUnit::s()), 1)
        {
            initializeMessageTypes();
            connect(&m_socket, &QTcpSocket::readyRead, this, &CFSDClient::readDataFromSocket);
            connect(&m_socket, qOverload<QAbstractSocket::SocketError>(&QTcpSocket::error), this, &CFSDClient::printSocketError, Qt::QueuedConnection);
            connect(&m_socket, qOverload<QAbstractSocket::SocketError>(&QTcpSocket::error), this, &CFSDClient::printSocketError, Qt::QueuedConnection);

            m_positionUpdateTimer.setObjectName(this->objectName().append(":m_positionUpdateTimer"));
            connect(&m_positionUpdateTimer, &QTimer::timeout, this, &CFSDClient::sendPilotDataUpdate);

            m_interimPositionUpdateTimer.setObjectName(this->objectName().append(":m_interimPositionUpdateTimer"));
            connect(&m_interimPositionUpdateTimer, &QTimer::timeout, this, &CFSDClient::sendInterimPilotDataUpdate);

            connect(&m_scheduledConfigUpdate, &QTimer::timeout, this, &CFSDClient::sendIncrementalAircraftConfig);

            fsdMessageSettingsChanged();

            if (!m_statistics && (CBuildConfig::isLocalDeveloperDebugBuild() || (sApp && sApp->getOwnDistribution().isRestricted())))
            {
                CLogMessage("Enabled network statistics");
                m_statistics = true;
            }
        }

        void CFSDClient::setClientIdAndKey(quint16 id, const QByteArray &key)
        {
            clientAuth = vatsim_auth_create(id, qPrintable(key));
            serverAuth = vatsim_auth_create(id, qPrintable(key));
        }

        void CFSDClient::setServer(const CServer &server)
        {
            Q_ASSERT_X(m_connectionStatus.isDisconnected(), Q_FUNC_INFO, "Can't change server details while still connected");

            m_server = server;

            if (m_server.getServerType() == CServer::FSDServerVatsim)
            {
                m_protocolRevision = PROTOCOL_REVISION_VATSIM_AUTH;
            }
            else
            {
                m_protocolRevision = PROTOCOL_REVISION_CLASSIC;
            }

            const QString codecName(server.getFsdSetup().getTextCodec());
            Q_ASSERT_X(!codecName.isEmpty(), Q_FUNC_INFO, "Missing code name");
            m_fsdTextCodec = QTextCodec::codecForName(codecName.toLocal8Bit());
            if (!m_fsdTextCodec) { m_fsdTextCodec = QTextCodec::codecForName("utf-8"); }
        }

        void CFSDClient::setSimulatorInfo(const CSimulatorPluginInfo &simInfo)
        {
            Q_ASSERT_X(m_connectionStatus.isDisconnected(), Q_FUNC_INFO, "Can't change server details while still connected");
            m_simulatorInfo = simInfo;
        }

        void CFSDClient::setCallsign(const CCallsign &callsign)
        {
            Q_ASSERT_X(m_connectionStatus.isDisconnected(), Q_FUNC_INFO, "Can't change callsign while still connected");
            m_ownCallsign = callsign;
            updateOwnCallsign(callsign);
        }

        void CFSDClient::setIcaoCodes(const CSimulatedAircraft &ownAircraft)
        {
            Q_ASSERT_X(m_connectionStatus.isDisconnected(), Q_FUNC_INFO, "Can't change ICAO codes while still connected");
            m_ownAircraftIcaoCode  = ownAircraft.getAircraftIcaoCode();
            m_ownAirlineIcaoCode   = ownAircraft.getAirlineIcaoCode();
            m_ownLivery = ownAircraft.getModel().getSwiftLiveryString();
            m_ownModelString = ownAircraft.getModelString();
            m_sendLiveryString = true;
            m_sendMModelString = true;
            updateOwnIcaoCodes(m_ownAircraftIcaoCode, m_ownAirlineIcaoCode);
        }

        void CFSDClient::setLiveryAndModelString(const QString &livery, bool sendLiveryString, const QString &modelString, bool sendModelString)
        {
            m_ownLivery = livery;
            m_ownModelString = modelString;
            m_sendLiveryString = sendLiveryString;
            m_sendMModelString = sendModelString;
        }

        void CFSDClient::setSimType(const CSimulatorPluginInfo &simInfo)
        {
            //! \fixme Define recognized simulators somewhere */
            const CSimulatorInfo::Simulator sim = simInfo.getSimulatorInfo().getSimulator();
            switch (sim)
            {
            case CSimulatorInfo::FSX: m_simType = SimType::MSFSX; break;
            case CSimulatorInfo::P3D: m_simType = SimType::P3Dv4; break;
            case CSimulatorInfo::FS9: m_simType = SimType::MSFS2004;    break;
            case CSimulatorInfo::FG:  m_simType = SimType::FlightGear;  break;
            case CSimulatorInfo::XPLANE: m_simType = SimType::XPLANE11; break;
            default: m_simType = SimType::Unknown; break;
            }
        }

        QStringList CFSDClient::getPresetValues() const
        {
            const QStringList v =
            {
                m_ownModelString,
                m_ownLivery,
                m_ownAircraftIcaoCode.getDesignator(),
                m_ownAirlineIcaoCode.getVDesignator(),
                m_ownCallsign.asString(),
                m_partnerCallsign.asString()
            };
            return v;
        }

        void CFSDClient::connectToServer()
        {
            if (m_socket.isOpen()) { return; }
            Q_ASSERT(!m_clientName.isEmpty());
            Q_ASSERT((m_versionMajor + m_versionMinor) > 0);
            Q_ASSERT(m_capabilities != Capabilities::None);

            if (m_hostApplication.isEmpty()) { m_hostApplication = getSimulatorNameAndVersion().replace(':', ' '); }

            clearState();
            m_filterPasswordFromLogin = true;

            updateConnectionStatus(CConnectionStatus::Connecting);

            QString host = m_server.getAddress();
            quint16 port = static_cast<quint16>(m_server.getPort());
            m_socket.connectToHost(host, port);
            startPositionTimers();
        }

        void CFSDClient::disconnectFromServer()
        {
            this->stopPositionTimers();
            this->updateConnectionStatus(CConnectionStatus::Disconnecting);

            // allow also to close if broken
            if (!m_socket.isOpen())
            {
                if (m_loginMode.isPilot()) { sendDeletePilot(); }
                else if (m_loginMode.isObserver()) { sendDeleteAtc(); }
            }
            m_socket.close();

            this->updateConnectionStatus(CConnectionStatus::Disconnected);
            this->clearState();
        }

        void CFSDClient::sendLogin()
        {
            const QString cid = m_server.getUser().getId();
            const QString password = m_server.getUser().getPassword();
            const QString name = m_server.getUser().getRealNameAndHomeBase();
            // const QString name = m_server.getUser().getRealName();

            if (m_loginMode.isPilot())
            {
                const AddPilot pilotLogin(m_ownCallsign.asString(), cid, password, m_pilotRating, m_protocolRevision, m_simType, name);
                sendMessage(pilotLogin);
            }
            else if (m_loginMode.isObserver())
            {
                const AddAtc addAtc(m_ownCallsign.asString(), name, cid, password, m_atcRating, m_protocolRevision);
                sendMessage(addAtc);
            }
        }

        void CFSDClient::sendDeletePilot()
        {
            const QString cid = m_server.getUser().getId();
            const DeletePilot deletePilot(m_ownCallsign.getFsdCallsignString(), cid);
            sendMessage(deletePilot);
        }

        void CFSDClient::sendDeleteAtc()
        {
            const QString cid = m_server.getUser().getId();
            const DeleteAtc deleteAtc(m_ownCallsign.asString(), cid);
            sendMessage(deleteAtc);
        }

        void CFSDClient::sendPilotDataUpdate()
        {
            if (m_connectionStatus.isDisconnected() && ! m_unitTestMode) { return; }
            const CSimulatedAircraft myAircraft(getOwnAircraft());
            if (m_loginMode == BlackMisc::Network::CLoginMode::Observer)
            {
                sendAtcDataUpdate(myAircraft.latitude().value(CAngleUnit::deg()), myAircraft.longitude().value(CAngleUnit::deg()));
            }
            else
            {
                PilotDataUpdate pilotDataUpdate(myAircraft.getTransponderMode(),
                                                m_ownCallsign.asString(),
                                                static_cast<qint16>(myAircraft.getTransponderCode()),
                                                PilotRating::Unknown,
                                                myAircraft.latitude().value(CAngleUnit::deg()),
                                                myAircraft.longitude().value(CAngleUnit::deg()),
                                                myAircraft.getAltitude().valueInteger(CLengthUnit::ft()),
                                                myAircraft.getPressureAltitude().valueInteger(CLengthUnit::ft()),
                                                myAircraft.getGroundSpeed().valueInteger(CSpeedUnit::kts()),
                                                myAircraft.getPitch().value(CAngleUnit::deg()),
                                                myAircraft.getBank().value(CAngleUnit::deg()),
                                                myAircraft.getHeading().value(CAngleUnit::deg()),
                                                myAircraft.getParts().isOnGround());
                sendMessage(pilotDataUpdate);
            }
        }

        void CFSDClient::sendInterimPilotDataUpdate()
        {
            if (m_connectionStatus.isDisconnected()) { return; }
            const CSimulatedAircraft myAircraft(getOwnAircraft());
            InterimPilotDataUpdate interimPilotDataUpdate(m_ownCallsign.asString(),
                    QString(),
                    myAircraft.latitude().value(CAngleUnit::deg()),
                    myAircraft.longitude().value(CAngleUnit::deg()),
                    myAircraft.getAltitude().valueInteger(CLengthUnit::ft()),
                    myAircraft.getGroundSpeed().valueInteger(CSpeedUnit::kts()),
                    myAircraft.getPitch().value(CAngleUnit::deg()),
                    myAircraft.getBank().value(CAngleUnit::deg()),
                    myAircraft.getHeading().value(CAngleUnit::deg()),
                    myAircraft.getParts().isOnGround());

            for (const auto &receiver : as_const(m_interimPositionReceivers))
            {
                interimPilotDataUpdate.setReceiver(receiver.asString());
                sendMessage(interimPilotDataUpdate);
                // statistics
            }
        }

        void CFSDClient::sendAtcDataUpdate(double latitude, double longitude)
        {
            const AtcDataUpdate atcDataUpdate(m_ownCallsign.asString(), 199998, CFacilityType::OBS, 300, AtcRating::Observer, latitude, longitude, 0);
            sendMessage(atcDataUpdate);
        }

        void CFSDClient::sendPing(const QString &receiver)
        {
            const qint64 msecSinceEpoch = QDateTime::currentMSecsSinceEpoch();
            const QString timeString = QString::number(msecSinceEpoch);

            const Ping ping(m_ownCallsign.asString(), receiver, timeString);
            sendMessage(ping);

            // statistics
            this->increaseStatisticsValue(QStringLiteral("sendPing"));
        }

        void CFSDClient::sendClientQueryIsValidAtc(const CCallsign &callsign)
        {
            sendClientQuery(ClientQueryType::IsValidATC, {}, { callsign.asString() });
        }

        void CFSDClient::sendClientQueryCapabilities(const CCallsign &callsign)
        {
            sendClientQuery(ClientQueryType::Capabilities, callsign);
        }

        void CFSDClient::sendClientQueryCom1Freq(const CCallsign &callsign)
        {
            sendClientQuery(ClientQueryType::Com1Freq, callsign);
        }

        void CFSDClient::sendClientQueryRealName(const CCallsign &callsign)
        {
            sendClientQuery(ClientQueryType::RealName, callsign);
        }

        void CFSDClient::sendClientQueryServer(const CCallsign &callsign)
        {
            sendClientQuery(ClientQueryType::Server, callsign);
        }

        void CFSDClient::sendClientQueryAtis(const CCallsign &callsign)
        {
            sendClientQuery(ClientQueryType::ATIS, callsign);
        }

        void CFSDClient::sendClientQueryFlightPlan(const CCallsign callsign)
        {
            sendClientQuery(ClientQueryType::FP, {}, { callsign.toQString() });
        }

        void CFSDClient::sendClientQueryAircraftConfig(const CCallsign callsign)
        {
            QString data = QJsonDocument(JsonPackets::aircraftConfigRequest()).toJson(QJsonDocument::Compact);
            data = convertToUnicodeEscaped(data);
            sendClientQuery(ClientQueryType::AircraftConfig, callsign, { data });
        }

        void CFSDClient::sendClientQuery(ClientQueryType queryType, const CCallsign &receiver, const QStringList &queryData)
        {
            if (queryType == ClientQueryType::Unknown) { return; }

            const QString reveiverCallsign = receiver.getFsdCallsignString();
            if (queryType == ClientQueryType::IsValidATC)
            {
                const ClientQuery clientQuery(m_ownCallsign.asString(), "SERVER", ClientQueryType::IsValidATC, queryData);
                sendMessage(clientQuery);
            }
            else if (queryType == ClientQueryType::Capabilities)
            {
                const ClientQuery clientQuery(m_ownCallsign.asString(), reveiverCallsign, ClientQueryType::Capabilities);
                sendMessage(clientQuery);
            }
            else if (queryType == ClientQueryType::Com1Freq)
            {
                const ClientQuery clientQuery(m_ownCallsign.asString(), reveiverCallsign, ClientQueryType::Com1Freq);
                sendMessage(clientQuery);
            }
            else if (queryType == ClientQueryType::RealName)
            {
                const ClientQuery clientQuery(m_ownCallsign.asString(), reveiverCallsign, ClientQueryType::RealName);
                sendMessage(clientQuery);
            }
            else if (queryType == ClientQueryType::Server)
            {
                ClientQuery clientQuery(m_ownCallsign.asString(), reveiverCallsign, ClientQueryType::Server);
                sendMessage(clientQuery);
            }
            else if (queryType == ClientQueryType::ATIS)
            {
                const ClientQuery clientQuery(m_ownCallsign.asString(), reveiverCallsign, ClientQueryType::ATIS);
                sendMessage(clientQuery);
                if (m_serverType != ServerType::Vatsim)
                {
                    m_pendingAtisQueries.insert(receiver, {});
                }
            }
            else if (queryType == ClientQueryType::PublicIP)
            {
                const ClientQuery clientQuery(m_ownCallsign.asString(), reveiverCallsign, ClientQueryType::PublicIP);
                sendMessage(clientQuery);
            }
            else if (queryType == ClientQueryType::INF)
            {
                const ClientQuery clientQuery(m_ownCallsign.asString(), reveiverCallsign, ClientQueryType::INF);
                sendMessage(clientQuery);
            }
            else if (queryType == ClientQueryType::FP)
            {
                if (queryData.size() == 0) { return; }
                ClientQuery clientQuery(m_ownCallsign.asString(), "SERVER", ClientQueryType::FP, queryData);
                sendMessage(clientQuery);
            }
            if (queryType == ClientQueryType::AircraftConfig)
            {
                if (queryData.size() == 0) { return; }
                const ClientQuery clientQuery(m_ownCallsign.asString(), reveiverCallsign, ClientQueryType::AircraftConfig, queryData);
                sendMessage(clientQuery);
            }

            this->increaseStatisticsValue(QStringLiteral("sendClientQuery"), toQString(queryType));
        }

        void CFSDClient::sendTextMessages(const CTextMessageList &messages)
        {
            if (messages.isEmpty()) { return; }

            const CTextMessageList privateMessages = messages.getPrivateMessages().markedAsSent();
            for (const auto &message : privateMessages)
            {
                if (message.getRecipientCallsign().isEmpty()) { continue; }
                const TextMessage textMessage(m_ownCallsign.asString(), message.getRecipientCallsign().getFsdCallsignString(), message.getMessage());
                sendMessage(textMessage);
                emit this->textMessageSent(message);

                this->increaseStatisticsValue(QStringLiteral("sendTextMessages"));
            }

            const CTextMessageList radioMessages = messages.getRadioMessages().markedAsSent();
            QVector<int> frequencies;
            for (const auto &message : radioMessages)
            {
                // I could send the same message to n frequencies in one step
                // if this is really required, I need to group by message
                // currently I send individual messages
                frequencies.clear();
                int freqkHz = message.getFrequency().valueInteger(CFrequencyUnit::kHz());
                if (m_server.getServerType() == CServer::FSDServerVatsim)
                {
                    // VATSIM always drops the last 5 kHz.
                    freqkHz = freqkHz / 10 * 10;
                }
                frequencies.push_back(freqkHz);
                sendRadioMessage(frequencies, message.getMessage());
                emit this->textMessageSent(message);
            }
        }

        void CFSDClient::sendTextMessage(const CTextMessage &message)
        {
            sendTextMessages({message});
        }

        void CFSDClient::sendTextMessage(TextMessageGroups receiverGroup, const QString &message)
        {
            QString receiver;
            if (receiverGroup == TextMessageGroups::AllClients)           { receiver = '*'; }
            else if (receiverGroup == TextMessageGroups::AllAtcClients)   { receiver = QStringLiteral("*A"); }
            else if (receiverGroup == TextMessageGroups::AllPilotClients) { receiver = QStringLiteral("*P"); }
            else if (receiverGroup == TextMessageGroups::AllSups)         { receiver = QStringLiteral("*S"); }
            else { return; }
            const TextMessage textMessage(m_ownCallsign.asString(), receiver, message);
            sendMessage(textMessage);
            this->increaseStatisticsValue(QStringLiteral("sendTextMessages"));
        }

        void CFSDClient::sendTextMessage(const QString &receiver, const QString &message)
        {
            const CTextMessage msg(message, getOwnCallsign(), { receiver });
            sendTextMessage(msg);
        }

        void CFSDClient::sendRadioMessage(const QVector<int> &frequencieskHz, const QString &message)
        {
            QStringList receivers;
            for (const int &frequency : frequencieskHz)
            {
                receivers.push_back(QStringLiteral("@%1").arg(frequency - 100000));
            }

            const TextMessage radioMessage(m_ownCallsign.asString(), receivers.join('&'), message);
            sendMessage(radioMessage);
            this->increaseStatisticsValue(QStringLiteral("sendTextMessages"));
        }

        void CFSDClient::sendFlightPlan(const CFlightPlan &flightPlan)
        {
            // Removed with T353 although it is standard
            // const QString route = QString(flightPlan.getRoute()).replace(" ", ".");

            QString route   = flightPlan.getRoute();
            QString remarks = flightPlan.getRemarks();
            route.remove(':');
            remarks.remove(':');

            //! \fixme that would be the official string, can this be used?
            const QString alt = flightPlan.getCruiseAltitude().asFpVatsimAltitudeString();
            // const QString alt = flightPlan.getCruiseAltitude().asFpAltitudeString();

            QString act = flightPlan.getCombinedPrefixIcaoSuffix();
            if (act.isEmpty()) { act = flightPlan.getAircraftIcao().getDesignator(); } // fallback

            FlightType flightType = FlightType::IFR;
            switch (flightPlan.getFlightRules())
            {
            case CFlightPlan::IFR:  flightType = FlightType::IFR;  break;
            case CFlightPlan::VFR:  flightType = FlightType::VFR;  break;
            case CFlightPlan::SVFR: flightType = FlightType::SVFR; break;
            case CFlightPlan::DVFR: flightType = FlightType::DVFR; break;
            default:                flightType = FlightType::IFR;  break;
            }

            const QList<int> timePartsEnroute = flightPlan.getEnrouteTime().getHrsMinSecParts();
            const QList<int> timePartsFuel    = flightPlan.getFuelTime().getHrsMinSecParts();
            const FlightPlan fp(m_ownCallsign.asString(), "SERVER", flightType, act,
                                flightPlan.getCruiseTrueAirspeed().valueInteger(CSpeedUnit::kts()),
                                flightPlan.getOriginAirportIcao().asString(),
                                flightPlan.getTakeoffTimePlanned().toUTC().toString("hhmm").toInt(),
                                flightPlan.getTakeoffTimeActual().toUTC().toString("hhmm").toInt(),
                                alt,
                                flightPlan.getDestinationAirportIcao().asString(),
                                timePartsEnroute[CTime::Hours],
                                timePartsEnroute[CTime::Minutes],
                                timePartsFuel[CTime::Hours],
                                timePartsFuel[CTime::Minutes],
                                flightPlan.getAlternateAirportIcao().asString(),
                                remarks,
                                route);

            sendMessage(fp);
            this->increaseStatisticsValue(QStringLiteral("sendFlightPlan"));
        }

        void CFSDClient::sendPlaneInfoRequest(const CCallsign &receiver)
        {
            PlaneInfoRequest planeInfoRequest(m_ownCallsign.asString(), receiver.toQString());
            sendMessage(planeInfoRequest);
            this->increaseStatisticsValue(QStringLiteral("sendPlaneInfoRequest"));
        }

        void CFSDClient::sendPlaneInfoRequestFsinn(const CCallsign &callsign)
        {
            Q_ASSERT_X(isConnected(), Q_FUNC_INFO, "Can't send to server when disconnected");
            const CSimulatedAircraft myAircraft(getOwnAircraft());
            QString modelString = m_ownModelString.isEmpty() ? myAircraft.getModelString() : m_ownModelString;
            if (modelString.isEmpty()) { modelString = noModelString(); }

            const PlaneInfoRequestFsinn planeInfoRequestFsinn(m_ownCallsign.asString(), callsign.toQString(),
                    myAircraft.getAirlineIcaoCodeDesignator(),
                    myAircraft.getAircraftIcaoCodeDesignator(),
                    myAircraft.getAircraftIcaoCombinedType(),
                    m_sendMModelString ? modelString : QString());
            sendMessage(planeInfoRequestFsinn);
            this->increaseStatisticsValue(QStringLiteral("sendPlaneInfoRequestFsinn"));
        }

        void CFSDClient::sendPlaneInformation(const QString &receiver, const QString &aircraft, const QString &airline, const QString &livery)
        {
            PlaneInformation planeInformation(m_ownCallsign.asString(), receiver, aircraft, airline, livery);
            sendMessage(planeInformation);
            this->increaseStatisticsValue(QStringLiteral("sendPlaneInformation"));
        }

        void CFSDClient::sendPlaneInformationFsinn(const CCallsign &callsign)
        {
            if (m_connectionStatus.isDisconnected() && ! m_unitTestMode) { return; }
            const CSimulatedAircraft myAircraft(getOwnAircraft());
            QString modelString = m_ownModelString.isEmpty() ? myAircraft.getModelString() : m_ownModelString;
            if (modelString.isEmpty()) { modelString = noModelString(); }

            PlaneInformationFsinn planeInformationFsinn(m_ownCallsign.asString(), callsign.toQString(),
                    myAircraft.getAirlineIcaoCodeDesignator(),
                    myAircraft.getAircraftIcaoCodeDesignator(),
                    myAircraft.getAircraftIcaoCombinedType(),
                    m_sendMModelString ? modelString : QString());
            sendMessage(planeInformationFsinn);
            this->increaseStatisticsValue(QStringLiteral("sendPlaneInformationFsinn"));
        }

        void CFSDClient::sendAircraftConfiguration(const QString &receiver, const QString &aircraftConfigJson)
        {
            sendClientQuery(ClientQueryType::AircraftConfig, receiver, { aircraftConfigJson });
        }

        void CFSDClient::sendFsdMessage(const QString &message)
        {
            parseMessage(message);
        }

        void CFSDClient::sendAuthChallenge(const QString &challenge)
        {
            AuthChallenge pduAuthChallenge(m_ownCallsign.asString(), "SERVER", challenge);
            sendMessage(pduAuthChallenge);
            this->increaseStatisticsValue(QStringLiteral("sendAuthChallenge"));
        }

        void CFSDClient::sendAuthResponse(const QString &response)
        {
            AuthResponse pduAuthResponse(m_ownCallsign.asString(), "SERVER", response);
            sendMessage(pduAuthResponse);
            this->increaseStatisticsValue(QStringLiteral("sendAuthResponse"));
        }

        void CFSDClient::sendPong(const QString &receiver, const QString &timestamp)
        {
            Pong pong(m_ownCallsign.asString(), receiver, timestamp);
            sendMessage(pong);
            this->increaseStatisticsValue(QStringLiteral("sendPong"));
        }

        void CFSDClient::sendClientResponse(ClientQueryType queryType, const QString &receiver)
        {
            QStringList responseData;
            if (queryType == ClientQueryType::IsValidATC)
            {
                this->handleIllegalFsdState("Never use sendClientResponse with IsValidATC from the client");
                return;
            }
            else if (queryType == ClientQueryType::Capabilities)
            {
                responseData.clear();
                if (m_capabilities & Capabilities::AtcInfo)
                    responseData.push_back(toQString(Capabilities::AtcInfo) % "=1");
                if (m_capabilities & Capabilities::SecondaryPos)
                    responseData.push_back(toQString(Capabilities::SecondaryPos) % "=1");
                if (m_capabilities & Capabilities::AircraftInfo)
                    responseData.push_back(toQString(Capabilities::AircraftInfo) % "=1");
                if (m_capabilities & Capabilities::OngoingCoord)
                    responseData.push_back(toQString(Capabilities::OngoingCoord) % "=1");
                if (m_capabilities & Capabilities::InterminPos)
                    responseData.push_back(toQString(Capabilities::InterminPos) % "=1");
                if (m_capabilities & Capabilities::FastPos)
                    responseData.push_back(toQString(Capabilities::FastPos) % "=1");
                if (m_capabilities & Capabilities::Stealth)
                    responseData.push_back(toQString(Capabilities::Stealth) % "=1");
                if (m_capabilities & Capabilities::AircraftConfig)
                    responseData.push_back(toQString(Capabilities::AircraftConfig) % "=1");
                const ClientResponse clientResponse(m_ownCallsign.asString(), receiver, ClientQueryType::Capabilities, responseData);
                sendMessage(clientResponse);
            }
            else if (queryType == ClientQueryType::Com1Freq)
            {
                const QString com1Frequency = QString::number(getOwnAircraft().getCom1System().getFrequencyActive().value(CFrequencyUnit::MHz()), 'f', 3);
                responseData.push_back(com1Frequency);
                const ClientResponse pduClientResponse(m_ownCallsign.asString(), receiver, ClientQueryType::Com1Freq, responseData);
                sendMessage(pduClientResponse);
            }
            else if (queryType == ClientQueryType::RealName)
            {
                // real name
                responseData.push_back(m_server.getUser().getRealName());
                // sector file in use (blank if pilot)
                responseData.push_back({});
                // current user rating
                if (m_loginMode.isObserver()) { responseData.push_back(toQString(m_atcRating)); }
                else { responseData.push_back(toQString(m_pilotRating)); }

                const ClientResponse pduClientQueryResponse(m_ownCallsign.asString(), receiver, ClientQueryType::RealName, responseData);
                sendMessage(pduClientQueryResponse);
            }
            else if (queryType == ClientQueryType::Server)
            {
                responseData.push_back(m_server.getAddress());
                ClientResponse pduClientQueryResponse(m_ownCallsign.asString(), receiver, ClientQueryType::Server, responseData);
                sendMessage(pduClientQueryResponse);
            }
            else if (queryType == ClientQueryType::ATIS)
            {
                this->handleIllegalFsdState(QStringLiteral("Dont send '%1' as pilot client!").arg(toQString(ClientQueryType::ATIS)));
                return;
            }
            else if (queryType == ClientQueryType::PublicIP)
            {
                this->handleIllegalFsdState(QStringLiteral("Dont send '%1' as pilot client!").arg(toQString(ClientQueryType::PublicIP)));
                return;
            }
            else if (queryType == ClientQueryType::INF)
            {
                QString userInfo;
                QString cid = m_server.getUser().getId();
                CSimulatedAircraft myAircraft(getOwnAircraft());
                const double latitude = getOwnAircraftPosition().latitude().value(CAngleUnit::deg());
                const double longitude = getOwnAircraftPosition().longitude().value(CAngleUnit::deg());
                const int altitude = getOwnAircraft().getAltitude().valueInteger(CLengthUnit::ft());
                const QString realName = m_server.getUser().getRealName();

                char sysuid[50];
                vatsim_get_system_unique_id(sysuid);

                userInfo += QString("CID=") % cid % " " % m_clientName % " IP=" % m_socket.localAddress().toString() %
                            " SYS_UID=" % sysuid % " FSVER=" % m_hostApplication % " LT=" % QString::number(latitude) %
                            " LO=" % QString::number(longitude) % " AL=" % QString::number(altitude) %
                            " " % realName;

                TextMessage textMessage(m_ownCallsign.asString(), receiver, userInfo);
                sendMessage(textMessage);
            }
            else if (queryType == ClientQueryType::FP)
            {
                this->handleIllegalFsdState(QStringLiteral("Dont send '%1' as pilot client!").arg(toQString(ClientQueryType::FP)));
                return;
            }
            else if (queryType == ClientQueryType::AircraftConfig)
            {
                this->handleIllegalFsdState(QStringLiteral("Dont send '%1' as pilot client!").arg(toQString(ClientQueryType::AircraftConfig)));
                return;
            }
            this->increaseStatisticsValue(QStringLiteral("sendClientResponse"), toQString(queryType));
        }

        void CFSDClient::sendClientIdentification(const QString &fsdChallenge)
        {
            char sysuid[50];
            vatsim_get_system_unique_id(sysuid);
            QString cid = m_server.getUser().getId();
            const ClientIdentification clientIdentification(m_ownCallsign.asString(), vatsim_auth_get_client_id(clientAuth), m_clientName, m_versionMajor, m_versionMinor, cid, sysuid, fsdChallenge);
            this->sendMessage(clientIdentification);
            this->sendLogin();
            this->updateConnectionStatus(CConnectionStatus::Connected);
            this->increaseStatisticsValue(QStringLiteral("sendClientIdentification"));
        }

        void CFSDClient::sendIncrementalAircraftConfig()
        {
            if (!this->isConnected()) { return; }
            if (!this->getSetupForServer().sendAircraftParts()) { return; }
            const CAircraftParts currentParts(this->getOwnAircraftParts());

            // If it hasn't changed, return
            if (m_sentAircraftConfig == currentParts) { return; }

            if (!m_tokenBucket.tryConsume()) { return; }

            const QJsonObject previousConfig = m_sentAircraftConfig.toJson();
            const QJsonObject currentConfig = currentParts.toJson();
            const QJsonObject incrementalConfig = getIncrementalObject(previousConfig, currentConfig);

            const QString dataStr = convertToUnicodeEscaped(QJsonDocument(QJsonObject { { "config", incrementalConfig } }).toJson(QJsonDocument::Compact));

            sendAircraftConfiguration("@94835", dataStr);
            m_sentAircraftConfig = currentParts;
        }

        void CFSDClient::initializeMessageTypes()
        {
            m_messageTypeMapping["#AA"] = MessageType::AddAtc;
            m_messageTypeMapping["#AP"] = MessageType::AddPilot;
            m_messageTypeMapping["%"]   = MessageType::AtcDataUpdate;
            m_messageTypeMapping["$ZC"] = MessageType::AuthChallenge;
            m_messageTypeMapping["$ZR"] = MessageType::AuthResponse;
            m_messageTypeMapping["$ID"] = MessageType::ClientIdentification;
            m_messageTypeMapping["$CQ"] = MessageType::ClientQuery;
            m_messageTypeMapping["$CR"] = MessageType::ClientResponse;
            m_messageTypeMapping["#DA"] = MessageType::DeleteATC;
            m_messageTypeMapping["#DP"] = MessageType::DeletePilot;
            m_messageTypeMapping["$FP"] = MessageType::FlightPlan;
            m_messageTypeMapping["$DI"] = MessageType::FsdIdentification;
            m_messageTypeMapping["$!!"] = MessageType::KillRequest;
            m_messageTypeMapping["@"]   = MessageType::PilotDataUpdate;
            m_messageTypeMapping["$PI"] = MessageType::Ping;
            m_messageTypeMapping["$PO"] = MessageType::Pong;
            m_messageTypeMapping["$ER"] = MessageType::ServerError;
            m_messageTypeMapping["#TM"] = MessageType::TextMessage;
            m_messageTypeMapping["#SB"] = MessageType::PilotClientCom;
        }

        void CFSDClient::handleAtcDataUpdate(const QStringList &tokens)
        {
            const AtcDataUpdate atcDataUpdate = AtcDataUpdate::fromTokens(tokens);
            const QString senderCs = atcDataUpdate.sender();
            const CCallsign cs(senderCs, CCallsign::Atc);

            // Filter non-ATC like OBS stations, like pilots logging in as shared cockpit co-pilots.
            if (atcDataUpdate.m_facility == CFacilityType::Unknown && !cs.isObserverCallsign()) { return; } // like in old version
            if (atcDataUpdate.m_facility == CFacilityType::OBS && !cs.hasSuffix()) { return; }

            CFrequency freq(atcDataUpdate.m_frequencykHz, CFrequencyUnit::kHz());
            freq.switchUnit(CFrequencyUnit::MHz()); // we would not need to bother, but this makes it easier to identify
            const CLength networkRange(atcDataUpdate.m_visibleRange, CLengthUnit::NM());

            const CLength range = fixAtcRange(networkRange, cs);
            const CCoordinateGeodetic position(atcDataUpdate.m_latitude, atcDataUpdate.m_longitude, 0);

            emit this->atcDataUpdateReceived(cs, freq, position, range);
        }

        void CFSDClient::handleAuthChallenge(const QStringList &tokens)
        {
            AuthChallenge authChallenge = AuthChallenge::fromTokens(tokens);
            char response[33];
            vatsim_auth_generate_response(clientAuth, qPrintable(authChallenge.m_challengeKey), response);
            sendAuthResponse(QString(response));

            char challenge[33];
            vatsim_auth_generate_challenge(serverAuth, challenge);
            m_lastServerAuthChallenge = QString(challenge);
            sendAuthChallenge(m_lastServerAuthChallenge);
        }

        void CFSDClient::handleAuthResponse(const QStringList &tokens)
        {
            AuthResponse authResponse = AuthResponse::fromTokens(tokens);

            char expectedResponse[33];
            vatsim_auth_generate_response(serverAuth, qPrintable(m_lastServerAuthChallenge), expectedResponse);
            if (authResponse.m_response != QString(expectedResponse))
            {
                CLogMessage().error(u"The server you are connected to is not a VATSIM server. Disconnecting!");
                disconnectFromServer();
            }
        }

        void CFSDClient::handleDeleteATC(const QStringList &tokens)
        {
            const DeleteAtc deleteAtc = DeleteAtc::fromTokens(tokens);
            emit deleteAtcReceived(deleteAtc.m_cid);
        }

        void CFSDClient::handleDeletePilot(const QStringList &tokens)
        {
            const DeletePilot deletePilot = DeletePilot::fromTokens(tokens);
            const CCallsign cs(deletePilot.sender(), CCallsign::Aircraft);
            clearState(cs);
            emit deletePilotReceived(deletePilot.m_cid);
        }

        void CFSDClient::handleTextMessage(const QStringList &tokens)
        {
            const TextMessage textMessage = TextMessage::fromTokens(tokens);

            const CCallsign sender(textMessage.sender());
            const CCallsign receiver(textMessage.receiver());

            if (textMessage.m_type == TextMessage::PrivateMessage)
            {
                // Other FSD servers send the controller ATIS as text message. The following conditions need to be met:
                // * non-VATSIM server. VATSIM has a specific ATIS message
                // * Receiver callsign must be owner callsign and not any type of broadcast.
                // * We have requested the ATIS of this controller before.
                if (m_server.getServerType() != CServer::FSDServerVatsim &&
                        m_ownCallsign.asString() == textMessage.receiver() &&
                        m_pendingAtisQueries.contains(sender))
                {
                    maybeHandleAtisReply(sender, receiver, textMessage.m_message);
                    return;
                }

                CTextMessage tm(textMessage.m_message, sender, receiver);
                tm.setCurrentUtcTime();
                this->consolidateTextMessage(tm); // emit textMessagesReceived({ tm });
            }
            else if (textMessage.m_type == TextMessage::RadioMessage)
            {
                const CFrequency com1 = getOwnAircraft().getCom1System().getFrequencyActive();
                const CFrequency com2 = getOwnAircraft().getCom2System().getFrequencyActive();
                QList<CFrequency> frequencies;

                for (int freqKhz : textMessage.m_frequencies)
                {
                    CFrequency f(freqKhz, CFrequencyUnit::kHz());
                    // VATSIM always drops the last 5 kHz. So round it to the correct channel spacing.
                    CComSystem::roundToChannelSpacing(f, CComSystem::ChannelSpacing25KHz);
                    if (f == com1 || f == com2)
                    {
                        frequencies.push_back(f);
                    }
                }
                if (frequencies.isEmpty()) { return; }
                CTextMessageList messages(textMessage.m_message, frequencies, CCallsign(textMessage.sender()));
                messages.setCurrentUtcTime();
                emit textMessagesReceived(messages);
            }
        }

        void CFSDClient::handlePilotDataUpdate(const QStringList &tokens)
        {
            PilotDataUpdate dataUpdate = PilotDataUpdate::fromTokens(tokens);
            const CCallsign callsign(dataUpdate.sender(), CCallsign::Aircraft);

            CAircraftSituation situation(
                callsign,
                CCoordinateGeodetic(dataUpdate.m_latitude, dataUpdate.m_longitude, dataUpdate.m_altitudeTrue),
                CHeading(dataUpdate.m_heading, CHeading::True, CAngleUnit::deg()),
                CAngle(dataUpdate.m_pitch, CAngleUnit::deg()),
                CAngle(dataUpdate.m_bank, CAngleUnit::deg()),
                CSpeed(dataUpdate.m_groundSpeed, CSpeedUnit::kts()));
            situation.setPressureAltitude(CAltitude(dataUpdate.m_altitudePressure, CAltitude::MeanSeaLevel, CAltitude::PressureAltitude, CLengthUnit::ft()));
            situation.setOnGround(dataUpdate.m_onGround);

            // Ref T297, default offset time
            situation.setCurrentUtcTime();
            const qint64 offsetTimeMs = receivedPositionFixTsAndGetOffsetTime(situation.getCallsign(), situation.getMSecsSinceEpoch());
            situation.setTimeOffsetMs(offsetTimeMs);

            // I did have a situation where I got wrong transponder codes (KB)
            // So I now check for a valid code in order to detect such codes
            CTransponder transponder;
            if (CTransponder::isValidTransponderCode(dataUpdate.m_transponderCode))
            {
                transponder = CTransponder(dataUpdate.m_transponderCode, dataUpdate.m_transponderMode);
            }
            else
            {
                if (CBuildConfig::isLocalDeveloperDebugBuild())
                {
                    CLogMessage(this).debug(u"Wrong transponder code '%1' for '%2'") << dataUpdate.m_transponderCode << callsign;
                }

                // I set a default: IFR standby is a reasonable default
                transponder = CTransponder(2000, CTransponder::StateStandby);
            }
            emit pilotDataUpdateReceived(situation, transponder);
        }

        void CFSDClient::handlePing(const QStringList &tokens)
        {
            const Ping ping = Ping::fromTokens(tokens);
            sendPong(ping.sender(), ping.m_timestamp);
        }

        void CFSDClient::handlePong(const QStringList &tokens)
        {
            Pong pong = Pong::fromTokens(tokens);
            qint64 msecSinceEpoch = QDateTime::currentMSecsSinceEpoch();
            qint64 elapsedTime = msecSinceEpoch - pong.m_timestamp.toLongLong();
            emit pongReceived(pong.sender(), elapsedTime);
        }

        void CFSDClient::handleKillRequest(const QStringList &tokens)
        {
            KillRequest killRequest = KillRequest::fromTokens(tokens);
            emit killRequestReceived(killRequest.m_reason);
            disconnectFromServer();
        }

        void CFSDClient::handleFlightPlan(const QStringList &tokens)
        {
            FlightPlan fp = FlightPlan::fromTokens(tokens);

            CFlightPlan::FlightRules rules = CFlightPlan::VFR;

            switch (fp.m_flightType)
            {
            case FlightType::VFR:  rules = CFlightPlan::VFR;  break;
            case FlightType::IFR:  rules = CFlightPlan::IFR;  break;
            case FlightType::DVFR: rules = CFlightPlan::DVFR; break;
            case FlightType::SVFR: rules = CFlightPlan::SVFR; break;
            }

            QString cruiseAltString = fp.m_cruiseAlt.trimmed();
            if (!cruiseAltString.isEmpty() && is09OnlyString(cruiseAltString))
            {
                int ca = cruiseAltString.toInt();
                // we have a 0-9 only string
                // we assume values like 24000 as FL
                // RefT323, also major tool such as PFPX and Simbrief do so
                if (rules == CFlightPlan::IFR)
                {
                    if (ca >= 1000)
                    {
                        cruiseAltString = u"FL" % QString::number(ca / 100);
                    }
                    else
                    {
                        cruiseAltString = u"FL" % cruiseAltString;
                    }
                }
                else // VFR
                {
                    if (ca >= 5000)
                    {
                        cruiseAltString = u"FL" % QString::number(ca / 100);
                    }
                    else
                    {
                        cruiseAltString = cruiseAltString % u"ft";
                    }
                }
            }
            CAltitude cruiseAlt;
            cruiseAlt.parseFromString(cruiseAltString, CPqString::SeparatorBestGuess);

            const QString depTimePlanned = QStringLiteral("0000").append(QString::number(fp.m_estimatedDepTime)).right(4);
            const QString depTimeActual  = QStringLiteral("0000").append(QString::number(fp.m_actualDepTime)).right(4);

            const CCallsign callsign(fp.sender(), CCallsign::Aircraft);
            const CFlightPlan flightPlan(
                callsign,
                fp.m_aircraftIcaoType,
                fp.m_depAirport,
                fp.m_destAirport,
                fp.m_altAirport,
                fromStringUtc(depTimePlanned, "hhmm"),
                fromStringUtc(depTimeActual, "hhmm"),
                CTime(fp.m_hoursEnroute * 60 + fp.m_minutesEnroute, CTimeUnit::min()),
                CTime(fp.m_fuelAvailHours * 60 + fp.m_fuelAvailMinutes, CTimeUnit::min()),
                cruiseAlt,
                CSpeed(fp.m_trueCruisingSpeed, CSpeedUnit::kts()),
                rules,
                fp.m_route,
                fp.m_remarks
            );

            emit flightPlanReceived(callsign, flightPlan);
        }

        void CFSDClient::handleClientQuery(const QStringList &tokens)
        {
            ClientQuery clientQuery = ClientQuery::fromTokens(tokens);

            if (clientQuery.m_queryType == ClientQueryType::Unknown)
            {
                return;
            }
            else if (clientQuery.m_queryType == ClientQueryType::IsValidATC)
            {
                // This is usually sent to the server only. If it ever arrives here, just ignore it.
                return;
            }
            else if (clientQuery.m_queryType == ClientQueryType::Capabilities)
            {
                sendClientResponse(ClientQueryType::Capabilities, clientQuery.sender());
                return;
            }
            else if (clientQuery.m_queryType == ClientQueryType::Com1Freq)
            {
                sendClientResponse(ClientQueryType::Com1Freq, clientQuery.sender());
                return;
            }
            else if (clientQuery.m_queryType == ClientQueryType::RealName)
            {
                sendClientResponse(ClientQueryType::RealName, clientQuery.sender());
                return;
            }
            else if (clientQuery.m_queryType == ClientQueryType::Server)
            {
                sendClientResponse(ClientQueryType::Server, clientQuery.sender());
                return;
            }
            else if (clientQuery.m_queryType == ClientQueryType::ATIS)
            {
                // This is answered by ATC clients only. If we get such a request, ignore it.
                return;
            }
            else if (clientQuery.m_queryType == ClientQueryType::PublicIP)
            {
                // This is usually sent to the server only. If it ever arrives here, just ignore it.
                return;
            }
            else if (clientQuery.m_queryType == ClientQueryType::INF)
            {
                sendClientResponse(ClientQueryType::INF, clientQuery.sender());
                return;
            }
            else if (clientQuery.m_queryType == ClientQueryType::FP)
            {
                // This is usually sent to the server only. If it ever arrives here, just ignore it.
                return;
            }
            else if (clientQuery.m_queryType == ClientQueryType::AircraftConfig)
            {
                QStringList aircraftConfigTokens = tokens.mid(3);
                QString aircraftConfigJson = aircraftConfigTokens.join(":");

                const CCallsign callsign(clientQuery.sender(), CCallsign::Aircraft);

                QJsonParseError parserError;
                const QByteArray json = aircraftConfigJson.toUtf8();
                const QJsonDocument doc = QJsonDocument::fromJson(json, &parserError);

                if (parserError.error != QJsonParseError::NoError)
                {
                    CLogMessage(this).warning(u"Failed to parse aircraft config packet: '%1' packet: '%2'") << parserError.errorString() << QString(json);
                    return; // we cannot parse the packet, so we give up here
                }

                const QJsonObject packet = doc.object();
                if (packet == JsonPackets::aircraftConfigRequest())
                {
                    // this MUST work for NOT IN RANGE aircraft as well
                    QJsonObject config = this->getOwnAircraftParts().toJson();
                    config.insert(CAircraftParts::attributeNameIsFullJson(), true);
                    QString data = QJsonDocument(QJsonObject { { "config", config } }).toJson(QJsonDocument::Compact);
                    data = convertToUnicodeEscaped(data);
                    sendAircraftConfiguration(clientQuery.sender(), data);
                    return;
                }

                const bool inRange = isAircraftInRange(callsign);
                if (!inRange) { return; } // sort out all broadcasted we DO NOT NEED
                if (!getSetupForServer().receiveAircraftParts()) { return; }
                const QJsonObject config = doc.object().value("config").toObject();
                if (config.empty()) { return; }

                const qint64 offsetTimeMs = currentOffsetTime(callsign);
                emit aircraftConfigReceived(clientQuery.sender(), config, offsetTimeMs);
                return;
            }
        }

        void CFSDClient::handleClientReponse(const QStringList &tokens)
        {
            ClientResponse clientResponse = ClientResponse::fromTokens(tokens);
            if (clientResponse.isUnknownQuery()) { return; }
            QString sender = clientResponse.sender();

            QString responseData1;
            QString responseData2;
            if (clientResponse.m_responseData.size() > 0)
            {
                responseData1 = clientResponse.m_responseData.at(0);
            }

            if (clientResponse.m_responseData.size() > 1)
            {
                responseData2 = clientResponse.m_responseData.at(1);
            }

            if (clientResponse.m_queryType == ClientQueryType::IsValidATC)
            {
                emit validAtcResponseReceived(responseData2, responseData1 == u"Y");
                return;
            }
            else if (clientResponse.m_queryType == ClientQueryType::Capabilities)
            {
                Capabilities capabilities = Capabilities::None;
                for (int i = 0; i < clientResponse.m_responseData.size(); ++i)
                {
                    QString keyValuePair = clientResponse.m_responseData.at(i);
                    if (keyValuePair.count('=') != 1) { continue; }

                    QStringList split = keyValuePair.split('=');
                    QString key = split.at(0);
                    QString value = split.at(1);

                    if (value == "1")
                    {
                        capabilities |= fromQString<Capabilities>(key);
                    }
                }

                CClient::Capabilities caps = CClient::None;
                if (capabilities & Capabilities::AtcInfo) { caps |= CClient::FsdAtisCanBeReceived; }
                if (capabilities & Capabilities::FastPos) { caps |= CClient::FsdWithInterimPositions; }
                if (capabilities & Capabilities::AircraftInfo)   { caps |= CClient::FsdWithIcaoCodes; }
                if (capabilities & Capabilities::AircraftConfig) { caps |= CClient::FsdWithAircraftConfig; }

                emit capabilityResponseReceived(clientResponse.sender(), caps);
                return;
            }
            else if (clientResponse.m_queryType == ClientQueryType::Com1Freq)
            {
                emit com1FrequencyResponseReceived(clientResponse.sender(), responseData1);
                return;
            }
            else if (clientResponse.m_queryType == ClientQueryType::RealName)
            {
                // The response also includes sector name and pilot rating, but we ignore them here.
                emit realNameResponseReceived(clientResponse.sender(), responseData1);
                return;
            }
            else if (clientResponse.m_queryType == ClientQueryType::Server)
            {
                emit serverResponseReceived(clientResponse.sender(), responseData1);
                return;
            }
            else if (clientResponse.m_queryType == ClientQueryType::ATIS)
            {
                if (responseData1.isEmpty())
                {
                    // networkLog(vatSeverityDebug, "VatFsdClient::handleClientQueryReponse", "ATIS line type cannot be empty!");
                    return;
                }
                updateAtisMap(clientResponse.sender(), fromQString<AtisLineType>(responseData1), responseData2);
                return;
            }
            else if (clientResponse.m_queryType == ClientQueryType::PublicIP)
            {
                // To be implemented if needed
                return;
            }
            else if (clientResponse.m_queryType == ClientQueryType::INF)
            {
                // To be implemented if needed
                return;
            }
            else if (clientResponse.m_queryType == ClientQueryType::FP)
            {
                // FP is sent back as a $FP answer from the server and never as part of a client response.
                return;
            }
            else if (clientResponse.m_queryType == ClientQueryType::AircraftConfig)
            {
                // Currently not existing.
                return;
            }
        }

        void CFSDClient::handleServerError(const QStringList &tokens)
        {
            ServerError serverError = ServerError::fromTokens(tokens);
            switch (serverError.m_errorNumber)
            {
            case ServerErrorCode::CallsignInUse:       CLogMessage(this).error(u"The requested callsign is already taken"); break;
            case ServerErrorCode::InvalidCallsign:     CLogMessage(this).error(u"The requested callsign is not valid"); break;
            case ServerErrorCode::InvalidCidPassword:  CLogMessage(this).error(u"Wrong user ID or password, inactive account"); break;
            case ServerErrorCode::InvalidRevision:     CLogMessage(this).error(u"This server does not support our protocol version"); break;
            case ServerErrorCode::RequestedLevelTooHigh:  CLogMessage(this).error(u"You are not authorized to use the requested pilot rating"); break;
            case ServerErrorCode::ServerFull:          CLogMessage(this).error(u"The server is full"); break;
            case ServerErrorCode::CidSuspended:        CLogMessage(this).error(u"Your user account is suspended"); break;
            case ServerErrorCode::RatingTooLow:        CLogMessage(this).error(u"You are not authorized to use the requested rating"); break;
            case ServerErrorCode::InvalidClient:       CLogMessage(this).error(u"This software is not authorized for use on this network"); break;

            case ServerErrorCode::NoError:             CLogMessage(this).info(u"OK"); break;
            case ServerErrorCode::SyntaxError:         CLogMessage(this).info(u"Malformed packet: Syntax error: %1") << serverError.m_causingParameter; break;
            case ServerErrorCode::InvalidSrcCallsign:  CLogMessage(this).info(u"FSD message was using an invalid callsign: %1 (%2)") << serverError.m_causingParameter << serverError.m_description; break;
            case ServerErrorCode::NoSuchCallsign:      CLogMessage(this).info(u"FSD Server: no such callsign: %1 %2") << serverError.m_causingParameter << serverError.m_description; break;
            case ServerErrorCode::NoFlightPlan:        CLogMessage(this).info(u"FSD Server: no flight plan"); break;
            case ServerErrorCode::NoWeatherProfile:    CLogMessage(this).info(u"FSD Server: requested weather profile does not exist"); break;

            // we have no idea what these mean
            case ServerErrorCode::AlreadyRegistered:   CLogMessage(this).info(u"Server says already registered: %1") << serverError.m_description; break;
            case ServerErrorCode::InvalidCtrl:         CLogMessage(this).info(u"Server invalid control: %1") << serverError.m_description; break;
            case ServerErrorCode::Unknown:             CLogMessage(this).info(u"Server sent unknown error code: %1 (%2)") << serverError.m_causingParameter << serverError.m_description; break;
            case ServerErrorCode::AuthTimeout:         CLogMessage(this).info(u"Client did not authenticate in time"); break;
            }
            if (serverError.isFatalError()) { disconnectFromServer(); }
        }

        void CFSDClient::handleCustomPilotPacket(const QStringList &tokens)
        {
            const QString subType = tokens.at(2);

            if (subType == u"PIR")
            {
                PlaneInfoRequest planeInfoRequest = PlaneInfoRequest::fromTokens(tokens);

                const QString airlineIcao = m_server.getFsdSetup().force3LetterAirlineCodes() ? getOwnAircraft().getAirlineIcaoCode().getDesignator()
                                            : getOwnAircraft().getAirlineIcaoCode().getVDesignator();
                const QString acTypeICAO    = getOwnAircraft().getAircraftIcaoCode().getDesignator();
                const QString livery        = getOwnAircraft().getModel().getSwiftLiveryString();

                sendPlaneInformation(planeInfoRequest.sender(), acTypeICAO, airlineIcao, livery);
            }
            else if (subType == "PI")
            {
                if (tokens.size() > 6 && tokens.at(3) == "X")
                {
                    // This is the old version of a plane info request and no active client should ever send it.
                }
                else if (tokens.size() > 4 && tokens.at(3) == "GEN")
                {
                    PlaneInformation planeInformation = PlaneInformation::fromTokens(tokens);
                    emit planeInformationReceived(planeInformation.sender(), planeInformation.m_aircraft, planeInformation.m_airline, planeInformation.m_livery);
                }
            }
            else if (subType == "I")
            {
                // SquawkBox' interim pilot position. This one is producing too many precision errors. Therefore ignore it.
            }
            else if (subType == "VI")
            {
                // swift's updated interim pilot update.
                if (!isInterimPositionReceivingEnabledForServer()) { return; }

                InterimPilotDataUpdate interimPilotDataUpdate = InterimPilotDataUpdate::fromTokens(tokens);
                const CCallsign callsign(interimPilotDataUpdate.sender(), CCallsign::Aircraft);

                CAircraftSituation situation(
                    callsign,
                    CCoordinateGeodetic(interimPilotDataUpdate.m_latitude, interimPilotDataUpdate.m_longitude, interimPilotDataUpdate.m_altitudeTrue),
                    CHeading(interimPilotDataUpdate.m_heading, CHeading::True, CAngleUnit::deg()),
                    CAngle(interimPilotDataUpdate.m_pitch, CAngleUnit::deg()),
                    CAngle(interimPilotDataUpdate.m_bank, CAngleUnit::deg()),
                    CSpeed(interimPilotDataUpdate.m_groundSpeed, CSpeedUnit::kts()));
                situation.setOnGround(interimPilotDataUpdate.m_onGround);

                // Ref T297, default offset time
                situation.setCurrentUtcTime();
                const qint64 offsetTimeMs = receivedPositionFixTsAndGetOffsetTime(situation.getCallsign(), situation.getMSecsSinceEpoch());
                situation.setTimeOffsetMs(offsetTimeMs);

                emit interimPilotDataUpdatedReceived(situation);
            }
            else if (subType == "FSIPI")
            {
                const PlaneInformationFsinn planeInformationFsinn = PlaneInformationFsinn::fromTokens(tokens);
                emit planeInformationFsinnReceived(planeInformationFsinn.sender(),
                                                   planeInformationFsinn.m_airlineIcao,
                                                   planeInformationFsinn.m_aircraftIcao,
                                                   planeInformationFsinn.m_aircraftIcaoCombinedType,
                                                   planeInformationFsinn.m_sendMModelString);
            }
            else if (subType == "FSIPIR")
            {
                const PlaneInfoRequestFsinn planeInfoRequestFsinn = PlaneInfoRequestFsinn::fromTokens(tokens);
                sendPlaneInformationFsinn(planeInfoRequestFsinn.sender());
                emit planeInformationFsinnReceived(planeInfoRequestFsinn.sender(),
                                                   planeInfoRequestFsinn.m_airlineIcao,
                                                   planeInfoRequestFsinn.m_aircraftIcao,
                                                   planeInfoRequestFsinn.m_aircraftIcaoCombinedType,
                                                   planeInfoRequestFsinn.m_sendMModelString);
            }
            else
            {
                // Unknown #SB opcode, just pass it on.
                const QString sender = tokens.at(0);
                const QStringList data = tokens.mid(3);
                emit customPilotPacketReceived(sender, data);
            }
        }

        void CFSDClient::handleFsdIdentification(const QStringList &tokens)
        {
            if (m_protocolRevision >= PROTOCOL_REVISION_VATSIM_AUTH)
            {
                const FSDIdentification fsdIdentification = FSDIdentification::fromTokens(tokens);
                vatsim_auth_set_initial_challenge(clientAuth, qPrintable(fsdIdentification.m_initialChallenge));

                char fsdChallenge[33];
                vatsim_auth_generate_challenge(serverAuth, fsdChallenge);
                vatsim_auth_set_initial_challenge(serverAuth, fsdChallenge);
                sendClientIdentification(QString(fsdChallenge));
            }
            else
            {
                CLogMessage(this).error(u"You tried to connect to a VATSIM server without using VATSIM protocol, disconnecting!");
                disconnectFromServer();
            }
        }

        void CFSDClient::handleUnknownPacket(const QStringList &tokens)
        {
            CLogMessage(this).warning(u"FSD unknown packet: %1") << tokens.join(", ");
        }

        void CFSDClient::printSocketError(QAbstractSocket::SocketError socketError)
        {
            Q_UNUSED(socketError)
            const QString error = m_socket.errorString();
            CLogMessage(this).error(u"FSD socket error: %1") << socketError;
        }

        void CFSDClient::handleSocketError(QAbstractSocket::SocketError socketError)
        {
            const QString error = m_socket.errorString();
            switch (socketError)
            {
            // all named here need a logoff
            case QAbstractSocket::RemoteHostClosedError:
                emit this->severeNetworkError(error);
                this->disconnectFromServer();
                break;
            default:
                break;
            }
        }

        void CFSDClient::updateConnectionStatus(CConnectionStatus newStatus)
        {
            if (m_connectionStatus != newStatus)
            {
                if (newStatus.isConnected())
                {
                    m_server.setConnectedSinceNow();
                    this->setCurrentEcosystem(m_server.getEcosystem());
                }
                else
                {
                    m_server.markAsDisconnected();
                }

                if (newStatus.isDisconnected())
                {
                    this->stopPositionTimers();
                    this->clearState();
                    this->setLastEcosystem(m_server.getEcosystem());
                    this->setCurrentEcosystem(CEcosystem::NoSystem);
                    this->saveNetworkStatistics(m_server.getName());
                }

                emit this->connectionStatusChanged(m_connectionStatus, newStatus);
                qSwap(m_connectionStatus, newStatus);
            }
        }

        void CFSDClient::consolidateTextMessage(const CTextMessage &textMessage)
        {
            if (textMessage.isSupervisorMessage())
            {
                emit this->textMessagesReceived(textMessage);
            }
            else
            {
                m_textMessagesToConsolidate.addConsolidatedTextMessage(textMessage);
                m_dsSendTextMessage.inputSignal(); // trigger
            }
        }

        void CFSDClient::emitConsolidatedTextMessages()
        {
            emit this->textMessagesReceived(m_textMessagesToConsolidate);
            m_textMessagesToConsolidate.clear();
        }

        qint64 CFSDClient::receivedPositionFixTsAndGetOffsetTime(const CCallsign &callsign, qint64 markerTs)
        {
            Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "Need callsign");

            if (markerTs < 0) { markerTs = QDateTime::currentMSecsSinceEpoch(); }
            if (!m_lastPositionUpdate.contains(callsign))
            {
                m_lastPositionUpdate.insert(callsign, markerTs);
                return CFsdSetup::c_positionTimeOffsetMsec;
            }
            const qint64 oldTs = m_lastPositionUpdate.value(callsign);
            m_lastPositionUpdate[callsign] = markerTs;

            // Ref T297, dynamic offsets
            const qint64 diff = qAbs(markerTs - oldTs);
            this->insertLatestOffsetTime(callsign, diff);

            int count = 0;
            static const qint64 minOffsetTime = CFsdSetup::c_interimPositionTimeOffsetMsec; // no longer needed with C++17
            const qint64 avgTimeMs = this->averageOffsetTimeMs(callsign, count, 3); // latest average
            qint64 offsetTime = CFsdSetup::c_positionTimeOffsetMsec;

            if (avgTimeMs < minOffsetTime && count >= 3)
            {
                offsetTime = CFsdSetup::c_interimPositionTimeOffsetMsec;
            }

            return m_additionalOffsetTime + offsetTime;
        }

        qint64 CFSDClient::currentOffsetTime(const CCallsign &callsign) const
        {
            Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "Need callsign");

            if (!m_lastOffsetTimes.contains(callsign) || m_lastOffsetTimes[callsign].isEmpty()) { return CFsdSetup::c_positionTimeOffsetMsec; }
            return m_lastOffsetTimes[callsign].front();
        }

        void CFSDClient::clearState()
        {
            m_textMessagesToConsolidate.clear();
            m_pendingAtisQueries.clear();
            m_lastPositionUpdate.clear();
            m_lastOffsetTimes.clear();
            m_sentAircraftConfig = CAircraftParts::null();
        }

        void CFSDClient::clearState(const CCallsign &callsign)
        {
            if (callsign.isEmpty()) { return; }
            m_pendingAtisQueries.remove(callsign);
            m_lastPositionUpdate.remove(callsign);
            m_interimPositionReceivers.remove(callsign);
            m_lastOffsetTimes.remove(callsign);
        }

        void CFSDClient::insertLatestOffsetTime(const CCallsign &callsign, qint64 offsetMs)
        {
            QList<qint64> &offsets = m_lastOffsetTimes[callsign];
            offsets.push_front(offsetMs);
            if (offsets.size() > MaxOffseTimes) { offsets.removeLast(); }
        }

        qint64 CFSDClient::averageOffsetTimeMs(const CCallsign &callsign, int &count, int maxLastValues) const
        {
            const QList<qint64> &offsets = m_lastOffsetTimes[callsign];
            if (offsets.size() < 1) { return -1; }
            qint64 sum = 0;
            count = 0;
            for (qint64 v : offsets)
            {
                count++;
                sum += v;
                if (count > maxLastValues) { break; }
            }
            return qRound(static_cast<double>(sum) / count);
        }

        qint64 CFSDClient::averageOffsetTimeMs(const CCallsign &callsign, int maxLastValues) const
        {
            int count = 0;
            return this->averageOffsetTimeMs(callsign, maxLastValues, count);
        }

        bool CFSDClient::isInterimPositionSendingEnabledForServer() const
        {
            const CFsdSetup::SendReceiveDetails d = this->getSetupForServer().getSendReceiveDetails();
            return (d & CFsdSetup::SendInterimPositions);
        }

        bool CFSDClient::isInterimPositionReceivingEnabledForServer() const
        {
            const CFsdSetup::SendReceiveDetails d = this->getSetupForServer().getSendReceiveDetails();
            return (d & CFsdSetup::ReceiveInterimPositions);
        }

        const CFsdSetup &CFSDClient::getSetupForServer() const
        {
            return m_server.getFsdSetup();
        }

        void CFSDClient::maybeHandleAtisReply(const CCallsign &sender, const CCallsign &receiver, const QString &message)
        {
            Q_ASSERT(m_pendingAtisQueries.contains(sender));
            PendingAtisQuery &pendingQuery = m_pendingAtisQueries[sender];
            pendingQuery.m_atisMessage.push_back(message);

            // Wait maximum 5 seconds for the reply and release as text message after
            if (pendingQuery.m_queryTime.secsTo(QDateTime::currentDateTimeUtc()) > 5)
            {
                const QString atisMessage(pendingQuery.m_atisMessage.join(QChar::LineFeed));
                CTextMessage tm(atisMessage, sender, receiver);
                tm.setCurrentUtcTime();
                this->consolidateTextMessage(tm); // emit textMessagesReceived(tm);
                m_pendingAtisQueries.remove(sender);
                return;
            }

            // 4 digits followed by z (e.g. 0200z) is always the last atis line.
            // Some controllers leave the logoff time empty. Hence we accept anything
            // between 0-4 digits.
            thread_local const QRegularExpression reLogoff("^\\d{0,4}z$");
            if (reLogoff.match(message).hasMatch())
            {
                emit atisLogoffTimeReplyReceived(sender, message);
                CInformationMessage atisMessage(CInformationMessage::ATIS);
                for (const auto &line : as_const(pendingQuery.m_atisMessage))
                {
                    if (!atisMessage.isEmpty()) atisMessage.appendMessage("\n");
                    atisMessage.appendMessage(line);
                }
                emit atisReplyReceived(CCallsign(sender.toQString(), CCallsign::Atc), atisMessage);
                m_pendingAtisQueries.remove(sender);
                return;
            }
        }

        void CFSDClient::fsdMessageSettingsChanged()
        {
            if (m_rawFsdMessageLogFile.isOpen()) { m_rawFsdMessageLogFile.close(); }
            const CRawFsdMessageSettings setting = m_fsdMessageSetting.get();
            m_rawFsdMessagesEnabled = setting.areRawFsdMessagesEnabled();

            if (setting.getFileWriteMode() == CRawFsdMessageSettings::None || setting.getFileDir().isEmpty()) { return; }
            if (setting.getFileWriteMode() == CRawFsdMessageSettings::Truncate)
            {
                const QString filePath = CFileUtils::appendFilePaths(setting.getFileDir(), "rawfsdmessages.log");
                m_rawFsdMessageLogFile.setFileName(filePath);
                m_rawFsdMessageLogFile.open(QIODevice::Text | QIODevice::WriteOnly);
            }
            else if (setting.getFileWriteMode() == CRawFsdMessageSettings::Append)
            {
                const QString filePath = CFileUtils::appendFilePaths(setting.getFileDir(), "rawfsdmessages.log");
                m_rawFsdMessageLogFile.setFileName(filePath);
                m_rawFsdMessageLogFile.open(QIODevice::Text | QIODevice::WriteOnly | QIODevice::Append);
            }
            else if (setting.getFileWriteMode() == CRawFsdMessageSettings::Timestamped)
            {
                QString filename("rawfsdmessages");
                filename += QLatin1String("_");
                filename += QDateTime::currentDateTime().toString(QStringLiteral("yyMMddhhmmss"));
                filename += QLatin1String(".log");
                const QString filePath = CFileUtils::appendFilePaths(setting.getFileDir(), filename);
                m_rawFsdMessageLogFile.setFileName(filePath);
                m_rawFsdMessageLogFile.open(QIODevice::Text | QIODevice::WriteOnly);
            }
        }

        BlackMisc::Aviation::CCallsignSet CFSDClient::getInterimPositionReceivers() const
        {
            return m_interimPositionReceivers;
        }

        void CFSDClient::setInterimPositionReceivers(const BlackMisc::Aviation::CCallsignSet &interimPositionReceivers)
        {
            m_interimPositionReceivers = interimPositionReceivers;
        }

        int CFSDClient::increaseStatisticsValue(const QString &identifier, const QString &appendix)
        {
            if (identifier.isEmpty() || !m_statistics) { return -1; }
            const QString i = appendix.isEmpty() ? identifier : identifier % u"." % appendix;
            int &v =  m_callStatistics[i];
            v++;

            constexpr int MaxTimeValues = 50;
            m_callByTime.push_front(QPair<qint64, QString>(QDateTime::currentMSecsSinceEpoch(), i));
            if (m_callByTime.size() > MaxTimeValues) { m_callByTime.removeLast(); }
            return v;
        }

        int CFSDClient::increaseStatisticsValue(const QString &identifier, int value)
        {
            return this->increaseStatisticsValue(identifier, QString::number(value));
        }

        void CFSDClient::clearStatistics()
        {
            m_callStatistics.clear();
            m_callByTime.clear();
        }

        QString CFSDClient::getNetworkStatisticsAsText(bool reset, const QString &separator)
        {
            QVector<std::pair<int, QString>> transformed;
            if (m_callStatistics.isEmpty()) { return QString(); }

            for (const auto pair : makePairsRange(as_const(m_callStatistics)))
            {
                // key is pair.first, value is pair.second
                transformed.push_back({ pair.second, pair.first });
            }

            // sorted by value
            std::sort(transformed.begin(), transformed.end(), std::greater<>());
            QString stats;
            for (const auto &pair : transformed)
            {
                stats +=
                    (stats.isEmpty() ? QString() : separator) %
                    pair.second % u": " % QString::number(pair.first);
            }

            for (const auto &pair : transformed)
            {
                stats +=
                    (stats.isEmpty() ? QString() : separator) %
                    pair.second % u": " % QString::number(pair.first);
            }

            if (!m_callByTime.isEmpty())
            {
                const qint64 lastTs = m_callByTime.front().first;
                for (const auto &pair : m_callByTime)
                {
                    const qint64 deltaTs = lastTs - pair.first;
                    stats += separator % QStringLiteral("%1").arg(deltaTs, 5, 10, QChar('0')) % u": " % pair.second;
                }
            }

            if (reset) { this->clearStatistics(); }
            return stats;
        }

        CLoginMode CFSDClient::getLoginMode() const
        {
            return m_loginMode;
        }

        void CFSDClient::readDataFromSocket()
        {
            while (m_socket.canReadLine())
            {
                const QByteArray dataEncoded = m_socket.readLine();
                const QString data = m_fsdTextCodec->toUnicode(dataEncoded);
                parseMessage(data);
            }
        }

        void CFSDClient::parseMessage(const QString &line)
        {
            MessageType messageType = MessageType::Unknown;
            QString cmd;

            if (m_printToConsole) { qDebug() << "FSD Recv=>" << line; }
            emitRawFsdMessage(line.trimmed(), false);

            for (const auto &str : makeKeysRange(as_const(m_messageTypeMapping)))
            {
                if (line.startsWith(str))
                {
                    cmd = str;
                    messageType = m_messageTypeMapping[str];
                    break;
                }
            }

            if (messageType != MessageType::Unknown)
            {
                // Cutoff the cmd from the beginning
                const QString payload = line.mid(cmd.size()).trimmed();

                // We expected a payload, but there is nothing
                if (payload.length() == 0) return;

                const QStringList tokens = payload.split(':');
                switch (messageType)
                {
                case MessageType::AddAtc: /* ignore */ return;
                case MessageType::AddPilot: /* ignore */ return;
                case MessageType::AtcDataUpdate: handleAtcDataUpdate(tokens); return;
                case MessageType::AuthChallenge: handleAuthChallenge(tokens); return;
                case MessageType::AuthResponse: handleAuthResponse(tokens); return;
                case MessageType::ClientIdentification: /* do nothing */ return;
                case MessageType::ClientQuery: handleClientQuery(tokens); return;
                case MessageType::ClientResponse: handleClientReponse(tokens); return;
                case MessageType::DeleteATC: handleDeleteATC(tokens); return;
                case MessageType::DeletePilot: handleDeletePilot(tokens); return;
                case MessageType::FlightPlan: handleFlightPlan(tokens); return;
                case MessageType::FsdIdentification: handleFsdIdentification(tokens); return;
                case MessageType::KillRequest: handleKillRequest(tokens); return;
                case MessageType::PilotDataUpdate: handlePilotDataUpdate(tokens); return;
                case MessageType::Ping: handlePing(tokens); return;
                case MessageType::Pong: handlePong(tokens); return;
                case MessageType::ServerError: handleServerError(tokens); return;
                case MessageType::TextMessage: handleTextMessage(tokens); return;
                case MessageType::PilotClientCom: handleCustomPilotPacket(tokens); return;
                case MessageType::Unknown: handleUnknownPacket(tokens); return;
                }
            }
        }

        void CFSDClient::emitRawFsdMessage(const QString &fsdMessage, bool isSent)
        {
            if (!m_unitTestMode && !m_rawFsdMessagesEnabled) { return; }
            QString fsdMessageFiltered(fsdMessage);
            if (m_filterPasswordFromLogin)
            {
                if (fsdMessageFiltered.startsWith("#AP"))
                {
                    thread_local const QRegularExpression re("^(#AP\\w+:SERVER:\\d+:)[^:]+(:\\d+:\\d+:\\d+:.+)$");
                    fsdMessageFiltered.replace(re, "\\1<password>\\2");
                    m_filterPasswordFromLogin = false;
                }
            }

            const QString prefix = isSent ? "FSD Sent=>" : "FSD Recv=>";
            CRawFsdMessage rawMessage(prefix + fsdMessageFiltered);
            rawMessage.setCurrentUtcTime();
            if (m_rawFsdMessageLogFile.isOpen())
            {
                QTextStream stream(&m_rawFsdMessageLogFile);
                stream << rawMessage.toQString().trimmed() << endl;
            }
            emit rawFsdMessage(rawMessage);
        }

        bool CFSDClient::saveNetworkStatistics(const QString &server)
        {
            if (m_callStatistics.isEmpty()) { return false; }

            const QString s = this->getNetworkStatisticsAsText(false, "\n");
            if (s.isEmpty()) { return false; }
            const QString fn = QStringLiteral("networkstatistics_%1_%2.log").arg(QDateTime::currentDateTimeUtc().toString("yyMMddhhmmss"), server);
            const QString fp = CFileUtils::appendFilePaths(CDirectoryUtils::logDirectory(), fn);
            return CFileUtils::writeStringToFile(s, fp);
        }

        void CFSDClient::startPositionTimers()
        {
            m_positionUpdateTimer.start(c_updatePostionIntervalMsec);
            m_scheduledConfigUpdate.start(c_processingIntervalMsec);
            if (this->isInterimPositionSendingEnabledForServer()) { m_interimPositionUpdateTimer.start(c_updateInterimPostionIntervalMsec); }
        }

        void CFSDClient::stopPositionTimers()
        {
            m_positionUpdateTimer.stop();
            m_interimPositionUpdateTimer.stop();
            m_scheduledConfigUpdate.stop();
        }

        void CFSDClient::updateAtisMap(const QString &callsign, AtisLineType type, const QString &line)
        {
            if (type == AtisLineType::VoiceRoom)
            {
                m_mapAtisMessages[callsign].voiceRoom = line;
                m_mapAtisMessages[callsign].lineCount++;
                return;
            }
            else if (type == AtisLineType::TextMessage)
            {
                m_mapAtisMessages[callsign].textLines.push_back(line);
                m_mapAtisMessages[callsign].lineCount++;
                return;
            }
            else if (type == AtisLineType::ZuluLogoff)
            {
                m_mapAtisMessages[callsign].zuluLogoff = line;
                m_mapAtisMessages[callsign].lineCount++;
                return;
            }
            else
            {
                if (! m_mapAtisMessages.contains(callsign)) { return; }

                // Ignore the check for line count.
                m_mapAtisMessages[callsign].lineCount++;

                const CCallsign cs(callsign, CCallsign::Atc);
                // emit atisVoiceRoomReplyReceived(cs, m_mapAtisMessages[callsign].voiceRoom);
                emit atisLogoffTimeReplyReceived(cs, m_mapAtisMessages[callsign].zuluLogoff);

                CInformationMessage atisMessage(CInformationMessage::ATIS);
                for (const QString &tm : m_mapAtisMessages[callsign].textLines)
                {
                    const QString fixed = tm.trimmed();
                    if (!fixed.isEmpty())
                    {
                        //  detect the stupid z1, z2, z3 placeholders
                        //! \fixme: Anything better as this stupid code here?
                        thread_local const QRegularExpression RegExp("[\\n\\t\\r]");
                        const QString test = fixed.toLower().remove(RegExp);
                        if (test == "z") return;
                        if (test.startsWith("z") && test.length() == 2) return; // z1, z2, ..
                        if (test.length() == 1) return; // sometimes just z

                        // append
                        if (!atisMessage.isEmpty()) atisMessage.appendMessage("\n");
                        atisMessage.appendMessage(fixed);
                    }
                }

                emit atisReplyReceived(cs, atisMessage);

                m_mapAtisMessages.remove(callsign);
                return;
            }
        }

        const CLength &CFSDClient::fixAtcRange(const CLength &networkRange, const CCallsign &cs)
        {
            /** T702, https://discordapp.com/channels/539048679160676382/539846348275449887/597814208125730826
            DEL 5 NM
            GND 10 NM
            TWR 25 NM
            DEP/APP 150 NM
            CTR 300 NM
            FSS fixed 1500NM, no minimum
            **/

            // ATIS often have a range of 0 nm. Correct this to a proper value.
            const QString suffix = cs.getSuffix();
            if (suffix.contains(QStringLiteral("ATIS"), Qt::CaseInsensitive)) { static const CLength l_Atis(150.0, CLengthUnit::NM()); return maxOrNotNull(networkRange, l_Atis); }
            if (suffix.contains(QStringLiteral("GND"),  Qt::CaseInsensitive)) { static const CLength l_Gnd(10.0, CLengthUnit::NM());   return maxOrNotNull(networkRange, l_Gnd);  }
            if (suffix.contains(QStringLiteral("TWR"),  Qt::CaseInsensitive)) { static const CLength l_Twr(25.0, CLengthUnit::NM());   return maxOrNotNull(networkRange, l_Twr);  }
            if (suffix.contains(QStringLiteral("DEP"),  Qt::CaseInsensitive)) { static const CLength l_Dep(150.0, CLengthUnit::NM());  return maxOrNotNull(networkRange, l_Dep);  }
            if (suffix.contains(QStringLiteral("APP"),  Qt::CaseInsensitive)) { static const CLength l_App(150.0, CLengthUnit::NM());  return maxOrNotNull(networkRange, l_App);  }
            if (suffix.contains(QStringLiteral("CTR"),  Qt::CaseInsensitive)) { static const CLength l_Ctr(300.0, CLengthUnit::NM());  return maxOrNotNull(networkRange, l_Ctr);  }
            if (suffix.contains(QStringLiteral("FSS"),  Qt::CaseInsensitive)) { static const CLength l_Fss(1500.0, CLengthUnit::NM()); return maxOrNotNull(networkRange, l_Fss);  }

            return networkRange;
        }

        const CLength &CFSDClient::maxOrNotNull(const CLength &l1, const CLength &l2)
        {
            if (l1.isNull()) { return l2; }
            if (l2.isNull()) { return l1; }
            return (l2 > l1) ? l2 : l1;
        }

        QString CFSDClient::noColons(const QString &input)
        {
            if (!input.contains(':')) { return input; }
            QString copy(input);
            return copy.remove(':');
        }

        void CFSDClient::handleIllegalFsdState(const QString &message)
        {
            if (CBuildConfig::isLocalDeveloperDebugBuild())
            {
                BLACK_VERIFY_X(false, Q_FUNC_INFO, "Illegal FSD state");
            }
            CLogMessage(this).warning(message);
        }

        const QJsonObject &CFSDClient::JsonPackets::aircraftConfigRequest()
        {
            static const QJsonObject jsonObject{ { "request", "full" } };
            return jsonObject;
        }
    }
}
