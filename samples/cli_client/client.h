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
    void requestPlaneInfoCmd(QTextStream& args);
    void sendPlaneInfoCmd(QTextStream& args);
    void help(QTextStream &args);
    void echo(QTextStream &args);
    void exit(QTextStream &args);
    void setServerDetailsCmd(QTextStream &args);
    void setUserCredentialsCmd(QTextStream &args);
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
    void pingCmd(QTextStream &args);
    void requestMetarCmd(QTextStream &args);
    void requestWeatherDataCmd(QTextStream &args);

signals: //to send to INetwork
    void setCallsign(const QString& callsign);
    void setServerDetails(const QString &hostname, quint16 port);
    void setUserCredentials(const QString &username, const QString &password);
    void setRealName(const QString &name);
    void initiateConnection();
    void terminateConnection();
    void sendPrivateTextMessage(const QString& callsign, const QString& msg);
    void sendRadioTextMessage(const QVector<BlackMisc::PhysicalQuantities::CFrequency>& freqs, const QString& msg);
    void sendIpQuery();
    void sendFreqQuery(const QString& callsign);
    void sendServerQuery(const QString& callsign);
    void sendAtcQuery(const QString& callsign);
    void sendAtisQuery(const QString& callsign);
    void sendNameQuery(const QString& callsign);
    void sendCapabilitiesQuery(const QString& callsign);
    void replyToFreqQuery(const QString& callsign, const BlackMisc::PhysicalQuantities::CFrequency& freq);
    void replyToNameQuery(const QString& callsign, const QString& realname);
    void requestPlaneInfo(const QString& callsign);
    void sendPlaneInfo(const QString& callsign, const QString& acTypeICAO, const QString& airlineICAO, const QString& livery);
    void ping(const QString& callsign);
    void requestMetar(const QString& airportICAO);
    void requestWeatherData(const QString& airportICAO);

public slots: //to receive from INetwork
    void atcPositionUpdate(const QString& callsign, const BlackMisc::PhysicalQuantities::CFrequency& freq,
        const BlackMisc::Geo::CCoordinateGeodetic& pos, const BlackMisc::PhysicalQuantities::CLength& range);
    void atcDisconnected(const QString& callsign);
    void connectionStatusIdle();
    void connectionStatusConnecting();
    void connectionStatusConnected();
    void connectionStatusDisconnected();
    void connectionStatusError();
    void freqQueryReplyReceived(const QString& callsign, const BlackMisc::PhysicalQuantities::CFrequency& freq);
    void serverQueryReplyReceived(const QString& callsign, const QString& hostname);
    void atcQueryReplyReceived(const QString& callsign, bool isATC);
    void atisQueryReplyReceived(const QString& callsign, const QString& data);
    void nameQueryReplyReceived(const QString& callsign, const QString& realname);
    void capabilitiesQueryReplyReceived(const QString& callsign, quint32 flags);
    void freqQueryRequestReceived(const QString& callsign);
    void nameQueryRequestReceived(const QString& callsign);
    void pilotDisconnected(const QString& callsign);
    void planeInfoReceived(const QString& callsign, const QString& acTypeICAO, const QString& airlineICAO, const QString& livery);
    void planeInfoRequestReceived(const QString& callsign);
    void pong(const QString& callsign, const BlackMisc::PhysicalQuantities::CTime& elapsedTime);
    void radioTextMessageReceived(const QString& callsign, const QString& msg, const QVector<BlackMisc::PhysicalQuantities::CFrequency>& freqs);
    void privateTextMessageReceived(const QString& fromCallsign, const QString& toCallsign, const QString& msg);
    void ipQueryReplyReceived(const QString &ip);
    void kicked(const QString &msg);
    void metarReceived(const QString &data);

private:
    QMap<QString, std::function<void(QTextStream &)>> m_commands;
    BlackCore::INetwork *m_net;
};

#endif //__BLACKSAMPLE_CLI_CLIENT_H__
