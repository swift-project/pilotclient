/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __BLACKSAMPLE_CLI_CLIENT_H__
#define __BLACKSAMPLE_CLI_CLIENT_H__

#include "blackcore/network.h"
#include "blackmisc/context.h"
#include <QObject>
#include <QTextStream>
#include <QMap>
#include <functional>

class Client : public QObject
{
    Q_OBJECT

public:
    Client(BlackMisc::IContext &);

signals:
    void quit();

public slots:
    void command(QString line);

private: //commands
    void help(QTextStream &args);
    void echo(QTextStream &args);
    void exit(QTextStream &args);
    void setServerCmd(QTextStream &args);
    void setCallsignCmd(QTextStream &args);
    void setRealNameCmd(QTextStream &args);
    void initiateConnectionCmd(QTextStream &args);
    void terminateConnectionCmd(QTextStream &args);
    void sendPrivateTextMessageCmd(QTextStream &args);
    void sendRadioTextMessageCmd(QTextStream &args);
    void sendIpQueryCmd(QTextStream &args);
    void sendFreqQueryCmd(QTextStream &args);
    void sendServerQueryCmd(QTextStream &args);
    void sendAtcQueryCmd(QTextStream &args);
    void sendAtisQueryCmd(QTextStream &args);
    void sendNameQueryCmd(QTextStream &args);
    void sendCapabilitiesQueryCmd(QTextStream &args);
    void replyToFreqQueryCmd(QTextStream &args);
    void replyToNameQueryCmd(QTextStream &args);
    void requestAircraftInfoCmd(QTextStream &args);
    void sendAircraftInfoCmd(QTextStream &args);
    void pingCmd(QTextStream &args);
    void requestMetarCmd(QTextStream &args);
    void requestWeatherDataCmd(QTextStream &args);

signals: //to send to INetwork
    void setServer(const BlackMisc::Network::CServer &server);
    void setCallsign(const BlackMisc::Aviation::CCallsign &callsign);
    void setRealName(const QString &name);
    void initiateConnection();
    void terminateConnection();
    void sendTextMessages(const BlackMisc::Network::CTextMessageList &textMessages);
    void sendRadioTextMessage(const QVector<BlackMisc::PhysicalQuantities::CFrequency> &freqs, const QString &msg);
    void sendIpQuery();
    void sendFreqQuery(const BlackMisc::Aviation::CCallsign &callsign);
    void sendServerQuery(const BlackMisc::Aviation::CCallsign &callsign);
    void sendAtcQuery(const BlackMisc::Aviation::CCallsign &callsign);
    void sendAtisQuery(const BlackMisc::Aviation::CCallsign &callsign);
    void sendNameQuery(const BlackMisc::Aviation::CCallsign &callsign);
    void sendCapabilitiesQuery(const BlackMisc::Aviation::CCallsign &callsign);
    void replyToFreqQuery(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CFrequency &freq);
    void replyToNameQuery(const BlackMisc::Aviation::CCallsign &callsign, const QString &realname);
    void requestPlaneInfo(const BlackMisc::Aviation::CCallsign &callsign);
    void sendAircraftInfo(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftIcao &icao);
    void ping(const BlackMisc::Aviation::CCallsign &callsign);
    void requestMetar(const QString &airportICAO);
    void requestWeatherData(const QString &airportICAO);

public slots: //to receive from INetwork
    void atcPositionUpdate(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CFrequency &freq,
                           const BlackMisc::Geo::CCoordinateGeodetic &pos, const BlackMisc::PhysicalQuantities::CLength &range);
    void atcDisconnected(const BlackMisc::Aviation::CCallsign &callsign);
    void connectionStatusIdle();
    void connectionStatusConnecting();
    void connectionStatusConnected();
    void connectionStatusDisconnected();
    void connectionStatusError();
    void ipQueryReplyReceived(const QString &ip);
    void freqQueryReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CFrequency &freq);
    void serverQueryReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &hostname);
    void atcQueryReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, bool isATC);
    void atisQueryReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &data);
    void nameQueryReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &realname);
    void capabilitiesQueryReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, quint32 flags);
    void freqQueryRequestReceived(const BlackMisc::Aviation::CCallsign &callsign);
    void nameQueryRequestReceived(const BlackMisc::Aviation::CCallsign &callsign);
    void kicked(const QString &msg);
    void metarReceived(const QString &data);
    void pilotDisconnected(const BlackMisc::Aviation::CCallsign &callsign);
    void aircraftInfoReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftIcao &icaoData);
    void aircraftInfoRequestReceived(const BlackMisc::Aviation::CCallsign &callsign);
    void pong(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CTime &elapsedTime);
    void textMessagesReceived(const BlackMisc::Network::CTextMessageList &messages);

private:
    QMap<QString, std::function<void(QTextStream &)>> m_commands;
    BlackCore::INetwork *m_net;
};

#endif //__BLACKSAMPLE_CLI_CLIENT_H__
