/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_CONTEXT_CONTEXTNETWORK_PROXY_H
#define BLACKCORE_CONTEXT_CONTEXTNETWORK_PROXY_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/context/contextnetwork.h"
#include "blackcore/corefacadeconfig.h"
#include "blackmisc/aviation/airporticaocode.h"
#include "blackmisc/aviation/atcstation.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/callsignset.h"
#include "blackmisc/aviation/flightplan.h"
#include "blackmisc/identifier.h"
#include "blackmisc/network/clientlist.h"
#include "blackmisc/network/loginmode.h"
#include "blackmisc/network/server.h"
#include "blackmisc/network/serverlist.h"
#include "blackmisc/network/textmessagelist.h"
#include "blackmisc/network/user.h"
#include "blackmisc/network/userlist.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/weather/metar.h"

#include <stdbool.h>
#include <QObject>
#include <QString>

// clazy:excludeall=const-signal-or-slot

class QDBusConnection;

namespace BlackMisc
{
    class CGenericDBusInterface;
    namespace Aviation
    {
        class CAircraftParts;
        class CCallsign;
    }
    namespace Simulation { class CAircraftModel; }
}

namespace BlackCore
{
    class CCoreFacade;
    namespace Context
    {
        //! Network context proxy
        //! \ingroup dbus
        class BLACKCORE_EXPORT CContextNetworkProxy : public IContextNetwork
        {
            Q_OBJECT
            friend class IContextNetwork;

        public:
            //! Destructor
            virtual ~CContextNetworkProxy() override {}

            //! Unit test relay signals
            //! \private
            static void unitTestRelaySignals();

