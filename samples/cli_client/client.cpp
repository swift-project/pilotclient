/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "client.h"
#include "blackcore/network_vatlib.h"
#include <iostream>
#include <QStringList>

Client::Client(QObject *parent)
    : QObject(parent), m_net(new BlackCore::CNetworkVatlib(this))
{
    using namespace BlackCore;
    connect(m_net, &INetwork::atcPositionUpdate,                this, &Client::atcPositionUpdate);
    connect(m_net, &INetwork::atcDisconnected,                  this, &Client::atcDisconnected);
    connect(m_net, &INetwork::connectionStatusChanged,          this, &Client::connectionStatusChanged);
    connect(m_net, &INetwork::ipReplyReceived,                  this, &Client::ipReplyReceived);
    connect(m_net, &INetwork::frequencyReplyReceived,           this, &Client::freqReplyReceived);
    connect(m_net, &INetwork::serverReplyReceived,              this, &Client::serverReplyReceived);
    connect(m_net, &INetwork::atcReplyReceived,                 this, &Client::atcReplyReceived);
    connect(m_net, &INetwork::atisReplyReceived,                this, &Client::atisReplyReceived);
    connect(m_net, &INetwork::realNameReplyReceived,            this, &Client::realNameReplyReceived);
    connect(m_net, &INetwork::capabilitiesReplyReceived,        this, &Client::capabilitiesReplyReceived);
    connect(m_net, &INetwork::kicked,                           this, &Client::kicked);
    connect(m_net, &INetwork::metarReplyReceived,               this, &Client::metarReplyReceived);
    connect(m_net, &INetwork::flightPlanReplyReceived,          this, &Client::flightPlanReplyReceived);
    connect(m_net, &INetwork::pilotDisconnected,                this, &Client::pilotDisconnected);
    connect(m_net, &INetwork::icaoCodesReplyReceived,           this, &Client::icaoCodesReplyReceived);
    connect(m_net, &INetwork::pongReceived,                     this, &Client::pongReceived);
    connect(m_net, &INetwork::textMessagesReceived,             this, &Client::textMessagesReceived);
    connect(m_net, &INetwork::customPacketReceived,             this, &Client::customPacketReceived);
    connect(m_net, &INetwork::statusMessage,                    this, &Client::statusMessage);

    connect(this, &Client::presetServer,                        m_net, &INetwork::presetServer);
    connect(this, &Client::presetCallsign,                      m_net, &INetwork::presetCallsign);
    connect(this, &Client::presetIcaoCodes,                     m_net, &INetwork::presetIcaoCodes);
    connect(this, &Client::presetLoginMode,                     m_net, &INetwork::presetLoginMode);
    connect(this, &Client::initiateConnection,                  m_net, &INetwork::initiateConnection);
    connect(this, &Client::terminateConnection,                 m_net, &INetwork::terminateConnection);
    connect(this, &Client::sendTextMessages,                    m_net, &INetwork::sendTextMessages);
    connect(this, &Client::sendIpQuery,                         m_net, &INetwork::sendIpQuery);
    connect(this, &Client::sendFreqQuery,                       m_net, &INetwork::sendFrequencyQuery);
    connect(this, &Client::sendServerQuery,                     m_net, &INetwork::sendServerQuery);
    connect(this, &Client::sendAtcQuery,                        m_net, &INetwork::sendAtcQuery);
    connect(this, &Client::sendAtisQuery,                       m_net, &INetwork::sendAtisQuery);
    connect(this, &Client::sendFlightPlan,                      m_net, &INetwork::sendFlightPlan);
    connect(this, &Client::sendFlightPlanQuery,                 m_net, &INetwork::sendFlightPlanQuery);
    connect(this, &Client::sendRealNameQuery,                   m_net, &INetwork::sendRealNameQuery);
    connect(this, &Client::sendCapabilitiesQuery,               m_net, &INetwork::sendCapabilitiesQuery);
    connect(this, &Client::sendIcaoCodesQuery,                  m_net, &INetwork::sendIcaoCodesQuery);
    connect(this, &Client::setOwnAircraft,                      m_net, &INetwork::setOwnAircraft);
    connect(this, &Client::setOwnAircraftPosition,              m_net, &INetwork::setOwnAircraftPosition);
    connect(this, &Client::setOwnAircraftSituation,             m_net, &INetwork::setOwnAircraftSituation);
    connect(this, &Client::setOwnAircraftAvionics,              m_net, &INetwork::setOwnAircraftAvionics);
    connect(this, &Client::sendPing,                            m_net, &INetwork::sendPing);
    connect(this, &Client::sendMetarQuery,                      m_net, &INetwork::sendMetarQuery);
    connect(this, &Client::sendWeatherDataQuery,                m_net, &INetwork::sendWeatherDataQuery);
    connect(this, &Client::sendCustomPacket,                    m_net, &INetwork::sendCustomPacket);

    using namespace std::placeholders;
    m_commands["help"]              = std::bind(&Client::help, this, _1);
    m_commands["echo"]              = std::bind(&Client::echo, this, _1);
    m_commands["exit"]              = std::bind(&Client::exit, this, _1);
    m_commands["getstatusurls"]     = std::bind(&Client::getStatusUrlsCmd, this, _1);
    m_commands["getservers"]        = std::bind(&Client::getKnownServersCmd, this, _1);
    m_commands["setserver"]         = std::bind(&Client::presetServerCmd, this, _1);
    m_commands["setcallsign"]       = std::bind(&Client::presetCallsignCmd, this, _1);
    m_commands["icaocodes"]         = std::bind(&Client::presetIcaoCodesCmd, this, _1);
    m_commands["loginmode"]         = std::bind(&Client::presetLoginModeCmd, this, _1);
    m_commands["initconnect"]       = std::bind(&Client::initiateConnectionCmd, this, _1);
    m_commands["termconnect"]       = std::bind(&Client::terminateConnectionCmd, this, _1);
    m_commands["privmsg"]           = std::bind(&Client::sendPrivateTextMessageCmd, this, _1);
    m_commands["textmsg"]           = std::bind(&Client::sendRadioTextMessageCmd, this, _1);
    m_commands["ip"]                = std::bind(&Client::sendIpQueryCmd, this, _1);
    m_commands["freq"]              = std::bind(&Client::sendFreqQueryCmd, this, _1);
    m_commands["server"]            = std::bind(&Client::sendServerQueryCmd, this, _1);
    m_commands["atc"]               = std::bind(&Client::sendAtcQueryCmd, this, _1);
    m_commands["atis"]              = std::bind(&Client::sendAtisQueryCmd, this, _1);
    m_commands["flightplan"]        = std::bind(&Client::sendFlightPlanCmd, this, _1);
    m_commands["getflightplan"]     = std::bind(&Client::sendFlightPlanQueryCmd, this, _1);
    m_commands["name"]              = std::bind(&Client::sendRealNameQueryCmd, this, _1);
    m_commands["caps"]              = std::bind(&Client::sendCapabilitiesQueryCmd, this, _1);
    m_commands["icao"]              = std::bind(&Client::sendIcaoCodesQueryCmd, this, _1);
    m_commands["setaircraft"]       = std::bind(&Client::setOwnAircraftCmd, this, _1);
    m_commands["setposition"]       = std::bind(&Client::setOwnAircraftPositionCmd, this, _1);
    m_commands["setsituation"]      = std::bind(&Client::setOwnAircraftSituationCmd, this, _1);
    m_commands["setavionics"]       = std::bind(&Client::setOwnAircraftAvionicsCmd, this, _1);
    m_commands["ping"]              = std::bind(&Client::sendPingCmd, this, _1);
    m_commands["metar"]             = std::bind(&Client::sendMetarQueryCmd, this, _1);
    m_commands["weather"]           = std::bind(&Client::sendWeatherDataQueryCmd, this, _1);
    m_commands["custom"]            = std::bind(&Client::sendCustomPacketCmd, this, _1);
}

