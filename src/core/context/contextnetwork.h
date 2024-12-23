// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_CONTEXT_CONTEXTNETWORK_H
#define SWIFT_CORE_CONTEXT_CONTEXTNETWORK_H

#include <functional>

#include <QCommandLineOption>
#include <QObject>
#include <QString>

#include "core/context/context.h"
#include "core/corefacade.h"
#include "core/corefacadeconfig.h"
#include "core/swiftcoreexport.h"
#include "misc/aviation/aircraftpartslist.h"
#include "misc/aviation/airporticaocode.h"
#include "misc/aviation/atcstation.h"
#include "misc/aviation/atcstationlist.h"
#include "misc/aviation/callsignset.h"
#include "misc/aviation/comsystem.h"
#include "misc/aviation/flightplan.h"
#include "misc/identifiable.h"
#include "misc/identifier.h"
#include "misc/network/clientlist.h"
#include "misc/network/connectionstatus.h"
#include "misc/network/loginmode.h"
#include "misc/network/server.h"
#include "misc/network/serverlist.h"
#include "misc/network/textmessagelist.h"
#include "misc/network/user.h"
#include "misc/network/userlist.h"
#include "misc/simulation/remoteaircraftprovider.h"
#include "misc/simulation/simulatedaircraftlist.h"
#include "misc/statusmessage.h"
#include "misc/weather/metar.h"

// clazy:excludeall=const-signal-or-slot

//! \ingroup dbus
//! DBus interface for context
#define SWIFT_CORE_CONTEXTNETWORK_INTERFACENAME "org.swift_project.swift_core.contextnetwork"

//! \ingroup dbus
//! DBus object path for context
#define SWIFT_CORE_CONTEXTNETWORK_OBJECTPATH "/network"

class QDBusConnection;
namespace swift::misc
{
    class CDBusServer;
    namespace aviation
    {
        class CAircraftParts;
        class CCallsign;
    } // namespace aviation
    namespace network
    {
        class CRawFsdMessage;
        class CTextMessage;
    } // namespace network
    namespace simulation
    {
        class CAircraftModel;
    }
} // namespace swift::misc
namespace swift::core::context
{
    //! Network context proxy
    class SWIFT_CORE_EXPORT IContextNetwork : public IContext
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", SWIFT_CORE_CONTEXTNETWORK_INTERFACENAME)

    public:
        //! DBus interface name
        static const QString &InterfaceName()
        {
            static QString s(SWIFT_CORE_CONTEXTNETWORK_INTERFACENAME);
            return s;
        }

        //! DBus object path
        static const QString &ObjectPath()
        {
            static QString s(SWIFT_CORE_CONTEXTNETWORK_OBJECTPATH);
            return s;
        }

        //! \copydoc IContext::getPathAndContextId()
        virtual QString getPathAndContextId() const override { return this->buildPathAndContextId(ObjectPath()); }

        //! Factory method
        static IContextNetwork *create(CCoreFacade *parent, CCoreFacadeConfig::ContextMode mode,
                                       swift::misc::CDBusServer *server, QDBusConnection &connection);

        //! Destructor
        virtual ~IContextNetwork() override {}

    signals:
        //! An aircraft disappeared
        void removedAircraft(const swift::misc::aviation::CCallsign &callsign);

        //! A new aircraft appeared
        void addedAircraft(const swift::misc::simulation::CSimulatedAircraft &remoteAircraft);

        //! Read for model matching
        void readyForModelMatching(const swift::misc::simulation::CSimulatedAircraft &renderedAircraft);

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
        void changedRemoteAircraftModel(const swift::misc::simulation::CSimulatedAircraft &aircraft,
                                        const swift::misc::CIdentifier &originator);

        //! Aircraft enabled / disabled
        //! \details All remote aircraft are stored in the network context. The aircraft can be enabled (for rendering)
        //! here
        //!          via \sa updateAircraftEnabled and then this signal is fired
        void changedRemoteAircraftEnabled(const swift::misc::simulation::CSimulatedAircraft &aircraft);