        public slots:
            //! \name Interface overrides
            //! @{
            virtual void requestAtcBookingsUpdate() const override;
            virtual BlackMisc::Aviation::CAtcStationList getAtcStationsOnline(bool recalculateDistance) const override;
            virtual BlackMisc::Aviation::CAtcStationList getClosestAtcStationsOnline(int number) const override;
            virtual BlackMisc::Aviation::CAtcStationList getAtcStationsBooked(bool recalculateDistance) const override;
            virtual BlackMisc::Simulation::CSimulatedAircraftList getAircraftInRange() const override;
            virtual BlackMisc::Aviation::CCallsignSet getAircraftInRangeCallsigns() const override;
            virtual int getAircraftInRangeCount() const override;
            virtual bool isAircraftInRange(const BlackMisc::Aviation::CCallsign &callsign) const override;
            virtual BlackMisc::Simulation::CSimulatedAircraft getAircraftInRangeForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const override;
            virtual BlackMisc::Aviation::CAtcStationList getOnlineStationsForFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency) const override;
            virtual BlackMisc::Aviation::CAtcStation getOnlineStationForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const override;
            virtual bool isOnlineStation(const BlackMisc::Aviation::CCallsign &callsign) const override;
            virtual BlackMisc::CStatusMessage connectToNetwork(const BlackMisc::Network::CServer &server, const QString &extraLiveryString, bool sendLivery, const QString &extraModelString, bool sendModelString, const BlackMisc::Aviation::CCallsign &partnerCallsign, BlackMisc::Network::CLoginMode mode) override;
            virtual BlackMisc::CStatusMessage disconnectFromNetwork() override;
            virtual bool isConnected() const override;
            virtual BlackMisc::Network::CServer getConnectedServer() const override;
            virtual BlackMisc::Network::CLoginMode getLoginMode() const override;
            virtual bool parseCommandLine(const QString &commandLine, const BlackMisc::CIdentifier &originator) override;
            virtual void sendTextMessages(const BlackMisc::Network::CTextMessageList &textMessages) override;
            virtual void sendFlightPlan(const BlackMisc::Aviation::CFlightPlan &flightPlan) override;
            virtual BlackMisc::Aviation::CFlightPlan loadFlightPlanFromNetwork(const BlackMisc::Aviation::CCallsign &callsign) const override;
            BlackMisc::Weather::CMetar getMetarForAirport(const BlackMisc::Aviation::CAirportIcaoCode &airportIcaoCode) const override;
            virtual BlackMisc::Aviation::CAtcStationList getSelectedAtcStations() const override;
            virtual BlackMisc::Network::CUserList getUsers() const override;
            virtual BlackMisc::Network::CUserList getUsersForCallsigns(const BlackMisc::Aviation::CCallsignSet &callsigns) const override;
            virtual BlackMisc::Network::CUser getUserForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const override;
            virtual BlackMisc::Network::CClientList getClients() const override;
            virtual BlackMisc::Network::CServerList getVatsimVoiceServers() const override;
            virtual BlackMisc::Network::CServerList getVatsimFsdServers() const override;
            virtual BlackMisc::Network::CClientList getClientsForCallsigns(const BlackMisc::Aviation::CCallsignSet &callsigns) const override;
            virtual bool setOtherClient(const BlackMisc::Network::CClient &client) override;
            virtual void requestAircraftDataUpdates()override;
            virtual void requestAtisUpdates() override;
            virtual bool updateAircraftEnabled(const BlackMisc::Aviation::CCallsign &callsign, bool enabledForRendering) override;
            virtual bool setAircraftEnabledFlag(const BlackMisc::Aviation::CCallsign &callsign, bool enabledForRendering) override;
            virtual bool updateAircraftModel(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Simulation::CAircraftModel &model, const BlackMisc::CIdentifier &originator) override;
            virtual bool updateAircraftNetworkModel(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Simulation::CAircraftModel &model, const BlackMisc::CIdentifier &originator) override;
            virtual bool updateFastPositionEnabled(const BlackMisc::Aviation::CCallsign &callsign, bool enableFastPositionSending) override;
            virtual bool updateAircraftSupportingGndFLag(const BlackMisc::Aviation::CCallsign &callsign, bool supportGndFlag) override;
            virtual int reInitializeAllAircraft() override;
            virtual void setFastPositionEnabledCallsigns(BlackMisc::Aviation::CCallsignSet &callsigns) override;
            virtual BlackMisc::Aviation::CCallsignSet getFastPositionEnabledCallsigns() const override;
            virtual BlackMisc::CStatusMessageList getReverseLookupMessages(const BlackMisc::Aviation::CCallsign &callsign) const override;
            virtual BlackMisc::Simulation::ReverseLookupLogging isReverseLookupMessagesEnabled() const override;
            virtual void enableReverseLookupMessages(BlackMisc::Simulation::ReverseLookupLogging enable) override;
            virtual BlackMisc::CStatusMessageList getAircraftPartsHistory(const BlackMisc::Aviation::CCallsign &callsign) const override;
            virtual BlackMisc::Aviation::CAircraftPartsList getRemoteAircraftParts(const BlackMisc::Aviation::CCallsign &callsign) const override;
            virtual QString getLibraryInfo(bool detailed) const override;
            virtual bool isAircraftPartsHistoryEnabled() const override;
            virtual bool isRemoteAircraftSupportingParts(const BlackMisc::Aviation::CCallsign &callsign) const override;
            virtual void enableAircraftPartsHistory(bool enabled) override;
            virtual QString getNetworkStatistics(bool reset, const QString &separator) override;
            virtual bool setNetworkStatisticsEnable(bool enabled) override;
            virtual QStringList getNetworkPresetValues() const override;
            virtual BlackMisc::Aviation::CCallsign getPartnerCallsign() const override;
            virtual void testCreateDummyOnlineAtcStations(int number) override;
            virtual void testAddAircraftParts(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftParts &parts, bool incremental) override;
            virtual void testReceivedTextMessages(const BlackMisc::Network::CTextMessageList &textMessages) override;
            virtual void testReceivedAtisMessage(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CInformationMessage &msg) override;
            virtual void testRequestAircraftConfig(const BlackMisc::Aviation::CCallsign &callsign) override;
            virtual bool testAddAltitudeOffset(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CLength &offset = BlackMisc::PhysicalQuantities::CLength::null()) override;
            //! @}

        public:
            //! \copydoc IContextNetwork::connectRawFsdMessageSignal
            virtual QMetaObject::Connection connectRawFsdMessageSignal(QObject *receiver, RawFsdMessageReceivedSlot rawFsdMessageReceivedSlot) override;

        private:
            BlackMisc::CGenericDBusInterface *m_dBusInterface; /*!< DBus interface */

            //! Relay connection signals to local signals.
            void relaySignals(const QString &serviceName, QDBusConnection &connection);

        protected:
            //! Constructor
            CContextNetworkProxy(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime) : IContextNetwork(mode, runtime), m_dBusInterface(nullptr) {}

            //! DBus version constructor
            CContextNetworkProxy(const QString &serviceName, QDBusConnection &connection, CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime);
        };
    } // ns
} // ns
#endif // guard