void Client::command(QString line)
{
    QTextStream stream(&line, QIODevice::ReadOnly);
    QString cmd;
    stream >> cmd;
    stream.skipWhiteSpace();

    auto found = m_commands.find(cmd);
    if (found == m_commands.end())
    {
        std::cout << "No such command" << std::endl;
    }
    else
    {
        (*found)(stream);
    }
}

/****************************************************************************/
/************                      Commands                     *************/
/****************************************************************************/

void Client::help(QTextStream &)
{
    std::cout << "Commands:" << std::endl;
    auto keys = m_commands.keys();
    for (auto i = keys.begin(); i != keys.end(); ++i)
    {
        std::cout << " " << i->toStdString() << std::endl;
    }
}

void Client::echo(QTextStream &line)
{
    std::cout << "echo: " << line.readAll().toStdString() << std::endl;
}

void Client::exit(QTextStream &)
{
    emit quit();
}

void Client::getStatusUrlsCmd(QTextStream &)
{
    auto urls = m_net->getStatusUrls();
    for (auto i = urls.begin(); i != urls.end(); ++i)
    {
        std::cout << i->toString().toStdString() << std::endl;
    }
}

void Client::getKnownServersCmd(QTextStream &)
{
    auto servers = m_net->getKnownServers();
    for (auto i = servers.begin(); i != servers.end(); ++i)
    {
        std::cout << i->toFormattedQString().toStdString() << std::endl;
    }
}

