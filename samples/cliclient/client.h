/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file
//! \ingroup samplecliclient

#ifndef BLACKSAMPLE_CLICLIENT_CLIENT_H
#define BLACKSAMPLE_CLICLIENT_CLIENT_H

#include "blackcore/fsd/fsdclient.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/identifiable.h"
#include "blackmisc/network/textmessagelist.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/time.h"
#include "blackmisc/simulation/ownaircraftprovider.h"

#include <QMap>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QtGlobal>
#include <functional>

class QTextStream;

namespace BlackMisc
{
    namespace Aviation
    {
        class CCallsign;
        class CComSystem;
        class CFlightPlan;
        class CInformationMessage;
        class CTransponder;
    }
    namespace Network
    {
        class CServer;
    }
    namespace Simulation
    {
        class CSimulatedAircraft;
    }
}
template <class T>
class QVector;

namespace BlackSample
{

    /*!
     * \brief Simple command line interface client
     *
     * This class implements a simple and limited command line interace class.
     * It accepts commands from the console via \sa command. Note that the class
     * does not send anything on its own, not even position packets.
     */
    class Client :
        public QObject,
        public BlackMisc::Simulation::COwnAircraftAware,
        public BlackMisc::CIdentifiable
    {
        Q_OBJECT

    public:
        //! Constructor
        Client(QObject *parent = nullptr);

    signals:
        //! Terminated
        void quit();

    public slots:
        //! Handle command
        void command(QString line);

    private: // commands
        void help(QTextStream &args);
        void echo(QTextStream &args);
        void exit(QTextStream &args);
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
        void sendFlightPlanQueryCmd(QTextStream &args);
        void sendRealNameQueryCmd(QTextStream &args);
        void sendCapabilitiesQueryCmd(QTextStream &args);
        void sendIcaoCodesQueryCmd(QTextStream &args);
        void setOwnAircraftCmd(QTextStream &args);
        void setOwnAircraftPositionCmd(QTextStream &args);
        void setOwnAircraftSituationCmd(QTextStream &args);
        void setOwnAircraftCockpitCmd(QTextStream &args);
        void sendPingCmd(QTextStream &args);
        void sendMetarQueryCmd(QTextStream &args);

    signals:
        //! \name Signals to INetwork
        //! @{

        //! to send to INetwork
        void presetServer(const BlackMisc::Network::CServer &server);
        void presetCallsign(const BlackMisc::Aviation::CCallsign &callsign);
        void presetRealName(const QString &name);
        void presetIcaoCodes(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);
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
        void sendFlightPlanQuery(const BlackMisc::Aviation::CCallsign &callsign);
        void sendRealNameQuery(const BlackMisc::Aviation::CCallsign &callsign);
        void sendCapabilitiesQuery(const BlackMisc::Aviation::CCallsign &callsign);
        void sendIcaoCodesQuery(const BlackMisc::Aviation::CCallsign &callsign);
        void setOwnAircraftCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2,
                                   const BlackMisc::Aviation::CTransponder &xpdr, const QString &originator);
        void sendPing(const BlackMisc::Aviation::CCallsign &callsign);
        void sendMetarQuery(const QString &airportICAO);
        //! @}

    public slots:
        //! \name Slots connected to INetwork
        //! @{

        //! Slots connected to INetwork
        void atcPositionUpdate(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CFrequency &freq,
                               const BlackMisc::Geo::CCoordinateGeodetic &pos, const BlackMisc::PhysicalQuantities::CLength &range);
        void atcDisconnected(const BlackMisc::Aviation::CCallsign &callsign);
        void connectionStatusChanged(BlackCore::INetwork::ConnectionStatus oldStatus, BlackCore::INetwork::ConnectionStatus newStatus);
        void ipReplyReceived(const QString &ip);
        void freqReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CFrequency &freq);
        void serverReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &server);
        void atcReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, bool isATC);
        void atisReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CInformationMessage &atis);
        void realNameReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &realname);
        void capabilitiesReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, quint32 flags);
        void kicked(const QString &msg);
        void metarReplyReceived(const QString &data);
        void flightPlanReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CFlightPlan &flightPlan);
        void pilotDisconnected(const BlackMisc::Aviation::CCallsign &callsign);
        void icaoCodesReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &aircraftIcaoDesignator, const QString &airlineIcaoDesignator, const QString &livery);
        void pongReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CTime &elapsedTime);
        void textMessagesReceived(const BlackMisc::Network::CTextMessageList &messages);
        void customPacketReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &packetId, const QStringList &data);
        //! @}

    private:
        QMap<QString, std::function<void(QTextStream &)>> m_commands;
        BlackCore::INetwork *m_net;
    };

} // ns

#endif // guard