        //! Aircraft enabled / disabled
        void changedFastPositionUpdates(const swift::misc::simulation::CSimulatedAircraft &aircraft);

        //! Changed gnd. flag capability
        void changedGndFlagCapability(const swift::misc::simulation::CSimulatedAircraft &aircraft);

        //! ATC station disconnected
        void atcStationDisconnected(const swift::misc::aviation::CAtcStation &atcStation);

        //! User has been kicked from network
        void kicked(const QString &kickMessage);

        //! Network error
        void severeNetworkError(const QString &errorMessage);

        //! Mute request received
        void muteRequestReceived(bool mute);

        //! Connection status changed
        //! \sa IContextNetwork::connectedServerChanged
        void connectionStatusChanged(const swift::misc::network::CConnectionStatus &from,
                                     const swift::misc::network::CConnectionStatus &to);

        //! The connected server has been changed
        //! \remark can also be used to determine if the ecosystem has been changed
        //! \sa swift::misc::network::CServer::getEcosysstem
        void connectedServerChanged(const swift::misc::network::CServer &server);

        //! Text messages received (also private chat messages, radio channel messages)
        void textMessagesReceived(const swift::misc::network::CTextMessageList &textMessages);

        //! A supervisor text message was received
        void supervisorTextMessageReceived(const swift::misc::network::CTextMessage &message);

        //! Text message sent (by me)
        void textMessageSent(const swift::misc::network::CTextMessage &sentMessage);

    public slots:
        //! The ATC list with online ATC controllers
        virtual swift::misc::aviation::CAtcStationList getAtcStationsOnline(bool recalculateDistance) const = 0;

        //! The ATC list with online ATC controllers
        //! \remark recalculates distance and picks closest elements
        //! \remark sorted by distance, nearest first
        virtual swift::misc::aviation::CAtcStationList getClosestAtcStationsOnline(int number) const = 0;

        //! Aircraft list
        virtual swift::misc::simulation::CSimulatedAircraftList getAircraftInRange() const = 0;

        //! Aircraft callsigns
        virtual swift::misc::aviation::CCallsignSet getAircraftInRangeCallsigns() const = 0;

        //! Aircraft for given callsign
        virtual swift::misc::simulation::CSimulatedAircraft
        getAircraftInRangeForCallsign(const swift::misc::aviation::CCallsign &callsign) const = 0;

        //! Aircraft count
        virtual int getAircraftInRangeCount() const = 0;

        //! Aircraft in range
        virtual bool isAircraftInRange(const swift::misc::aviation::CCallsign &callsign) const = 0;

        //! Get METAR, if not available request it (code such as EDDF, KLAX)
        virtual swift::misc::weather::CMetar
        getMetarForAirport(const swift::misc::aviation::CAirportIcaoCode &airportIcaoCode) const = 0;

        //! Online station for callsign
        virtual swift::misc::aviation::CAtcStation
        getOnlineStationForCallsign(const swift::misc::aviation::CCallsign &callsign) const = 0;

        //! Online stations for frequency
        virtual swift::misc::aviation::CAtcStationList
        getOnlineStationsForFrequency(const swift::misc::physical_quantities::CFrequency &frequency) const = 0;

        //! Online station for callsign?
        virtual bool isOnlineStation(const swift::misc::aviation::CCallsign &callsign) const = 0;

        //! Get all users
        virtual swift::misc::network::CUserList getUsers() const = 0;

        //! Users for given callsigns, e.g. for voice room resolution
        virtual swift::misc::network::CUserList
        getUsersForCallsigns(const swift::misc::aviation::CCallsignSet &callsigns) const = 0;

        //! User for given callsign, e.g. for text messages
        virtual swift::misc::network::CUser
        getUserForCallsign(const swift::misc::aviation::CCallsign &callsign) const = 0;

