// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_CONTEXT_CONTEXTNETWORK_H
#define BLACKCORE_CONTEXT_CONTEXTNETWORK_H

#include "blackcore/context/context.h"
#include "blackcore/corefacade.h"
#include "blackcore/corefacadeconfig.h"
#include "blackcore/blackcoreexport.h"
#include "blackmisc/simulation/remoteaircraftprovider.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/aviation/aircraftpartslist.h"
#include "blackmisc/aviation/airporticaocode.h"
#include "blackmisc/aviation/atcstation.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/callsignset.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/aviation/flightplan.h"
#include "blackmisc/network/connectionstatus.h"
#include "blackmisc/network/clientlist.h"
#include "blackmisc/network/loginmode.h"
#include "blackmisc/network/server.h"
#include "blackmisc/network/serverlist.h"
#include "blackmisc/network/textmessagelist.h"
#include "blackmisc/network/user.h"
#include "blackmisc/network/userlist.h"
#include "blackmisc/identifiable.h"
#include "blackmisc/identifier.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/weather/metar.h"

#include <QObject>
#include <QString>
#include <QCommandLineOption>
#include <functional>

// clazy:excludeall=const-signal-or-slot

//! \ingroup dbus
//! DBus interface for context
#define BLACKCORE_CONTEXTNETWORK_INTERFACENAME "org.swift_project.blackcore.contextnetwork"

//! \ingroup dbus
//! DBus object path for context
#define BLACKCORE_CONTEXTNETWORK_OBJECTPATH "/network"