void Client::presetServerCmd(QTextStream &args)
{
    QString hostname;
    quint16 port;
    QString username;
    QString password;
    args >> hostname >> port >> username >> password;
    args.skipWhiteSpace();
    emit presetServer(BlackMisc::Network::CServer("", "", hostname, port, BlackMisc::Network::CUser(username, args.readAll(), "", password)));
}

void Client::presetCallsignCmd(QTextStream &args)
{
    QString callsign;
    args >> callsign;
    emit presetCallsign(callsign);
}

void Client::presetIcaoCodesCmd(QTextStream &args)
{
    QString acTypeICAO;
    QString descriptionICAO;
    QString airlineICAO;
    QString livery;
    QString color;
    args >> acTypeICAO >> descriptionICAO >> airlineICAO >> livery >> color;
    emit presetIcaoCodes(BlackMisc::Aviation::CAircraftIcao(acTypeICAO, descriptionICAO, airlineICAO, livery, color));
}

void Client::presetLoginModeCmd(QTextStream &args)
{
    QString modeString;
    args >> modeString;
    BlackCore::INetwork::LoginMode mode;
    if (modeString == "normal") { mode = BlackCore::INetwork::LoginNormal; }
    if (modeString == "observer") { mode = BlackCore::INetwork::LoginAsObserver; }
    if (modeString == "stealth") { mode = BlackCore::INetwork::LoginStealth; }
    emit presetLoginMode(mode);
}

void Client::initiateConnectionCmd(QTextStream &)
{
    emit initiateConnection();
}

void Client::terminateConnectionCmd(QTextStream &)
{
    emit terminateConnection();
}

void Client::sendPrivateTextMessageCmd(QTextStream &args)
{
    QString callsign;
    args >> callsign;
    args.skipWhiteSpace();
    BlackMisc::Network::CTextMessageList msgs(args.readAll(), BlackMisc::Aviation::CCallsign(callsign));
    emit sendTextMessages(msgs);
}

void Client::sendRadioTextMessageCmd(QTextStream &args)
{
    QString freqsBlob;
    args >> freqsBlob;
    QStringList freqStrings = freqsBlob.split("|");
    QList<BlackMisc::PhysicalQuantities::CFrequency> frequencies;
    for (auto i = freqStrings.begin(); i != freqStrings.end(); ++i)
    {
        frequencies.push_back(BlackMisc::PhysicalQuantities::CFrequency(i->toDouble(), BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz()));
    }
    BlackMisc::Network::CTextMessageList msgs(args.readAll(), frequencies);
    emit sendTextMessages(msgs);
}

