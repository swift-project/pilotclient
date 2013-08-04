/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "client.h"
#include "blackmisc/context.h"
#include <iostream>
#include <QStringList>

Client::Client()
    : m_net(BlackMisc::IContext::getInstance().singleton<BlackCore::INetwork>())
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
    connect(m_net, &INetwork::freqQueryReplyReceived,           this, &Client::freqQueryReplyReceived);
    connect(m_net, &INetwork::serverQueryReplyReceived,         this, &Client::serverQueryReplyReceived);
    connect(m_net, &INetwork::atcQueryReplyReceived,            this, &Client::atcQueryReplyReceived);
    connect(m_net, &INetwork::atisQueryReplyReceived,           this, &Client::atisQueryReplyReceived);
    connect(m_net, &INetwork::nameQueryReplyReceived,           this, &Client::nameQueryReplyReceived);
    connect(m_net, &INetwork::capabilitiesQueryReplyReceived,   this, &Client::capabilitiesQueryReplyReceived);
    connect(m_net, &INetwork::freqQueryRequestReceived,         this, &Client::freqQueryRequestReceived);
    connect(m_net, &INetwork::nameQueryRequestReceived,         this, &Client::nameQueryRequestReceived);
    connect(m_net, &INetwork::kicked,                           this, &Client::kicked);
    connect(m_net, &INetwork::metarReceived,                    this, &Client::metarReceived);
    connect(m_net, &INetwork::pilotDisconnected,                this, &Client::pilotDisconnected);
    connect(m_net, &INetwork::planeInfoReceived,                this, &Client::planeInfoReceived);
    connect(m_net, &INetwork::planeInfoRequestReceived,         this, &Client::planeInfoRequestReceived);
    connect(m_net, &INetwork::pong,                             this, &Client::pong);
    connect(m_net, &INetwork::radioTextMessageReceived,         this, &Client::radioTextMessageReceived);
    connect(m_net, &INetwork::privateTextMessageReceived,       this, &Client::privateTextMessageReceived);

    connect(this, &Client::setServerDetails,                    m_net, &INetwork::setServerDetails);
    connect(this, &Client::setUserCredentials,                  m_net, &INetwork::setUserCredentials);
    connect(this, &Client::setCallsign,                         m_net, &INetwork::setCallsign);
    connect(this, &Client::setRealName,                         m_net, &INetwork::setRealName);
    connect(this, &Client::initiateConnection,                  m_net, &INetwork::initiateConnection);
    connect(this, &Client::terminateConnection,                 m_net, &INetwork::terminateConnection);
    connect(this, &Client::sendPrivateTextMessage,              m_net, &INetwork::sendPrivateTextMessage);
    connect(this, &Client::sendRadioTextMessage,                m_net, &INetwork::sendRadioTextMessage);
    connect(this, &Client::sendIpQuery,                         m_net, &INetwork::sendIpQuery);
    connect(this, &Client::sendFreqQuery,                       m_net, &INetwork::sendFreqQuery);
    connect(this, &Client::sendServerQuery,                     m_net, &INetwork::sendServerQuery);
    connect(this, &Client::sendAtcQuery,                        m_net, &INetwork::sendAtcQuery);
    connect(this, &Client::sendAtisQuery,                       m_net, &INetwork::sendAtisQuery);
    connect(this, &Client::sendNameQuery,                       m_net, &INetwork::sendNameQuery);
    connect(this, &Client::sendCapabilitiesQuery,               m_net, &INetwork::sendCapabilitiesQuery);
    connect(this, &Client::replyToFreqQuery,                    m_net, &INetwork::replyToFreqQuery);
    connect(this, &Client::replyToNameQuery,                    m_net, &INetwork::replyToNameQuery);
    connect(this, &Client::requestPlaneInfo,                    m_net, &INetwork::requestPlaneInfo);
    connect(this, &Client::sendPlaneInfo,                       m_net, &INetwork::sendPlaneInfo);
    connect(this, &Client::ping,                                m_net, &INetwork::ping);
    connect(this, &Client::requestMetar,                        m_net, &INetwork::requestMetar);
    connect(this, &Client::requestWeatherData,                  m_net, &INetwork::requestWeatherData);

    using namespace std::placeholders;
    m_commands["help"]              = std::bind(&Client::help, this, _1);
    m_commands["echo"]              = std::bind(&Client::echo, this, _1);
    m_commands["exit"]              = std::bind(&Client::exit, this, _1);
    m_commands["setserver"]         = std::bind(&Client::setServerDetailsCmd, this, _1);
    m_commands["setuser"]           = std::bind(&Client::setUserCredentialsCmd, this, _1);
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
    m_commands["planeinfo"]         = std::bind(&Client::requestPlaneInfoCmd, this, _1);
    m_commands["planeinforeply"]    = std::bind(&Client::sendPlaneInfoCmd, this, _1);
    m_commands["ping"]              = std::bind(&Client::pingCmd, this, _1);
    m_commands["metar"]             = std::bind(&Client::requestMetarCmd, this, _1);
    m_commands["weather"]           = std::bind(&Client::requestWeatherDataCmd, this, _1);
}

