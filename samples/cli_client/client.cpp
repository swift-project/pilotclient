/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "client.h"
#include "blackmisc/context.h"
#include <iostream>
#include <QStringList>

Client::Client(BlackMisc::IContext &ctx)
    : m_net(&ctx.getObject<BlackCore::INetwork>())
{
    using namespace BlackCore;
    connect(m_net, &INetwork::atcPositionUpdate,                this, &Client::atcPositionUpdate);
    connect(m_net, &INetwork::atcDisconnected,                  this, &Client::atcDisconnected);
    connect(m_net, &INetwork::connectionStatusIdle,             this, &Client::connectionStatusIdle);
    connect(m_net, &INetwork::connectionStatusConnecting,       this, &Client::connectionStatusConnecting);
    connect(m_net, &INetwork::connectionStatusConnected,        this, &Client::connectionStatusConnected);
    connect(m_net, &INetwork::connectionStatusDisconnected,     this, &Client::connectionStatusDisconnected);
    connect(m_net, &INetwork::connectionStatusError,            this, &Client::connectionStatusError);
    connect(m_net, &INetwork::ipQueryReplyReceived,             this, &Client::ipQueryReplyReceived);
    connect(m_net, &INetwork::frequencyQueryReplyReceived,      this, &Client::freqQueryReplyReceived);
    connect(m_net, &INetwork::serverQueryReplyReceived,         this, &Client::serverQueryReplyReceived);
    connect(m_net, &INetwork::atcQueryReplyReceived,            this, &Client::atcQueryReplyReceived);
    connect(m_net, &INetwork::atisQueryReplyReceived,           this, &Client::atisQueryReplyReceived);
    connect(m_net, &INetwork::nameQueryReplyReceived,           this, &Client::nameQueryReplyReceived);
    connect(m_net, &INetwork::capabilitiesQueryReplyReceived,   this, &Client::capabilitiesQueryReplyReceived);
    connect(m_net, &INetwork::frequencyQueryRequestReceived,    this, &Client::freqQueryRequestReceived);
    connect(m_net, &INetwork::nameQueryRequestReceived,         this, &Client::nameQueryRequestReceived);
    connect(m_net, &INetwork::kicked,                           this, &Client::kicked);
    connect(m_net, &INetwork::metarReceived,                    this, &Client::metarReceived);
    connect(m_net, &INetwork::pilotDisconnected,                this, &Client::pilotDisconnected);
    connect(m_net, &INetwork::aircraftInfoReceived,             this, &Client::aircraftInfoReceived);
    connect(m_net, &INetwork::aircraftInfoRequestReceived,      this, &Client::aircraftInfoRequestReceived);
    connect(m_net, &INetwork::pong,                             this, &Client::pong);
    connect(m_net, &INetwork::textMessagesReceived,             this, &Client::textMessagesReceived);

    connect(this, &Client::setServer,                           m_net, &INetwork::setServer);
    connect(this, &Client::setCallsign,                         m_net, &INetwork::setCallsign);
    connect(this, &Client::setRealName,                         m_net, &INetwork::setRealName);
    connect(this, &Client::initiateConnection,                  m_net, &INetwork::initiateConnection);
    connect(this, &Client::terminateConnection,                 m_net, &INetwork::terminateConnection);
    connect(this, &Client::sendTextMessages,                    m_net, &INetwork::sendTextMessages);
    connect(this, &Client::sendIpQuery,                         m_net, &INetwork::sendIpQuery);
    connect(this, &Client::sendFreqQuery,                       m_net, &INetwork::sendFrequencyQuery);
    connect(this, &Client::sendServerQuery,                     m_net, &INetwork::sendServerQuery);
    connect(this, &Client::sendAtcQuery,                        m_net, &INetwork::sendAtcQuery);
    connect(this, &Client::sendAtisQuery,                       m_net, &INetwork::sendAtisQuery);
    connect(this, &Client::sendNameQuery,                       m_net, &INetwork::sendNameQuery);
    connect(this, &Client::sendCapabilitiesQuery,               m_net, &INetwork::sendCapabilitiesQuery);
    connect(this, &Client::replyToFreqQuery,                    m_net, &INetwork::replyToFrequencyQuery);
    connect(this, &Client::replyToNameQuery,                    m_net, &INetwork::replyToNameQuery);
    connect(this, &Client::requestPlaneInfo,                    m_net, &INetwork::requestAircraftInfo);
    connect(this, &Client::sendAircraftInfo,                    m_net, &INetwork::sendAircraftInfo);
    connect(this, &Client::ping,                                m_net, &INetwork::ping);
    connect(this, &Client::requestMetar,                        m_net, &INetwork::requestMetar);
    connect(this, &Client::requestWeatherData,                  m_net, &INetwork::requestWeatherData);

    using namespace std::placeholders;
    m_commands["help"]              = std::bind(&Client::help, this, _1);
    m_commands["echo"]              = std::bind(&Client::echo, this, _1);
    m_commands["exit"]              = std::bind(&Client::exit, this, _1);
    m_commands["setserver"]         = std::bind(&Client::setServerCmd, this, _1);
    m_commands["setcallsign"]       = std::bind(&Client::setCallsignCmd, this, _1);
    m_commands["setrealname"]       = std::bind(&Client::setRealNameCmd, this, _1);
    m_commands["initconnect"]       = std::bind(&Client::initiateConnectionCmd, this, _1);
    m_commands["termconnect"]       = std::bind(&Client::terminateConnectionCmd, this, _1);
    m_commands["privmsg"]           = std::bind(&Client::sendPrivateTextMessageCmd, this, _1);
    m_commands["textmsg"]           = std::bind(&Client::sendRadioTextMessageCmd, this, _1);
    m_commands["ip"]                = std::bind(&Client::sendIpQueryCmd, this, _1);
    m_commands["freq"]              = std::bind(&Client::sendFreqQueryCmd, this, _1);
    m_commands["server"]            = std::bind(&Client::sendServerQueryCmd, this, _1);
    m_commands["atc"]               = std::bind(&Client::sendAtcQueryCmd, this, _1);
    m_commands["atis"]              = std::bind(&Client::sendAtisQueryCmd, this, _1);
    m_commands["name"]              = std::bind(&Client::sendNameQueryCmd, this, _1);
    m_commands["caps"]              = std::bind(&Client::sendCapabilitiesQueryCmd, this, _1);
    m_commands["freqreply"]         = std::bind(&Client::replyToFreqQueryCmd, this, _1);
    m_commands["namereply"]         = std::bind(&Client::replyToNameQueryCmd, this, _1);
    m_commands["aircraftinfo"]      = std::bind(&Client::requestAircraftInfoCmd, this, _1);
    m_commands["aircraftinforeply"] = std::bind(&Client::sendAircraftInfoCmd, this, _1);
    m_commands["ping"]              = std::bind(&Client::pingCmd, this, _1);
    m_commands["metar"]             = std::bind(&Client::requestMetarCmd, this, _1);
    m_commands["weather"]           = std::bind(&Client::requestWeatherDataCmd, this, _1);
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

void Client::setServerCmd(QTextStream &args)
{
    QString hostname;
    quint16 port;
    QString username;
    QString password;
    args >> hostname >> port >> username >> password;
    emit setServer(BlackMisc::Network::CServer("", "", hostname, port, BlackMisc::Network::CUser(username, "", "", password)));
}

void Client::setCallsignCmd(QTextStream &args)
{
    QString callsign;
    args >> callsign;
    emit setCallsign(callsign);
}

void Client::setRealNameCmd(QTextStream &args)
{
    emit setRealName(args.readAll());
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

void Client::sendNameQueryCmd(QTextStream &args)
{
    QString callsign;
    args >> callsign;
    emit sendNameQuery(callsign);
}

void Client::sendCapabilitiesQueryCmd(QTextStream &args)
{
    QString callsign;
    args >> callsign;
    emit sendCapabilitiesQuery(callsign);
}

void Client::replyToFreqQueryCmd(QTextStream &args)
{
    QString callsign;
    double num;
    args >> callsign >> num;
    BlackMisc::PhysicalQuantities::CFrequency freq(num, BlackMisc::PhysicalQuantities::CFrequencyUnit::kHz());
    emit replyToFreqQuery(callsign, freq);
}

void Client::replyToNameQueryCmd(QTextStream &args)
{
    QString callsign;
    QString realname;
    args >> callsign >> realname;
    emit replyToNameQuery(callsign, realname);
}

void Client::requestAircraftInfoCmd(QTextStream &args)
{
    QString callsign;
    args >> callsign;
    emit requestPlaneInfo(callsign);
}

void Client::sendAircraftInfoCmd(QTextStream &args)
{
    QString callsign;
    QString acTypeICAO;
    QString airlineICAO;
    QString livery;
    args >> callsign >> acTypeICAO >> airlineICAO >> livery;
    BlackMisc::Aviation::CAircraftIcao icao(acTypeICAO, "L2J", airlineICAO, livery, "");
    emit sendAircraftInfo(callsign, icao);
}

void Client::pingCmd(QTextStream &args)
{
    QString callsign;
    args >> callsign;
    emit ping(callsign);
}

void Client::requestMetarCmd(QTextStream &args)
{
    QString airportICAO;
    args >> airportICAO;
    emit requestMetar(airportICAO);
}

void Client::requestWeatherDataCmd(QTextStream &args)
{
    QString airportICAO;
    args >> airportICAO;
    emit requestWeatherData(airportICAO);
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

void Client::connectionStatusIdle()
{
    std::cout << "CONN_STATUS_IDLE" << std::endl;
}

void Client::connectionStatusConnecting()
{
    std::cout << "CONN_STATUS_CONNECTING" << std::endl;
}

void Client::connectionStatusConnected()
{
    std::cout << "CONN_STATUS_CONNECTED" << std::endl;
}

void Client::connectionStatusDisconnected()
{
    std::cout << "CONN_STATUS_DISCONNECTED" << std::endl;
}

void Client::connectionStatusError()
{
    std::cout << "CONN_STATUS_ERROR" << std::endl;
}

void Client::ipQueryReplyReceived(const QString &ip)
{
    std::cout << "IP_REPLY " << ip.toStdString() << std::endl;
}

void Client::freqQueryReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CFrequency &freq)
{
    std::cout << "FREQ_REPLY " << callsign << " " << freq << std::endl;
}

void Client::serverQueryReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &hostname)
{
    std::cout << "SERVER_REPLY " << callsign << " " << hostname.toStdString() << std::endl;
}

void Client::atcQueryReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, bool isATC)
{
    std::cout << "ATC_REPLY " << callsign << (isATC ? " yes" : " no") << std::endl;
}

