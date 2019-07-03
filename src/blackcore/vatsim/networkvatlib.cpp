/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \cond PRIVATE

#include "blackconfig/buildconfig.h"
#include "blackcore/application.h"
#include "blackcore/vatsim/networkvatlib.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/altitude.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/aviation/flightplan.h"
#include "blackmisc/aviation/heading.h"
#include "blackmisc/aviation/informationmessage.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/aviation/transponder.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/geo/latitude.h"
#include "blackmisc/geo/longitude.h"
#include "blackmisc/network/fsdsetup.h"
#include "blackmisc/network/textmessage.h"
#include "blackmisc/network/user.h"
#include "blackmisc/pq/angle.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/speed.h"
#include "blackmisc/pq/time.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/compare.h"
#include "blackmisc/json.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/logcategory.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/verify.h"
#include "vatlib/vatlib.h"

#include <QChar>
#include <QCommandLineOption>
#include <QCoreApplication>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonValue>
#include <QList>
#include <QRegularExpression>
#include <QStringBuilder>
#include <QTextCodec>
#include <QTextStream>
#include <QVector>
#include <Qt>
#include <QtDebug>
#include <QtGlobal>
#include <algorithm>
#include <type_traits>

static_assert(! std::is_abstract<BlackCore::Vatsim::CNetworkVatlib>::value, "Must implement all pure virtuals");
static_assert(VAT_LIBVATLIB_VERSION == 908, "Wrong vatlib header installed");

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Json;
using namespace BlackMisc::Network;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;

namespace BlackCore
{
    namespace Vatsim
    {
        const CLogCategoryList &CNetworkVatlib::getLogCategories()
        {
            static const CLogCategoryList cats({ CLogCategory::vatsimSpecific(), CLogCategory::network() });
            return cats;
        }

        CNetworkVatlib::CNetworkVatlib(IClientProvider *clientProvider,
                                       IOwnAircraftProvider *ownAircraftProvider, QObject *parent)
            : INetwork(clientProvider, ownAircraftProvider, parent),
              m_loginMode(LoginNormal),
              m_status(vatStatusDisconnected),
              m_tokenBucket(10, CTime(5, CTimeUnit::s()), 1)
        {
            connect(this, &CNetworkVatlib::terminate, this, &INetwork::terminateConnection, Qt::QueuedConnection);

            Q_ASSERT_X(Vat_GetVersion() == VAT_LIBVATLIB_VERSION, "swift.network", "Wrong vatlib shared library installed");

            Vat_SetNetworkLogHandler(vatSeverityError, CNetworkVatlib::networkLogHandler);

            m_processingTimer.setObjectName(this->objectName().append(":m_processingTimer"));
            m_positionUpdateTimer.setObjectName(this->objectName().append(":m_positionUpdateTimer"));
            m_interimPositionUpdateTimer.setObjectName(this->objectName().append(":m_interimPositionUpdateTimer"));

            connect(&m_processingTimer, &QTimer::timeout, this, &CNetworkVatlib::process);
            connect(&m_positionUpdateTimer, &QTimer::timeout, this, &CNetworkVatlib::sendPositionUpdate);
            connect(&m_interimPositionUpdateTimer, &QTimer::timeout, this, &CNetworkVatlib::sendInterimPositions);
            connect(&m_scheduledConfigUpdate, &QTimer::timeout, this, &CNetworkVatlib::sendIncrementalAircraftConfig);
            m_scheduledConfigUpdate.setSingleShot(true);

            m_processingTimer.start(c_processingIntervalMsec);
        }

        void CNetworkVatlib::initializeSession()
        {
            Q_ASSERT_X(isDisconnected(), Q_FUNC_INFO, "attempted to reinitialize session while still connected");
            Q_ASSERT_X(sApp, Q_FUNC_INFO, "Need sApp");

            int clientCapabilities = vatCapsAircraftInfo | vatCapsFastPos | vatCapsAtcInfo | vatCapsAircraftConfig;
            if (m_loginMode == LoginStealth)
            {
                clientCapabilities |= vatCapsStealth;
            }

            int clientId = 0;
            QString clientKey;
            if (!getCmdLineClientIdAndKey(clientId, clientKey))
            {
                clientId = CBuildConfig::vatsimClientId();
                clientKey = CBuildConfig::vatsimPrivateKey();
            }

            VatServerType serverType;
            switch (m_server.getServerType())
            {
            case CServer::FSDServerVatsim: serverType = vatServerVatsim; break;
            default: serverType = vatServerLegacyFsd; break;
            }

            const QString hostApplication = this->getNetworkHostApplicationString().replace(':', ' ');
            m_net.reset(Vat_CreateNetworkSession(serverType, sApp->swiftVersionChar(),
                                                 CBuildConfig::getVersion().majorVersion(),
                                                 CBuildConfig::getVersion().minorVersion(),
                                                 toFSD(hostApplication), clientId, toFSD(clientKey),
                                                 clientCapabilities));

            Vat_SetStateChangeHandler(m_net.data(), onConnectionStatusChanged, this);
            Vat_SetTextMessageHandler(m_net.data(), onTextMessageReceived, this);
            Vat_SetRadioMessageHandler(m_net.data(), onRadioMessageReceived, this);
            Vat_SetDeletePilotHandler(m_net.data(), onPilotDisconnected, this);
            Vat_SetDeleteAtcHandler(m_net.data(), onControllerDisconnected, this);
            Vat_SetPilotPositionHandler(m_net.data(), onPilotPositionUpdate, this);
            Vat_SetInterimPilotPositionHandler(m_net.data(), onInterimPilotPositionUpdate, this);
            Vat_SetAtcPositionHandler(m_net.data(), onAtcPositionUpdate, this);
            Vat_SetKillHandler(m_net.data(), onKicked, this);
            Vat_SetPongHandler(m_net.data(), onPong, this);
            Vat_SetMetarResponseHandler(m_net.data(), onMetarReceived, this);
            Vat_SetClientQueryHandler(m_net.data(), onInfoQueryRequestReceived, this);
            Vat_SetClientQueryResponseHandler(m_net.data(), onInfoQueryReplyReceived, this);
            Vat_SetClientCapabilitiesReplyHandler(m_net.data(), onCapabilitiesReplyReceived, this);
            Vat_SetControllerAtisHandler(m_net.data(), onAtisReplyReceived, this);
            Vat_SetFlightPlanHandler(m_net.data(), onFlightPlanReceived, this);
            Vat_SetServerErrorHandler(m_net.data(), onErrorReceived, this);
            Vat_SetAircraftInfoRequestHandler(m_net.data(), onPilotInfoRequestReceived, this);
            Vat_SetAircraftInfoHandler(m_net.data(), onPilotInfoReceived, this);
            Vat_SetCustomPilotPacketHandler(m_net.data(), onCustomPacketReceived, this);
            Vat_SetAircraftConfigHandler(m_net.data(), onAircraftConfigReceived, this);

            fsdMessageSettingsChanged();
        }

        CNetworkVatlib::~CNetworkVatlib()
        {
            BLACK_VERIFY_X(isDisconnected(), Q_FUNC_INFO, "CNetworkVatlib destroyed while still connected.");
            terminateConnection();
        }

        const QString &CNetworkVatlib::getLibraryInfo(bool detailed) const
        {
            static const QString vs(QStringLiteral("VATLIB: ") + Vat_GetVersionText());
            static const QString vd(QStringLiteral("VATLIB: ") + Vat_GetVersionText() + QStringLiteral("\n") + Vat_GetBuildInfo());
            return detailed ? vd : vs;
        }

        void CNetworkVatlib::process()
        {
            if (!m_net) { return; }
            sendIncrementalAircraftConfig();
            Vat_ExecuteNetworkTasks(m_net.data());
        }

