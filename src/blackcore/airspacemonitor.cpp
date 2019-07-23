/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/vatsim/networkvatlib.h"
#include "blackcore/vatsim/vatsimbookingreader.h"
#include "blackcore/vatsim/vatsimdatafilereader.h"
#include "blackcore/airspaceanalyzer.h"
#include "blackcore/airspacemonitor.h"
#include "blackcore/aircraftmatcher.h"
#include "blackcore/application.h"
#include "blackcore/webdataservices.h"
#include "blackcore/context/contextnetwork.h"
#include "blackmisc/aviation/aircraftparts.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/aviation/modulator.h"
#include "blackmisc/aviation/transponder.h"
#include "blackmisc/aviation/logutils.h"
#include "blackmisc/audio/voiceroom.h"
#include "blackmisc/geo/elevationplane.h"
#include "blackmisc/network/user.h"
#include "blackmisc/network/voicecapabilities.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/test/testing.h"
#include "blackmisc/compare.h"
#include "blackmisc/iterator.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/propertyindexvariantmap.h"
#include "blackmisc/range.h"
#include "blackmisc/sequence.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/threadutils.h"
#include "blackmisc/variant.h"
#include "blackmisc/verify.h"
#include "blackmisc/worker.h"
#include "blackconfig/buildconfig.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QEventLoop>
#include <QReadLocker>
#include <QThread>
#include <QTime>
#include <QVariant>
#include <QWriteLocker>
#include <Qt>

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Test;
using namespace BlackMisc::Json;
using namespace BlackMisc::Network;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Weather;
using namespace BlackCore::Vatsim;

namespace BlackCore
{
    CAirspaceMonitor::CAirspaceMonitor(IOwnAircraftProvider *ownAircraftProvider, IAircraftModelSetProvider *modelSetProvider, INetwork *network, QObject *parent)
        : CRemoteAircraftProvider(parent),
          COwnAircraftAware(ownAircraftProvider),
          CAircraftModelSetAware(modelSetProvider),
          m_network(network),
          m_analyzer(new CAirspaceAnalyzer(ownAircraftProvider, network, this))
    {
        this->setObjectName("CAirspaceMonitor");
        this->enableReverseLookupMessages(sApp->isDeveloperFlagSet() || CBuildConfig::isLocalDeveloperDebugBuild() ? RevLogEnabled : RevLogEnabledSimplified);

        connect(m_network, &INetwork::atcPositionUpdate,             this, &CAirspaceMonitor::onAtcPositionUpdate);
        connect(m_network, &INetwork::atisReplyReceived,             this, &CAirspaceMonitor::onAtisReceived);
        connect(m_network, &INetwork::atisVoiceRoomReplyReceived,    this, &CAirspaceMonitor::onAtisVoiceRoomReceived);
        connect(m_network, &INetwork::atisLogoffTimeReplyReceived,   this, &CAirspaceMonitor::onAtisLogoffTimeReceived);
        connect(m_network, &INetwork::flightPlanReplyReceived,       this, &CAirspaceMonitor::onFlightPlanReceived);
        connect(m_network, &INetwork::realNameReplyReceived,         this, &CAirspaceMonitor::onRealNameReplyReceived);
        connect(m_network, &INetwork::icaoCodesReplyReceived,        this, &CAirspaceMonitor::onIcaoCodesReceived);
        connect(m_network, &INetwork::pilotDisconnected,             this, &CAirspaceMonitor::onPilotDisconnected);
        connect(m_network, &INetwork::atcDisconnected,               this, &CAirspaceMonitor::onAtcControllerDisconnected);
        connect(m_network, &INetwork::aircraftPositionUpdate,        this, &CAirspaceMonitor::onAircraftUpdateReceived);
        connect(m_network, &INetwork::aircraftInterimPositionUpdate, this, &CAirspaceMonitor::onAircraftInterimUpdateReceived);
        connect(m_network, &INetwork::frequencyReplyReceived,        this, &CAirspaceMonitor::onFrequencyReceived);
        connect(m_network, &INetwork::capabilitiesReplyReceived,     this, &CAirspaceMonitor::onCapabilitiesReplyReceived);
        connect(m_network, &INetwork::customFSInnPacketReceived,     this, &CAirspaceMonitor::onCustomFSInnPacketReceived);
        connect(m_network, &INetwork::serverReplyReceived,           this, &CAirspaceMonitor::onServerReplyReceived);
        connect(m_network, &INetwork::aircraftConfigPacketReceived,  this, &CAirspaceMonitor::onAircraftConfigReceived);
        connect(m_network, &INetwork::connectionStatusChanged,       this, &CAirspaceMonitor::onConnectionStatusChanged);

        // AutoConnection: this should also avoid race conditions by updating the bookings
        Q_ASSERT_X(sApp && sApp->getWebDataServices(), Q_FUNC_INFO, "Missing data reader");

        // optional readers
        if (sApp->getWebDataServices()->getBookingReader())
        {
            connect(sApp->getWebDataServices()->getBookingReader(), &CVatsimBookingReader::atcBookingsRead,          this, &CAirspaceMonitor::onReceivedAtcBookings);
            connect(sApp->getWebDataServices()->getBookingReader(), &CVatsimBookingReader::atcBookingsReadUnchanged, this, &CAirspaceMonitor::onReadUnchangedAtcBookings);
        }

        if (this->supportsVatsimDataFile())
        {
            connect(sApp->getWebDataServices()->getVatsimDataFileReader(), &CVatsimDataFileReader::dataFileRead, this, &CAirspaceMonitor::onReceivedVatsimDataFile);
        }

        // Force snapshot in the main event loop
        connect(m_analyzer, &CAirspaceAnalyzer::airspaceAircraftSnapshot, this, &CAirspaceMonitor::airspaceAircraftSnapshot, Qt::QueuedConnection);

        // Analyzer
        connect(m_analyzer, &CAirspaceAnalyzer::timeoutAircraft, this, &CAirspaceMonitor::onPilotDisconnected,         Qt::QueuedConnection);
        connect(m_analyzer, &CAirspaceAnalyzer::timeoutAtc,      this, &CAirspaceMonitor::onAtcControllerDisconnected, Qt::QueuedConnection);

        // timer
        connect(&m_processTimer, &QTimer::timeout, this, &CAirspaceMonitor::process);
        m_processTimer.start(ProcessInterval);
    }

    bool CAirspaceMonitor::updateFastPositionEnabled(const CCallsign &callsign, bool enableFastPositonUpdates)
    {
        const bool r = CRemoteAircraftProvider::updateFastPositionEnabled(callsign, enableFastPositonUpdates);
        if (m_network && sApp && !sApp->isShuttingDown())
        {
            // thread safe update of m_network
            const QPointer<CAirspaceMonitor> myself(this);
            QTimer::singleShot(0, m_network, [ = ]
            {
                if (!myself) { return; }
                if (m_network)
                {
                    if (enableFastPositonUpdates)
                    {
                        m_network->addInterimPositionReceiver(callsign);
                    }
                    else
                    {
                        m_network->removeInterimPositionReceiver(callsign);
                    }
                }
            });
        }
        return r;
    }

    const CLogCategoryList &CAirspaceMonitor::getLogCategories()
    {
        static const CLogCategoryList cats { CLogCategory::matching(), CLogCategory::network() };
        return cats;
    }

    CAirspaceAircraftSnapshot CAirspaceMonitor::getLatestAirspaceAircraftSnapshot() const
    {
        Q_ASSERT_X(m_analyzer, Q_FUNC_INFO, "No analyzer");
        return m_analyzer->getLatestAirspaceAircraftSnapshot();
    }

    CFlightPlan CAirspaceMonitor::loadFlightPlanFromNetwork(const CCallsign &callsign)
    {
        CFlightPlan plan;
        QPointer<CAirspaceMonitor> myself(this);

        // use cache, but not for own callsign (always reload)
        if (m_flightPlanCache.contains(callsign)) { plan = m_flightPlanCache[callsign]; }
        const bool ownAircraft = this->getOwnCallsign() == callsign;
        if (ownAircraft || !plan.wasSentOrLoaded() || plan.timeDiffSentOrLoadedMs() > 30 * 1000)
        {
            // outdated, or not in cache at all, or NOT own aircraft
            plan = CFlightPlan(); // reset
            m_flightPlanCache.remove(callsign); // loading FP from network
            m_network->sendFlightPlanQuery(callsign);

            // with this little trick we try to make an asynchronous signal / slot based approach
            // a synchronous return value
            const QTime waitForFlightPlan = QTime::currentTime().addMSecs(1500);
            while (sApp && !sApp->isShuttingDown() && QTime::currentTime() < waitForFlightPlan)
            {
                // process some other events and hope network answer is received already
                // CEventLoop::processEventsUntil cannot be used, as a received flight plan might be for another callsign
                sApp->processEventsFor(100);
                if (!myself || !sApp || sApp->isShuttingDown()) { return CFlightPlan(); }
                if (m_flightPlanCache.contains(callsign))
                {
                    plan = m_flightPlanCache[callsign];
                    break;
                }
            }
        }
        return plan;
    }