class QDBusConnection;
namespace BlackMisc
{
    class CDBusServer;
    namespace Aviation
    {
        class CAircraftParts;
        class CCallsign;
    }
    namespace Network
    {
        class CRawFsdMessage;
        class CTextMessage;
    }
    namespace Simulation
    {
        class CAircraftModel;
    }
}
namespace BlackCore::Context
{
    //! Network context proxy
    class BLACKCORE_EXPORT IContextNetwork : public IContext
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTNETWORK_INTERFACENAME)

    public:
        //! DBus interface name
        static const QString &InterfaceName()
        {
            static QString s(BLACKCORE_CONTEXTNETWORK_INTERFACENAME);
            return s;
        }

        //! DBus object path
        static const QString &ObjectPath()
        {
            static QString s(BLACKCORE_CONTEXTNETWORK_OBJECTPATH);
            return s;
        }

        //! \copydoc IContext::getPathAndContextId()
        virtual QString getPathAndContextId() const override { return this->buildPathAndContextId(ObjectPath()); }

        //! Factory method
        static IContextNetwork *create(CCoreFacade *parent, CCoreFacadeConfig::ContextMode mode, BlackMisc::CDBusServer *server, QDBusConnection &connection);

        //! Destructor
        virtual ~IContextNetwork() override {}

    signals:
        //! An aircraft disappeared
        void removedAircraft(const BlackMisc::Aviation::CCallsign &callsign);

        //! A new aircraft appeared
        void addedAircraft(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft);

        //! Read for model matching
        void readyForModelMatching(const BlackMisc::Simulation::CSimulatedAircraft &renderedAircraft);

        //! ATC station (online) list has been changed
        void changedAtcStationsOnline();

        //! Digest signal changedAtcStationsOnline()
        void changedAtcStationsOnlineDigest();

        //! Aircraft list has been changed
        void changedAircraftInRange();

        //! Digest signal changedAircraftInRange()
        void changedAircraftInRangeDigest();

        //! Aircraft model was changed
        //! \details All remote aircraft are stored in the network context. The model can be updated here
        //!          via \sa updateAircraftModel and then this signal is fired
        void changedRemoteAircraftModel(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const BlackMisc::CIdentifier &originator);

        //! Aircraft enabled / disabled
        //! \details All remote aircraft are stored in the network context. The aircraft can be enabled (for rendering) here
        //!          via \sa updateAircraftEnabled and then this signal is fired
        void changedRemoteAircraftEnabled(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

        //! Aircraft enabled / disabled
        void changedFastPositionUpdates(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

        //! Changed gnd. flag capability
        void changedGndFlagCapability(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

        //! Connection status changed for online station
        void changedAtcStationOnlineConnectionStatus(const BlackMisc::Aviation::CAtcStation &atcStation, bool connected);

        //! User has been kicked from network
        void kicked(const QString &kickMessage);

        //! Network error
        void severeNetworkError(const QString &errorMessage);

        //! Connection status changed
        //! \sa IContextNetwork::connectedServerChanged
        void connectionStatusChanged(const BlackMisc::Network::CConnectionStatus &from, const BlackMisc::Network::CConnectionStatus &to);

        //! The connected server has been changed
        //! \remark can also be used to determine if the ecosystem has been changed
        //! \sa BlackMisc::Network::CServer::getEcosysstem
        void connectedServerChanged(const BlackMisc::Network::CServer &server);

        //! Text messages received (also private chat messages, radio channel messages)
        void textMessagesReceived(const BlackMisc::Network::CTextMessageList &textMessages);

        //! A supervisor text message was received
        void supervisorTextMessageReceived(const BlackMisc::Network::CTextMessage &message);

        //! Text message sent (by me)
        void textMessageSent(const BlackMisc::Network::CTextMessage &sentMessage);

    public slots:
        //! The ATC list with online ATC controllers
        virtual BlackMisc::Aviation::CAtcStationList getAtcStationsOnline(bool recalculateDistance) const = 0;

        //! The ATC list with online ATC controllers
        //! \remark recalculates distance and picks closest elements
        //! \remark sorted by distance, nearest first
        virtual BlackMisc::Aviation::CAtcStationList getClosestAtcStationsOnline(int number) const = 0;

        //! Aircraft list
        virtual BlackMisc::Simulation::CSimulatedAircraftList getAircraftInRange() const = 0;

        //! Aircraft callsigns
        virtual BlackMisc::Aviation::CCallsignSet getAircraftInRangeCallsigns() const = 0;

        //! Aircraft for given callsign
        virtual BlackMisc::Simulation::CSimulatedAircraft getAircraftInRangeForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const = 0;

        //! Aircraft count
        virtual int getAircraftInRangeCount() const = 0;

        //! Aircraft in range
        virtual bool isAircraftInRange(const BlackMisc::Aviation::CCallsign &callsign) const = 0;

        //! Get METAR, if not available request it (code such as EDDF, KLAX)
        virtual BlackMisc::Weather::CMetar getMetarForAirport(const BlackMisc::Aviation::CAirportIcaoCode &airportIcaoCode) const = 0;

        //! Online station for callsign
        virtual BlackMisc::Aviation::CAtcStation getOnlineStationForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const = 0;

        //! Online stations for frequency
        virtual BlackMisc::Aviation::CAtcStationList getOnlineStationsForFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency) const = 0;

        //! Online station for callsign?
        virtual bool isOnlineStation(const BlackMisc::Aviation::CCallsign &callsign) const = 0;

        //! Get all users
        virtual BlackMisc::Network::CUserList getUsers() const = 0;

        //! Users for given callsigns, e.g. for voice room resolution
        virtual BlackMisc::Network::CUserList getUsersForCallsigns(const BlackMisc::Aviation::CCallsignSet &callsigns) const = 0;

        //! User for given callsign, e.g. for text messages
        virtual BlackMisc::Network::CUser getUserForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const = 0;

        //! Information about other clients
        virtual BlackMisc::Network::CClientList getClients() const = 0;

        //! Clients for given callsign, e.g. to test/fetch direct aircraft model
        virtual BlackMisc::Network::CClientList getClientsForCallsigns(const BlackMisc::Aviation::CCallsignSet &callsigns) const = 0;

        //! Set client for given callsign
        virtual bool setOtherClient(const BlackMisc::Network::CClient &client) = 0;

        //! Known FSD servers, available when data file was first read
        virtual BlackMisc::Network::CServerList getVatsimFsdServers() const = 0;

        //! Callsigns enabled for fast position updates
        virtual void setFastPositionEnabledCallsigns(BlackMisc::Aviation::CCallsignSet &callsigns) = 0;

        //! Callsigns enabled for fast position updates
        virtual BlackMisc::Aviation::CCallsignSet getFastPositionEnabledCallsigns() const = 0;

        //! Connect to Network
        //! \return messages generated during connecting
        virtual BlackMisc::CStatusMessage connectToNetwork(
            const BlackMisc::Network::CServer &server,
            const QString &extraLiveryString, bool sendLivery,
            const QString &extraModelString, bool sendModelString,
            const BlackMisc::Aviation::CCallsign &partnerCallsign,
            BlackMisc::Network::CLoginMode loginMode) = 0;

        //! Server which is connected, if not connected empty default object.
        virtual BlackMisc::Network::CServer getConnectedServer() const = 0;

        //! Login mode
        virtual BlackMisc::Network::CLoginMode getLoginMode() const = 0;

        //! Mode as string
        QString getLoginModeAsString() const { return this->getLoginMode().toQString(); }

        //! Disconnect from network
        //! \return messages generated during disconnecting
        virtual BlackMisc::CStatusMessage disconnectFromNetwork() = 0;

        //! Network connected?
        virtual bool isConnected() const = 0;

        //! Text messages (radio and private chat messages)
        virtual void sendTextMessages(const BlackMisc::Network::CTextMessageList &textMessages) = 0;

        //! Send flight plan
        virtual void sendFlightPlan(const BlackMisc::Aviation::CFlightPlan &flightPlan) = 0;

        //! Load flight plan (from network)
        virtual BlackMisc::Aviation::CFlightPlan loadFlightPlanFromNetwork(const BlackMisc::Aviation::CCallsign &callsign) const = 0;

        //! Use the selected COM1/2 frequencies, and get the corresponding ATC stations for it
        virtual BlackMisc::Aviation::CAtcStationList getSelectedAtcStations() const = 0;

        //! Request data updates (pilot's frequencies, ATIS, ..)
        virtual void requestAircraftDataUpdates() = 0;

        //! Request ATIS updates (for all stations)
        virtual void requestAtisUpdates() = 0;

        //! Enable/disable rendering
        virtual bool updateAircraftEnabled(const BlackMisc::Aviation::CCallsign &callsign, bool enabledForRendering) = 0;

        //! Set the enabled flag without any further logic like sending signals
        virtual bool setAircraftEnabledFlag(const BlackMisc::Aviation::CCallsign &callsign, bool enabledForRendering) = 0;

        //! Enable/disable support of gnd. flag
        virtual bool updateAircraftSupportingGndFLag(const BlackMisc::Aviation::CCallsign &callsign, bool supportGndFlag) = 0;

        //! Change model (for remote aircraft)
        virtual bool updateAircraftModel(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Simulation::CAircraftModel &model, const BlackMisc::CIdentifier &originator) = 0;

        //! Change network model (for remote aircraft)
        virtual bool updateAircraftNetworkModel(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Simulation::CAircraftModel &model, const BlackMisc::CIdentifier &originator) = 0;

        //! Change fast position updates
        virtual bool updateFastPositionEnabled(const BlackMisc::Aviation::CCallsign &callsign, bool enableFastPositionSending) = 0;

        //! Re-initialize all aircraft
        virtual int reInitializeAllAircraft() = 0;

        //! Get reverse lookup messages
        virtual BlackMisc::CStatusMessageList getReverseLookupMessages(const BlackMisc::Aviation::CCallsign &callsign) const = 0;

        //! Enabled reverse lookup logging?
        virtual BlackMisc::Simulation::ReverseLookupLogging isReverseLookupMessagesEnabled() const = 0;

        //! Enable reverse lookup logging
        virtual void enableReverseLookupMessages(BlackMisc::Simulation::ReverseLookupLogging enable) = 0;

        //! Get aircraft parts history
        virtual BlackMisc::CStatusMessageList getAircraftPartsHistory(const BlackMisc::Aviation::CCallsign &callsign) const = 0;

        //! Get remote aircraft parts
        virtual BlackMisc::Aviation::CAircraftPartsList getRemoteAircraftParts(const BlackMisc::Aviation::CCallsign &callsign) const = 0;

        //! Parts supported?
        virtual bool isRemoteAircraftSupportingParts(const BlackMisc::Aviation::CCallsign &callsign) const = 0;

        //! Is storing the aircraft parts history enabled?
        virtual bool isAircraftPartsHistoryEnabled() const = 0;

        //! Enable storing of aircraft parts
        virtual void enableAircraftPartsHistory(bool enabled) = 0;

        //! Version of the underlying library (e.g. VATLIB)
        virtual QString getLibraryInfo(bool detailed) const = 0;

        //! Statistics info
        virtual QString getNetworkStatistics(bool reset, const QString &separator) = 0;

        //! Statistics enable/disable
        virtual bool setNetworkStatisticsEnable(bool enabled) = 0;

        //! Network preset values
        virtual QStringList getNetworkPresetValues() const = 0;

        //! Partner callsign if any
        virtual BlackMisc::Aviation::CCallsign getPartnerCallsign() const = 0;

        // ------------------------ testing ------------------------

        //! Create dummy ATC stations for performance tests etc.
        virtual void testCreateDummyOnlineAtcStations(int number) = 0;

        //! Inject aircraft parts for testing
        virtual void testAddAircraftParts(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftParts &parts, bool incremental) = 0;

        //! Inject a text message as received
        virtual void testReceivedTextMessages(const BlackMisc::Network::CTextMessageList &textMessages) = 0;

        //! Inject an ATIS
        virtual void testReceivedAtisMessage(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CInformationMessage &msg) = 0;

        //! Request parts for callsign (from another client)
        virtual void testRequestAircraftConfig(const BlackMisc::Aviation::CCallsign &callsign) = 0;

        //! Add altitude offset for testing
        virtual bool testAddAltitudeOffset(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CLength &offset = BlackMisc::PhysicalQuantities::CLength::null()) = 0;

    public:
        //! Raw FSD message receiver functor
        using RawFsdMessageReceivedSlot = std::function<void(const BlackMisc::Network::CRawFsdMessage &)>;

        //! Connect to receive raw fsd messages
        virtual QMetaObject::Connection connectRawFsdMessageSignal(QObject *receiver, RawFsdMessageReceivedSlot rawFsdMessageReceivedSlot) = 0;

        //! Cmd.line arguments
        static const QList<QCommandLineOption> &getCmdLineOptions();

        //! Client id and key from cmd.line
        static bool getCmdLineClientIdAndKey(int &id, QString &key);

    protected:
        //! Constructor
        IContextNetwork(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime) : IContext(mode, runtime) {}
    };
} // ns
#endif // guard