        //! Information about other clients
        virtual swift::misc::network::CClientList getClients() const = 0;

        //! Clients for given callsign, e.g. to test/fetch direct aircraft model
        virtual swift::misc::network::CClientList
        getClientsForCallsigns(const swift::misc::aviation::CCallsignSet &callsigns) const = 0;

        //! Set client for given callsign
        virtual bool setOtherClient(const swift::misc::network::CClient &client) = 0;

        //! Known FSD servers, available when data file was first read
        virtual swift::misc::network::CServerList getVatsimFsdServers() const = 0;

        //! Callsigns enabled for fast position updates
        virtual void setFastPositionEnabledCallsigns(swift::misc::aviation::CCallsignSet &callsigns) = 0;

        //! Callsigns enabled for fast position updates
        virtual swift::misc::aviation::CCallsignSet getFastPositionEnabledCallsigns() const = 0;

        //! Connect to Network
        //! \return messages generated during connecting
        virtual swift::misc::CStatusMessage connectToNetwork(const swift::misc::network::CServer &server,
                                                             const QString &extraLiveryString, bool sendLivery,
                                                             const QString &extraModelString, bool sendModelString,
                                                             const swift::misc::aviation::CCallsign &partnerCallsign,
                                                             swift::misc::network::CLoginMode loginMode) = 0;

        //! Server which is connected, if not connected empty default object.
        virtual swift::misc::network::CServer getConnectedServer() const = 0;

        //! Login mode
        virtual swift::misc::network::CLoginMode getLoginMode() const = 0;

        //! Mode as string
        QString getLoginModeAsString() const { return this->getLoginMode().toQString(); }

        //! Disconnect from network
        //! \return messages generated during disconnecting
        virtual swift::misc::CStatusMessage disconnectFromNetwork() = 0;

        //! Network connected?
        virtual bool isConnected() const = 0;

        //! Text messages (radio and private chat messages)
        virtual void sendTextMessages(const swift::misc::network::CTextMessageList &textMessages) = 0;

        //! Send flight plan
        virtual void sendFlightPlan(const swift::misc::aviation::CFlightPlan &flightPlan) = 0;

        //! Load flight plan (from network)
        virtual swift::misc::aviation::CFlightPlan
        loadFlightPlanFromNetwork(const swift::misc::aviation::CCallsign &callsign) const = 0;

        //! Request data updates (pilot's frequencies, ATIS, ..)
        virtual void requestAircraftDataUpdates() = 0;

        //! Request ATIS updates (for all stations)
        virtual void requestAtisUpdates() = 0;

        //! Enable/disable rendering
        virtual bool updateAircraftEnabled(const swift::misc::aviation::CCallsign &callsign,
                                           bool enabledForRendering) = 0;

        //! Set the enabled flag without any further logic like sending signals
        virtual bool setAircraftEnabledFlag(const swift::misc::aviation::CCallsign &callsign,
                                            bool enabledForRendering) = 0;

        //! Enable/disable support of gnd. flag
        virtual bool updateAircraftSupportingGndFLag(const swift::misc::aviation::CCallsign &callsign,
                                                     bool supportGndFlag) = 0;

        //! Change model (for remote aircraft)
        virtual bool updateAircraftModel(const swift::misc::aviation::CCallsign &callsign,
                                         const swift::misc::simulation::CAircraftModel &model,
                                         const swift::misc::CIdentifier &originator) = 0;

        //! Change network model (for remote aircraft)
        virtual bool updateAircraftNetworkModel(const swift::misc::aviation::CCallsign &callsign,
                                                const swift::misc::simulation::CAircraftModel &model,
                                                const swift::misc::CIdentifier &originator) = 0;

        //! Change fast position updates
        virtual bool updateFastPositionEnabled(const swift::misc::aviation::CCallsign &callsign,
                                               bool enableFastPositionSending) = 0;