    CFlightPlanRemarks CAirspaceMonitor::tryToGetFlightPlanRemarks(const CCallsign &callsign) const
    {
        if (callsign.isEmpty()) { return CFlightPlanRemarks(); }

        // full flight plan's remarks
        if (m_flightPlanCache.contains(callsign)) { return m_flightPlanCache[callsign].getFlightPlanRemarks(); }

        // remarks only
        if (this->supportsVatsimDataFile()) { return sApp->getWebDataServices()->getVatsimDataFileReader()->getFlightPlanRemarksForCallsign(callsign); }

        // unsupported
        return CFlightPlanRemarks();
    }

    CAtcStationList CAirspaceMonitor::getAtcStationsOnlineRecalculated()
    {
        m_atcStationsOnline.calculcateAndUpdateRelativeDistanceAndBearing(this->getOwnAircraftSituation());
        return m_atcStationsOnline;
    }

    CAtcStationList CAirspaceMonitor::getAtcStationsBookedRecalculated()
    {
        m_atcStationsBooked.calculcateAndUpdateRelativeDistanceAndBearing(this->getOwnAircraftSituation());
        return m_atcStationsBooked;
    }

    CUserList CAirspaceMonitor::getUsers() const
    {
        CUserList users;
        for (const CAtcStation &station : m_atcStationsOnline)
        {
            CUser user = station.getController();
            if (!user.hasCallsign()) { user.setCallsign(station.getCallsign()); }
            users.push_back(user);
        }
        for (const CSimulatedAircraft &aircraft : this->getAircraftInRange())
        {
            CUser user = aircraft.getPilot();
            if (!user.hasCallsign()) { user.setCallsign(aircraft.getCallsign()); }
            users.push_back(user);
        }
        return users;
    }

    CUserList CAirspaceMonitor::getUsersForCallsigns(const CCallsignSet &callsigns) const
    {
        CUserList users;
        if (callsigns.isEmpty()) { return users; }
        CCallsignSet searchList(callsigns);

        // myself, which is not in the lists below
        const CSimulatedAircraft myAircraft(getOwnAircraft());
        if (!myAircraft.getCallsign().isEmpty() && searchList.contains(myAircraft.getCallsign()))
        {
            searchList.remove(myAircraft.getCallsign());
            users.push_back(myAircraft.getPilot());
        }

        // do aircraft first, this will handle most callsigns
        for (const CSimulatedAircraft &aircraft : this->getAircraftInRange())
        {
            if (searchList.isEmpty()) { break; }
            const CCallsign callsign = aircraft.getCallsign();
            if (searchList.contains(callsign))
            {
                const CUser user = aircraft.getPilot();
                users.push_back(user);
                searchList.remove(callsign);
            }
        }

        for (const CAtcStation &station : m_atcStationsOnline)
        {
            if (searchList.isEmpty()) { break; }
            const CCallsign callsign = station.getCallsign();
            if (searchList.contains(callsign))
            {
                const CUser user = station.getController();
                users.push_back(user);
                searchList.remove(callsign);
            }
        }

        // we might have unresolved callsigns
        // those are the ones not in range
        for (const CCallsign &callsign : as_const(searchList))
        {
            const CUserList usersByCallsign = sApp->getWebDataServices()->getUsersForCallsign(callsign);
            if (usersByCallsign.isEmpty())
            {
                const CUser user(callsign);
                users.push_back(user);
            }
            else
            {
                users.push_back(usersByCallsign[0]);
            }
        }
        return users;
    }

    CAtcStation CAirspaceMonitor::getAtcStationForComUnit(const CComSystem &comSystem) const
    {
        CAtcStation station;
        CAtcStationList stations = m_atcStationsOnline.findIfComUnitTunedInChannelSpacing(comSystem);
        if (stations.isEmpty()) { return station; }
        stations.sortByDistanceToReferencePosition();
        return stations.front();
    }

    void CAirspaceMonitor::requestAircraftDataUpdates()
    {
        if (!this->isConnectedAndNotShuttingDown()) { return; }
        const CSimulatedAircraftList aircraftInRange(this->getAircraftInRange());
        for (const CSimulatedAircraft &aircraft : aircraftInRange)
        {
            // staggered version
            const CCallsign cs(aircraft.getCallsign());
            if (!m_queryPilot.contains(cs))
            {
                m_queryPilot.enqueue(aircraft.getCallsign());
            }

            /**
            m_network->sendFrequencyQuery(cs);
            if (!aircraft.hasAircraftDesignator())
            {
                m_network->sendIcaoCodesQuery(cs);
            }
            **/
        }
    }

    void CAirspaceMonitor::requestAtisUpdates()
    {
        if (!this->isConnectedAndNotShuttingDown()) { return; }
        const CAtcStationList stations(this->getAtcStationsOnline());
        for (const CAtcStation &station : stations)
        {
            const CCallsign cs = station.getCallsign();

            // changed to staggered version
            // m_network->sendAtisQuery(cs); // for each online station
            if (!m_queryAtis.contains(cs))
            {
                m_queryAtis.enqueue(cs);
            }
        }
    }

    void CAirspaceMonitor::requestAtcBookingsUpdate()
    {
        if (!sApp || sApp->isShuttingDown()) { return; }
        sApp->getWebDataServices()->readInBackground(BlackMisc::Network::CEntityFlags::BookingEntity);
        m_bookingsRequested = true;
    }

    bool CAirspaceMonitor::enableAnalyzer(bool enable)
    {
        if (!this->analyzer()) { return false; }
        this->analyzer()->setEnabled(enable);
        return true;
    }

    void CAirspaceMonitor::testCreateDummyOnlineAtcStations(int number)
    {
        if (number < 1) { return; }
        m_atcStationsOnline.push_back(CTesting::createAtcStations(number));
        emit this->changedAtcStationsOnline();
    }

    void CAirspaceMonitor::testAddAircraftParts(const CCallsign &callsign, const CAircraftParts &parts, bool incremental)
    {
        this->onAircraftConfigReceived(callsign,
                                       incremental ? parts.toIncrementalJson() : parts.toFullJson(),
                                       5000);
    }

    const QString &CAirspaceMonitor::enumFlagToString(CAirspaceMonitor::MatchingReadinessFlag r)
    {
        static const QString nr("not ready");
        static const QString icao("rec. ICAO");
        static const QString fsinn("rec. FsInn");
        static const QString ready("ready sent");
        static const QString rec("recursive");
        switch (r)
        {
        case NotReady:             return nr;
        case ReceivedIcaoCodes:    return icao;
        case ReceivedFsInnPacket:  return fsinn;
        case ReadyForMatchingSent: return ready;
        case RecursiveCall:        return rec;
        default: break;
        }
        static const QString unknown("????");
        return unknown;
    }

    QString CAirspaceMonitor::enumToString(MatchingReadiness r)
    {
        QStringList s;
        if (r.testFlag(NotReady))             { s << enumFlagToString(NotReady); }
        if (r.testFlag(ReceivedIcaoCodes))    { s << enumFlagToString(ReceivedIcaoCodes); }
        if (r.testFlag(ReceivedFsInnPacket))  { s << enumFlagToString(ReceivedFsInnPacket); }
        if (r.testFlag(ReadyForMatchingSent)) { s << enumFlagToString(ReadyForMatchingSent); }
        if (r.testFlag(RecursiveCall))        { s << enumFlagToString(RecursiveCall); }
        return s.join(", ");
    }

    void CAirspaceMonitor::process()
    {
        if (this->isConnectedAndNotShuttingDown())
        {
            // only send one
            const bool send = this->sendNextStaggeredAtisQuery();
            if (!send) { this->sendNextStaggeredPilotDataQuery(); }
        }
    }

    void CAirspaceMonitor::clear()
    {
        m_flightPlanCache.clear();
        m_tempFsInnPackets.clear();
        m_readiness.clear();
        this->removeAllOnlineAtcStations();
        this->removeAllAircraft();
        this->clearClients();
    }