void Client::sendIpQueryCmd(QTextStream &)
{
    emit sendIpQuery();
}

void Client::sendFreqQueryCmd(QTextStream &args)
{
    QString callsign;
    args >> callsign;
    emit sendFreqQuery(callsign);
}

void Client::sendFlightPlanCmd(QTextStream &args)
{
    QString equipmentIcao;
    QString originAirportIcao;
    QString destinationAirportIcao;
    QString alternateAirportIcao;
    QString takeoffTimePlanned;
    QString takeoffTimeActual;
    int enrouteTime;
    int fuelTime;
    int cruiseAltitude;
    int cruiseTrueAirspeed;
    QString flightRulesString;
    QString route;
    args >> equipmentIcao >> originAirportIcao >> destinationAirportIcao >> alternateAirportIcao >> takeoffTimePlanned >> takeoffTimeActual
         >> enrouteTime >> fuelTime >> cruiseAltitude >> cruiseTrueAirspeed >> flightRulesString >> route;

    BlackMisc::Aviation::CFlightPlan::FlightRules flightRules;
    if (flightRulesString == "IFR") { flightRules = BlackMisc::Aviation::CFlightPlan::IFR; }
    else if (flightRulesString == "SVFR") { flightRules = BlackMisc::Aviation::CFlightPlan::SVFR; }
    else { flightRules = BlackMisc::Aviation::CFlightPlan::VFR; }

    BlackMisc::Aviation::CFlightPlan
    fp(equipmentIcao, originAirportIcao, destinationAirportIcao, alternateAirportIcao,
       QDateTime::fromString(takeoffTimePlanned, "hhmm"), QDateTime::fromString(takeoffTimeActual, "hhmm"),
       BlackMisc::PhysicalQuantities::CTime(enrouteTime, BlackMisc::PhysicalQuantities::CTimeUnit::hrmin()),
       BlackMisc::PhysicalQuantities::CTime(fuelTime, BlackMisc::PhysicalQuantities::CTimeUnit::hrmin()),
       BlackMisc::Aviation::CAltitude(cruiseAltitude, BlackMisc::Aviation::CAltitude::MeanSeaLevel, BlackMisc::PhysicalQuantities::CLengthUnit::ft()),
       BlackMisc::PhysicalQuantities::CSpeed(cruiseTrueAirspeed, BlackMisc::PhysicalQuantities::CSpeedUnit::kts()),
       flightRules, route, args.readAll());
    emit sendFlightPlan(fp);
}

void Client::sendFlightPlanQueryCmd(QTextStream &args)
{
    QString callsign;
    args >> callsign;
    emit sendFlightPlanQuery(callsign);
}

void Client::sendServerQueryCmd(QTextStream &args)
{
    QString callsign;
    args >> callsign;
    emit sendServerQuery(callsign);
}

void Client::sendAtcQueryCmd(QTextStream &args)
{
    QString callsign;
    args >> callsign;
    emit sendAtcQuery(callsign);
}

void Client::sendAtisQueryCmd(QTextStream &args)
{
    QString callsign;
    args >> callsign;
    emit sendAtisQuery(callsign);
}

void Client::sendRealNameQueryCmd(QTextStream &args)
{
    QString callsign;
    args >> callsign;
    emit sendRealNameQuery(callsign);
}

void Client::sendCapabilitiesQueryCmd(QTextStream &args)
{
    QString callsign;
    args >> callsign;
    emit sendCapabilitiesQuery(callsign);
}

void Client::sendIcaoCodesQueryCmd(QTextStream &args)
{
    QString callsign;
    args >> callsign;
    emit sendIcaoCodesQuery(callsign);
}