        //! Re-initialize all aircraft
        virtual int reInitializeAllAircraft() = 0;

        //! Get reverse lookup messages
        virtual swift::misc::CStatusMessageList
        getReverseLookupMessages(const swift::misc::aviation::CCallsign &callsign) const = 0;

        //! Enabled reverse lookup logging?
        virtual swift::misc::simulation::ReverseLookupLogging isReverseLookupMessagesEnabled() const = 0;

        //! Enable reverse lookup logging
        virtual void enableReverseLookupMessages(swift::misc::simulation::ReverseLookupLogging enable) = 0;

        //! Get aircraft parts history
        virtual swift::misc::CStatusMessageList
        getAircraftPartsHistory(const swift::misc::aviation::CCallsign &callsign) const = 0;

        //! Get remote aircraft parts
        virtual swift::misc::aviation::CAircraftPartsList
        getRemoteAircraftParts(const swift::misc::aviation::CCallsign &callsign) const = 0;

        //! Parts supported?
        virtual bool isRemoteAircraftSupportingParts(const swift::misc::aviation::CCallsign &callsign) const = 0;

        //! Is storing the aircraft parts history enabled?
        virtual bool isAircraftPartsHistoryEnabled() const = 0;

        //! Enable storing of aircraft parts
        virtual void enableAircraftPartsHistory(bool enabled) = 0;

        //! Statistics info
        virtual QString getNetworkStatistics(bool reset, const QString &separator) = 0;

        //! Statistics enable/disable
        virtual bool setNetworkStatisticsEnable(bool enabled) = 0;

        //! Network preset values
        virtual QStringList getNetworkPresetValues() const = 0;

        //! Partner callsign if any
        virtual swift::misc::aviation::CCallsign getPartnerCallsign() const = 0;

        // ------------------------ testing ------------------------

        //! Create dummy ATC stations for performance tests etc.
        virtual void testCreateDummyOnlineAtcStations(int number) = 0;

        //! Inject aircraft parts for testing
        virtual void testAddAircraftParts(const swift::misc::aviation::CCallsign &callsign,
                                          const swift::misc::aviation::CAircraftParts &parts, bool incremental) = 0;

        //! Inject a text message as received
        virtual void testReceivedTextMessages(const swift::misc::network::CTextMessageList &textMessages) = 0;

        //! Inject an ATIS
        virtual void testReceivedAtisMessage(const swift::misc::aviation::CCallsign &callsign,
                                             const swift::misc::aviation::CInformationMessage &msg) = 0;

        //! Request parts for callsign (from another client)
        virtual void testRequestAircraftConfig(const swift::misc::aviation::CCallsign &callsign) = 0;

        //! Add altitude offset for testing
        virtual bool testAddAltitudeOffset(const swift::misc::aviation::CCallsign &callsign,
                                           const swift::misc::physical_quantities::CLength &offset =
                                               swift::misc::physical_quantities::CLength::null()) = 0;

    public:
        //! Raw FSD message receiver functor
        using RawFsdMessageReceivedSlot = std::function<void(const swift::misc::network::CRawFsdMessage &)>;

        //! Connect to receive raw fsd messages
        virtual QMetaObject::Connection
        connectRawFsdMessageSignal(QObject *receiver, RawFsdMessageReceivedSlot rawFsdMessageReceivedSlot) = 0;

        //! Cmd.line arguments
        static const QList<QCommandLineOption> &getCmdLineOptions();

#ifdef SWIFT_VATSIM_SUPPORT
        //! Client id and key from cmd.line
        static bool getCmdLineClientIdAndKey(int &id, QString &key);
#endif

    protected:
        //! Constructor
        IContextNetwork(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime) : IContext(mode, runtime) {}
    };
} // namespace swift::core::context
#endif // SWIFT_CORE_CONTEXT_CONTEXTNETWORK_H