    void CAirspaceMonitor::gracefulShutdown()
    {
        if (m_analyzer) { m_analyzer->setEnabled(false); }
        QObject::disconnect(this);
    }

    int CAirspaceMonitor::reInitializeAllAircraft()
    {
        const CSimulatedAircraftList aircraft = this->getAircraftInRange();
        CRemoteAircraftProvider::removeAllAircraft();
        this->asyncAddNewAircraftInRange(aircraft, true);
        return aircraft.size();
    }

    void CAirspaceMonitor::onRealNameReplyReceived(const CCallsign &callsign, const QString &realname)
    {
        if (!this->isConnectedAndNotShuttingDown() || realname.isEmpty()) { return; }
        int wasAtc = false;

        if (callsign.hasSuffix())
        {
            // very likely and ATC callsign
            const CPropertyIndexVariantMap vm = CPropertyIndexVariantMap({ CAtcStation::IndexController, CUser::IndexRealName }, realname);
            const int c1 = this->updateOnlineStation(callsign, vm, false, true);
            const int c2 = this->updateBookedStation(callsign, vm, false, true);
            wasAtc = c1 > 0 || c2 > 0;
        }

        if (!wasAtc)
        {
            const CPropertyIndexVariantMap vm = CPropertyIndexVariantMap({CSimulatedAircraft::IndexPilot, CUser::IndexRealName}, realname);
            this->updateAircraftInRange(callsign, vm);
        }

        // Client
        if (!sApp || sApp->isShuttingDown() || !sApp->getWebDataServices()) { return; }
        const CVoiceCapabilities voiceCaps = sApp->getWebDataServices()->getVoiceCapabilityForCallsign(callsign);
        CPropertyIndexVariantMap vm = CPropertyIndexVariantMap({ CClient::IndexUser, CUser::IndexRealName }, realname);
        vm.addValue({ CClient::IndexVoiceCapabilities }, voiceCaps);
        this->updateOrAddClient(callsign, vm, false);
    }

    void CAirspaceMonitor::onCapabilitiesReplyReceived(const CCallsign &callsign, int clientCaps)
    {
        if (!this->isConnectedAndNotShuttingDown() || callsign.isEmpty()) { return; }
        const CClient::Capabilities caps = static_cast<CClient::Capabilities>(clientCaps);
        const CVoiceCapabilities voiceCaps = sApp->getWebDataServices()->getVoiceCapabilityForCallsign(callsign);
        CPropertyIndexVariantMap vm(CClient::IndexCapabilities, CVariant::from(clientCaps));
        vm.addValue({CClient::IndexVoiceCapabilities}, voiceCaps);
        this->updateOrAddClient(callsign, vm, false);

        // for aircraft parts
        if (caps.testFlag(CClient::FsdWithAircraftConfig)) { m_network->sendAircraftConfigQuery(callsign); }
    }

    void CAirspaceMonitor::onServerReplyReceived(const CCallsign &callsign, const QString &server)
    {
        if (!this->isConnectedAndNotShuttingDown() || callsign.isEmpty() || server.isEmpty()) { return; }
        const CPropertyIndexVariantMap vm(CClient::IndexServer, server);
        this->updateOrAddClient(callsign, vm);
    }

    void CAirspaceMonitor::onFlightPlanReceived(const CCallsign &callsign, const CFlightPlan &flightPlan)
    {
        if (!this->isConnectedAndNotShuttingDown() || callsign.isEmpty()) { return; }
        CFlightPlan plan(flightPlan);
        plan.setWhenLastSentOrLoaded(QDateTime::currentDateTimeUtc());
        m_flightPlanCache.insert(callsign, plan);
    }

    void CAirspaceMonitor::removeAllOnlineAtcStations()
    {
        m_atcStationsOnline.clear();
        m_queryAtis.clear();
    }

    void CAirspaceMonitor::removeAllAircraft()
    {
        CRemoteAircraftProvider::removeAllAircraft();

        // non thread safe parts
        m_flightPlanCache.clear();
        m_readiness.clear();
        m_queryPilot.clear();
    }

    void CAirspaceMonitor::removeFromAircraftCachesAndLogs(const CCallsign &callsign)
    {
        if (callsign.isEmpty()) { return; }
        m_flightPlanCache.remove(callsign);
        m_readiness.remove(callsign);
        this->removeReverseLookupMessages(callsign);
    }

    void CAirspaceMonitor::onReceivedAtcBookings(const CAtcStationList &bookedStations)
    {
        Q_ASSERT(CThreadUtils::isCurrentThreadObjectThread(this));
        if (bookedStations.isEmpty())
        {
            m_atcStationsBooked.clear();
        }
        else
        {
            CAtcStationList newBookedStations(bookedStations); // modifyable copy
            for (CAtcStation &bookedStation : newBookedStations)
            {
                // exchange booking and online data, both sides are updated
                m_atcStationsOnline.synchronizeWithBookedStation(bookedStation);
            }
            m_atcStationsBooked = newBookedStations;
        }
        m_bookingsRequested = false; // we already emit here
        m_atcStationsBooked.calculcateAndUpdateRelativeDistanceAndBearing(this->getOwnAircraftSituation());

        emit this->changedAtcStationsBooked(); // all booked stations reloaded
    }

    void CAirspaceMonitor::onReadUnchangedAtcBookings()
    {
        if (!m_bookingsRequested) { return; }
        m_bookingsRequested = false;
        emit this->changedAtcStationsBooked(); // treat as stations were changed
    }

    void CAirspaceMonitor::onReceivedVatsimDataFile()
    {
        Q_ASSERT(CThreadUtils::isCurrentThreadObjectThread(this));
        if (!sApp || sApp->isShuttingDown() || !sApp->getWebDataServices()) { return; }
        CClientList clients(this->getClients()); // copy
        bool changed = false;
        for (auto client = clients.begin(); client != clients.end(); ++client)
        {
            if (client->hasSpecifiedVoiceCapabilities()) { continue; } // we already have voice caps
            const CVoiceCapabilities vc = sApp->getWebDataServices()->getVoiceCapabilityForCallsign(client->getCallsign());
            if (vc.isUnknown()) { continue; }
            changed = true;
            client->setVoiceCapabilities(vc);
        }
        if (!changed) { return; }
        this->setClients(clients);
    }

    CAirspaceMonitor::Readiness &CAirspaceMonitor::addMatchingReadinessFlag(const CCallsign &callsign, CAirspaceMonitor::MatchingReadinessFlag mrf)
    {
        Readiness &readiness = m_readiness[callsign].addFlag(mrf);
        return readiness;
    }

    void CAirspaceMonitor::sendReadyForModelMatching(const CCallsign &callsign, MatchingReadinessFlag rf)
    {
        if (!this->isConnectedAndNotShuttingDown()) { return; }
        Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "missing callsign");

        // times
        constexpr qint64 CheckAgainMs    = 2000;
        constexpr qint64 MaxAgeMs        = CheckAgainMs * 3;
        constexpr qint64 MaxAgeThreshold = CheckAgainMs * 10;

        // set flag and init ts
        Readiness &readiness = this->addMatchingReadinessFlag(callsign, rf);

        // skip if already sent in the last x seconds
        const qint64 ageMs = readiness.getAgeMs();
        if (readiness.wasMatchingSent() && readiness.getAgeMs() < MaxAgeThreshold) { return; }

        // checking for min. situations ensures the aircraft is stable, can be interpolated ...
        const CSimulatedAircraft remoteAircraft = this->getAircraftInRangeForCallsign(callsign);
        const bool validRemoteCs = remoteAircraft.hasValidCallsign();
        const bool minSituations = this->remoteAircraftSituationsCount(callsign) > 1;
        const bool complete = validRemoteCs &&
                              minSituations && (
                                  readiness.receivedAll() ||
                                  (remoteAircraft.getModel().getModelType() == CAircraftModel::TypeFSInnData) || // here we know we have all data
                                  (remoteAircraft.hasModelString()) // we cannot expect more info
                              );