void Client::setOwnAircraftCmd(QTextStream &args)
{
    double lat;
    double lon;
    double alt;
    double hdg;
    double pitch;
    double bank;
    double gs;
    double com1;
    double com2;
    int xpdrCode;
    QString xpdrMode;
    args >> lat >> lon >> alt >> hdg >> pitch >> bank >> gs >> com1 >> com2 >> xpdrCode >> xpdrMode;
    BlackMisc::Aviation::CAircraft aircraft("", BlackMisc::Network::CUser(), BlackMisc::Aviation::CAircraftSituation(
            BlackMisc::Geo::CCoordinateGeodetic(lat, lon, 0),
            BlackMisc::Aviation::CAltitude(alt, BlackMisc::Aviation::CAltitude::MeanSeaLevel, BlackMisc::PhysicalQuantities::CLengthUnit::ft()),
            BlackMisc::Aviation::CHeading(hdg, BlackMisc::Aviation::CHeading::True, BlackMisc::PhysicalQuantities::CAngleUnit::deg()),
            BlackMisc::PhysicalQuantities::CAngle(pitch, BlackMisc::PhysicalQuantities::CAngleUnit::deg()),
            BlackMisc::PhysicalQuantities::CAngle(bank, BlackMisc::PhysicalQuantities::CAngleUnit::deg()),
            BlackMisc::PhysicalQuantities::CSpeed(gs, BlackMisc::PhysicalQuantities::CSpeedUnit::kts())
                                            ));
    aircraft.setCom1System(BlackMisc::Aviation::CComSystem("COM1", BlackMisc::PhysicalQuantities::CFrequency(com1, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz())));
    aircraft.setCom2System(BlackMisc::Aviation::CComSystem("COM2", BlackMisc::PhysicalQuantities::CFrequency(com2, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz())));
    aircraft.setTransponder(BlackMisc::Aviation::CTransponder("Transponder", xpdrCode, xpdrMode));
    emit setOwnAircraft(aircraft);
}

void Client::setOwnAircraftPositionCmd(QTextStream &args)
{
    double lat;
    double lon;
    double alt;
    args >> lat >> lon >> alt;
    emit setOwnAircraftPosition(BlackMisc::Geo::CCoordinateGeodetic(lat, lon, 0),
                                BlackMisc::Aviation::CAltitude(alt, BlackMisc::Aviation::CAltitude::MeanSeaLevel, BlackMisc::PhysicalQuantities::CLengthUnit::ft()));
}

void Client::setOwnAircraftSituationCmd(QTextStream &args)
{
    double lat;
    double lon;
    double alt;
    double hdg;
    double pitch;
    double bank;
    double gs;
    args >> lat >> lon >> alt >> hdg >> pitch >> bank >> gs;
    emit setOwnAircraftSituation(BlackMisc::Aviation::CAircraftSituation(
                                     BlackMisc::Geo::CCoordinateGeodetic(lat, lon, 0),
                                     BlackMisc::Aviation::CAltitude(alt, BlackMisc::Aviation::CAltitude::MeanSeaLevel, BlackMisc::PhysicalQuantities::CLengthUnit::ft()),
                                     BlackMisc::Aviation::CHeading(hdg, BlackMisc::Aviation::CHeading::True, BlackMisc::PhysicalQuantities::CAngleUnit::deg()),
                                     BlackMisc::PhysicalQuantities::CAngle(pitch, BlackMisc::PhysicalQuantities::CAngleUnit::deg()),
                                     BlackMisc::PhysicalQuantities::CAngle(bank, BlackMisc::PhysicalQuantities::CAngleUnit::deg()),
                                     BlackMisc::PhysicalQuantities::CSpeed(gs, BlackMisc::PhysicalQuantities::CSpeedUnit::kts())
                                 ));
}