void Client::command(QString line)
{
    QTextStream stream (&line, QIODevice::ReadOnly);
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

void Client::help(QTextStream&)
{
    std::cout << "Commands:" << std::endl;
    auto keys = m_commands.keys();
    for (auto i = keys.begin(); i != keys.end(); ++i)
    {
        std::cout << " " << i->toStdString() << std::endl;
    }
}

void Client::echo(QTextStream& line)
{
    std::cout << "echo: " << line.readAll().toStdString() << std::endl;
}

void Client::exit(QTextStream&)
{
    emit quit();
}

void Client::setServerDetailsCmd(QTextStream& args)
{
    QString hostname;
    quint16 port;
    args >> hostname >> port;
    emit setServerDetails(hostname, port);
}

void Client::setUserCredentialsCmd(QTextStream& args)
{
    QString username;
    QString password;
    args >> username >> password;
    emit setUserCredentials(username, password);
}

void Client::setCallsignCmd(QTextStream& args)
{
    QString callsign;
    args >> callsign;
    emit setCallsign(callsign);
}

void Client::setRealNameCmd(QTextStream& args)
{
    emit setRealName(args.readAll());
}

void Client::initiateConnectionCmd(QTextStream&)
{
    emit initiateConnection();
}

void Client::terminateConnectionCmd(QTextStream&)
{
    emit terminateConnection();
}

void Client::sendPrivateTextMessageCmd(QTextStream& args)
{
    QString callsign;
    args >> callsign;
    args.skipWhiteSpace();
    emit sendPrivateTextMessage(callsign, args.readAll());
}

void Client::sendRadioTextMessageCmd(QTextStream& args)
{
    QString freqsBlob;
    args >> freqsBlob;
    QStringList freqStrings = freqsBlob.split("|");
    QVector<BlackMisc::PhysicalQuantities::CFrequency> freqs;
    for (auto i = freqStrings.begin(); i != freqStrings.end(); ++i)
    {
        freqs.push_back(BlackMisc::PhysicalQuantities::CFrequency(i->toDouble(), BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz()));
    }
    emit sendRadioTextMessage(freqs, args.readAll());
}

void Client::sendIpQueryCmd(QTextStream&)
{
    emit sendIpQuery();
}

void Client::sendFreqQueryCmd(QTextStream& args)
{
    QString callsign;
    args >> callsign;
    emit sendFreqQuery(callsign);
}

void Client::sendServerQueryCmd(QTextStream& args)
{
    QString callsign;
    args >> callsign;
    emit sendServerQuery(callsign);
}

void Client::sendAtcQueryCmd(QTextStream& args)
{
    QString callsign;
    args >> callsign;
    emit sendAtcQuery(callsign);
}

void Client::sendAtisQueryCmd(QTextStream& args)
{
    QString callsign;
    args >> callsign;
    emit sendAtisQuery(callsign);
}

void Client::sendNameQueryCmd(QTextStream& args)
{
    QString callsign;
    args >> callsign;
    emit sendNameQuery(callsign);
}

void Client::sendCapabilitiesQueryCmd(QTextStream& args)
{
    QString callsign;
    args >> callsign;
    emit sendCapabilitiesQuery(callsign);
}

void Client::replyToFreqQueryCmd(QTextStream& args)
{
    QString callsign;
    double num;
    args >> callsign >> num;
    BlackMisc::PhysicalQuantities::CFrequency freq (num, BlackMisc::PhysicalQuantities::CFrequencyUnit::kHz());
    emit replyToFreqQuery(callsign, freq);
}

void Client::replyToNameQueryCmd(QTextStream& args)
{
    QString callsign;
    QString realname;
    args >> callsign >> realname;
    emit replyToNameQuery(callsign, realname);
}

void Client::requestPlaneInfoCmd(QTextStream& args)
{
    QString callsign;
    args >> callsign;
    emit requestPlaneInfo(callsign);
}

void Client::sendPlaneInfoCmd(QTextStream& args)
{
    QString callsign;
    QString acTypeICAO;
    QString airlineICAO;
    QString livery;
    args >> callsign >> acTypeICAO >> airlineICAO >> livery;
    emit sendPlaneInfo(callsign, acTypeICAO, airlineICAO, livery);
}

void Client::pingCmd(QTextStream& args)
{
    QString callsign;
    args >> callsign;
    emit ping(callsign);
}

void Client::requestMetarCmd(QTextStream& args)
{
    QString airportICAO;
    args >> airportICAO;
    emit requestMetar(airportICAO);
}

void Client::requestWeatherDataCmd(QTextStream& args)
{
    QString airportICAO;
    args >> airportICAO;
    emit requestWeatherData(airportICAO);
}

/****************************************************************************/
/************      Slots to receive signals from INetwork       *************/
/****************************************************************************/

void Client::atcPositionUpdate(const QString& callsign, const BlackMisc::PhysicalQuantities::CFrequency& freq,
    const BlackMisc::Geo::CCoordinateGeodetic& pos, const BlackMisc::PhysicalQuantities::CLength& range)
{
    std::cout << "POSITION " << callsign.toStdString() << " " << freq << " " << pos << " " << range << std::endl;
}

void Client::atcDisconnected(const QString& callsign)
{
    std::cout << "ATC_DISCONNECTED " << callsign.toStdString() << std::endl;
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

void Client::ipQueryReplyReceived(const QString& ip)
{
    std::cout << "IP_REPLY " << ip.toStdString() << std::endl;
}

void Client::freqQueryReplyReceived(const QString& callsign, const BlackMisc::PhysicalQuantities::CFrequency& freq)
{
    std::cout << "FREQ_REPLY " << callsign.toStdString() << " " << freq << std::endl;
}

void Client::serverQueryReplyReceived(const QString& callsign, const QString& hostname)
{
    std::cout << "SERVER_REPLY " << callsign.toStdString() << " " << hostname.toStdString() << std::endl;
}

void Client::atcQueryReplyReceived(const QString& callsign, bool isATC)
{
    std::cout << "ATC_REPLY " << callsign.toStdString() << (isATC ? " yes" : " no") << std::endl;
}

void Client::atisQueryReplyReceived(const QString& callsign, const QString& data)
{
    std::cout << "ATIS_REPLY " << callsign.toStdString() << " " << data.toStdString() << std::endl;
}

void Client::nameQueryReplyReceived(const QString& callsign, const QString& realname)
{
    std::cout << "NAME_REPLY " << callsign.toStdString() << " " << realname.toStdString() << std::endl;
}

void Client::capabilitiesQueryReplyReceived(const QString& callsign, quint32 flags)
{
    std::cout << "CAPS_REPLY " << callsign.toStdString() << " " << flags << std::endl;
}

void Client::freqQueryRequestReceived(const QString& callsign)
{
    std::cout << "FREQ_QUERY " << callsign.toStdString() << std::endl;
}

void Client::nameQueryRequestReceived(const QString& callsign)
{
    std::cout << "NAME_QUERY " << callsign.toStdString() << std::endl;
}

void Client::kicked(const QString& msg)
{
    std::cout << "KICKED " << msg.toStdString() << std::endl;
}

void Client::metarReceived(const QString& data)
{
    std::cout << "METAR " << data.toStdString() << std::endl;
}

void Client::pilotDisconnected(const QString& callsign)
{
    std::cout << "PILOT_DISCONNECTED " << callsign.toStdString() << std::endl;
}

void Client::planeInfoReceived(const QString& callsign, const QString& acTypeICAO, const QString& airlineICAO, const QString& livery)
{
    std::cout << "PLANE_INFO_REPLY " << callsign.toStdString() << " " << acTypeICAO.toStdString() << " " << airlineICAO.toStdString() << " " << livery.toStdString() << std::endl;
}

void Client::planeInfoRequestReceived(const QString& callsign)
{
    std::cout << "PLANE_INFO_QUERY " << callsign.toStdString() << std::endl;
}

void Client::pong(const QString& callsign, const BlackMisc::PhysicalQuantities::CTime& elapsedTime)
{
    std::cout << "PONG " << callsign.toStdString() << " " << elapsedTime << std::endl;
}

void Client::radioTextMessageReceived(const QString& callsign, const QString& msg, const QVector<BlackMisc::PhysicalQuantities::CFrequency>& freqs)
{
    QString freqsBlob = freqs[0].toQString();
    for (auto i = freqs.begin() + 1; i != freqs.end(); ++i)
    {
        freqsBlob.append("|");
        freqsBlob.append(i->toQString());
    }
    std::cout << "TEXT_MSG " << callsign.toStdString() << " " << freqsBlob.toStdString() << " " << msg.toStdString() << std::endl;
}

void Client::privateTextMessageReceived(const QString& fromCallsign, const QString& toCallsign, const QString& msg)
{
    std::cout << "PRIV_MSG " << fromCallsign.toStdString() << " -> " << toCallsign.toStdString() << msg.toStdString() << std::endl;
}