void Client::atisQueryReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &data)
{
    std::cout << "ATIS_REPLY " << callsign << " " << data.toStdString() << std::endl;
}

void Client::nameQueryReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &realname)
{
    std::cout << "NAME_REPLY " << callsign << " " << realname.toStdString() << std::endl;
}

void Client::capabilitiesQueryReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, quint32 flags)
{
    std::cout << "CAPS_REPLY " << callsign << " " << flags << std::endl;
}

void Client::freqQueryRequestReceived(const BlackMisc::Aviation::CCallsign &callsign)
{
    std::cout << "FREQ_QUERY " << callsign << std::endl;
}

void Client::nameQueryRequestReceived(const BlackMisc::Aviation::CCallsign &callsign)
{
    std::cout << "NAME_QUERY " << callsign << std::endl;
}

void Client::kicked(const QString &msg)
{
    std::cout << "KICKED " << msg.toStdString() << std::endl;
}

void Client::metarReceived(const QString &data)
{
    std::cout << "METAR " << data.toStdString() << std::endl;
}

void Client::pilotDisconnected(const BlackMisc::Aviation::CCallsign &callsign)
{
    std::cout << "PILOT_DISCONNECTED " << callsign << std::endl;
}

void Client::aircraftInfoReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftIcao &icaoData)
{
    std::cout << "PLANE_INFO_REPLY " << callsign << " " << icaoData.toStdString();
}

void Client::aircraftInfoRequestReceived(const BlackMisc::Aviation::CCallsign &callsign)
{
    std::cout << "PLANE_INFO_QUERY " << callsign << std::endl;
}

void Client::pong(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CTime &elapsedTime)
{
    std::cout << "PONG " << callsign << " " << elapsedTime << std::endl;
}

void Client::textMessagesReceived(const BlackMisc::Network::CTextMessageList &list)
{
    std::cout << "TEXT MESSAGE" << list.toStdString();
}