void Client::setOwnAircraftAvionicsCmd(QTextStream &args)
{
    double com1;
    double com2;
    int xpdrCode;
    QString xpdrMode;
    args >> com1 >> com2 >> xpdrCode >> xpdrMode;
    emit setOwnAircraftAvionics(
        BlackMisc::Aviation::CComSystem("COM1", BlackMisc::PhysicalQuantities::CFrequency(com1, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz())),
        BlackMisc::Aviation::CComSystem("COM2", BlackMisc::PhysicalQuantities::CFrequency(com2, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz())),
        BlackMisc::Aviation::CTransponder("Transponder", xpdrCode, xpdrMode)
    );
}

void Client::sendPingCmd(QTextStream &args)
{
    QString callsign;
    args >> callsign;
    emit sendPing(callsign);
}

void Client::sendMetarQueryCmd(QTextStream &args)
{
    QString airportICAO;
    args >> airportICAO;
    emit sendMetarQuery(airportICAO);
}

void Client::sendWeatherDataQueryCmd(QTextStream &args)
{
    QString airportICAO;
    args >> airportICAO;
    emit sendWeatherDataQuery(airportICAO);
}

void Client::sendCustomPacketCmd(QTextStream &args)
{
    QString callsign;
    QString packetId;
    args >> callsign >> packetId;
    QStringList data;
    while (!args.atEnd())
    {
        QString field;
        args >> field;
        data.push_back(field);
    }
    emit sendCustomPacket(callsign, packetId, data);
}

/****************************************************************************/
/************      Slots to receive signals from INetwork       *************/
/****************************************************************************/

void Client::atcPositionUpdate(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CFrequency &freq,
                               const BlackMisc::Geo::CCoordinateGeodetic &pos, const BlackMisc::PhysicalQuantities::CLength &range)
{
    std::cout << "POSITION " << callsign << " " << freq << " " << pos << " " << range << std::endl;
}

void Client::atcDisconnected(const BlackMisc::Aviation::CCallsign &callsign)
{
    std::cout << "ATC_DISCONNECTED " << callsign << std::endl;
}

void Client::connectionStatusChanged(BlackCore::INetwork::ConnectionStatus oldStatus, BlackCore::INetwork::ConnectionStatus newStatus,
                                     const QString &errorMessage)
{
    switch (newStatus)
    {
    case BlackCore::INetwork::Disconnected:         std::cout << "CONN_STATUS_DISCONNECTED"; break;
    case BlackCore::INetwork::Disconnecting:        std::cout << "CONN_STATUS_DISCONNECTING"; break;
    case BlackCore::INetwork::DisconnectedError:    std::cout << "CONN_STATUS_DISCONNECTED_ERROR"; break;
    case BlackCore::INetwork::DisconnectedFailed:   std::cout << "CONN_STATUS_DISCONNECTED_FAILED"; break;
    case BlackCore::INetwork::DisconnectedLost:     std::cout << "CONN_STATUS_DISCONNECTED_LOST"; break;
    case BlackCore::INetwork::Connecting:           std::cout << "CONN_STATUS_CONNECTING"; break;
    case BlackCore::INetwork::Connected:            std::cout << "CONN_STATUS_CONNECTED"; break;
    }
    switch (oldStatus)
    {
    case BlackCore::INetwork::Disconnected:         std::cout << " (was CONN_STATUS_DISCONNECTED)\n"; break;
    case BlackCore::INetwork::Disconnecting:        std::cout << " (was CONN_STATUS_DISCONNECTING)\n"; break;
    case BlackCore::INetwork::DisconnectedError:    std::cout << " (was CONN_STATUS_DISCONNECTED_ERROR)\n"; break;
    case BlackCore::INetwork::DisconnectedFailed:   std::cout << " (was CONN_STATUS_DISCONNECTED_FAILED)\n"; break;
    case BlackCore::INetwork::DisconnectedLost:     std::cout << " (was CONN_STATUS_DISCONNECTED_LOST)\n"; break;
    case BlackCore::INetwork::Connecting:           std::cout << " (was CONN_STATUS_CONNECTING)\n"; break;
    case BlackCore::INetwork::Connected:            std::cout << " (was CONN_STATUS_CONNECTED)\n"; break;
    }
    if (!errorMessage.isEmpty())
    {
        std::cout << "REASON " << errorMessage.toStdString() << std::endl;
    }
}