        const ReverseLookupLogging revLogEnabled = this->whatToReverseLog();
        if (validRemoteCs && ageMs <= MaxAgeMs && !complete)
        {
            static const QString ws("Wait for further data, '%1' age: %2ms ts: %3");
            static const QString format("hh:mm:ss.zzz");
            if (!revLogEnabled.testFlag(RevLogSimplifiedInfo)) { this->addReverseLookupMessage(callsign, ws.arg(readiness.toQString()).arg(ageMs).arg(QDateTime::currentDateTimeUtc().toString(format))); }

            const QPointer<CAirspaceMonitor> myself(this);
            QTimer::singleShot(CheckAgainMs, this, [ = ]()
            {
                if (!myself || !sApp || sApp->isShuttingDown()) { return; }
                if (!this->isAircraftInRange(callsign))
                {
                    const CStatusMessage m = CLogUtilities::logMessage(callsign, "No longer in range", CAirspaceMonitor::getLogCategories());
                    this->addReverseLookupMessage(callsign, m);
                    return;
                }
                if (rf != ReceivedFsInnPacket)
                {
                    // here we call recursively like an FsInn packet was received
                    if (this->recallFsInnPacket(callsign)) { return; }
                }

                // check again
                this->sendReadyForModelMatching(callsign, RecursiveCall); // recursively
            });

            // end as we will call again in some time
            return;

        } // not yet complete