        void CNetworkVatlib::sendPositionUpdate()
        {
            if (!m_net) { return; }

            if (isConnected())
            {
                CSimulatedAircraft myAircraft(getOwnAircraft());
                if (m_loginMode == LoginAsObserver)
                {
                    // Observer
                    VatAtcPosition pos;
                    pos.facility = vatFacilityTypeUnknown;
                    pos.visibleRange = 10; // NM
                    pos.latitude  = myAircraft.latitude().value(CAngleUnit::deg());
                    pos.longitude = myAircraft.longitude().value(CAngleUnit::deg());
                    pos.elevation = 0;
                    pos.rating = vatAtcRatingObserver;
                    pos.frequency = 199998;
                    Vat_SendATCUpdate(m_net.data(), &pos);

                    // statistics
                    this->increaseStatisticsValue(QStringLiteral("Vat_SendATCUpdate"));
                }
                else
                {
                    // Normal / Stealth mode
                    VatPilotPosition pos;
                    pos.altitudePressure = myAircraft.getPressureAltitude().valueInteger(CLengthUnit::ft());
                    pos.altitudeTrue = myAircraft.getAltitude().valueInteger(CLengthUnit::ft());
                    pos.heading      = myAircraft.getHeading().value(CAngleUnit::deg());
                    pos.pitch        = myAircraft.getPitch().value(CAngleUnit::deg());
                    pos.bank         = myAircraft.getBank().value(CAngleUnit::deg());
                    pos.onGround     = myAircraft.getParts().isOnGround();
                    pos.latitude     = myAircraft.latitude().value(CAngleUnit::deg());
                    pos.longitude    = myAircraft.longitude().value(CAngleUnit::deg());
                    pos.groundSpeed  = myAircraft.getGroundSpeed().valueInteger(CSpeedUnit::kts());
                    pos.rating = vatPilotRatingUnknown;
                    pos.transponderCode = static_cast<qint16>(myAircraft.getTransponderCode());
                    pos.transponderMode = vatTransponderModeStandby;
                    switch (myAircraft.getTransponderMode())
                    {
                    case CTransponder::ModeC: pos.transponderMode = vatTransponderModeCharlie; break;
                    case CTransponder::StateIdent: pos.transponderMode = vatTransponderModeIdent; break;
                    default: pos.transponderMode = vatTransponderModeStandby; break;
                    }
                    Vat_SendPilotUpdate(m_net.data(), &pos);

                    // statistics
                    this->increaseStatisticsValue(QStringLiteral("Vat_SendPilotUpdate"));
                }
            }
        }

        void CNetworkVatlib::sendInterimPositions()
        {
            if (!m_net) { return; }
            if (isConnected())
            {
                CSimulatedAircraft myAircraft(getOwnAircraft());
                if (m_loginMode == LoginNormal)
                {
                    VatInterimPilotPosition pos;
                    pos.altitudeTrue = myAircraft.getAltitude().valueInteger(CLengthUnit::ft());
                    pos.heading      = myAircraft.getHeading().value(CAngleUnit::deg());
                    pos.pitch        = myAircraft.getPitch().value(CAngleUnit::deg());
                    pos.bank         = myAircraft.getBank().value(CAngleUnit::deg());
                    pos.onGround     = myAircraft.getParts().isOnGround();
                    pos.latitude     = myAircraft.latitude().value(CAngleUnit::deg());
                    pos.longitude    = myAircraft.longitude().value(CAngleUnit::deg());

                    for (const auto &receiver : as_const(m_interimPositionReceivers))
                    {
                        Vat_SendInterimPilotUpdate(m_net.data(), toFSD(receiver), &pos);

                        // statistics
                        this->increaseStatisticsValue(QStringLiteral("Vat_SendInterimPilotUpdate"));
                    }
                }
            }
        }

        //! Convert vatlib status code to INetwork::ConnectionStatus
        INetwork::ConnectionStatus convertConnectionStatus(VatConnectionStatus status)
        {
            switch (status)
            {
            case vatStatusConnecting:    return INetwork::Connecting;
            case vatStatusConnected:     return INetwork::Connected;
            case vatStatusDisconnected:  return INetwork::Disconnected;
            case vatStatusDisconnecting: return INetwork::Disconnecting;
            }
            qFatal("unrecognised connection status");
            return INetwork::DisconnectedError;
        }

        void CNetworkVatlib::changeConnectionStatus(VatConnectionStatus status)
        {
            if (m_status != status)
            {
                qSwap(m_status, status);
                if (m_status == vatStatusConnected)
                {
                    m_server.setConnectedSinceNow();
                    this->setCurrentEcosystem(m_server.getEcosystem());
                }
                else
                {
                    m_server.markAsDisconnected();
                }

                if (this->isDisconnected())
                {
                    this->stopPositionTimers();
                    this->clearState();
                    this->setLastEcosystem(m_server.getEcosystem());
                    this->setCurrentEcosystem(CEcosystem::NoSystem);
                }

                emit this->connectionStatusChanged(convertConnectionStatus(status), convertConnectionStatus(m_status));
            }
        }

        QByteArray CNetworkVatlib::toFSDnoColon(const QString &qstr) const
        {
            if (!qstr.contains(':')) { return toFSD(qstr); }
            if (CBuildConfig::isLocalDeveloperDebugBuild())
            {
                // so we can investigate
                BLACK_VERIFY_X(false, Q_FUNC_INFO, "Illegal char :");
            }
            QString copy(qstr);
            return toFSD(copy.remove(':'));
        }

        QByteArray CNetworkVatlib::toFSD(const QString &qstr) const
        {
            Q_ASSERT_X(m_fsdTextCodec, Q_FUNC_INFO, "Missing codec");
            return m_fsdTextCodec->fromUnicode(qstr);
        }

        QByteArray CNetworkVatlib::toFSD(const CCallsign &callsign) const
        {
            return toFSD(callsign.getStringAsSet());
        }

        std::function<const char **()> CNetworkVatlib::toFSD(const QStringList &qstrList) const
        {
            QVector<QByteArray> bytesVec;
            bytesVec.reserve(qstrList.size());
            for (auto i = qstrList.cbegin(); i != qstrList.cend(); ++i)
            {
                bytesVec.push_back(toFSD(*i));
            }

            return [ cstrVec = QVector<const char *>(), bytesVec = std::move(bytesVec) ]() mutable
            {
                Q_ASSERT_X(cstrVec.isEmpty(), Q_FUNC_INFO, "toFSD lambda called twice");
                for (auto i = bytesVec.cbegin(); i != bytesVec.cend(); ++i)
                {
                    cstrVec.push_back(i->constData());
                }
                return const_cast<const char **>(cstrVec.constData());
            };
        }

        QString CNetworkVatlib::fromFSD(const char *cstr) const
        {
            Q_ASSERT_X(m_fsdTextCodec, Q_FUNC_INFO, "Missing codec");
            return m_fsdTextCodec->toUnicode(cstr);
        }

        QString CNetworkVatlib::getNetworkHostApplicationString() const
        {
            return this->getSimulatorNameAndVersion();
        }

        QStringList CNetworkVatlib::fromFSD(const char **cstrArray, int size) const
        {
            QStringList qstrList;
            qstrList.reserve(size);
            for (int i = 0; i < size; ++i)
            {
                qstrList.push_back(fromFSD(cstrArray[i]));
            }
            return qstrList;
        }

        bool CNetworkVatlib::isInterimPositionSendingEnabledForServer() const
        {
            const CFsdSetup::SendReceiveDetails d = this->getSetupForServer().getSendReceiveDetails();
            return (d & CFsdSetup::SendInterimPositions);
        }

        bool CNetworkVatlib::isInterimPositionReceivingEnabledForServer() const
        {
            const CFsdSetup::SendReceiveDetails d = this->getSetupForServer().getSendReceiveDetails();
            return (d & CFsdSetup::ReceiveInterimPositions);
        }

        const CFsdSetup &CNetworkVatlib::getSetupForServer() const
        {
            return m_server.getFsdSetup();
        }

        void CNetworkVatlib::startPositionTimers()
        {
            m_positionUpdateTimer.start(c_updatePostionIntervalMsec);
            if (this->isInterimPositionSendingEnabledForServer()) { m_interimPositionUpdateTimer.start(c_updateInterimPostionIntervalMsec); }
        }

        void CNetworkVatlib::stopPositionTimers()
        {
            m_positionUpdateTimer.stop();
            m_interimPositionUpdateTimer.stop();
        }