void Client::ipReplyReceived(const QString &ip)
{
    std::cout << "IP_REPLY " << ip.toStdString() << std::endl;
}

void Client::freqReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CFrequency &freq)
{
    std::cout << "FREQ_REPLY " << callsign << " " << freq << std::endl;
}

void Client::serverReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &hostname)
{
    std::cout << "SERVER_REPLY " << callsign << " " << hostname.toStdString() << std::endl;
}

void Client::atcReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, bool isATC)
{
    std::cout << "ATC_REPLY " << callsign << (isATC ? " yes" : " no") << std::endl;
}

void Client::atisReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CInformationMessage &atis)
{
    std::cout << "ATIS_REPLY " << callsign << " " << atis << std::endl;
}

void Client::realNameReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &realname)
{
    std::cout << "NAME_REPLY " << callsign << " " << realname.toStdString() << std::endl;
}

void Client::capabilitiesReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, quint32 flags)
{
    std::cout << "CAPS_REPLY " << callsign << " " << flags << std::endl;
}

void Client::kicked(const QString &msg)
{
    std::cout << "KICKED " << msg.toStdString() << std::endl;
}

void Client::metarReplyReceived(const QString &data)
{
    std::cout << "METAR " << data.toStdString() << std::endl;
}

void Client::flightPlanReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CFlightPlan &flightPlan)
{
    std::string rules;
    switch (flightPlan.getFlightRules())
    {
    default:
    case BlackMisc::Aviation::CFlightPlan::IFR:  rules = "IFR"; break;
    case BlackMisc::Aviation::CFlightPlan::VFR:  rules = "VFR"; break;
    case BlackMisc::Aviation::CFlightPlan::SVFR: rules = "SVFR"; break;
    }

    std::cout << "FLIGHTPLAN "  << callsign
              << flightPlan.getEquipmentIcao().toStdString() << " " << flightPlan.getOriginAirportIcao() << " "
              << flightPlan.getDestinationAirportIcao() << " " << flightPlan.getAlternateAirportIcao() << " "
              << flightPlan.getTakeoffTimePlannedHourMin().toStdString() << " " << flightPlan.getTakeoffTimeActualHourMin().toStdString() << " "
              << flightPlan.getEnrouteTime() << " " << flightPlan.getFuelTime() << " "
              << flightPlan.getCruiseAltitude() << " " << flightPlan.getCruiseTrueAirspeed() << " " << rules << " "
              << flightPlan.getRoute().toStdString() << " " << flightPlan.getRemarks().toStdString() << "\n";
}

void Client::pilotDisconnected(const BlackMisc::Aviation::CCallsign &callsign)
{
    std::cout << "PILOT_DISCONNECTED " << callsign << std::endl;
}

void Client::icaoCodesReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftIcao &icaoData)
{
    std::cout << "PLANE_INFO_REPLY " << callsign << " " << icaoData.toStdString();
}

void Client::pongReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CTime &elapsedTime)
{
    std::cout << "PONG " << callsign << " " << elapsedTime << std::endl;
}

void Client::textMessagesReceived(const BlackMisc::Network::CTextMessageList &list)
{
    std::cout << "TEXT MESSAGE" << list.toStdString();
}

void Client::customPacketReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &packetId, const QStringList &data)
{
    std::cout << "CUSTOM " << callsign << " " << packetId.toStdString() << " ";
    for (auto i = data.begin(); i != data.end(); ++i)
    {
        std::cout << i->toStdString() << std::endl;
    }
    std::cout << std::endl;
}

void Client::statusMessage(const BlackMisc::CStatusMessage &message)
{
    std::cout << "STATUS " << message.getSeverityAsString().toStdString() << " " << message.getTypeAsString().toStdString() << " "
              << message.getMessage().toStdString() << "\n";
}
