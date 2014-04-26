/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __BLACKSAMPLE_CLI_CLIENT_H__
#define __BLACKSAMPLE_CLI_CLIENT_H__

#include "blackcore/network.h"
#include <QObject>
#include <QTextStream>
#include <QMap>
#include <functional>

class Client : public QObject
{
    Q_OBJECT

public:
    Client(QObject *parent = nullptr);

signals:
    void quit();

public slots:
    void command(QString line);

private: //commands
    void help(QTextStream &args);
    void echo(QTextStream &args);
    void exit(QTextStream &args);
    void getStatusUrlsCmd(QTextStream &args);
    void getKnownServersCmd(QTextStream &args);
    void presetServerCmd(QTextStream &args);
    void presetCallsignCmd(QTextStream &args);
    void presetIcaoCodesCmd(QTextStream &args);
    void presetLoginModeCmd(QTextStream &args);
    void initiateConnectionCmd(QTextStream &args);
    void terminateConnectionCmd(QTextStream &args);
    void sendPrivateTextMessageCmd(QTextStream &args);
    void sendRadioTextMessageCmd(QTextStream &args);
    void sendIpQueryCmd(QTextStream &args);
    void sendFreqQueryCmd(QTextStream &args);
    void sendServerQueryCmd(QTextStream &args);
    void sendAtcQueryCmd(QTextStream &args);
    void sendAtisQueryCmd(QTextStream &args);
    void sendFlightPlanCmd(QTextStream &args);
    void sendRealNameQueryCmd(QTextStream &args);
    void sendCapabilitiesQueryCmd(QTextStream &args);
    void sendIcaoCodesQueryCmd(QTextStream &args);
    void setOwnAircraftCmd(QTextStream &args);
    void setOwnAircraftPositionCmd(QTextStream &args);
    void setOwnAircraftSituationCmd(QTextStream &args);
    void setOwnAircraftAvionicsCmd(QTextStream &args);
    void sendPingCmd(QTextStream &args);
    void sendMetarQueryCmd(QTextStream &args);
    void sendWeatherDataQueryCmd(QTextStream &args);
    void sendCustomPacketCmd(QTextStream &args);

signals: //to send to INetwork
    void presetServer(const BlackMisc::Network::CServer &server);
    void presetCallsign(const BlackMisc::Aviation::CCallsign &callsign);
    void presetRealName(const QString &name);
    void presetIcaoCodes(const BlackMisc::Aviation::CAircraftIcao &icao);
    void presetLoginMode(BlackCore::INetwork::LoginMode mode);
    void initiateConnection();
    void terminateConnection();
    void sendTextMessages(const BlackMisc::Network::CTextMessageList &textMessages);
    void sendRadioTextMessage(const QVector<BlackMisc::PhysicalQuantities::CFrequency> &freqs, const QString &msg);
    void sendIpQuery();
    void sendFreqQuery(const BlackMisc::Aviation::CCallsign &callsign);
    void sendServerQuery(const BlackMisc::Aviation::CCallsign &callsign);
    void sendAtcQuery(const BlackMisc::Aviation::CCallsign &callsign);
    void sendAtisQuery(const BlackMisc::Aviation::CCallsign &callsign);
    void sendFlightPlan(const BlackMisc::Aviation::CFlightPlan &fp);
    void sendRealNameQuery(const BlackMisc::Aviation::CCallsign &callsign);
    void sendCapabilitiesQuery(const BlackMisc::Aviation::CCallsign &callsign);
    void sendIcaoCodesQuery(const BlackMisc::Aviation::CCallsign &callsign);
    void setOwnAircraft(const BlackMisc::Aviation::CAircraft &aircraft);
    void setOwnAircraftPosition(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude);
    void setOwnAircraftSituation(const BlackMisc::Aviation::CAircraftSituation &situation);
    void setOwnAircraftAvionics(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2,
        const BlackMisc::Aviation::CTransponder &xpdr);
    void sendPing(const BlackMisc::Aviation::CCallsign &callsign);
    void sendMetarQuery(const QString &airportICAO);
    void sendWeatherDataQuery(const QString &airportICAO);
    void sendCustomPacket(const BlackMisc::Aviation::CCallsign &callsign, const QString &packetId, const QStringList &data);

public slots: //to receive from INetwork
    void atcPositionUpdate(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CFrequency &freq,
                           const BlackMisc::Geo::CCoordinateGeodetic &pos, const BlackMisc::PhysicalQuantities::CLength &range);
    void atcDisconnected(const BlackMisc::Aviation::CCallsign &callsign);
    void connectionStatusChanged(BlackCore::INetwork::ConnectionStatus oldStatus, BlackCore::INetwork::ConnectionStatus newStatus,
                                 const QString &errorMessage);
    void ipReplyReceived(const QString &ip);
    void freqReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CFrequency &freq);
    void serverReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &hostname);
    void atcReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, bool isATC);
    void atisReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CInformationMessage &atis);
    void realNameReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &realname);
    void capabilitiesReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, quint32 flags);
    void kicked(const QString &msg);
    void metarReplyReceived(const QString &data);
    void pilotDisconnected(const BlackMisc::Aviation::CCallsign &callsign);
    void icaoCodesReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftIcao &icaoData);
    void pongReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CTime &elapsedTime);
    void textMessagesReceived(const BlackMisc::Network::CTextMessageList &messages);
    void customPacketReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &packetId, const QStringList &data);

private:
    QMap<QString, std::function<void(QTextStream &)>> m_commands;
    BlackCore::INetwork *m_net;
};

#endif //__BLACKSAMPLE_CLI_CLIENT_H__