        QString CNetworkVatlib::convertToUnicodeEscaped(const QString &str)
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
                    escaped += "\\u";
                    escaped += QString::number(code, 16).rightJustified(4, '0');
                }
            }
            return escaped;
        }

        VatSimType CNetworkVatlib::convertToSimType(CSimulatorPluginInfo &simInfo)
        {
            //! \fixme Define recognized simulators somewhere */
            if (simInfo.getSimulator() == "fs9")
            {
                return vatSimTypeMSFS2004;
            }
            else if (simInfo.getSimulator() == "fsx")
            {
                return vatSimTypeMSFSX;
            }
            else if (simInfo.getSimulator() == "p3d")
            {
                return vatSimTypeP3Dv4; // we always set the latest, as we have only one flag
            }
            else if (simInfo.getSimulator() == "xplane")
            {
                return vatSimTypeXPLANE11; // latest, as there is only one flag
            }
            else
            {
                return vatSimTypeUnknown;
            }
        }

        /********************************** * * * * * * * * * * * * * * * * * * * ************************************/
        /**********************************             INetwork functions        ************************************/
        /********************************** * * * * * * * * * * * * * * * * * * * ************************************/

        void CNetworkVatlib::presetServer(const CServer &server)
        {
            Q_ASSERT_X(isDisconnected(), Q_FUNC_INFO, "Can't change server details while still connected");

            // If the server type changed, we need to destroy the existing vatlib session
            if (m_server.getServerType() != server.getServerType()) { m_net.reset(); }

            m_server = server;
            const QString codecName(server.getFsdSetup().getTextCodec());
            Q_ASSERT_X(!codecName.isEmpty(), Q_FUNC_INFO, "Missing code name");
            m_fsdTextCodec = QTextCodec::codecForName(codecName.toLocal8Bit());
            if (!m_fsdTextCodec) { m_fsdTextCodec = QTextCodec::codecForName("latin1"); }
        }

        void CNetworkVatlib::presetSimulatorInfo(const CSimulatorPluginInfo &simInfo)
        {
            Q_ASSERT_X(isDisconnected(), Q_FUNC_INFO, "Can't change server details while still connected");
            m_simulatorInfo = simInfo;
        }

        void CNetworkVatlib::presetCallsign(const CCallsign &callsign)
        {
            Q_ASSERT_X(isDisconnected(), Q_FUNC_INFO, "Can't change callsign while still connected");
            m_ownCallsign = callsign;
            updateOwnCallsign(callsign);
        }

        void CNetworkVatlib::presetIcaoCodes(const CSimulatedAircraft &ownAircraft)
        {
            Q_ASSERT_X(isDisconnected(), Q_FUNC_INFO, "Can't change ICAO codes while still connected");
            m_ownAircraftIcaoCode  = ownAircraft.getAircraftIcaoCode();
            m_ownAirlineIcaoCode   = ownAircraft.getAirlineIcaoCode();
            m_ownLiveryDescription = removeColon(ownAircraft.getModel().getSwiftLiveryString());
            updateOwnIcaoCodes(m_ownAircraftIcaoCode, m_ownAirlineIcaoCode);
        }

        void CNetworkVatlib::presetLoginMode(LoginMode mode)
        {
            Q_ASSERT_X(isDisconnected(), Q_FUNC_INFO, "Can't change login mode while still connected");
            m_loginMode = mode;
            m_net.reset(nullptr);
        }

        void CNetworkVatlib::initiateConnection()
        {
            Q_ASSERT_X(isDisconnected(), Q_FUNC_INFO, "Can't connect while still connected");
            if (!m_net) { initializeSession(); }
            this->clearState();
            m_filterPasswordFromLogin = true;
            QByteArray callsign = toFSDnoColon(m_ownCallsign.asString());
            QByteArray name;
            if (m_loginMode == LoginAsObserver)
            {
                // Observer mode
                VatAtcConnection info;
                name = toFSDnoColon(m_server.getUser().getRealName());
                info.name = name.data();
                info.rating = vatAtcRatingObserver;
                info.callsign = callsign.data();
                Vat_SpecifyATCLogon(m_net.data(), toFSD(m_server.getAddress()), m_server.getPort(),
                                    toFSDnoColon(m_server.getUser().getId()),
                                    toFSDnoColon(m_server.getUser().getPassword()),
                                    &info);

                // statistics
                this->increaseStatisticsValue(QStringLiteral("Vat_SpecifyATCLogon"));
            }
            else
            {
                // normal scenario, also used in STEALTH
                VatPilotConnection info;
                info.callsign = callsign.data();
                name = toFSDnoColon(m_server.getUser().getRealNameAndHomeBase());
                info.name = name.data();
                info.rating = vatPilotRatingStudent; // as documented, expected to be vatPilotRatingStudent only
                info.simType = convertToSimType(m_simulatorInfo);
                Vat_SpecifyPilotLogon(m_net.data(), toFSD(m_server.getAddress()), m_server.getPort(),
                                      toFSDnoColon(m_server.getUser().getId()),
                                      toFSDnoColon(m_server.getUser().getPassword()),
                                      &info);

                // statistics
                this->increaseStatisticsValue(QStringLiteral("Vat_SpecifyPilotLogon"));
            }

            Vat_Logon(m_net.data());
            this->startPositionTimers();

            // statistics
            this->increaseStatisticsValue(QStringLiteral("Vat_Logon"));
        }

        void CNetworkVatlib::terminateConnection()
        {
            this->stopPositionTimers();
            if (m_net && !isDisconnected())
            {
                // Process all pending tasks before logging off
                process();
                Vat_Logoff(m_net.data());

                // statistics
                this->increaseStatisticsValue(QStringLiteral("Vat_Logoff"));
            }
            this->clearState();
        }

        void CNetworkVatlib::sendTextMessages(const CTextMessageList &messages)
        {
            BLACK_VERIFY_X(this->isConnected(), Q_FUNC_INFO, "Sending text message, but not connected");
            if (!this->isConnected()) { return; }
            if (messages.isEmpty()) { return; }
            CTextMessageList privateMessages = messages.getPrivateMessages();
            privateMessages.markAsSent();
            for (const auto &message : as_const(privateMessages))
            {
                if (message.getRecipientCallsign().isEmpty()) { continue; }
                Vat_SendTextMessage(m_net.data(), toFSD(message.getRecipientCallsign()), toFSD(message.getMessage()));
                emit this->textMessageSent(message);

                // statistics
                this->increaseStatisticsValue(QStringLiteral("Vat_SendTextMessage"));
            }

            CTextMessageList radioMessages = messages.getRadioMessages();
            radioMessages.markAsSent();
            QVector<int> freqsVec;
            for (const auto &message : radioMessages)
            {
                // I could send the same message to n frequencies in one step
                // if this is really required, I need to group by message
                // currently I send individual messages
                freqsVec.clear();
                int freqkHz = message.getFrequency().valueInteger(CFrequencyUnit::kHz());
                if (m_server.getServerType() == CServer::FSDServerVatsim)
                {
                    // VATSIM always drops the last 5 kHz.
                    freqkHz = freqkHz / 10 * 10;
                }
                freqsVec.push_back(freqkHz);
                Vat_SendRadioMessage(m_net.data(), freqsVec.data(), static_cast<unsigned int>(freqsVec.size()), toFSDnoColon(message.getMessage()));
                emit this->textMessageSent(message);

                // statistics
                this->increaseStatisticsValue(QStringLiteral("Vat_SendRadioMessage"));
            }
        }

        void CNetworkVatlib::sendWallopMessage(const QString &message)
        {
            if (message.isEmpty()) {return; }
            BLACK_VERIFY_X(this->isConnected(), Q_FUNC_INFO, "Sending wallop, but not connected");
            if (!this->isConnected()) { return; }
            Vat_SendWallop(m_net.data(), toFSDnoColon(simplifyTextMessage(message)));

            // statistics
            this->increaseStatisticsValue(QStringLiteral("Vat_SendWallop"));
        }

        void CNetworkVatlib::sendCustomPacket(const CCallsign &callsign, const QString &packetId, const QStringList &data)
        {
            Q_ASSERT_X(isConnected(), Q_FUNC_INFO, "Can't send to server when disconnected");
            Vat_SendCustomPilotPacket(m_net.data(), toFSD(callsign), toFSD(packetId), toFSD(data)(), data.size());

            // statistics
            this->increaseStatisticsValue(QStringLiteral("Vat_SendCustomPilotPacket"));
        }

        void CNetworkVatlib::sendIpQuery()
        {
            Q_ASSERT_X(isConnected(), Q_FUNC_INFO, "Can't send to server when disconnected");
            Vat_SendClientQuery(m_net.data(), vatClientQueryIP, nullptr);

            // statistics
            this->increaseStatisticsValue(QStringLiteral("Vat_SendClientQuery"));
        }

        void CNetworkVatlib::sendFrequencyQuery(const CCallsign &callsign)
        {
            Q_ASSERT_X(isConnected(), Q_FUNC_INFO, "Can't send to server when disconnected");
            Vat_SendClientQuery(m_net.data(), vatClientQueryFreq, toFSD(callsign));

            // statistics
            this->increaseStatisticsValue(QStringLiteral("Vat_SendClientQuery"));
        }

        void CNetworkVatlib::sendUserInfoQuery(const CCallsign &callsign)
        {
            Q_ASSERT_X(isConnected(), Q_FUNC_INFO, "Can't send to server when disconnected");
            Vat_SendClientQuery(m_net.data(), vatClientQueryInfo, toFSD(callsign));

            // statistics
            this->increaseStatisticsValue(QStringLiteral("Vat_SendClientQuery"));
        }

        void CNetworkVatlib::setInterimPositionReceivers(const CCallsignSet &receivers)
        {
            m_interimPositionReceivers = receivers;
        }

        const CCallsignSet &CNetworkVatlib::getInterimPositionReceivers() const
        {
            return m_interimPositionReceivers;
        }

        QStringList CNetworkVatlib::vatlibArguments()
        {
            QStringList args;
            int id = 0;
            QString key;
            if (!CNetworkVatlib::getCmdLineClientIdAndKey(id, key)) { return args; }

            // from cmd. line
            args << "--idAndKey";
            args << sApp->getParserValue("clientIdAndKey"); // as typed in
            return args;
        }

        void CNetworkVatlib::sendServerQuery(const CCallsign &callsign)
        {
            Q_ASSERT_X(isConnected(), Q_FUNC_INFO, "Can't send to server when disconnected");
            Vat_SendClientQuery(m_net.data(), vatClientQueryServer, toFSD(callsign));

            // statistics
            this->increaseStatisticsValue(QStringLiteral("Vat_SendClientQuery"));
        }

        void CNetworkVatlib::sendAtcQuery(const CCallsign &callsign)
        {
            Q_ASSERT_X(isConnected(), Q_FUNC_INFO, "Can't send to server when disconnected");
            Vat_SendClientQuery(m_net.data(), vatClientQueryAtc, toFSD(callsign));

            // statistics
            this->increaseStatisticsValue(QStringLiteral("Vat_SendClientQuery"));
        }

        void CNetworkVatlib::sendAtisQuery(const CCallsign &callsign)
        {
            Q_ASSERT_X(isConnected(), Q_FUNC_INFO, "Can't send to server when disconnected");
            if (m_server.getServerType() != CServer::FSDServerVatsim)
            {
                m_pendingAtisQueries.insert(callsign, {});
            }
            Vat_SendClientQuery(m_net.data(), vatClientQueryAtis, toFSD(callsign));

            // statistics
            this->increaseStatisticsValue(QStringLiteral("Vat_SendClientQuery"));
        }

        void CNetworkVatlib::sendFlightPlan(const CFlightPlan &flightPlan)
        {
            Q_ASSERT_X(isConnected(), Q_FUNC_INFO, "Can't send to server when disconnected");

            VatFlightPlan vatlibFP;

            // Removed with T353 although it is standard
            // const QString route = QString(flightPlan.getRoute()).replace(" ", ".");
            const QString route = flightPlan.getRoute();
            const QString remarks = QString(flightPlan.getRemarks());

            //! \fixme that would be the official string, can this be used?
            const QString alt = flightPlan.getCruiseAltitude().asFpVatsimAltitudeString();
            // const QString alt = flightPlan.getCruiseAltitude().asFpAltitudeString();

            QString act = flightPlan.getCombinedPrefixIcaoSuffix();
            if (act.isEmpty()) { act = flightPlan.getAircraftIcao().getDesignator(); } // fallback

            QByteArray acTypeTemp, altAptTemp, cruiseAltTemp, depAptTemp, destAptTemp, routeTemp, remarksTemp;
            vatlibFP.aircraftType     = acTypeTemp    = toFSDnoColon(act);
            vatlibFP.alternateAirport = altAptTemp    = toFSDnoColon(flightPlan.getAlternateAirportIcao().asString());
            vatlibFP.cruiseAltitude   = cruiseAltTemp = toFSDnoColon(alt);
            vatlibFP.departAirport    = depAptTemp    = toFSDnoColon(flightPlan.getOriginAirportIcao().asString());
            vatlibFP.destAirport      = destAptTemp   = toFSDnoColon(flightPlan.getDestinationAirportIcao().asString());
            vatlibFP.departTimeActual = flightPlan.getTakeoffTimeActual().toUTC().toString("hhmm").toInt();
            vatlibFP.departTime       = flightPlan.getTakeoffTimePlanned().toUTC().toString("hhmm").toInt();

            QList<int> timeParts = flightPlan.getEnrouteTime().getHrsMinSecParts();
            vatlibFP.enrouteHrs  = timeParts[CTime::Hours];
            vatlibFP.enrouteMins = timeParts[CTime::Minutes];

            timeParts = flightPlan.getFuelTime().getHrsMinSecParts();
            vatlibFP.fuelHrs  = timeParts[CTime::Hours];
            vatlibFP.fuelMins = timeParts[CTime::Minutes];
            vatlibFP.remarks  = remarksTemp = toFSDnoColon(remarks);
            vatlibFP.route    = routeTemp = toFSDnoColon(route);
            vatlibFP.trueCruisingSpeed = flightPlan.getCruiseTrueAirspeed().valueInteger(CSpeedUnit::kts());
            switch (flightPlan.getFlightRules())
            {
            default:
            case CFlightPlan::IFR:  vatlibFP.flightType = vatFlightTypeIFR; break;
            case CFlightPlan::VFR:  vatlibFP.flightType = vatFlightTypeVFR; break;
            case CFlightPlan::SVFR: vatlibFP.flightType = vatFlightTypeSVFR; break;
            case CFlightPlan::DVFR: vatlibFP.flightType = vatFlightTypeDVFR; break;
            }
            Vat_SendFlightPlan(m_net.data(), &vatlibFP);

            // statistics
            this->increaseStatisticsValue(QStringLiteral("Vat_SendFlightPlan"));
        }

        void CNetworkVatlib::sendFlightPlanQuery(const CCallsign &callsign)
        {
            Q_ASSERT_X(isConnected(), Q_FUNC_INFO, "Can't send to server when disconnected");
            Vat_SendClientQuery(m_net.data(), vatClientQueryFP, toFSD(callsign));

            // statistics
            this->increaseStatisticsValue(QStringLiteral("Vat_SendClientQuery"));
        }

        void CNetworkVatlib::sendRealNameQuery(const CCallsign &callsign)
        {
            Q_ASSERT_X(isConnected(), Q_FUNC_INFO, "Can't send to server when disconnected");
            Vat_SendClientQuery(m_net.data(), vatClientQueryName, toFSD(callsign));

            // statistics
            this->increaseStatisticsValue(QStringLiteral("Vat_SendClientQuery"));
        }

        void CNetworkVatlib::sendCapabilitiesQuery(const CCallsign &callsign)
        {
            Q_ASSERT_X(isConnected(), Q_FUNC_INFO, "Can't send to server when disconnected");
            Vat_SendClientQuery(m_net.data(), vatClientQueryCaps, toFSD(callsign));

            // statistics
            this->increaseStatisticsValue(QStringLiteral("Vat_SendClientQuery"));
        }

        void CNetworkVatlib::replyToFrequencyQuery(const CCallsign &callsign) // private
        {
            QStringList response { QString::number(getOwnAircraft().getCom1System().getFrequencyActive().value(CFrequencyUnit::MHz()), 'f', 3)};
            Vat_SendClientQueryResponse(m_net.data(), vatClientQueryFreq, toFSD(callsign), toFSD(response)(), response.size());

            // statistics
            this->increaseStatisticsValue(QStringLiteral("Vat_SendClientQueryResponse"));
        }

        void CNetworkVatlib::replyToNameQuery(const CCallsign &callsign) // private
        {
            QStringList response { removeColon(m_server.getUser().getRealNameAndHomeBase()), "" };
            Vat_SendClientQueryResponse(m_net.data(), vatClientQueryName, toFSD(callsign), toFSD(response)(), response.size());

            // statistics
            this->increaseStatisticsValue(QStringLiteral("Vat_SendClientQueryResponse"));
        }

        void CNetworkVatlib::replyToConfigQuery(const CCallsign &callsign)
        {
            QJsonObject config = this->getOwnAircraftParts().toJson();
            config.insert(CAircraftParts::attributeNameIsFullJson(), true);
            QString data = QJsonDocument(QJsonObject { { "config", config } }).toJson(QJsonDocument::Compact);
            data = convertToUnicodeEscaped(data);
            Vat_SendAircraftConfig(m_net.data(), toFSD(callsign), toFSD(data));

            // statistics
            this->increaseStatisticsValue(QStringLiteral("Vat_SendAircraftConfig"));
        }

        void CNetworkVatlib::sendIcaoCodesQuery(const CCallsign &callsign)
        {
            Q_ASSERT_X(isConnected(), Q_FUNC_INFO, "Can't send to server when disconnected");
            Vat_RequestAircraftInfo(m_net.data(), toFSD(callsign));

            // statistics
            this->increaseStatisticsValue(QStringLiteral("Vat_RequestAircraftInfo"));
        }

        void CNetworkVatlib::sendAircraftInfo(const CCallsign &callsign) // private
        {
            const QString airlineIcao = m_server.getFsdSetup().force3LetterAirlineCodes() ? m_ownAirlineIcaoCode.getDesignator() : m_ownAirlineIcaoCode.getVDesignator();
            const QByteArray acTypeICAObytes  = toFSDnoColon(m_ownAircraftIcaoCode.getDesignator());
            const QByteArray liverybytes      = toFSDnoColon(m_ownLiveryDescription);
            const QByteArray airlineICAObytes = toFSDnoColon(airlineIcao);

            VatAircraftInfo aircraftInfo {acTypeICAObytes, airlineICAObytes, liverybytes};
            Vat_SendAircraftInfo(m_net.data(), toFSD(callsign), &aircraftInfo);

            // statistics
            this->increaseStatisticsValue(QStringLiteral("Vat_SendAircraftInfo"));
        }

        void CNetworkVatlib::sendIncrementalAircraftConfig()
        {
            if (!this->isConnected()) { return; }
            if (!this->getSetupForServer().sendAircraftParts()) { return; }
            const CAircraftParts currentParts(this->getOwnAircraftParts());

            // If it hasn't changed, return
            if (m_sentAircraftConfig == currentParts) { return; }

            if (!m_tokenBucket.tryConsume())
            {
                // If timer is not yet active, start it
                if (!m_scheduledConfigUpdate.isActive()) m_scheduledConfigUpdate.start(1000);
                return;
            }

            // Method could have been triggered by another change in aircraft config
            // so a previous update might still be scheduled. Stop it.
            if (m_scheduledConfigUpdate.isActive()) m_scheduledConfigUpdate.stop();
            const QJsonObject previousConfig = m_sentAircraftConfig.toJson();
            const QJsonObject currentConfig = currentParts.toJson();
            const QJsonObject incrementalConfig = getIncrementalObject(previousConfig, currentConfig);
            broadcastAircraftConfig(incrementalConfig);
            m_sentAircraftConfig = currentParts;
        }

        void CNetworkVatlib::sendPing(const CCallsign &callsign)
        {
            Q_ASSERT_X(isConnected(), Q_FUNC_INFO, "Can't send to server when disconnected");
            Vat_SendPing(m_net.data(), toFSD(callsign));

            // statistics
            this->increaseStatisticsValue(QStringLiteral("Vat_SendPing"));
        }

        void CNetworkVatlib::sendMetarQuery(const CAirportIcaoCode &airportIcao)
        {
            Q_ASSERT_X(isConnected(), Q_FUNC_INFO, "Can't send to server when disconnected");
            Vat_RequestMetar(m_net.data(), toFSD(airportIcao.asString()));

            // statistics
            this->increaseStatisticsValue(QStringLiteral("Vat_RequestMetar"));
        }

        const QList<QCommandLineOption> &CNetworkVatlib::getCmdLineOptions()
        {
            static const QList<QCommandLineOption> e;
            static const QList<QCommandLineOption> opts
            {
                QCommandLineOption({ "idAndKey", "clientIdAndKey" },
                QCoreApplication::translate("networkvatlib", "Client id and key pair separated by ':', e.g. <id>:<key>."), "clientIdAndKey")
            };

            // only in not officially shipped versions
            return (CBuildConfig::isLocalDeveloperDebugBuild()) ? opts : e;
        }

        bool CNetworkVatlib::getCmdLineClientIdAndKey(int &id, QString &key)
        {
            // init values
            id = 0;
            key = "";

            // split parser values
            if (CNetworkVatlib::getCmdLineOptions().isEmpty()) { return false; } // no such option, avoid warnings
            const QString clientIdAndKey = sApp->getParserValue("clientIdAndKey").toLower();
            if (clientIdAndKey.isEmpty() || !clientIdAndKey.contains(':')) { return false; }
            const QStringList stringList = clientIdAndKey.split(':');
            const QString clientIdAsString = stringList[0];
            bool ok = true;
            id = clientIdAsString.toInt(&ok, 0); // base 0 means C convention
            if (!ok || id == 0) { return false; }
            key = stringList[1];
            return true;
        }

        void CNetworkVatlib::sendCustomFsinnQuery(const CCallsign &callsign)
        {
            Q_ASSERT_X(isConnected(), Q_FUNC_INFO, "Can't send to server when disconnected");
            const CSimulatedAircraft myAircraft(getOwnAircraft());
            const QString modelString = myAircraft.hasModelString() ? myAircraft.getModelString() : noModelString();
            const QStringList data { { "0" },
                myAircraft.getAirlineIcaoCodeDesignator(),
                myAircraft.getAircraftIcaoCodeDesignator(),
                { "" }, { "" }, { "" }, { "" },
                myAircraft.getAircraftIcaoCombinedType(),
                removeColon(modelString)
            };
            sendCustomPacket(callsign, "FSIPIR", data);
        }

        void CNetworkVatlib::sendCustomFsinnReponse(const CCallsign &callsign)
        {
            Q_ASSERT_X(isConnected(), Q_FUNC_INFO, "Can't send to server when disconnected");

            static const QStringList dataTemplate{ "0", "", "", "", "", "", "", "", "" };
            const CSimulatedAircraft myAircraft(getOwnAircraft());

            QStringList data(dataTemplate);
            data[1] = myAircraft.getAirlineIcaoCodeDesignator();
            data[2] = myAircraft.getAircraftIcaoCodeDesignator();
            data[7] = myAircraft.getAircraftIcaoCombinedType();
            data[8] = removeColon(myAircraft.hasModelString() ? myAircraft.getModelString() : noModelString());
            this->sendCustomPacket(callsign, "FSIPI", data);
        }

        void CNetworkVatlib::broadcastAircraftConfig(const QJsonObject &config)
        {
            const QString dataStr = convertToUnicodeEscaped(QJsonDocument(QJsonObject { { "config", config } }).toJson(QJsonDocument::Compact));
            const QByteArray data(toFSD(dataStr));
            Vat_SendAircraftConfigBroadcast(m_net.data(), data);

            // statistics
            this->increaseStatisticsValue(QStringLiteral("Vat_SendAircraftConfigBroadcast"));
        }

        void CNetworkVatlib::sendAircraftConfigQuery(const CCallsign &callsign)
        {
            static const QString dataStr { QJsonDocument(JsonPackets::aircraftConfigRequest()).toJson(QJsonDocument::Compact) };
            static const QByteArray data(toFSD(dataStr));
            Vat_SendAircraftConfig(m_net.data(), toFSD(callsign), data);

            // statistics
            this->increaseStatisticsValue(QStringLiteral("Vat_SendAircraftConfig"));
        }

        /********************************** * * * * * * * * * * * * * * * * * * * ************************************/
        /**********************************           shimlib callbacks           ************************************/
        /********************************** * * * * * * * * * * * * * * * * * * * ************************************/

        // Cast void* to a pointer of CNetworkVatlib
        CNetworkVatlib *cbvar_cast(void *cbvar)
        {
            return static_cast<CNetworkVatlib *>(cbvar);
        }

        void CNetworkVatlib::onConnectionStatusChanged(VatFsdClient *, VatConnectionStatus, VatConnectionStatus newStatus, void *cbvar)
        {
            cbvar_cast(cbvar)->changeConnectionStatus(newStatus);
        }

        void CNetworkVatlib::onTextMessageReceived(VatFsdClient *, const char *from, const char *to, const char *msg, void *cbvar)
        {
            auto *self = cbvar_cast(cbvar);

            // statistics
            self->increaseStatisticsValue(__func__);

            const CCallsign sender(self->fromFSD(from));
            const CCallsign receiver(self->fromFSD(to));
            const QString message(self->fromFSD(msg));

            // Other FSD servers send the controller ATIS as text message. The following conditions need to be met:
            // * non-VATSIM server. VATSIM has a specific ATIS message
            // * Receiver callsign must be owner callsign and not any type of broadcast.
            // * We have requested the ATIS of this controller before.
            if (self->m_server.getServerType() != CServer::FSDServerVatsim &&
                    self->m_ownCallsign == receiver &&
                    self->m_pendingAtisQueries.contains(sender))
            {
                self->maybeHandleAtisReply(sender, receiver, message);
                return;
            }

            CTextMessage tm(message, sender, receiver);
            tm.setCurrentUtcTime();
            cbvar_cast(cbvar)->consolidateTextMessage(tm);
        }

        void CNetworkVatlib::onRadioMessageReceived(VatFsdClient *, const char *from, unsigned int numFreq, int *freqList, const char *msg, void *cbvar)
        {
            auto *self = cbvar_cast(cbvar);

            // statistics
            self->increaseStatisticsValue(__func__);

            const CFrequency com1 = self->getOwnAircraft().getCom1System().getFrequencyActive();
            const CFrequency com2 = self->getOwnAircraft().getCom2System().getFrequencyActive();
            QList<CFrequency> frequencies;
            for (unsigned int i = 0; i < numFreq; ++i)
            {
                CFrequency f(freqList[i], CFrequencyUnit::kHz());
                // VATSIM always drops the last 5 kHz. So round it to the correct channel spacing.
                CComSystem::roundToChannelSpacing(f, CComSystem::ChannelSpacing25KHz);
                if (f == com1 || f == com2)
                {
                    frequencies.push_back(f);
                }
            }
            if (frequencies.isEmpty()) { return; }
            CTextMessageList messages(self->fromFSD(msg), frequencies, CCallsign(self->fromFSD(from)));
            messages.setCurrentUtcTime();
            emit cbvar_cast(cbvar)->textMessagesReceived(messages);
        }

        void CNetworkVatlib::onPilotDisconnected(VatFsdClient *, const char *callsign, void *cbvar)
        {
            auto *self = cbvar_cast(cbvar);
            const CCallsign cs(self->fromFSD(callsign), CCallsign::Aircraft);
            self->clearState(cs);
            emit self->pilotDisconnected(cs);
        }

        void CNetworkVatlib::onControllerDisconnected(VatFsdClient *, const char *callsign, void *cbvar)
        {
            emit cbvar_cast(cbvar)->atcDisconnected(CCallsign(cbvar_cast(cbvar)->fromFSD(callsign), CCallsign::Atc));
        }

        void CNetworkVatlib::onPilotPositionUpdate(VatFsdClient *, const char *callsignChar, const VatPilotPosition *position, void *cbvar)
        {
            auto *self = cbvar_cast(cbvar);

            // statistics
            self->increaseStatisticsValue(__func__);

            const CCallsign callsign(callsignChar, CCallsign::Aircraft);
            CAircraftSituation situation(
                callsign,
                CCoordinateGeodetic(position->latitude, position->longitude, position->altitudeTrue),
                CHeading(position->heading, CHeading::True, CAngleUnit::deg()),
                CAngle(position->pitch, CAngleUnit::deg()),
                CAngle(position->bank, CAngleUnit::deg()),
                CSpeed(position->groundSpeed, CSpeedUnit::kts())
            );

            // Ref T297, default offset time
            situation.setCurrentUtcTime();
            const qint64 offsetTimeMs = self->receivedPositionFixTsAndGetOffsetTime(situation.getCallsign(), situation.getMSecsSinceEpoch());
            situation.setTimeOffsetMs(offsetTimeMs);

            CTransponder::TransponderMode mode = CTransponder::StateStandby;
            switch (position->transponderMode)
            {
            case vatTransponderModeCharlie: mode = CTransponder::ModeC; break;
            case vatTransponderModeStandby: mode = CTransponder::StateStandby; break;
            case vatTransponderModeIdent:   mode = CTransponder::StateIdent; break;
            default: break;
            }

            // I did have a situation where I got wrong transponder codes (KB)
            // So I now check for a valid code in order to detect such codes
            CTransponder transponder;
            if (CTransponder::isValidTransponderCode(position->transponderCode))
            {
                transponder = CTransponder(position->transponderCode, mode);
            }
            else
            {
                if (CBuildConfig::isLocalDeveloperDebugBuild())
                {
                    CLogMessage(static_cast<CNetworkVatlib *>(nullptr)).debug(u"Wrong transponder code '%1' for '%2'") << position->transponderCode << callsign;
                }

                // I set a default: IFR standby is a reasonable default
                transponder = CTransponder(2000, CTransponder::StateStandby);
            }
            emit cbvar_cast(cbvar)->aircraftPositionUpdate(situation, transponder);
        }

        void CNetworkVatlib::onAircraftConfigReceived(VatFsdClient *, const char *callsignChar, const char *aircraftConfig, void *cbvar)
        {
            auto *self = cbvar_cast(cbvar);
            const CCallsign callsign(self->fromFSD(callsignChar), CCallsign::Aircraft);

            // statistics
            self->increaseStatisticsValue(__func__);

            QJsonParseError parserError;
            const QByteArray json = self->fromFSD(aircraftConfig).toUtf8();
            const QJsonDocument doc = QJsonDocument::fromJson(json, &parserError);

            if (parserError.error != QJsonParseError::NoError)
            {
                CLogMessage(self).warning(u"Failed to parse aircraft config packet: '%1' packet: '%2'") << parserError.errorString() << QString(json);
                return; // we cannot parse the packet, so we give up here
            }

            const QJsonObject packet = doc.object();
            if (packet == JsonPackets::aircraftConfigRequest())
            {
                self->replyToConfigQuery(callsign);
                return;
            }

            if (!self->getSetupForServer().receiveAircraftParts()) { return; }
            const QJsonObject config = doc.object().value("config").toObject();
            if (config.empty()) { return; }

            const qint64 offsetTimeMs = self->currentOffsetTime(callsign);
            emit self->aircraftConfigPacketReceived(callsign, config, offsetTimeMs);
        }

        void CNetworkVatlib::onInterimPilotPositionUpdate(VatFsdClient *, const char *sender, const VatInterimPilotPosition *position, void *cbvar)
        {
            CNetworkVatlib *self = cbvar_cast(cbvar);
            if (!self->isInterimPositionReceivingEnabledForServer()) { return; }

            // statistics
            self->increaseStatisticsValue(__func__);

            CAircraftSituation situation(
                CCallsign(self->fromFSD(sender), CCallsign::Aircraft),
                CCoordinateGeodetic(position->latitude, position->longitude, position->altitudeTrue),
                CHeading(position->heading, CHeading::True, CAngleUnit::deg()),
                CAngle(position->pitch, CAngleUnit::deg()),
                CAngle(position->bank, CAngleUnit::deg()),
                CSpeed::null() // there is no speed information in an interim packet
            );

            // Ref T297, default offset time
            situation.setCurrentUtcTime();
            const qint64 offsetTimeMs = self->receivedPositionFixTsAndGetOffsetTime(situation.getCallsign(), situation.getMSecsSinceEpoch());
            situation.setTimeOffsetMs(offsetTimeMs);
            situation.setInterimFlag(true);

            emit self->aircraftInterimPositionUpdate(situation);
        }

        void CNetworkVatlib::onAtcPositionUpdate(VatFsdClient *, const char *callsign, const VatAtcPosition *pos, void *cbvar)
        {
            CNetworkVatlib *self = cbvar_cast(cbvar);

            // statistics
            self->increaseStatisticsValue(__func__);

            const int frequencyKHz = pos->frequency;
            CFrequency freq(frequencyKHz, CFrequencyUnit::kHz());
            freq.switchUnit(CFrequencyUnit::MHz()); // we would not need to bother, but this makes it easier to identify
            CLength range(pos->visibleRange, CLengthUnit::NM());
            const QString cs = cbvar_cast(cbvar)->fromFSD(callsign);

            // Filter non-ATC like OBS stations, like pilots logging in as shared cockpit co-pilots.
            if (pos->facility == vatFacilityTypeUnknown && !cs.endsWith("_OBS")) { return; }

            // ATIS often have a range of 0 nm. Correct this to a proper value.
            if (cs.contains("_ATIS") && pos->visibleRange == 0) { range.setValueSameUnit(150.0); }
            const CCoordinateGeodetic position(pos->latitude, pos->longitude, 0);

            emit self->atcPositionUpdate(CCallsign(cs, CCallsign::Atc), freq, position, range);
        }

        void CNetworkVatlib::onKicked(VatFsdClient *, const char *reason, void *cbvar)
        {
            emit cbvar_cast(cbvar)->kicked(cbvar_cast(cbvar)->fromFSD(reason));
        }

        void CNetworkVatlib::onPong(VatFsdClient *, const char *sender, double elapsedTime, void *cbvar)
        {
            emit cbvar_cast(cbvar)->pongReceived(cbvar_cast(cbvar)->fromFSD(sender), CTime(elapsedTime, CTimeUnit::ms()));
        }

        void CNetworkVatlib::onCustomPacketReceived(VatFsdClient *, const char *callsign, const char *packetId, const char **data, int dataSize, void *cbvar)
        {
            // statistics en detail handled in customPacketDispatcher
            CNetworkVatlib *self = cbvar_cast(cbvar);
            self->increaseStatisticsValue(__func__);
            self->customPacketDispatcher(cbvar_cast(cbvar)->fromFSD(callsign), cbvar_cast(cbvar)->fromFSD(packetId), cbvar_cast(cbvar)->fromFSD(data, dataSize));
        }

        void CNetworkVatlib::onRawFsdMessage(VatFsdClient *, const char *message, void *cbvar)
        {
            cbvar_cast(cbvar)->handleRawFsdMessage(cbvar_cast(cbvar)->fromFSD(message));
        }

        void CNetworkVatlib::customPacketDispatcher(const CCallsign &callsign, const QString &packetId, const QStringList &data)
        {
            if (packetId.compare("FSIPI", Qt::CaseInsensitive) == 0)
            {
                if (data.size() < 9)
                {
                    CLogMessage(this).warning(u"Malformed FSIPI packet");
                }
                else
                {
                    // It doesn't matter whether it was a query or response. The information
                    // is the same for both.
                    emit this->customFSInnPacketReceived(callsign, data[1], data[2], data[7], data[8]);

                    // statistics
                    this->increaseStatisticsValue(__func__, packetId);
                }
            }
            else if (packetId.compare("FSIPIR", Qt::CaseInsensitive) == 0)
            {
                if (data.size() < 9)
                {
                    CLogMessage(this).warning(u"Malformed FSIPIR packet");
                }
                else
                {
                    this->sendCustomFsinnReponse(callsign);
                    // It doesn't matter whether it was a query or response. The information
                    // is the same for both.
                    emit this->customFSInnPacketReceived(callsign, data[1], data[2], data[7], data[8]);

                    // statistics
                    this->increaseStatisticsValue(__func__, packetId);
                }
            }
            else if (packetId.compare("FSIP2PR", Qt::CaseInsensitive) == 0)
            {
                // FSInn peer2peer protocol - ignore, not supported

                // statistics
                this->increaseStatisticsValue(__func__, packetId);
            }
            else if (packetId.compare("FSIP2P", Qt::CaseInsensitive) == 0)
            {
                // FSInn peer2peer protocol - ignore, not supported

                // statistics
                this->increaseStatisticsValue(__func__, packetId);
            }
            else
            {
                // statistics
                this->increaseStatisticsValue(__func__, packetId);

                CLogMessage(this).warning(u"Unknown custom packet from %1 - id: %2") << callsign.toQString() << packetId;
            }
        }

        void CNetworkVatlib::handleRawFsdMessage(const QString &fsdMessage)
        {
            if (!m_rawFsdMessagesEnabled) { return; }
            QString fsdMessageFiltered(fsdMessage);
            if (m_filterPasswordFromLogin)
            {
                if (fsdMessageFiltered.startsWith("FSD Sent=>#AP"))
                {
                    thread_local const QRegularExpression re("^(FSD Sent=>#AP\\w+:SERVER:\\d+:)[^:]+(:\\d+:\\d+:\\d+:.+)$");
                    fsdMessageFiltered.replace(re, "\\1<password>\\2");
                    m_filterPasswordFromLogin = false;
                }
            }

            CRawFsdMessage rawFsdMessage(fsdMessageFiltered);
            rawFsdMessage.setCurrentUtcTime();
            if (m_rawFsdMessageLogFile.isOpen())
            {
                QTextStream stream(&m_rawFsdMessageLogFile);
                stream << rawFsdMessage.toQString() << endl;
            }
            emit rawFsdMessageReceived(rawFsdMessage);
        }

        void CNetworkVatlib::fsdMessageSettingsChanged()
        {
            if (!m_net) { return; }
            if (m_rawFsdMessageLogFile.isOpen()) { m_rawFsdMessageLogFile.close(); }
            const CRawFsdMessageSettings setting = m_fsdMessageSetting.get();

            // Workaround bug in vatlib v0.9.7. Handlers cannot be updated.
            m_rawFsdMessagesEnabled = setting.areRawFsdMessagesEnabled();
            /*if (!setting.areRawFsdMessagesEnabled())
            {
                Vat_SetFsdMessageHandler(m_net.data(), nullptr, this);
                return;
            }*/

            Vat_SetFsdMessageHandler(m_net.data(), CNetworkVatlib::onRawFsdMessage, this);

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
                const QString filename = u"rawfsdmessages" %
                                         QLatin1String("_") %
                                         QDateTime::currentDateTime().toString(QStringLiteral("yyMMddhhmmss")) %
                                         QLatin1String(".log");
                const QString filePath = CFileUtils::appendFilePaths(setting.getFileDir(), filename);
                m_rawFsdMessageLogFile.setFileName(filePath);
                m_rawFsdMessageLogFile.open(QIODevice::Text | QIODevice::WriteOnly);
            }
        }

        void CNetworkVatlib::consolidateTextMessage(const CTextMessage &textMessage)
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

        void CNetworkVatlib::emitConsolidatedTextMessages()
        {
            emit this->textMessagesReceived(m_textMessagesToConsolidate);
            m_textMessagesToConsolidate.clear();
        }

        void CNetworkVatlib::onMetarReceived(VatFsdClient *, const char *data, void *cbvar)
        {
            auto *self = cbvar_cast(cbvar);
            emit self->metarReplyReceived(self->fromFSD(data));
        }

        void CNetworkVatlib::maybeHandleAtisReply(const CCallsign &sender, const CCallsign &receiver, const QString &message)
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
                this->consolidateTextMessage(tm);
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
                CInformationMessage atisMessage;
                atisMessage.setType(CInformationMessage::ATIS);
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

        qint64 CNetworkVatlib::receivedPositionFixTsAndGetOffsetTime(const CCallsign &callsign, qint64 markerTs)
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

        qint64 CNetworkVatlib::currentOffsetTime(const CCallsign &callsign) const
        {
            Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "Need callsign");

            if (!m_lastOffsetTimes.contains(callsign) || m_lastOffsetTimes[callsign].isEmpty()) { return CFsdSetup::c_positionTimeOffsetMsec; }
            return m_lastOffsetTimes[callsign].front();
        }

        void CNetworkVatlib::clearState()
        {
            m_textMessagesToConsolidate.clear();
            m_pendingAtisQueries.clear();
            m_lastPositionUpdate.clear();
            m_lastOffsetTimes.clear();
            m_sentAircraftConfig = CAircraftParts::null();
        }

        void CNetworkVatlib::clearState(const CCallsign &callsign)
        {
            if (callsign.isEmpty()) { return; }
            m_pendingAtisQueries.remove(callsign);
            m_lastPositionUpdate.remove(callsign);
            m_interimPositionReceivers.remove(callsign);
            m_lastOffsetTimes.remove(callsign);
        }

        void CNetworkVatlib::insertLatestOffsetTime(const CCallsign &callsign, qint64 offsetMs)
        {
            QList<qint64> &offsets = m_lastOffsetTimes[callsign];
            offsets.push_front(offsetMs);
            if (offsets.size() > MaxOffseTimes) { offsets.removeLast(); }
        }

        qint64 CNetworkVatlib::averageOffsetTimeMs(const CCallsign &callsign, int &count, int maxLastValues) const
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

        qint64 CNetworkVatlib::averageOffsetTimeMs(const CCallsign &callsign, int maxLastValues) const
        {
            int count = 0;
            return this->averageOffsetTimeMs(callsign, maxLastValues, count);
        }

        QString CNetworkVatlib::removeColon(const QString &candidate)
        {
            QString r = candidate;
            return r.remove(':');
        }

        void CNetworkVatlib::onInfoQueryRequestReceived(VatFsdClient *, const char *callsignString, VatClientQueryType type, const char *, void *cbvar)
        {
            QPointer<CNetworkVatlib> self(cbvar_cast(cbvar));
            const CCallsign callsign(self->fromFSD(callsignString));
            const bool valid = !callsign.isEmpty();
            BLACK_AUDIT_X(valid, Q_FUNC_INFO, "No callsign");
            if (!valid) { return; }

            switch (type)
            {
            case vatClientQueryFreq:
                QTimer::singleShot(0, self, [ = ]() { if (self) self->replyToFrequencyQuery(callsign); });
                break;
            case vatClientQueryName:
                QTimer::singleShot(0, self, [ = ]() { if (self) self->replyToNameQuery(callsign); });
                break;
            default:
                break;
            }
        }

        void CNetworkVatlib::onInfoQueryReplyReceived(VatFsdClient *, const char *callsign, VatClientQueryType type, const char *data, const char *data2, void *cbvar)
        {
            auto *self = cbvar_cast(cbvar);
            switch (type)
            {
            case vatClientQueryFreq:   emit self->frequencyReplyReceived(self->fromFSD(callsign), CFrequency(self->fromFSD(data).toDouble(), CFrequencyUnit::MHz())); break;
            case vatClientQueryServer: emit self->serverReplyReceived(self->fromFSD(callsign), self->fromFSD(data)); break;
            case vatClientQueryAtc:    emit self->atcReplyReceived(CCallsign(self->fromFSD(data2), CCallsign::Atc), *data == 'Y'); break;
            case vatClientQueryName:   emit self->realNameReplyReceived(self->fromFSD(callsign), self->fromFSD(data)); break;
            case vatClientQueryIP:     emit self->ipReplyReceived(self->fromFSD(data)); break;
            default: break;
            }
        }

        void CNetworkVatlib::onCapabilitiesReplyReceived(VatFsdClient *, const char *callsign, int capabilityFlags, void *cbvar)
        {
            CClient::Capabilities caps = CClient::None;
            if (capabilityFlags & vatCapsAtcInfo) { caps |= CClient::FsdAtisCanBeReceived; }
            if (capabilityFlags & vatCapsFastPos) { caps |= CClient::FsdWithInterimPositions; }
            if (capabilityFlags & vatCapsAircraftInfo)   { caps |= CClient::FsdWithIcaoCodes; }
            if (capabilityFlags & vatCapsAircraftConfig) { caps |= CClient::FsdWithAircraftConfig; }
            auto *self = cbvar_cast(cbvar);
            emit self->capabilitiesReplyReceived(self->fromFSD(callsign), static_cast<int>(caps));
        }

        void CNetworkVatlib::onAtisReplyReceived(VatFsdClient *, const char *callsign, const VatControllerAtis *atis, void *cbvar)
        {
            auto *self = cbvar_cast(cbvar);
            const CCallsign cs(self->fromFSD(callsign), CCallsign::Atc);
            emit self->atisVoiceRoomReplyReceived(cs, self->fromFSD(atis->voiceRoom));
            emit self->atisLogoffTimeReplyReceived(cs, self->fromFSD(atis->zuluLogoff));

            CInformationMessage atisMessage;
            atisMessage.setType(CInformationMessage::ATIS);
            for (unsigned int i = 0; i < atis->textLineCount; ++i)
            {
                const QString fixed = cbvar_cast(cbvar)->fromFSD(atis->textLines[i]).trimmed();
                if (! fixed.isEmpty())
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

            emit self->atisReplyReceived(cs, atisMessage);
        }

        void CNetworkVatlib::onFlightPlanReceived(VatFsdClient *, const char *callsignChar, const VatFlightPlan *fp, void *cbvar)
        {
            CFlightPlan::FlightRules rules = CFlightPlan::VFR;
            switch (fp->flightType)
            {
            case vatFlightTypeVFR:  rules = CFlightPlan::VFR;  break;
            case vatFlightTypeIFR:  rules = CFlightPlan::IFR;  break;
            case vatFlightTypeDVFR: rules = CFlightPlan::DVFR; break;
            case vatFlightTypeSVFR: rules = CFlightPlan::SVFR; break;
            default: rules = CFlightPlan::UNKNOWN; break;
            }

            auto *self = cbvar_cast(cbvar);
            QString cruiseAltString = self->fromFSD(fp->cruiseAltitude).trimmed();
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

            const QString depTimePlanned = QString("0000").append(QString::number(fp->departTime)).right(4);
            const QString depTimeActual  = QString("0000").append(QString::number(fp->departTimeActual)).right(4);

            const CCallsign callsign(self->fromFSD(callsignChar), CCallsign::Aircraft);
            const CFlightPlan flightPlan(
                callsign,
                self->fromFSD(fp->aircraftType),
                self->fromFSD(fp->departAirport),
                self->fromFSD(fp->destAirport),
                self->fromFSD(fp->alternateAirport),
                fromStringUtc(depTimePlanned, "hhmm"),
                fromStringUtc(depTimeActual, "hhmm"),
                CTime(fp->enrouteHrs * 60 + fp->enrouteMins, CTimeUnit::min()),
                CTime(fp->fuelHrs * 60 + fp->fuelMins, CTimeUnit::min()),
                cruiseAlt,
                CSpeed(fp->trueCruisingSpeed, CSpeedUnit::kts()),
                rules,
                self->fromFSD(fp->route),
                self->fromFSD(fp->remarks)
            );

            emit self->flightPlanReplyReceived(callsign, flightPlan);
        }

        void CNetworkVatlib::onErrorReceived(VatFsdClient *, VatServerError error, const char *msg, const char *data, void *cbvar)
        {
            auto *self = cbvar_cast(cbvar);
            switch (error)
            {
            case vatServerErrorCsInUs:          CLogMessage(self).error(u"The requested callsign is already taken"); emit self->terminate(); break;
            case vatServerErrorCallsignInvalid: CLogMessage(self).error(u"The requested callsign is not valid"); emit self->terminate(); break;
            case vatServerErrorCidInvalid:      CLogMessage(self).error(u"Wrong user ID or password, inactive account"); emit self->terminate(); break;
            case vatServerErrorRevision:        CLogMessage(self).error(u"This server does not support our protocol version"); emit self->terminate(); break;
            case vatServerErrorLevel:           CLogMessage(self).error(u"You are not authorized to use the requested pilot rating"); emit self->terminate(); break;
            case vatServerErrorServFull:        CLogMessage(self).error(u"The server is full"); emit self->terminate(); break;
            case vatServerErrorCsSuspended:     CLogMessage(self).error(u"Your user account is suspended"); emit self->terminate(); break;
            case vatServerErrorInvPos:          CLogMessage(self).error(u"You are not authorized to use the requested rating"); emit self->terminate(); break;
            case vatServerErrorUnAuth:          CLogMessage(self).error(u"This software is not authorized for use on this network %1") << self->m_server.getName(); emit self->terminate(); break;

            case vatServerErrorNone:            CLogMessage(self).info(u"OK"); break;
            case vatServerErrorSyntax:          CLogMessage(self).info(u"Malformed packet: Syntax error: %1") << self->fromFSD(data); break;
            case vatServerErrorSrcInvalid:      CLogMessage(self).info(u"Server: source invalid %1") << self->fromFSD(data); break;
            case vatServerErrorNoSuchCs:        CLogMessage(self).info(u"FSD message was using an invalid callsign: %1 (%2)") << self->fromFSD(msg) << self->fromFSD(data); break;
            case vatServerErrorNoFP:            CLogMessage(self).info(u"Server: no flight plan"); break;
            case vatServerErrorNoWeather:       CLogMessage(self).info(u"Server: requested weather profile does not exist"); break;

            // we have no idea what these mean
            case vatServerErrorRegistered:      CLogMessage(self).info(u"vatServerErrorRegistered: ") << self->fromFSD(msg); break;
            case vatServerErrorInvalidCtrl:     CLogMessage(self).info(u"vatServerErrorInvalidCtrl: ") << self->fromFSD(msg); break;
            case vatServerWrongType:            CLogMessage(self).info(u"vatServerWrongType: ") << self->fromFSD(msg); break;

            // default:                         qFatal("vatlib: %s (error %d)", msg, error); emit self->terminate();
            // KB: Why the hard termination?
            default:                            CLogMessage(self).error(u"vatlib: %1 (error %2) server: '%3'") << msg << error << self->m_server.getName(); emit self->terminate(); break;
            }
        }

        void CNetworkVatlib::onPilotInfoRequestReceived(VatFsdClient *, const char *callsignChar, void *cbvar)
        {
            QPointer<CNetworkVatlib> self(cbvar_cast(cbvar));
            const CCallsign callsign(self->fromFSD(callsignChar));
            QTimer::singleShot(0, self, [ = ]() { if (self) { self->sendAircraftInfo(callsign); }});
        }

        void CNetworkVatlib::onPilotInfoReceived(VatFsdClient *, const char *callsignChar, const VatAircraftInfo *aircraftInfo, void *cbvar)
        {
            auto *self = cbvar_cast(cbvar);
            const CCallsign callsign(self->fromFSD(callsignChar), CCallsign::Aircraft);
            emit self->icaoCodesReplyReceived(
                callsign,
                self->fromFSD(aircraftInfo->aircraftType).trimmed().toUpper(),
                self->fromFSD(aircraftInfo->airline).trimmed().toUpper(),
                self->fromFSD(aircraftInfo->livery).trimmed().toUpper()
            );
        }

        void CNetworkVatlib::networkLogHandler(VatSeverityLevel /** severity **/, const char *context, const char *message)
        {
            const QString errorMessage = QStringLiteral("vatlib ") % context % QStringLiteral(": ") % message;
            CLogMessage(static_cast<CNetworkVatlib *>(nullptr)).error(errorMessage);
        }

        QString CNetworkVatlib::simplifyTextMessage(const QString &msg)
        {
            if (msg.isEmpty()) { return {}; }

            // per discussion of T519 only simplify, do not remove accents
            // return asciiOnlyString(simplifyAccents(msg.simplified().trimmed()));
            return msg.simplified().trimmed();
        }

        const QJsonObject &CNetworkVatlib::JsonPackets::aircraftConfigRequest()
        {
            static const QJsonObject jsonObject{ { "request", "full" } };
            return jsonObject;
        }
    } // namespace
} // namespace

//! \endcond