        // some checks for special conditions, e.g. logout -> empty list, but still signals pending
        if (validRemoteCs)
        {
            static const QString readyForMatching("Ready for matching callsign '%1' with model type '%2', '%3'");

            readiness.setFlag(ReadyForMatchingSent); // stored as readiness is reference

            const QString readyMsg = readyForMatching.arg(callsign.toQString(), remoteAircraft.getModel().getModelTypeAsString(), readiness.toQString());
            const CStatusMessage m = CLogUtilities::logMessage(callsign, readyMsg, getLogCategories());
            this->addReverseLookupMessage(callsign, m);

            emit this->readyForModelMatching(remoteAircraft);
        }
        else
        {
            const CStatusMessage m = CLogUtilities::logMessage(callsign, "Ignoring this aircraft, not found in range list, disconnected, or no callsign", CAirspaceMonitor::getLogCategories(), CStatusMessage::SeverityWarning);
            this->addReverseLookupMessage(callsign, m);
            m_readiness.remove(callsign);
        }
    }

    void CAirspaceMonitor::onAtcPositionUpdate(const CCallsign &callsign, const CFrequency &frequency, const CCoordinateGeodetic &position, const BlackMisc::PhysicalQuantities::CLength &range)
    {
        Q_ASSERT_X(CThreadUtils::isCurrentThreadObjectThread(this), Q_FUNC_INFO, "wrong thread");
        if (!this->isConnectedAndNotShuttingDown()) { return; }

        const CAtcStationList stationsWithCallsign = m_atcStationsOnline.findByCallsign(callsign);
        if (stationsWithCallsign.isEmpty())
        {
            // new station, init with data from data file
            CAtcStation station(sApp->getWebDataServices()->getAtcStationsForCallsign(callsign).frontOrDefault());
            station.setCallsign(callsign);
            station.setRange(range);
            station.setFrequency(frequency);
            station.setPosition(position);
            station.setOnline(true);
            station.calculcateAndUpdateRelativeDistanceAndBearing(this->getOwnAircraftPosition());

            // sync with bookings
            if (m_atcStationsBooked.containsCallsign(callsign))
            {
                CAtcStation bookedStation(m_atcStationsBooked.findFirstByCallsign(callsign));
                station.synchronizeWithBookedStation(bookedStation); // both will be aligned
                m_atcStationsBooked.replaceIf(&CAtcStation::getCallsign, callsign, bookedStation);
            }

            m_atcStationsOnline.push_back(station);

            // subsequent queries
            this->sendInitialAtcQueries(callsign);

            // update distances
            m_atcStationsOnline.calculcateAndUpdateRelativeDistanceAndBearing(this->getOwnAircraftSituation());

            emit this->changedAtcStationsOnline();
            // Remark: this->changedAtcStationOnlineConnectionStatus
            // will be triggered in onAtisVoiceRoomReceived
        }
        else
        {
            // update
            CPropertyIndexVariantMap vm;
            vm.addValue(CAtcStation::IndexFrequency, frequency);
            vm.addValue(CAtcStation::IndexPosition, position);
            vm.addValue(CAtcStation::IndexRange, range);
            const int changed = m_atcStationsOnline.applyIfCallsign(callsign, vm, true);
            if (changed > 0) { emit this->changedAtcStationsOnline(); }
        }
    }

    void CAirspaceMonitor::onAtcControllerDisconnected(const CCallsign &callsign)
    {
        Q_ASSERT(CThreadUtils::isCurrentThreadObjectThread(this));
        if (!this->isConnectedAndNotShuttingDown()) { return; }

        this->removeClient(callsign);
        if (m_atcStationsOnline.containsCallsign(callsign))
        {
            const CAtcStation removedStation = m_atcStationsOnline.findFirstByCallsign(callsign);
            m_atcStationsOnline.removeByCallsign(callsign);
            emit this->changedAtcStationsOnline();
            emit this->changedAtcStationOnlineConnectionStatus(removedStation, false);
        }

        // booked
        this->updateBookedStation(callsign, CPropertyIndexVariantMap(CAtcStation::IndexIsOnline, CVariant::from(false)), true, false);
    }

    void CAirspaceMonitor::onAtisReceived(const CCallsign &callsign, const CInformationMessage &atisMessage)
    {
        Q_ASSERT(CThreadUtils::isCurrentThreadObjectThread(this));
        if (!this->isConnectedAndNotShuttingDown() || callsign.isEmpty()) return;
        const bool changedAtis = m_atcStationsOnline.updateIfMessageChanged(atisMessage, callsign, true);

        // receiving an ATIS means station is online, update in bookings
        m_atcStationsBooked.setOnline(callsign, true);

        // signal
        if (changedAtis) { emit this->changedAtisReceived(callsign); }
    }

    void CAirspaceMonitor::onAtisVoiceRoomReceived(const CCallsign &callsign, const QString &url)
    {
        Q_ASSERT(CThreadUtils::isCurrentThreadObjectThread(this));
        if (!this->isConnectedAndNotShuttingDown()) { return; }

        // URL
        const QString trimmedUrl = url.trimmed();
        CPropertyIndexVariantMap vm({ CAtcStation::IndexVoiceRoom, CVoiceRoom::IndexUrl }, trimmedUrl);
        const int changedOnline = this->updateOnlineStation(callsign, vm, true, true);
        if (changedOnline < 1) { return; }

        Q_ASSERT(changedOnline == 1);
        const CAtcStation station = m_atcStationsOnline.findFirstByCallsign(callsign);
        emit this->changedAtcStationOnlineConnectionStatus(station, true); // send when voice room url is available

        vm.addValue(CAtcStation::IndexIsOnline, true); // with voice room ATC is online
        this->updateBookedStation(callsign, vm);

        // receiving voice room means ATC has voice
        if (!trimmedUrl.isEmpty())
        {
            vm = CPropertyIndexVariantMap(CClient::IndexVoiceCapabilities, CVariant::from(CVoiceCapabilities::fromVoiceCapabilities(CVoiceCapabilities::Voice)));
            this->updateOrAddClient(callsign, vm, false);
        }
    }

    void CAirspaceMonitor::onAtisLogoffTimeReceived(const CCallsign &callsign, const QString &zuluTime)
    {
        Q_ASSERT(CThreadUtils::isCurrentThreadObjectThread(this));
        if (!this->isConnectedAndNotShuttingDown()) { return; }

        if (zuluTime.length() == 4)
        {
            // Logic to set logoff time
            bool ok;
            const int h = zuluTime.leftRef(2).toInt(&ok);
            if (!ok) { return; }
            const int m = zuluTime.rightRef(2).toInt(&ok);
            if (!ok) { return; }
            QDateTime logoffDateTime = QDateTime::currentDateTimeUtc();
            logoffDateTime.setTime(QTime(h, m));

            const CPropertyIndexVariantMap vm(CAtcStation::IndexBookedUntil, CVariant(logoffDateTime));
            this->updateOnlineStation(callsign, vm);
            this->updateBookedStation(callsign, vm);
        }
    }

    void CAirspaceMonitor::onCustomFSInnPacketReceived(const CCallsign &callsign, const QString &airlineIcaoDesignator, const QString &aircraftIcaoDesignator, const QString &combinedAircraftType, const QString &modelString)
    {
        // it can happen this is called before any queries
        // ES sends FsInn packets for callsigns such as ACCGER1, which are hard to distinguish
        // 1) checking if they are already in the list checks again ATC position which is safe
        // 2) the ATC alike callsign check is guessing
        Q_ASSERT_X(CThreadUtils::isCurrentThreadObjectThread(this), Q_FUNC_INFO, "not in main thread");
        if (!callsign.isValid()) { return; } // aircraft OBS, other invalid callsigns
        if (!this->isConnectedAndNotShuttingDown()) { return; }

        const bool isAircraft = this->isAircraftInRange(callsign);
        const bool isAtc = m_atcStationsOnline.containsCallsign(callsign);
        if (!isAircraft && !isAtc)
        {
            // we have no idea what we are dealing with, so we store it
            const FsInnPacket fsInn(aircraftIcaoDesignator, airlineIcaoDesignator, combinedAircraftType, modelString);
            m_tempFsInnPackets[callsign] = fsInn;
            return;
        }

        // Request of other client, I can get the other's model from that
        // we do not ignore model string here
        const CPropertyIndexVariantMap vm(CClient::IndexModelString, modelString);
        this->updateOrAddClient(callsign, vm);

        if (isAircraft)
        {
            this->addMatchingReadinessFlag(callsign, ReceivedFsInnPacket); // in any case we did receive it

            const ReverseLookupLogging reverseLookupEnabled = this->isReverseLookupMessagesEnabled();
            CStatusMessageList reverseLookupMessages;
            CStatusMessageList *pReverseLookupMessages = reverseLookupEnabled.testFlag(RevLogEnabled) ? &reverseLookupMessages : nullptr;

            CLogUtilities::addLogDetailsToList(pReverseLookupMessages, callsign,
                                               QStringLiteral("FsInn data from network: aircraft '%1', airline '%2', model '%3', combined '%4'").
                                               arg(aircraftIcaoDesignator, airlineIcaoDesignator, modelString, combinedAircraftType));

            QString usedModelString = modelString;

            const CAircraftMatcherSetup setup = m_matchingSettings.get();
            if (!modelString.isEmpty() && !setup.isReverseLookupModelString())
            {
                usedModelString.clear();
                CLogUtilities::addLogDetailsToList(pReverseLookupMessages, callsign,
                                                   QStringLiteral("FsInn modelstring '%1' ignored because of setuo").arg(modelString));
            }
            else if (!CAircraftMatcher::isKnownModelString(modelString, callsign, pReverseLookupMessages))
            {
                // from the T701 test, do NOT use if model string is unknown
                // this can overrride "swift livery strings", FsInn here only is useful with a known model string
                usedModelString.clear();
                CLogUtilities::addLogDetailsToList(pReverseLookupMessages, callsign,
                                                   QStringLiteral("FsInn modelstring ignored, as modelstring '%1' is not known").arg(modelString));
            }

            // with no model we pretend to be a normal "queried model"
            const CAircraftModel::ModelType modelType = usedModelString.isEmpty() ? CAircraftModel::TypeQueriedFromNetwork : CAircraftModel::TypeFSInnData;
            this->addOrUpdateAircraftInRange(callsign, aircraftIcaoDesignator, airlineIcaoDesignator, QString(), usedModelString, modelType, pReverseLookupMessages);
            this->addReverseLookupMessages(callsign, reverseLookupMessages);
            this->sendReadyForModelMatching(callsign, ReceivedFsInnPacket); // from FSInn
        }
    }

    void CAirspaceMonitor::onIcaoCodesReceived(const CCallsign &callsign, const QString &aircraftIcaoDesignator, const QString &airlineIcaoDesignator, const QString &livery)
    {
        Q_ASSERT_X(CThreadUtils::isCurrentThreadObjectThread(this), Q_FUNC_INFO, "not in main thread");
        if (!this->isConnectedAndNotShuttingDown()) { return; }

        BLACK_VERIFY_X(callsign.isValid(), Q_FUNC_INFO, "invalid callsign");
        if (!callsign.isValid()) { return; }
        const ReverseLookupLogging reverseLookupEnabled = this->isReverseLookupMessagesEnabled();
        CStatusMessageList reverseLookupMessages;
        CStatusMessageList *pReverseLookupMessages = reverseLookupEnabled.testFlag(RevLogEnabled) ? &reverseLookupMessages : nullptr;
        CLogUtilities::addLogDetailsToList(pReverseLookupMessages, callsign,
                                           QStringLiteral("Data from network: aircraft '%1', airline '%2', livery '%3'").
                                           arg(aircraftIcaoDesignator, airlineIcaoDesignator, livery),
                                           CAirspaceMonitor::getLogCategories());

        const CClient client = this->getClientOrDefaultForCallsign(callsign);
        this->addOrUpdateAircraftInRange(callsign, aircraftIcaoDesignator, airlineIcaoDesignator, livery, client.getQueriedModelString(), CAircraftModel::TypeQueriedFromNetwork, pReverseLookupMessages);
        this->addReverseLookupMessages(callsign, reverseLookupMessages);
        this->sendReadyForModelMatching(callsign, ReceivedIcaoCodes); // ICAO codes received

        emit this->requestedNewAircraft(callsign, aircraftIcaoDesignator, airlineIcaoDesignator, livery);
    }

    CAircraftModel CAirspaceMonitor::reverseLookupModelWithFlightplanData(
        const CCallsign &callsign, const QString &aircraftIcaoString,
        const QString &airlineIcaoString, const QString &liveryString, const QString &modelString,
        CAircraftModel::ModelType type, CStatusMessageList *log, bool runMatchinScript)
    {
        const int modelSetCount = this->getModelSetCount();
        CLogUtilities::addLogDetailsToList(log, callsign, QStringLiteral("Reverse lookup (with FP data), model set count: %1").arg(modelSetCount), CAirspaceMonitor::getLogCategories());

        const DBTripleIds ids = CAircraftModel::parseNetworkLiveryString(liveryString);
        const bool hasAnyId = ids.hasAnyId();
        if (hasAnyId) { this->markAsSwiftClient(callsign); }

        CAircraftModel lookupModel; // result
        const CAircraftModelList modelSet = this->getModelSet();
        const CAircraftMatcherSetup setup = m_matchingSettings.get();
        do
        {
            // directly check model string
            if (!modelString.isEmpty())
            {
                lookupModel = CAircraftMatcher::reverseLookupModelStringInDB(modelString, callsign, setup.isReverseLookupModelString(), log);
                if (lookupModel.hasValidDbKey()) { break; } // found by model string
            }

            CLivery livery;
            CAirlineIcaoCode airlineIcao;
            CAircraftIcaoCode aircraftIcao;

            if (!setup.isReverseLookupSwiftLiveryIds())
            {
                CLogUtilities::addLogDetailsToList(log, callsign, QStringLiteral("Reverse lookup of livery string '%1' disabled").arg(liveryString));
            }
            else if (hasAnyId)
            {
                if (ids.model >= 0)
                {
                    lookupModel = CAircraftMatcher::reverseLookupModelId(ids.model, callsign, log);
                    if (lookupModel.hasValidDbKey()) { break; } // found by model id from livery string
                };

                CAircraftMatcher::reverseLookupByIds(ids, aircraftIcao, livery, callsign, log);
                if (livery.hasValidDbKey()) { airlineIcao = livery.getAirlineIcaoCode(); }

                if (aircraftIcao.hasValidDbKey() && livery.hasValidDbKey())
                {
                    CLogUtilities::addLogDetailsToList(log, callsign, QStringLiteral("Using DB livery %1 and aircraft ICAO %2 to create model").arg(livery.getDbKeyAsString(), aircraftIcao.getDbKeyAsString()), CAirspaceMonitor::getLogCategories());

                    // we have a valid livery from DB + valid aircraft ICAO from DB
                    lookupModel = CAircraftModel(modelString, type, "By DB livery and aircraft ICAO", aircraftIcao, livery);
                    break;
                }
            }

            // now fuzzy search on aircraft
            if (!aircraftIcao.hasValidDbKey())
            {
                aircraftIcao = CAircraftIcaoCode(aircraftIcaoString);
                const bool knownAircraftIcao = CAircraftMatcher::isKnownAircraftDesignator(aircraftIcaoString, callsign, log);
                if (airlineIcao.isLoadedFromDb() && !knownAircraftIcao)
                {
                    // we have no valid aircraft ICAO, so we do a fuzzy search among those
                    CLogUtilities::addLogDetailsToList(log, callsign, QStringLiteral("Fuzzy search among airline aircraft because '%1' is not known ICAO designator").arg(aircraftIcaoString));
                    const CAircraftIcaoCode foundIcao = CAircraftMatcher::searchAmongAirlineAircraft(aircraftIcaoString, airlineIcao, callsign, log);
                    if (foundIcao.isLoadedFromDb()) { aircraftIcao = foundIcao; }
                }
            }

            // if we have a livery, we already know the airline, or the livery is a color livery
            if (!airlineIcao.hasValidDbKey() && !livery.hasValidDbKey())
            {
                const CFlightPlanRemarks fpRemarks = this->tryToGetFlightPlanRemarks(callsign);
                // const bool hasParsedAirlineRemarks = fpRemarks.hasParsedAirlineRemarks();

                if (fpRemarks.isEmpty())
                {
                    CLogUtilities::addLogDetailsToList(log, callsign, QStringLiteral("No flight plan remarks"));
                }
                else
                {
                    CLogUtilities::addLogDetailsToList(log, callsign, QStringLiteral("FP remarks: '%1'").arg(fpRemarks.getRemarks()));
                    CLogUtilities::addLogDetailsToList(log, callsign, QStringLiteral("FP rem.parsed: '%1'").arg(fpRemarks.toQString(true)));
                }

                // FP data if any
                const QString telephonyFromFp   = CAircraftMatcher::reverseLookupTelephonyDesignator(fpRemarks.getRadioTelephony(), callsign, log);
                const QString airlineNameFromFp = CAircraftMatcher::reverseLookupAirlineName(fpRemarks.getFlightOperator(), callsign, log);
                const QString airlineIcaoFromFp = fpRemarks.getAirlineIcao().getDesignator();

                // turn into names as in DB
                const QString airlineNameLookup = CAircraftMatcher::reverseLookupAirlineName(airlineNameFromFp);
                const QString telephonyLookup   = CAircraftMatcher::reverseLookupTelephonyDesignator(telephonyFromFp);
                if (!airlineNameLookup.isEmpty()) { CLogUtilities::addLogDetailsToList(log, callsign, QStringLiteral("Using resolved airline name '%1' found by FP name '%2'").arg(airlineNameLookup, airlineNameFromFp), CAirspaceMonitor::getLogCategories()); }
                if (!telephonyLookup.isEmpty())   { CLogUtilities::addLogDetailsToList(log, callsign, QStringLiteral("Using resolved telephony designator '%1' found by FP telephoy '%2'").arg(telephonyLookup, telephonyFromFp), CAirspaceMonitor::getLogCategories()); }

                // INFO: CModelMatcherComponent::reverseLookup() contains the simulated lookup
                // changed with T701: resolve first against model set, then all DB data
                // if an airline is ambiguous most likely the one in the set is the best choice
                airlineIcao = CAircraftMatcher::failoverValidAirlineIcaoDesignatorModelsFirst(callsign, airlineIcaoString, airlineIcaoFromFp, true, airlineNameFromFp, telephonyFromFp, modelSet, log);

                // not found, create a seatch patterm
                if (!airlineIcao.isLoadedFromDb())
                {
                    if (!airlineIcao.hasValidDesignator()) { airlineIcao.setDesignator(airlineIcaoString.isEmpty() ? callsign.getAirlinePrefix() : airlineIcaoString); }
                    if (!airlineNameLookup.isEmpty()) { airlineIcao.setName(airlineNameLookup); }
                    if (!telephonyLookup.isEmpty())   { airlineIcao.setTelephonyDesignator(telephonyLookup); }

                    // already try to resolve at this stage by a smart lookup with all the filled data from above
                    airlineIcao = CAircraftMatcher::reverseLookupAirlineIcao(airlineIcao, callsign, log);
                }
            }

            CLogUtilities::addLogDetailsToList(log, callsign, QStringLiteral("Used aircraft ICAO: '%1'").arg(aircraftIcao.toQString(true)), CAirspaceMonitor::getLogCategories());
            CLogUtilities::addLogDetailsToList(log, callsign, QStringLiteral("Used airline ICAO: '%1'").arg(airlineIcao.toQString(true)), CAirspaceMonitor::getLogCategories());

            // matching script is used below
            lookupModel = CAircraftMatcher::reverseLookupModel(callsign, aircraftIcao, airlineIcao, liveryString, modelString, setup, modelSet, type, log);
        }
        while (false);

        // model found
        lookupModel.setCallsign(callsign);

        // script
        if (runMatchinScript && setup.doRunMsReverseLookupScript())
        {
            const MatchingScriptReturnValues rv = CAircraftMatcher::reverseLookupScript(lookupModel, setup, modelSet, log);
            if (rv.runScriptAndModified())
            {
                if (rv.runScriptAndRerun())
                {
                    CLogUtilities::addLogDetailsToList(log, callsign, QStringLiteral("Matching script: Re-run reverseLookupModelWithFlightplanData"), CAirspaceMonitor::getLogCategories());
                    return CAirspaceMonitor::reverseLookupModelWithFlightplanData(callsign,
                            rv.model.getAircraftIcaoCodeDesignator(), rv.model.getAirlineIcaoCodeVDesignator(), rv.model.getLivery().getCombinedCode(),
                            modelString, type, log, false);
                }
                else
                {
                    lookupModel = rv.model;
                    CLogUtilities::addLogDetailsToList(log, callsign, QStringLiteral("Matching script: Using model from matching script"), CAirspaceMonitor::getLogCategories());
                }
            }
        }
        else
        {
            CLogUtilities::addLogDetailsToList(log, callsign, QStringLiteral("No reverse lookup script used"));
        }

        // done
        lookupModel.setCallsign(callsign); // set again just in case modified by script
        return lookupModel;
    }

    bool CAirspaceMonitor::addNewAircraftInRange(const CSimulatedAircraft &aircraft)
    {
        const CCallsign callsign = aircraft.getCallsign();
        Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "Missing callsign");

        if (!sApp || sApp->isShuttingDown() || !sApp->getWebDataServices()) { return false; }

        CSimulatedAircraft newAircraft(aircraft);
        newAircraft.setRendered(false); // reset rendering
        newAircraft.calculcateAndUpdateRelativeDistanceAndBearing(this->getOwnAircraftPosition()); // distance from myself

        if (this->getConnectedServer().getEcosystem() == CEcosystem::vatsim())
        {
            sApp->getWebDataServices()->updateWithVatsimDataFileData(newAircraft);
        }
        const bool added = CRemoteAircraftProvider::addNewAircraftInRange(newAircraft);
        if (added && aircraft.hasModelString())
        {
            // most likely I could take the CG at this time from aircraft
            // to make sure it is really the DB value i query again
            const CAircraftModel model = sApp->getWebDataServices()->getModelForModelString(aircraft.getModelString());
            const CLength cg = model.hasValidDbKey() ? model.getCG() : CLength::null();
            this->rememberCGFromDB(cg, aircraft.getModelString());
            this->rememberCGFromDB(cg, aircraft.getCallsign());
        }
        return added;
    }

    void CAirspaceMonitor::asyncAddNewAircraftInRange(const CSimulatedAircraftList &aircraft, bool readyForModelMatching)
    {
        if (aircraft.isEmpty()) { return; }
        if (!sApp || sApp->isShuttingDown()) { return; }

        int c = 1;
        QPointer<CAirspaceMonitor> myself(this);
        for (const CSimulatedAircraft &ac : aircraft)
        {
            QTimer::singleShot(c++ * 25, this, [ = ]
            {
                if (!myself) { return; }
                myself->addNewAircraftInRange(ac);
                if (!readyForModelMatching) { return; }
                const CCallsign cs = ac.getCallsign();

                m_readiness.remove(cs); // cleanup
                const MatchingReadinessFlag ready = ReceivedAll;
                myself->sendReadyForModelMatching(cs, ready); // airspace monitor adding all aicraft
            });
        }
    }

    int CAirspaceMonitor::updateOnlineStation(const CCallsign &callsign, const CPropertyIndexVariantMap &vm, bool skipEqualValues, bool sendSignal)
    {
        const int c = m_atcStationsOnline.applyIfCallsign(callsign, vm, skipEqualValues);
        if (c > 0 && sendSignal)
        {
            emit this->changedAtcStationsOnline();
        }
        return c;
    }

    int CAirspaceMonitor::updateBookedStation(const CCallsign &callsign, const CPropertyIndexVariantMap &vm, bool skipEqualValues, bool sendSignal)
    {
        // do not used applyFirst here, more stations wit callsign at a time
        const int c = m_atcStationsBooked.applyIfCallsign(callsign, vm, skipEqualValues);
        if (c > 0 && sendSignal)
        {
            emit this->changedAtcStationsBooked();
        }
        return c;
    }

    void CAirspaceMonitor::copilotDetected()
    {
        // for future usage
    }

    bool CAirspaceMonitor::recallFsInnPacket(const CCallsign &callsign)
    {
        if (!m_tempFsInnPackets.contains(callsign)) { return false; }
        const FsInnPacket packet = m_tempFsInnPackets[callsign];
        m_tempFsInnPackets.remove(callsign);
        this->onCustomFSInnPacketReceived(callsign, packet.airlineIcaoDesignator, packet.aircraftIcaoDesignator, packet.combinedCode, packet.modelString);
        return true;
    }

    CSimulatedAircraft CAirspaceMonitor::addOrUpdateAircraftInRange(
        const CCallsign &callsign, const QString &aircraftIcao, const QString &airlineIcao, const QString &livery,
        const QString &modelString, CAircraftModel::ModelType modelType, CStatusMessageList *log)
    {
        const CSimulatedAircraft aircraft = this->getAircraftInRangeForCallsign(callsign);
        if (aircraft.hasValidCallsign())
        {
            // only if we do not have a DB model yet
            if (!aircraft.getModel().hasValidDbKey())
            {
                CAircraftModel model = this->reverseLookupModelWithFlightplanData(callsign, aircraftIcao, airlineIcao, livery, modelString, modelType, log);
                model.updateMissingParts(aircraft.getModel());
                // Use anonymous as originator here, since the remote aircraft provider is ourselves and the call to updateAircraftModel() would
                // return without doing anything.
                this->updateAircraftModel(callsign, model, CIdentifier::null());
                this->updateAircraftNetworkModel(callsign, model, CIdentifier::null());
            }
        }
        else
        {
            const CAircraftModel model = this->reverseLookupModelWithFlightplanData(callsign, aircraftIcao, airlineIcao, livery, modelString, modelType, log);
            const CSimulatedAircraft initAircraft(model);
            this->addNewAircraftInRange(initAircraft);
        }
        return aircraft;
    }

    void CAirspaceMonitor::onAircraftUpdateReceived(const CAircraftSituation &situation, const CTransponder &transponder)
    {
        Q_ASSERT_X(CThreadUtils::isCurrentThreadObjectThread(this), Q_FUNC_INFO, "Called in different thread");
        if (!this->isConnectedAndNotShuttingDown()) { return; }

        const CCallsign callsign(situation.getCallsign());
        Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "Empty callsign");

        if (this->isCopilotAircraft(callsign)) { return; }

        // update client info
        this->autoAdjustCientGndCapability(situation);

        // store situation history
        this->storeAircraftSituation(situation); // updates situation

        const bool existsInRange = this->isAircraftInRange(callsign);
        const bool hasFsInnPacket = m_tempFsInnPackets.contains(callsign);

        if (!existsInRange)
        {
            CSimulatedAircraft aircraft;
            aircraft.setCallsign(callsign);
            aircraft.setSituation(situation);
            aircraft.setTransponder(transponder);
            this->addNewAircraftInRange(aircraft);
            this->sendInitialPilotQueries(callsign, true, !hasFsInnPacket);

            // new client, there is a chance it has been already created by custom packet
            const CClient client(callsign);
            this->addNewClient(client);
        }
        else
        {
            // update, aircraft already exists
            CPropertyIndexVariantMap vm;
            vm.addValue(CSimulatedAircraft::IndexTransponder, transponder);
            vm.addValue(CSimulatedAircraft::IndexSituation, situation);
            vm.addValue(CSimulatedAircraft::IndexRelativeDistance, this->calculateDistanceToOwnAircraft(situation));
            vm.addValue(CSimulatedAircraft::IndexRelativeBearing, this->calculateBearingToOwnAircraft(situation));
            this->updateAircraftInRange(callsign, vm);
        }
    }

    void CAirspaceMonitor::onAircraftInterimUpdateReceived(const CAircraftSituation &situation)
    {
        Q_ASSERT_X(CThreadUtils::isCurrentThreadObjectThread(this), Q_FUNC_INFO, "Called in different thread");
        if (!this->isConnectedAndNotShuttingDown()) { return; }

        const CCallsign callsign(situation.getCallsign());

        // checks
        Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "Empty callsign");

        if (isCopilotAircraft(callsign)) { return; }

        if (CBuildConfig::isLocalDeveloperDebugBuild())
        {
            Q_ASSERT_X(!situation.isNaNVectorDouble(), Q_FUNC_INFO, "Detected NaN");
            Q_ASSERT_X(!situation.isInfVectorDouble(), Q_FUNC_INFO, "Detected inf");
            Q_ASSERT_X(situation.isValidVectorRange(), Q_FUNC_INFO, "out of range [-1,1]");
        }

        // Interim packets do not have groundspeed, hence set the last known value.
        // If there is no full position available yet, throw this interim position away.
        CAircraftSituation interimSituation(situation);
        CAircraftSituationList history = this->remoteAircraftSituations(callsign);
        if (history.empty()) { return; } // we need one full situation at least
        const CAircraftSituation lastSituation = history.latestObject();

        // changed position, continue and copy values
        interimSituation.setCurrentUtcTime();
        interimSituation.setGroundSpeed(lastSituation.getGroundSpeed());

        // store situation history
        this->storeAircraftSituation(interimSituation);

        // if we have no aircraft in range yet, we stop here
        if (!this->isAircraftInRange(callsign)) { return; }

        const bool samePosition = lastSituation.equalNormalVectorDouble(interimSituation);
        if (samePosition) { return; } // nothing to update

        // update aircraft
        this->updateAircraftInRangeDistanceBearing(
            callsign, interimSituation,
            this->calculateDistanceToOwnAircraft(interimSituation),
            this->calculateBearingToOwnAircraft(interimSituation)
        );
    }

    void CAirspaceMonitor::onConnectionStatusChanged(INetwork::ConnectionStatus oldStatus, INetwork::ConnectionStatus newStatus)
    {
        Q_UNUSED(oldStatus);
        switch (newStatus)
        {
        case INetwork::Connected:
            break;
        case INetwork::Disconnected:
        case INetwork::DisconnectedError:
        case INetwork::DisconnectedLost:
        case INetwork::DisconnectedFailed:
            this->clear();
            break;
        default:
            break;
        }
    }

    void CAirspaceMonitor::onPilotDisconnected(const CCallsign &callsign)
    {
        Q_ASSERT(CThreadUtils::isCurrentThreadObjectThread(this));

        // in case of inconsistencies I always remove here
        this->removeFromAircraftCachesAndLogs(callsign);
        const bool removed = CRemoteAircraftProvider::removeAircraft(callsign);
        this->removeClient(callsign);
        if (removed) { emit this->removedAircraft(callsign); }
    }

    void CAirspaceMonitor::onFrequencyReceived(const CCallsign &callsign, const CFrequency &frequency)
    {
        Q_ASSERT(CThreadUtils::isCurrentThreadObjectThread(this));

        // update
        const CPropertyIndexVariantMap vm({CSimulatedAircraft::IndexCom1System, CComSystem::IndexActiveFrequency}, CVariant::from(frequency));
        this->updateAircraftInRange(callsign, vm);
    }

    void CAirspaceMonitor::onAircraftConfigReceived(const CCallsign &callsign, const QJsonObject &jsonObject, qint64 currentOffsetMs)
    {
        Q_ASSERT(CThreadUtils::isCurrentThreadObjectThread(this));
        BLACK_AUDIT_X(!callsign.isEmpty(), Q_FUNC_INFO, "Need callsign");
        if (callsign.isEmpty()) { return; }

        // store parts
        this->storeAircraftParts(callsign, jsonObject, currentOffsetMs);

        // update client capability
        CClient client = this->getClientOrDefaultForCallsign(callsign);
        client.setUserCallsign(callsign); // make valid by setting a callsign
        if (client.hasCapability(CClient::FsdWithAircraftConfig)) { return; }
        client.addCapability(CClient::FsdWithAircraftConfig);
        this->setOtherClient(client);
    }

    CAircraftSituation CAirspaceMonitor::storeAircraftSituation(const CAircraftSituation &situation, bool allowTestOffset)
    {
        const CCallsign callsign(situation.getCallsign());
        BLACK_VERIFY_X(!callsign.isEmpty(), Q_FUNC_INFO, "empty callsign");
        if (callsign.isEmpty()) { return situation; }

        CAircraftSituation correctedSituation(allowTestOffset ? this->addTestAltitudeOffsetToSituation(situation) : situation);
        bool haveRequestedElevation = false;
        const bool canLikelySkipNearGround = correctedSituation.canLikelySkipNearGroundInterpolation();
        if (!correctedSituation.hasGroundElevation() && !canLikelySkipNearGround)
        {
            // fetch from cache or request
            const CAircraftSituationList situations = this->remoteAircraftSituations(callsign);
            const CAircraftSituation situationWithElv = situations.findCLosestElevationWithinRange(correctedSituation, correctedSituation.getDistancePerTime(100, CElevationPlane::singlePointRadius()));
            if (!situationWithElv.getGroundElevation().isNull())
            {
                correctedSituation.transferGroundElevation(situationWithElv);
            }
            else
            {
                const CLength distance(correctedSituation.getDistancePerTime250ms(CElevationPlane::singlePointRadius())); // distance per ms
                const CElevationPlane ep = this->findClosestElevationWithinRangeOrRequest(correctedSituation, distance, callsign);
                haveRequestedElevation = ep.isNull();  // NULL means we requested
                Q_ASSERT_X(ep.isNull() || !ep.getRadius().isNull(), Q_FUNC_INFO, "null radius");
                correctedSituation.setGroundElevation(ep, CAircraftSituation::FromCache);
            }

            if (!correctedSituation.hasGroundElevation())
            {
                // we have a new situation, so we try to get the elevation
                // so far we have requested it, but we set it upfront either by
                // a) average value from other plane in the vicinity or
                // b) by extrapolating
                const CElevationPlane averagePlane = this->averageElevationOfNonMovingAircraft(situation, CElevationPlane::majorAirportRadius(), 2);
                if (!averagePlane.isNull())
                {
                    correctedSituation.setGroundElevation(averagePlane, CAircraftSituation::Average);
                }
                else
                {
                    // values before updating (i.e. "storing") so the new situation is not yet considered
                    const CAircraftSituationList oldSituations = this->remoteAircraftSituations(callsign);
                    const CAircraftSituationChangeList oldChanges = this->remoteAircraftSituationChanges(callsign);
                    if (oldSituations.size() > 1)
                    {
                        const bool extrapolated = correctedSituation.extrapolateElevation(oldSituations[0], oldSituations[1], oldChanges.frontOrDefault());
                        Q_UNUSED(extrapolated);
                    }
                }
            } // gnd. elevation
        }

        // do we already have ground details?
        if (situation.getOnGroundDetails() == CAircraftSituation::NotSetGroundDetails)
        {
            const CClient client = this->getClientOrDefaultForCallsign(callsign);
            if (client.hasCapability(CClient::FsdWithGroundFlag))
            {
                // we rely on situation gnd.flag
                correctedSituation.setOnGroundDetails(CAircraftSituation::InFromNetwork);
            }
            else if (client.hasCapability(CClient::FsdWithAircraftConfig))
            {
                const CAircraftPartsList parts = this->remoteAircraftParts(callsign);
                if (!parts.isEmpty()) { correctedSituation.adjustGroundFlag(parts, true); }
            }
        }

        // CG from provider
        const CLength cg = this->getSimulatorOrDbCG(callsign, this->getCGFromDB(callsign)); // always x-check against simulator to override guessed values and reflect changed CGs
        if (!cg.isNull()) { correctedSituation.setCG(cg); }

        // store corrected situation
        correctedSituation = CRemoteAircraftProvider::storeAircraftSituation(correctedSituation, false); // we already added offset if any

        // check if we STILL want to request
        if (!haveRequestedElevation && !canLikelySkipNearGround)
        {
            // we have not requested so far, but we are NEAR ground
            // we expect at least not transferred cache or we are moving and have no provider elevation yet
            if (correctedSituation.isOtherElevationInfoBetter(CAircraftSituation::FromCache, false) || (correctedSituation.isMoving() && correctedSituation.isOtherElevationInfoBetter(CAircraftSituation::FromProvider, false)))
            {
                haveRequestedElevation = this->requestElevation(correctedSituation);
            }
        }

        Q_UNUSED(haveRequestedElevation);
        return correctedSituation;
    }

    void CAirspaceMonitor::sendInitialAtcQueries(const CCallsign &callsign)
    {
        if (!this->isConnectedAndNotShuttingDown()) { return; }
        m_network->sendRealNameQuery(callsign);
        m_network->sendAtisQuery(callsign); // request ATIS and voice rooms
        m_network->sendCapabilitiesQuery(callsign);
        m_network->sendServerQuery(callsign);
    }

    bool CAirspaceMonitor::sendNextStaggeredAtisQuery()
    {
        if (m_queryAtis.isEmpty()) { return false; }
        if (!this->isConnectedAndNotShuttingDown()) { return false; }
        const CCallsign cs = m_queryAtis.dequeue();
        if (!m_atcStationsOnline.containsCallsign(cs)) { return false; }
        m_network->sendAtisQuery(cs);
        return true;
    }

    void CAirspaceMonitor::sendInitialPilotQueries(const CCallsign &callsign, bool withIcaoQuery, bool withFsInn)
    {
        if (!this->isConnectedAndNotShuttingDown()) { return; }

        if (withIcaoQuery) { m_network->sendIcaoCodesQuery(callsign); }
        if (withFsInn) { m_network->sendCustomFsinnQuery(callsign); }

        m_network->sendFrequencyQuery(callsign);
        m_network->sendRealNameQuery(callsign);
        m_network->sendCapabilitiesQuery(callsign);
        m_network->sendServerQuery(callsign);
    }

    bool CAirspaceMonitor::sendNextStaggeredPilotDataQuery()
    {
        if (m_queryPilot.isEmpty()) { return false; }
        if (!this->isConnectedAndNotShuttingDown()) { return false; }
        const CCallsign cs = m_queryPilot.dequeue();
        if (!this->isAircraftInRange(cs)) { return false; }
        m_network->sendFrequencyQuery(cs);

        // we only query ICAO if we have none yet
        // it happens sometimes with some FSD servers (e.g our testserver) a first query is skipped
        // Important: this is only a workaround and must not replace a sendInitialPilotQueries
        if (!this->getAircraftInRangeForCallsign(cs).hasAircraftDesignator())
        {
            m_network->sendIcaoCodesQuery(cs);
        }
        return true;
    }

    bool CAirspaceMonitor::isConnected() const
    {
        return m_network && m_network->isConnected();
    }

    bool CAirspaceMonitor::isConnectedAndNotShuttingDown() const
    {
        if (!sApp || sApp->isShuttingDown()) { return false; }
        return this->isConnected();
    }

    const CServer &CAirspaceMonitor::getConnectedServer() const
    {
        static const CServer empty;
        if (!this->isConnected()) { return empty; }
        return m_network->getPresetServer();
    }

    const CEcosystem &CAirspaceMonitor::getCurrentEcosystem() const
    {
        return this->getConnectedServer().getEcosystem();
    }

    bool CAirspaceMonitor::supportsVatsimDataFile() const
    {
        const bool dataFile = sApp && sApp->getWebDataServices() && sApp->getWebDataServices()->getVatsimDataFileReader();
        return dataFile && this->getConnectedServer().getEcosystem().isSystem(CEcosystem::VATSIM);
    }

    CLength CAirspaceMonitor::calculateDistanceToOwnAircraft(const CAircraftSituation &situation) const
    {
        CLength distance = this->getOwnAircraft().calculateGreatCircleDistance(situation);
        distance.switchUnit(CLengthUnit::NM());
        return distance;
    }

    CAngle CAirspaceMonitor::calculateBearingToOwnAircraft(const CAircraftSituation &situation) const
    {
        CAngle angle = this->getOwnAircraft().calculateBearing(situation);
        angle.switchUnit(CAngleUnit::deg());
        return angle;
    }

    bool CAirspaceMonitor::isCopilotAircraft(const CCallsign &callsign) const
    {
        if (!sApp || sApp->isShuttingDown() || !sApp->getIContextNetwork()) { return false; }

        // It is only relevant if we are logged in as observer
        if (sApp->getIContextNetwork()->getLoginMode() != INetwork::LoginAsObserver) { return false; }

        const CCallsign ownCallsign = this->getOwnAircraft().getCallsign();
        return ownCallsign.isMaybeCopilotCallsign(callsign);
    }

    CAirspaceMonitor::FsInnPacket::FsInnPacket(const QString &aircraftIcaoDesignator, const QString &airlineIcaoDesignator, const QString &combinedCode, const QString &modelString) :
        aircraftIcaoDesignator(aircraftIcaoDesignator.trimmed().toUpper()), airlineIcaoDesignator(airlineIcaoDesignator.trimmed().toUpper()), combinedCode(combinedCode.trimmed().toUpper()), modelString(modelString.trimmed())
    { }
} // namespace
