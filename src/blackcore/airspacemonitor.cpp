/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/airspaceanalyzer.h"
#include "blackcore/airspacemonitor.h"
#include "blackcore/aircraftmatcher.h"
#include "blackcore/matchingutils.h"
#include "blackcore/application.h"
#include "blackcore/vatsim/networkvatlib.h"
#include "blackcore/vatsim/vatsimbookingreader.h"
#include "blackcore/vatsim/vatsimdatafilereader.h"
#include "blackcore/webdataservices.h"
#include "blackmisc/audio/voiceroom.h"
#include "blackmisc/aviation/aircraftparts.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/aviation/modulator.h"
#include "blackmisc/aviation/transponder.h"
#include "blackmisc/geo/elevationplane.h"
#include "blackmisc/network/user.h"
#include "blackmisc/network/voicecapabilities.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/compare.h"
#include "blackmisc/iterator.h"
#include "blackmisc/json.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/propertyindexvariantmap.h"
#include "blackmisc/range.h"
#include "blackmisc/sequence.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/testing.h"
#include "blackmisc/threadutils.h"
#include "blackmisc/variant.h"
#include "blackmisc/verify.h"
#include "blackmisc/worker.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QEventLoop>
#include <QReadLocker>
#include <QThread>
#include <QTime>
#include <QVariant>
#include <QWriteLocker>
#include <Qt>

using namespace BlackMisc;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Json;
using namespace BlackMisc::Network;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Weather;
using namespace BlackCore::Vatsim;

namespace BlackCore
{
    CAirspaceMonitor::CAirspaceMonitor(BlackMisc::Simulation::IOwnAircraftProvider *ownAircraftProvider, INetwork *network, QObject *parent)
        : QObject(parent),
          COwnAircraftAware(ownAircraftProvider),
          CIdentifiable(this),
          m_network(network),
          m_analyzer(new CAirspaceAnalyzer(ownAircraftProvider, this, network, this))
    {
        this->setObjectName("CAirspaceMonitor");
        this->m_enableReverseLookupMsgs = sApp->isRunningInDeveloperEnvironment();

        this->connect(this->m_network, &INetwork::atcPositionUpdate, this, &CAirspaceMonitor::ps_atcPositionUpdate);
        this->connect(this->m_network, &INetwork::atisReplyReceived, this, &CAirspaceMonitor::ps_atisReceived);
        this->connect(this->m_network, &INetwork::atisVoiceRoomReplyReceived, this, &CAirspaceMonitor::ps_atisVoiceRoomReceived);
        this->connect(this->m_network, &INetwork::atisLogoffTimeReplyReceived, this, &CAirspaceMonitor::ps_atisLogoffTimeReceived);
        this->connect(this->m_network, &INetwork::flightPlanReplyReceived, this, &CAirspaceMonitor::ps_flightPlanReceived);
        this->connect(this->m_network, &INetwork::realNameReplyReceived, this, &CAirspaceMonitor::ps_realNameReplyReceived);
        this->connect(this->m_network, &INetwork::icaoCodesReplyReceived, this, &CAirspaceMonitor::ps_icaoCodesReceived);
        this->connect(this->m_network, &INetwork::pilotDisconnected, this, &CAirspaceMonitor::ps_pilotDisconnected);
        this->connect(this->m_network, &INetwork::atcDisconnected, this, &CAirspaceMonitor::ps_atcControllerDisconnected);
        this->connect(this->m_network, &INetwork::aircraftPositionUpdate, this, &CAirspaceMonitor::ps_aircraftUpdateReceived);
        this->connect(this->m_network, &INetwork::aircraftInterimPositionUpdate, this, &CAirspaceMonitor::ps_aircraftInterimUpdateReceived);
        this->connect(this->m_network, &INetwork::frequencyReplyReceived, this, &CAirspaceMonitor::ps_frequencyReceived);
        this->connect(this->m_network, &INetwork::capabilitiesReplyReceived, this, &CAirspaceMonitor::ps_capabilitiesReplyReceived);
        this->connect(this->m_network, &INetwork::customFSInnPacketReceived, this, &CAirspaceMonitor::ps_customFSInnPacketReceived);
        this->connect(this->m_network, &INetwork::serverReplyReceived, this, &CAirspaceMonitor::ps_serverReplyReceived);
        this->connect(this->m_network, &INetwork::aircraftConfigPacketReceived, this, &CAirspaceMonitor::ps_aircraftConfigReceived);
        this->connect(this->m_network, &INetwork::connectionStatusChanged, this, &CAirspaceMonitor::ps_connectionStatusChanged);

        // AutoConnection: this should also avoid race conditions by updating the bookings
        Q_ASSERT_X(sApp->getWebDataServices(), Q_FUNC_INFO, "Missing data reader");
        this->connect(sApp->getWebDataServices()->getBookingReader(), &CVatsimBookingReader::atcBookingsRead, this, &CAirspaceMonitor::ps_receivedAtcBookings);
        this->connect(sApp->getWebDataServices()->getBookingReader(), &CVatsimBookingReader::atcBookingsReadUnchanged, this, &CAirspaceMonitor::ps_readUnchangedAtcBookings);
        this->connect(sApp->getWebDataServices()->getVatsimDataFileReader(), &CVatsimDataFileReader::dataFileRead, this, &CAirspaceMonitor::ps_receivedDataFile);

        // Force snapshot in the main event loop
        this->connect(this->m_analyzer, &CAirspaceAnalyzer::airspaceAircraftSnapshot, this, &CAirspaceMonitor::airspaceAircraftSnapshot, Qt::QueuedConnection);

        // Analyzer
        this->connect(this->m_analyzer, &CAirspaceAnalyzer::timeoutAircraft, this, &CAirspaceMonitor::ps_pilotDisconnected, Qt::QueuedConnection);
        this->connect(this->m_analyzer, &CAirspaceAnalyzer::timeoutAtc, this, &CAirspaceMonitor::ps_atcControllerDisconnected, Qt::QueuedConnection);
    }

    const CLogCategoryList &CAirspaceMonitor::getLogCategories()
    {
        static const BlackMisc::CLogCategoryList cats { CLogCategory::matching(), CLogCategory::network() };
        return cats;
    }

    CSimulatedAircraftList CAirspaceMonitor::getAircraftInRange() const
    {
        QReadLocker l(&m_lockAircraft);
        return m_aircraftInRange;
    }

    CCallsignSet CAirspaceMonitor::getAircraftInRangeCallsigns() const
    {
        return this->getAircraftInRange().getCallsigns();
    }

    CSimulatedAircraft CAirspaceMonitor::getAircraftInRangeForCallsign(const CCallsign &callsign) const
    {
        return this->getAircraftInRange().findFirstByCallsign(callsign);
    }

    CAircraftModel CAirspaceMonitor::getAircraftInRangeModelForCallsign(const CCallsign &callsign) const
    {
        CSimulatedAircraft aircraft(getAircraftInRangeForCallsign(callsign)); // threadsafe
        return aircraft.getModel();
    }

    int CAirspaceMonitor::getAircraftInRangeCount() const
    {
        return this->getAircraftInRange().size();
    }

    CAirspaceAircraftSnapshot CAirspaceMonitor::getLatestAirspaceAircraftSnapshot() const
    {
        Q_ASSERT_X(this->m_analyzer, Q_FUNC_INFO, "No analyzer");
        return this->m_analyzer->getLatestAirspaceAircraftSnapshot();
    }

    CAircraftSituationList CAirspaceMonitor::remoteAircraftSituations(const CCallsign &callsign) const
    {
        QReadLocker l(&m_lockSituations);
        static const CAircraftSituationList empty;
        if (!m_situationsByCallsign.contains(callsign)) { return empty; }
        return m_situationsByCallsign[callsign];
    }

    int CAirspaceMonitor::remoteAircraftSituationsCount(const CCallsign &callsign) const
    {
        QReadLocker l(&m_lockSituations);
        if (!m_situationsByCallsign.contains(callsign)) { return -1; }
        return m_situationsByCallsign[callsign].size();
    }

    CAircraftPartsList CAirspaceMonitor::remoteAircraftParts(const CCallsign &callsign, qint64 cutoffTimeValuesBefore) const
    {
        QReadLocker l(&m_lockParts);
        static const CAircraftPartsList empty;
        if (!m_partsByCallsign.contains(callsign)) { return empty; }
        if (cutoffTimeValuesBefore < 0)
        {
            return m_partsByCallsign[callsign];
        }
        else
        {
            return m_partsByCallsign[callsign].findBefore(cutoffTimeValuesBefore);
        }
    }

    bool CAirspaceMonitor::isRemoteAircraftSupportingParts(const CCallsign &callsign) const
    {
        QReadLocker l(&m_lockParts);
        return m_aircraftSupportingParts.contains(callsign);
    }

    CCallsignSet CAirspaceMonitor::remoteAircraftSupportingParts() const
    {
        QReadLocker l(&m_lockParts);
        return m_aircraftSupportingParts;
    }

    QList<QMetaObject::Connection> CAirspaceMonitor::connectRemoteAircraftProviderSignals(
        QObject *receiver,
        std::function<void(const CAircraftSituation &)>                                     situationSlot,
        std::function<void(const BlackMisc::Aviation::CCallsign &, const CAircraftParts &)> partsSlot,
        std::function<void(const CCallsign &)>                                              removedAircraftSlot,
        std::function<void(const CAirspaceAircraftSnapshot &)>                              aircraftSnapshotSlot
    )
    {
        Q_ASSERT_X(receiver, Q_FUNC_INFO, "Missing receiver");

        // bind does not allow to define connection type, so we use receiver as workaround
        const QMetaObject::Connection c1 = connect(this, &CAirspaceMonitor::addedAircraftSituation, receiver, situationSlot);
        Q_ASSERT_X(c1, Q_FUNC_INFO, "connect failed");
        const QMetaObject::Connection c2 = connect(this, &CAirspaceMonitor::addedAircraftParts, receiver, partsSlot);
        Q_ASSERT_X(c2, Q_FUNC_INFO, "connect failed");
        const QMetaObject::Connection c3 = connect(this, &CAirspaceMonitor::removedAircraft, receiver, removedAircraftSlot);
        Q_ASSERT_X(c3, Q_FUNC_INFO, "connect failed");
        // trick is to use the Queued signal here
        // analyzer (own thread) -> airspaceAircraftSnapshot -> AirspaceMonitor -> airspaceAircraftSnapshot queued in main thread
        const QMetaObject::Connection c4 = this->connect(this->m_analyzer, &CAirspaceAnalyzer::airspaceAircraftSnapshot, receiver, aircraftSnapshotSlot, Qt::QueuedConnection);
        Q_ASSERT_X(c4, Q_FUNC_INFO, "connect failed");
        return QList<QMetaObject::Connection>({ c1, c2, c3, c4});
    }

    bool CAirspaceMonitor::updateAircraftEnabled(const CCallsign &callsign, bool enabledForRedering)
    {
        const CPropertyIndexVariantMap vm(CSimulatedAircraft::IndexEnabled, CVariant::fromValue(enabledForRedering));
        const int c = this->updateAircraftInRange(callsign, vm);
        return c > 0;
    }

    bool CAirspaceMonitor::updateAircraftModel(const CCallsign &callsign, const CAircraftModel &model, const CIdentifier &originator)
    {
        if (CIdentifiable::isMyIdentifier(originator)) { return false; }
        const CPropertyIndexVariantMap vm(CSimulatedAircraft::IndexModel, CVariant::from(model));
        const int c = this->updateAircraftInRange(callsign, vm);
        return c > 0;
    }

    bool CAirspaceMonitor::updateAircraftNetworkModel(const CCallsign &callsign, const CAircraftModel &model, const CIdentifier &originator)
    {
        if (CIdentifiable::isMyIdentifier(originator)) { return false; }
        const CPropertyIndexVariantMap vm(CSimulatedAircraft::IndexNetworkModel, CVariant::from(model));
        const int c = this->updateAircraftInRange(callsign, vm);
        return c > 0;
    }

    bool CAirspaceMonitor::updateFastPositionEnabled(const CCallsign &callsign, bool enableFastPositonUpdates)
    {
        const CPropertyIndexVariantMap vm(CSimulatedAircraft::IndexFastPositionUpdates, CVariant::fromValue(enableFastPositonUpdates));
        const int c = this->updateAircraftInRange(callsign, vm);
        {
            QReadLocker l(&m_lockAircraft);
            CSimulatedAircraftList enabledAircrafts = m_aircraftInRange.findBy(&CSimulatedAircraft::fastPositionUpdates, true);
            m_network->setInterimPositionReceivers(enabledAircrafts.getCallsigns());
        }
        return c > 0;
    }

    bool CAirspaceMonitor::updateAircraftRendered(const CCallsign &callsign, bool rendered)
    {
        const CPropertyIndexVariantMap vm(CSimulatedAircraft::IndexRendered, CVariant::fromValue(rendered));
        const int c = this->updateAircraftInRange(callsign, vm);
        return c > 0;
    }

    bool CAirspaceMonitor::updateAircraftGroundElevation(const CCallsign &callsign, const CElevationPlane &elevation)
    {
        QWriteLocker l(&m_lockAircraft);
        const int c = m_aircraftInRange.setGroundElevation(callsign, elevation.geodeticHeight());
        return c > 0;
    }

    void CAirspaceMonitor::updateMarkAllAsNotRendered()
    {
        QWriteLocker l(&m_lockAircraft);
        m_aircraftInRange.markAllAsNotRendered();
    }

    CFlightPlan CAirspaceMonitor::loadFlightPlanFromNetwork(const CCallsign &callsign)
    {
        CFlightPlan plan;

        // use cache, but not for own callsign (always reload)
        if (this->m_flightPlanCache.contains(callsign)) { plan = this->m_flightPlanCache[callsign]; }
        if (!plan.wasSentOrLoaded() || plan.timeDiffSentOrLoadedMs() > 30 * 1000)
        {
            // outdated, or not in cache at all
            this->m_network->sendFlightPlanQuery(callsign);

            // with this little trick we try to make an asynchronous signal / slot
            // based approach a synchronous return value
            const QTime waitForFlightPlan = QTime::currentTime().addMSecs(1000);
            while (QTime::currentTime() < waitForFlightPlan)
            {
                // process some other events and hope network answer is received already
                QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
                if (m_flightPlanCache.contains(callsign))
                {
                    plan = this->m_flightPlanCache[callsign];
                    break;
                }
            }
        }
        return plan;
    }

    CUserList CAirspaceMonitor::getUsers() const
    {
        CUserList users;
        for (const CAtcStation &station : this->m_atcStationsOnline)
        {
            const CUser user = station.getController();
            users.push_back(user);
        }
        for (const CSimulatedAircraft &aircraft : this->getAircraftInRange())
        {
            const CUser user = aircraft.getPilot();
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
            if (searchList.isEmpty()) break;
            const CCallsign callsign = aircraft.getCallsign();
            if (searchList.contains(callsign))
            {
                const CUser user = aircraft.getPilot();
                users.push_back(user);
                searchList.remove(callsign);
            }
        }

        for (const CAtcStation &station : this->m_atcStationsOnline)
        {
            if (searchList.isEmpty()) break;
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
                CUser user;
                user.setCallsign(callsign);
                users.push_back(user);
            }
            else
            {
                users.push_back(usersByCallsign[0]);
            }
        }
        return users;
    }

    CClientList CAirspaceMonitor::getOtherClientsForCallsigns(const CCallsignSet &callsigns) const
    {
        CClientList clients;
        if (callsigns.isEmpty()) { return clients; }
        clients.push_back(this->getOtherClients().findByCallsigns(callsigns));
        return clients;
    }

    CClient CAirspaceMonitor::getOtherClientOrDefaultForCallsign(const CCallsign &callsign) const
    {
        return this->getOtherClients().findFirstByCallsign(callsign);
    }

    bool CAirspaceMonitor::hasClientInfo(const CCallsign &callsign) const
    {
        return this->getOtherClients().containsCallsign(callsign);
    }

    bool CAirspaceMonitor::isAircraftInRange(const CCallsign &callsign) const
    {
        if (callsign.isEmpty()) { return false; }
        QReadLocker l(&m_lockAircraft);
        return m_aircraftInRange.containsCallsign(callsign);
    }

    CClientList CAirspaceMonitor::getOtherClients() const
    {
        QReadLocker l(&m_lockClient);
        return m_otherClients;
    }

    CAtcStation CAirspaceMonitor::getAtcStationForComUnit(const CComSystem &comSystem)
    {
        CAtcStation station;
        CAtcStationList stations = this->m_atcStationsOnline.findIfComUnitTunedIn25KHz(comSystem);
        if (stations.isEmpty()) { return station; }
        stations.sortByDistanceToOwnAircraft();
        return stations.front();
    }

    void CAirspaceMonitor::enableReverseLookupMessages(bool enabled)
    {
        QWriteLocker l(&m_lockMessages);
        this->m_enableReverseLookupMsgs = enabled;
    }

    bool CAirspaceMonitor::isReverseLookupMessagesEnabled() const
    {
        QReadLocker l(&m_lockMessages);
        return this->m_enableReverseLookupMsgs;
    }

    CStatusMessageList CAirspaceMonitor::getReverseLookupMessages(const CCallsign &callsign) const
    {
        QReadLocker l(&m_lockMessages);
        return this->m_reverseLookupMessages.value(callsign);
    }

    CStatusMessageList CAirspaceMonitor::getAircraftPartsHistory(const CCallsign &callsign) const
    {
        QReadLocker l(&m_lockPartsHistory);
        return this->m_aircraftPartsHistory.value(callsign);
    }

    bool CAirspaceMonitor::isAircraftPartsHistoryEnabled() const
    {
        QReadLocker l(&m_lockPartsHistory);
        return this->m_enableAircraftPartsHistory;
    }

    void CAirspaceMonitor::enableAircraftPartsHistory(bool enabled)
    {
        QWriteLocker l(&m_lockPartsHistory);
        m_enableAircraftPartsHistory = enabled;
    }

    void CAirspaceMonitor::requestDataUpdates()
    {
        if (!this->isConnected()) { return; }
        for (const CSimulatedAircraft &aircraft : this->getAircraftInRange())
        {
            const CCallsign cs(aircraft.getCallsign());
            this->m_network->sendFrequencyQuery(cs);

            // we only query ICAO if we have none yet
            // it happens sometimes with some FSD servers (e.g our testserver) a first query is skipped
            if (!aircraft.hasAircraftDesignator())
            {
                this->m_network->sendIcaoCodesQuery(cs);
            }
        }
    }

    void CAirspaceMonitor::requestAtisUpdates()
    {
        if (!this->isConnected()) { return; }
        for (const CAtcStation &station : this->m_atcStationsOnline)
        {
            this->m_network->sendAtisQuery(station.getCallsign());
        }
    }

    void CAirspaceMonitor::requestAtcBookingsUpdate()
    {
        Q_ASSERT_X(sApp->getWebDataServices(), Q_FUNC_INFO, "missing reader");
        sApp->getWebDataServices()->readInBackground(BlackMisc::Network::CEntityFlags::BookingEntity);
        this->m_bookingsRequested = true;
    }

    void CAirspaceMonitor::testCreateDummyOnlineAtcStations(int number)
    {
        if (number < 1) { return; }
        this->m_atcStationsOnline.push_back(
            BlackMisc::Aviation::CTesting::createAtcStations(number)
        );
        emit this->changedAtcStationsOnline();
    }

    void CAirspaceMonitor::testAddAircraftParts(const BlackMisc::Aviation::CCallsign &callsign, const CAircraftParts &parts, bool incremental)
    {
        this->ps_aircraftConfigReceived(callsign, parts.toJson(), !incremental);
    }

    void CAirspaceMonitor::clear()
    {
        m_flightPlanCache.clear();
        m_tempFsInnPackets.clear();
        removeAllOnlineAtcStations();
        removeAllAircraft();
        removeAllOtherClients();
    }

    void CAirspaceMonitor::gracefulShutdown()
    {
        if (this->m_analyzer) { this->m_analyzer->gracefulShutdown(); }
        QObject::disconnect(this);
    }

    void CAirspaceMonitor::ps_realNameReplyReceived(const CCallsign &callsign, const QString &realname)
    {
        if (!this->isConnected() || realname.isEmpty()) { return; }

        CPropertyIndexVariantMap vm;
        int wasAtc = false;

        if (callsign.hasSuffix())
        {
            // very likely and ATC callsign
            vm = CPropertyIndexVariantMap({CAtcStation::IndexController, CUser::IndexRealName}, realname);
            const int c1 = this->updateOnlineStation(callsign, vm, false, true);
            const int c2 = this->updateBookedStation(callsign, vm, false, true);
            wasAtc = c1 > 0 || c2 > 0;
        }

        if (!wasAtc)
        {
            vm = CPropertyIndexVariantMap({CSimulatedAircraft::IndexPilot, CUser::IndexRealName}, realname);
            this->updateAircraftInRange(callsign, vm);
        }

        // Client
        const CVoiceCapabilities caps = sApp->getWebDataServices()->getVoiceCapabilityForCallsign(callsign);
        vm = CPropertyIndexVariantMap({CClient::IndexUser, CUser::IndexRealName}, realname);
        vm.addValue({ CClient::IndexVoiceCapabilities }, caps);
        this->updateOrAddClient(callsign, vm, false);
    }

    void CAirspaceMonitor::ps_capabilitiesReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, quint32 flags)
    {
        if (!this->isConnected() || callsign.isEmpty()) { return; }
        CPropertyIndexVariantMap capabilities;
        capabilities.addValue(CClient::FsdAtisCanBeReceived, (flags & INetwork::AcceptsAtisResponses));
        capabilities.addValue(CClient::FsdWithInterimPositions, (flags & INetwork::SupportsInterimPosUpdates));
        capabilities.addValue(CClient::FsdWithIcaoCodes, (flags & INetwork::SupportsIcaoCodes));
        capabilities.addValue(CClient::FsdWithAircraftConfig, (flags & INetwork::SupportsAircraftConfigs));

        CPropertyIndexVariantMap vm(CClient::IndexCapabilities, CVariant::from(capabilities));
        const CVoiceCapabilities caps = sApp->getWebDataServices()->getVoiceCapabilityForCallsign(callsign);
        vm.addValue({CClient::IndexVoiceCapabilities}, caps);
        this->updateOrAddClient(callsign, vm, false);

        // for aircraft parts
        if (flags & INetwork::SupportsAircraftConfigs) { m_network->sendAircraftConfigQuery(callsign); }
    }

    void CAirspaceMonitor::ps_serverReplyReceived(const CCallsign &callsign, const QString &server)
    {
        if (!this->isConnected() || callsign.isEmpty() || server.isEmpty()) { return; }
        const CPropertyIndexVariantMap vm(CClient::IndexServer, server);
        this->updateOrAddClient(callsign, vm);
    }

    void CAirspaceMonitor::ps_flightPlanReceived(const CCallsign &callsign, const CFlightPlan &flightPlan)
    {
        CFlightPlan plan(flightPlan);
        plan.setWhenLastSentOrLoaded(QDateTime::currentDateTimeUtc());
        this->m_flightPlanCache.insert(callsign, plan);
    }

    void CAirspaceMonitor::removeAllOnlineAtcStations()
    {
        m_atcStationsOnline.clear();
    }

    void CAirspaceMonitor::removeAllAircraft()
    {
        for (const CSimulatedAircraft &aircraft : getAircraftInRange())
        {
            const CCallsign cs(aircraft.getCallsign());
            emit removedAircraft(cs);
        }

        // locked members
        { QWriteLocker l(&m_lockParts); m_partsByCallsign.clear(); m_aircraftSupportingParts.clear(); }
        { QWriteLocker l(&m_lockSituations); m_situationsByCallsign.clear(); }
        { QWriteLocker l(&m_lockMessages); m_reverseLookupMessages.clear(); }
        { QWriteLocker l(&m_lockAircraft);  m_aircraftInRange.clear(); }

        // non thread safe parts
        m_flightPlanCache.clear();
    }

    void CAirspaceMonitor::removeAllOtherClients()
    {
        QWriteLocker l1(&m_lockClient);
        m_otherClients.clear();
    }

    void CAirspaceMonitor::removeFromAircraftCachesAndLogs(const CCallsign &callsign)
    {
        if (callsign.isEmpty()) { return; }
        this->m_flightPlanCache.remove(callsign);

        QWriteLocker l(&m_lockMessages);
        this->m_reverseLookupMessages.remove(callsign);
    }

    void CAirspaceMonitor::ps_receivedAtcBookings(const CAtcStationList &bookedStations)
    {
        Q_ASSERT(CThreadUtils::isCurrentThreadObjectThread(this));
        if (bookedStations.isEmpty())
        {
            this->m_atcStationsBooked.clear();
        }
        else
        {
            CAtcStationList newBookedStations(bookedStations); // modifyable copy
            for (CAtcStation &bookedStation : newBookedStations)
            {
                // exchange booking and online data, both sides are updated
                this->m_atcStationsOnline.synchronizeWithBookedStation(bookedStation);
            }
            this->m_atcStationsBooked = newBookedStations;
        }
        this->m_bookingsRequested = false; // we already emit here
        emit this->changedAtcStationsBooked(); // all booked stations reloaded
    }

    void CAirspaceMonitor::ps_readUnchangedAtcBookings()
    {
        if (!this->m_bookingsRequested) { return; }
        this->m_bookingsRequested = false;
        emit this->changedAtcStationsBooked(); // treat as stations were changed
    }

    void CAirspaceMonitor::ps_receivedDataFile()
    {
        Q_ASSERT(CThreadUtils::isCurrentThreadObjectThread(this));
        CClientList clients(this->getOtherClients()); // copy
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
        QWriteLocker l(&m_lockClient);
        this->m_otherClients = clients;
    }

    void CAirspaceMonitor::ps_sendReadyForModelMatching(const CCallsign &callsign, int trial)
    {
        Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "missing callsign");
        if (!this->isConnected()) { return; }

        const CSimulatedAircraft remoteAircraft = this->getAircraftInRangeForCallsign(callsign);
        const bool complete = (remoteAircraft.hasValidCallsign() && remoteAircraft.getModel().hasModelString()) || (remoteAircraft.getModel().getModelType() == CAircraftModel::TypeFSInnData);

        if (trial < 2 && !complete)
        {
            this->addReverseLookupMessage(callsign, "Wait for further data");
            QTimer::singleShot(1500, this, [ = ]()
            {
                this->ps_sendReadyForModelMatching(callsign, trial + 1);
            });
            return;
        }

        // some checks for special conditions, e.g. logout -> empty list, but still signals pending
        if (this->isConnected() && remoteAircraft.hasValidCallsign())
        {
            const QString msg = QString("Ready for matching '%1' with model type '%2'").arg(callsign.toQString(), remoteAircraft.getModel().getModelTypeAsString());
            const CStatusMessage m = CMatchingUtils::logMessage(callsign, msg, getLogCategories());
            this->addReverseLookupMessage(callsign, m);
            emit this->readyForModelMatching(remoteAircraft);
        }
        else
        {
            const CStatusMessage m = CMatchingUtils::logMessage(callsign, "Ignoring this aircraft, not found in range list", getLogCategories());
            this->addReverseLookupMessage(callsign, m);
        }
    }

    void CAirspaceMonitor::ps_atcPositionUpdate(const CCallsign &callsign, const BlackMisc::PhysicalQuantities::CFrequency &frequency, const CCoordinateGeodetic &position, const BlackMisc::PhysicalQuantities::CLength &range)
    {
        Q_ASSERT(CThreadUtils::isCurrentThreadObjectThread(this));
        if (!this->isConnected()) { return; }
        const CAtcStationList stationsWithCallsign = this->m_atcStationsOnline.findByCallsign(callsign);
        if (stationsWithCallsign.isEmpty())
        {
            // new station, init with data from data file
            CAtcStation station(sApp->getWebDataServices()->getAtcStationsForCallsign(callsign).frontOrDefault());
            station.setCallsign(callsign);
            station.setRange(range);
            station.setFrequency(frequency);
            station.setPosition(position);
            station.setOnline(true);
            station.calculcateAndUpdateRelativeDistanceAndBearing(getOwnAircraftPosition());

            // sync with bookings
            if (this->m_atcStationsBooked.containsCallsign(callsign))
            {
                CAtcStation bookedStation(this->m_atcStationsBooked.findFirstByCallsign(callsign));
                station.synchronizeWithBookedStation(bookedStation); // both will be aligned
                this->m_atcStationsBooked.replaceIf(&CAtcStation::getCallsign, callsign, bookedStation);
            }

            this->m_atcStationsOnline.push_back(station);

            // subsequent queries
            this->sendInitialAtcQueries(callsign);

            emit this->changedAtcStationsOnline();
            // Remark: this->changedAtcStationOnlineConnectionStatus
            // will be sent in psFsdAtisVoiceRoomReceived
        }
        else
        {
            // update
            CPropertyIndexVariantMap vm;
            vm.addValue(CAtcStation::IndexFrequency, frequency);
            vm.addValue(CAtcStation::IndexPosition, position);
            vm.addValue(CAtcStation::IndexRange, range);
            const int changed = this->m_atcStationsOnline.applyIfCallsign(callsign, vm, true);
            if (changed > 0) { emit this->changedAtcStationsOnline(); }
        }

        this->recallFsInnPacket(callsign);
    }

    void CAirspaceMonitor::ps_atcControllerDisconnected(const CCallsign &callsign)
    {
        Q_ASSERT(CThreadUtils::isCurrentThreadObjectThread(this));

        this->m_otherClients.removeByCallsign(callsign);
        if (this->m_atcStationsOnline.containsCallsign(callsign))
        {
            const CAtcStation removedStation = this->m_atcStationsOnline.findFirstByCallsign(callsign);
            this->m_atcStationsOnline.removeByCallsign(callsign);
            emit this->changedAtcStationsOnline();
            emit this->changedAtcStationOnlineConnectionStatus(removedStation, false);
        }

        // booked
        this->updateBookedStation(callsign, CPropertyIndexVariantMap(CAtcStation::IndexIsOnline, CVariant::from(false)), true, false);
    }

    void CAirspaceMonitor::ps_atisReceived(const CCallsign &callsign, const CInformationMessage &atisMessage)
    {
        Q_ASSERT(CThreadUtils::isCurrentThreadObjectThread(this));
        if (!this->isConnected() || callsign.isEmpty()) return;
        CPropertyIndexVariantMap vm(CAtcStation::IndexAtis, CVariant::from(atisMessage));
        this->updateOnlineStation(callsign, vm);

        // receiving an ATIS means station is online, update in bookings
        vm.addValue(CAtcStation::IndexIsOnline, true);
        this->updateBookedStation(callsign, vm);
    }

    void CAirspaceMonitor::ps_atisVoiceRoomReceived(const CCallsign &callsign, const QString &url)
    {
        Q_ASSERT(CThreadUtils::isCurrentThreadObjectThread(this));
        if (!this->isConnected()) { return; }
        const QString trimmedUrl = url.trimmed();
        CPropertyIndexVariantMap vm({ CAtcStation::IndexVoiceRoom, CVoiceRoom::IndexUrl }, trimmedUrl);
        const int changedOnline = this->updateOnlineStation(callsign, vm, true, true);
        if (changedOnline < 1) { return; }

        Q_ASSERT(changedOnline == 1);
        CAtcStation station = this->m_atcStationsOnline.findFirstByCallsign(callsign);
        emit this->changedAtcStationOnlineConnectionStatus(station, true); // send when voice room url is available

        vm.addValue(CAtcStation::IndexIsOnline, true); // with voice room ATC is online
        this->updateBookedStation(callsign, vm);

        // receiving voice room means ATC has voice
        vm = CPropertyIndexVariantMap(CClient::IndexVoiceCapabilities, CVariant::from(CVoiceCapabilities::fromVoiceCapabilities(CVoiceCapabilities::Voice)));
        this->updateOrAddClient(callsign, vm, false);
    }

    void CAirspaceMonitor::ps_atisLogoffTimeReceived(const CCallsign &callsign, const QString &zuluTime)
    {
        Q_ASSERT(CThreadUtils::isCurrentThreadObjectThread(this));
        if (!this->isConnected()) { return; }
        if (zuluTime.length() == 4)
        {
            // Logic to set logoff time
            bool ok;
            const int h = zuluTime.left(2).toInt(&ok);
            if (!ok) { return; }
            const int m = zuluTime.right(2).toInt(&ok);
            if (!ok) { return; }
            QDateTime logoffDateTime = QDateTime::currentDateTimeUtc();
            logoffDateTime.setTime(QTime(h, m));

            const CPropertyIndexVariantMap vm(CAtcStation::IndexBookedUntil, CVariant(logoffDateTime));
            this->updateOnlineStation(callsign, vm);
            this->updateBookedStation(callsign, vm);
        }
    }

    void CAirspaceMonitor::ps_customFSInnPacketReceived(const CCallsign &callsign, const QString &airlineIcaoDesignator, const QString &aircraftIcaoDesignator, const QString &combinedAircraftType, const QString &modelString)
    {
        // it can happen this is called before any questions
        // ES sends FsInn packets for callsigns such as ACCGER1, which are hard to distinguish
        // 1) checking if they are already in the list checks again ATC position which is safe
        // 2) the ATC alike callsign check is guessing

        Q_ASSERT_X(CThreadUtils::isCurrentThreadObjectThread(this), Q_FUNC_INFO, "not in main thread");
        BLACK_VERIFY_X(callsign.isValid(), Q_FUNC_INFO, "invalid callsign");
        if (!callsign.isValid()) { return; }
        if (!this->isConnected()) { return; }

        const bool isAircraft = this->m_aircraftInRange.containsCallsign(callsign);
        const bool isAtc = this->m_atcStationsOnline.containsCallsign(callsign);
        if (!isAircraft && !isAtc)
        {
            // we have no idea what we are dealing with, so we store it
            FsInnPacket fsInn(aircraftIcaoDesignator, airlineIcaoDesignator, combinedAircraftType, modelString);
            m_tempFsInnPackets[callsign] = fsInn;
            return;
        }

        // Request of other client, I can get the other's model from that
        const CPropertyIndexVariantMap vm(CClient::IndexModelString, modelString);
        this->updateOrAddClient(callsign, vm);

        if (isAircraft)
        {
            CStatusMessageList reverseLookupMessages;
            CStatusMessageList *pReverseLookupMessages = this->isReverseLookupMessagesEnabled() ? &reverseLookupMessages : nullptr;
            CMatchingUtils::addLogDetailsToList(pReverseLookupMessages, callsign, QString("FsInn data from network: aircraft '%1', airline '%2', model '%3', combined '%4'").
                                                arg(aircraftIcaoDesignator).arg(airlineIcaoDesignator).
                                                arg(modelString).arg(combinedAircraftType));

            this->addOrUpdateAircraftInRange(callsign, aircraftIcaoDesignator, airlineIcaoDesignator, "", modelString, CAircraftModel::TypeFSInnData, pReverseLookupMessages);
            this->addReverseLookupMessages(callsign, reverseLookupMessages);
            this->ps_sendReadyForModelMatching(callsign);
        }
    }

    void CAirspaceMonitor::ps_icaoCodesReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &aircraftIcaoDesignator, const QString &airlineIcaoDesignator, const QString &livery)
    {
        Q_ASSERT_X(CThreadUtils::isCurrentThreadObjectThread(this), Q_FUNC_INFO, "not in main thread");
        BLACK_VERIFY_X(callsign.isValid(), Q_FUNC_INFO, "invalid callsign");
        if (!callsign.isValid()) { return; }
        if (!this->isConnected()) { return; }

        CStatusMessageList reverseLookupMessages;
        CStatusMessageList *pReverseLookupMessages = this->isReverseLookupMessagesEnabled() ? &reverseLookupMessages : nullptr;
        CMatchingUtils::addLogDetailsToList(pReverseLookupMessages, callsign, QString("Data from network: aircraft '%1', airline '%2', livery '%3'").
                                            arg(aircraftIcaoDesignator).arg(airlineIcaoDesignator).arg(livery),
                                            getLogCategories());

        const CClient client = this->getOtherClientOrDefaultForCallsign(callsign);
        this->addOrUpdateAircraftInRange(callsign, aircraftIcaoDesignator, airlineIcaoDesignator, livery, client.getQueriedModelString(), CAircraftModel::TypeQueriedFromNetwork, pReverseLookupMessages);
        this->addReverseLookupMessages(callsign, reverseLookupMessages);
        this->ps_sendReadyForModelMatching(callsign);

        emit this->requestedNewAircraft(callsign, aircraftIcaoDesignator, airlineIcaoDesignator, livery);
    }

    void CAirspaceMonitor::addReverseLookupMessages(const CCallsign &callsign, const CStatusMessageList &messages)
    {
        if (callsign.isEmpty()) { return; }
        if (messages.isEmpty()) { return; }
        QWriteLocker l(&m_lockMessages);
        if (!this->m_enableReverseLookupMsgs) { return; }
        if (this->m_reverseLookupMessages.contains(callsign))
        {
            CStatusMessageList &msgs = this->m_reverseLookupMessages[callsign];
            msgs.push_back(messages);
        }
        else
        {
            this->m_reverseLookupMessages.insert(callsign, messages);
        }
    }

    void CAirspaceMonitor::addReverseLookupMessage(const CCallsign &callsign, const CStatusMessage &message)
    {
        if (callsign.isEmpty()) { return; }
        if (message.isEmpty()) { return; }
        this->addReverseLookupMessages(callsign, CStatusMessageList({ message }));
    }

    void CAirspaceMonitor::addReverseLookupMessage(const CCallsign &callsign, const QString &message, CStatusMessage::StatusSeverity severity)
    {
        if (callsign.isEmpty()) { return; }
        if (message.isEmpty()) { return; }
        const CStatusMessage m = CMatchingUtils::logMessage(callsign, message, getLogCategories(), severity);
        this->addReverseLookupMessage(callsign, m);
    }

    bool CAirspaceMonitor::addNewAircraftInRange(const CSimulatedAircraft &aircraft)
    {
        const CCallsign callsign = aircraft.getCallsign();
        Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "Missing callsign");
        if (this->isAircraftInRange(callsign)) { return false; }

        CSimulatedAircraft newAircraft(aircraft);
        newAircraft.calculcateAndUpdateRelativeDistanceAndBearing(this->getOwnAircraftPosition()); // distance from myself
        sApp->getWebDataServices()->updateWithVatsimDataFileData(newAircraft);

        // store
        {
            QWriteLocker l(&m_lockAircraft);
            this->m_aircraftInRange.push_back(newAircraft);
        }
        emit this->addedAircraft(aircraft);
        emit this->changedAircraftInRange();
        return true;
    }

    int CAirspaceMonitor::updateAircraftInRange(const CCallsign &callsign, const CPropertyIndexVariantMap &vm, bool skipEqualValues)
    {
        Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "Missing callsign");
        int c = 0;
        {
            QWriteLocker l(&m_lockAircraft);
            c = this->m_aircraftInRange.applyIfCallsign(callsign, vm, skipEqualValues);
        }
        if (c > 0)
        {
            emit this->changedAircraftInRange();
        }
        return c;
    }

    bool CAirspaceMonitor::addNewClient(const CClient &client)
    {
        const CCallsign callsign = client.getCallsign();
        Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "invalid callsign");
        if (this->hasClientInfo(callsign)) { return false; }
        QWriteLocker l(&m_lockClient);
        this->m_otherClients.push_back(client);
        return true;
    }

    int CAirspaceMonitor::updateOrAddClient(const CCallsign &callsign, const CPropertyIndexVariantMap &vm, bool skipEqualValues)
    {
        Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "Missing callsign");
        int c = 0;
        if (!this->hasClientInfo(callsign))
        {
            CClient client(callsign);
            c = client.apply(vm).size();
            this->addNewClient(client);
        }
        else
        {
            QWriteLocker l(&m_lockClient);
            c = this->m_otherClients.applyIfCallsign(callsign, vm, skipEqualValues);
        }
        return c;
    }

    int CAirspaceMonitor::updateOnlineStation(const CCallsign &callsign, const CPropertyIndexVariantMap &vm, bool skipEqualValues, bool sendSignal)
    {
        const int c = this->m_atcStationsOnline.applyIfCallsign(callsign, vm, skipEqualValues);
        if (c > 0 && sendSignal)
        {
            emit this->changedAtcStationsOnline();
        }
        return c;
    }

    int CAirspaceMonitor::updateBookedStation(const CCallsign &callsign, const CPropertyIndexVariantMap &vm, bool skipEqualValues, bool sendSignal)
    {
        // do not used applyFirst here, more stations wit callsign at a time
        const int c = this->m_atcStationsBooked.applyIfCallsign(callsign, vm, skipEqualValues);
        if (c > 0 && sendSignal)
        {
            emit this->changedAtcStationsBooked();
        }
        return c;
    }

    void CAirspaceMonitor::recallFsInnPacket(const CCallsign &callsign)
    {
        if (!m_tempFsInnPackets.contains(callsign)) { return; }
        const FsInnPacket packet = m_tempFsInnPackets[callsign];
        m_tempFsInnPackets.remove(callsign);
        this->ps_customFSInnPacketReceived(callsign, packet.airlineIcaoDesignator, packet.aircraftIcaoDesignator, packet.combinedCode, packet.modelString);
    }

    CSimulatedAircraft CAirspaceMonitor::addOrUpdateAircraftInRange(const CCallsign &callsign, const QString &aircraftIcao, const QString &airlineIcao, const QString &livery, const QString &modelString, CAircraftModel::ModelType type, CStatusMessageList *log)
    {
        CSimulatedAircraft aircraft = this->getAircraftInRangeForCallsign(callsign);
        if (aircraft.hasValidCallsign())
        {
            CAircraftModel model = CAircraftMatcher::reverselLookupModel(callsign, aircraftIcao, airlineIcao, livery, modelString, type, log);
            model.updateMissingParts(aircraft.getModel());
            // Use anonymous as originator here, since the remote aircraft provider is ourselves and the call to updateAircraftModel() would
            // return without doing anything.
            this->updateAircraftModel(callsign, model, CIdentifier::anonymous());
            this->updateAircraftNetworkModel(callsign, model, CIdentifier::anonymous());
        }
        else
        {
            aircraft = CAirspaceMonitor::initNewAircraft(callsign, aircraftIcao, airlineIcao, livery, modelString, type, log);
            this->addNewAircraftInRange(aircraft);
        }
        return aircraft;
    }

    CSimulatedAircraft CAirspaceMonitor::initNewAircraft(const CCallsign &callsign, const QString &aircraftIcao, const QString &airlineIcao, const QString &livery, const QString &modelString, CAircraftModel::ModelType type, CStatusMessageList *log)
    {
        const CAircraftModel model = CAircraftMatcher::reverselLookupModel(callsign, aircraftIcao, airlineIcao, livery, modelString, type, log);
        const CSimulatedAircraft aircraft(model);
        return aircraft;
    }

    void CAirspaceMonitor::ps_aircraftUpdateReceived(const CAircraftSituation &situation, const CTransponder &transponder)
    {
        Q_ASSERT_X(CThreadUtils::isCurrentThreadObjectThread(this), Q_FUNC_INFO, "Called in different thread");
        if (!this->isConnected()) { return; }

        const CCallsign callsign(situation.getCallsign());
        Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "Empty callsign");

        // store situation history
        this->storeAircraftSituation(situation);
        emit this->addedAircraftSituation(situation);

        const bool existsInRange = this->isAircraftInRange(callsign);
        const bool hasFsInnPacket = this->m_tempFsInnPackets.contains(callsign);

        if (!existsInRange)
        {
            CSimulatedAircraft aircraft;
            aircraft.setCallsign(callsign);
            aircraft.setSituation(situation);
            aircraft.setTransponder(transponder);
            this->addNewAircraftInRange(aircraft);
            this->sendInitialPilotQueries(callsign, !hasFsInnPacket);

            // new client, there is a chance it has been already created by custom packet
            const CClient c(callsign);
            this->addNewClient(c);
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

        this->recallFsInnPacket(callsign);
    }

    void CAirspaceMonitor::ps_aircraftInterimUpdateReceived(const CAircraftSituation &situation)
    {
        Q_ASSERT_X(CThreadUtils::isCurrentThreadObjectThread(this), Q_FUNC_INFO, "Called in different thread");
        if (!this->isConnected()) { return; }

        const CCallsign callsign(situation.getCallsign());
        Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "Empty callsign");

        // Interim packets do not have groundspeed, hence set the last known value.
        // If there is no full position available yet, throw this interim position away.
        CAircraftSituation interimSituation(situation);
        CAircraftSituationList history;
        {
            QReadLocker l(&m_lockSituations);
            history = this->m_situationsByCallsign[callsign];
        }
        if (history.empty()) { return; } // we need one full situation at least
        const CAircraftSituation lastSituation = history.latestObject();
        if (lastSituation.getPosition() == interimSituation.getPosition()) { return; } // same position, ignore

        // changed position, continue and copy values
        interimSituation.setCurrentUtcTime();
        interimSituation.setGroundSpeed(lastSituation.getGroundSpeed());

        // store situation history
        this->storeAircraftSituation(interimSituation);
        emit this->addedAircraftSituation(interimSituation);

        // if we have not aircraft in range yer, we stop here
        if (!this->isAircraftInRange(callsign)) { return; }

        // update aircraft
        CPropertyIndexVariantMap vm;
        vm.addValue(CSimulatedAircraft::IndexSituation, interimSituation);
        vm.addValue(CSimulatedAircraft::IndexRelativeDistance, this->calculateDistanceToOwnAircraft(interimSituation));
        vm.addValue(CSimulatedAircraft::IndexRelativeBearing, this->calculateBearingToOwnAircraft(interimSituation));
        this->updateAircraftInRange(callsign, vm);
    }

    void CAirspaceMonitor::ps_connectionStatusChanged(INetwork::ConnectionStatus oldStatus, INetwork::ConnectionStatus newStatus)
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

    void CAirspaceMonitor::ps_pilotDisconnected(const CCallsign &callsign)
    {
        Q_ASSERT(CThreadUtils::isCurrentThreadObjectThread(this));

        // in case of inconsistencies I always remove here
        this->removeFromAircraftCachesAndLogs(callsign);

        { QWriteLocker l1(&m_lockParts); m_partsByCallsign.remove(callsign); m_aircraftSupportingParts.remove(callsign); }
        { QWriteLocker l2(&m_lockSituations); m_situationsByCallsign.remove(callsign); }
        { QWriteLocker l3(&m_lockClient); m_otherClients.removeByCallsign(callsign); }
        { QWriteLocker l4(&m_lockPartsHistory); m_aircraftPartsHistory.remove(callsign); }

        bool removedCallsign = false;
        {
            QWriteLocker l(&m_lockAircraft);
            int c = this->m_aircraftInRange.removeByCallsign(callsign);
            removedCallsign = c > 0;
        }
        if (removedCallsign) { emit this->removedAircraft(callsign); }
    }

    void CAirspaceMonitor::ps_frequencyReceived(const CCallsign &callsign, const CFrequency &frequency)
    {
        Q_ASSERT(CThreadUtils::isCurrentThreadObjectThread(this));

        // update
        const CPropertyIndexVariantMap vm( {CSimulatedAircraft::IndexCom1System, CComSystem::IndexActiveFrequency}, CVariant::from(frequency));
        this->updateAircraftInRange(callsign, vm);
    }

    void CAirspaceMonitor::ps_aircraftConfigReceived(const BlackMisc::Aviation::CCallsign &callsign, const QJsonObject &jsonObject, bool isFull)
    {
        Q_ASSERT(CThreadUtils::isCurrentThreadObjectThread(this));
        CSimulatedAircraft simAircraft(getAircraftInRangeForCallsign(callsign));

        // If we are not yet synchronized, we throw away any incremental packet
        if (!simAircraft.hasValidCallsign()) { return; }
        if (!simAircraft.isPartsSynchronized() && !isFull) { return; }

        CAircraftParts parts;
        try
        {
            if (isFull)
            {
                parts.convertFromJson(jsonObject);
            }
            else
            {
                // incremental update
                parts = this->remoteAircraftParts(callsign).frontOrDefault();
                QJsonObject config = applyIncrementalObject(parts.toJson(), jsonObject);
                parts.convertFromJson(config);
            }
        }
        catch (const CJsonException &ex)
        {
            CStatusMessage message = ex.toStatusMessage(this, "Invalid parts packet");
            message.setSeverity(CStatusMessage::SeverityDebug);
            CLogMessage::preformatted(message);
        }

        // make sure in any case right time
        parts.setCurrentUtcTime();

        // store part history (parts always absolute)
        this->storeAircraftParts(callsign, parts);
        emit this->addedAircraftParts(callsign, parts);

        if (m_enableAircraftPartsHistory)
        {
            QJsonDocument doc(jsonObject);
            QString partsAsString = doc.toJson(QJsonDocument::Compact);
            CStatusMessage message(getLogCategories(), BlackMisc::CStatusMessage::SeverityInfo, callsign.isEmpty() ? callsign.toQString() + ": " + partsAsString.trimmed() : partsAsString.trimmed());
            if (this->m_aircraftPartsHistory.contains(callsign))
            {
                CStatusMessageList &msgs = this->m_aircraftPartsHistory[callsign];
                msgs.push_back(message);
            }
            else
            {
                this->m_aircraftPartsHistory.insert(callsign, message);
            }
        }

        // here I expect always a changed value
        QWriteLocker l(&m_lockAircraft);
        this->m_aircraftInRange.setAircraftParts(callsign, parts);
    }

    void CAirspaceMonitor::storeAircraftSituation(const CAircraftSituation &situation)
    {
        const CCallsign callsign(situation.getCallsign());
        BLACK_VERIFY_X(!callsign.isEmpty(), Q_FUNC_INFO, "empty callsign");
        if (callsign.isEmpty()) { return; }

        // list from new to old
        QWriteLocker lock(&m_lockSituations);
        CAircraftSituationList &situationList = this->m_situationsByCallsign[callsign];
        situationList.push_frontMaxElements(situation, MaxSituationsPerCallsign);

        // check sort order
        Q_ASSERT_X(situationList.size() < 2 || situationList[0].getMSecsSinceEpoch() >= situationList[1].getMSecsSinceEpoch(), Q_FUNC_INFO, "wrong sort order");

        // a full position update received after an interim position update should use the time offset of the interim position
        if (situationList.size() >= 2 && (situationList[0].isInterim() || situationList[1].isInterim()))
        {
            situationList[0].setTimeOffsetMs(std::min(situationList[0].getTimeOffsetMs(), situationList[1].getTimeOffsetMs()));
        }
    }

    void CAirspaceMonitor::storeAircraftParts(const CCallsign &callsign, const CAircraftParts &parts)
    {
        BLACK_VERIFY_X(!callsign.isEmpty(), Q_FUNC_INFO, "empty callsign");
        if (callsign.isEmpty()) { return; }

        // get time offset from situation
        qint64 timeOffsetMs = CNetworkVatlib::c_positionTimeOffsetMsec;
        {
            QReadLocker lock(&m_lockSituations);
            const CAircraftSituationList &situationList = this->m_situationsByCallsign[callsign];
            if (!situationList.isEmpty()) { timeOffsetMs = situationList[0].getTimeOffsetMs(); }
        }

        // list sorted from new to old
        QWriteLocker lock(&m_lockParts);
        CAircraftPartsList &partsList = this->m_partsByCallsign[callsign];
        partsList.push_front(parts);
        partsList.front().setTimeOffsetMs(timeOffsetMs);

        // remove outdated parts (but never remove the most recent one)
        IRemoteAircraftProvider::removeOutdatedParts(partsList);

        // aircraft supporting parts
        if (!m_aircraftSupportingParts.contains(callsign))
        {
            m_aircraftSupportingParts.push_back(callsign); // mark as callsign which supports parts
        }

        // check sort order
        Q_ASSERT_X(partsList.size() < 2 || partsList[0].getMSecsSinceEpoch() >= partsList[1].getMSecsSinceEpoch(), Q_FUNC_INFO, "wrong sort order");
    }

    void CAirspaceMonitor::sendInitialAtcQueries(const CCallsign &callsign)
    {
        if (!this->isConnected()) { return; }
        this->m_network->sendRealNameQuery(callsign);
        this->m_network->sendAtisQuery(callsign); // request ATIS and voice rooms
        this->m_network->sendCapabilitiesQuery(callsign);
        this->m_network->sendServerQuery(callsign);
    }

    void CAirspaceMonitor::sendInitialPilotQueries(const CCallsign &callsign, bool withFsInn)
    {
        if (!this->isConnected()) { return; }
        this->m_network->sendFrequencyQuery(callsign);
        this->m_network->sendRealNameQuery(callsign);
        this->m_network->sendCapabilitiesQuery(callsign);
        this->m_network->sendServerQuery(callsign);
        if (withFsInn)
        {
            this->m_network->sendCustomFsinnQuery(callsign);
        }
    }

    bool CAirspaceMonitor::isConnected() const
    {
        return this->m_network && this->m_network->isConnected();
    }

    CLength CAirspaceMonitor::calculateDistanceToOwnAircraft(const CAircraftSituation &situation) const
    {
        CLength distance = getOwnAircraft().calculateGreatCircleDistance(situation);
        distance.switchUnit(CLengthUnit::NM());
        return distance;
    }

    CAngle CAirspaceMonitor::calculateBearingToOwnAircraft(const CAircraftSituation &situation) const
    {
        CAngle angle = getOwnAircraft().calculateBearing(situation);
        angle.switchUnit(CAngleUnit::deg());
        return angle;
    }

    CAirspaceMonitor::FsInnPacket::FsInnPacket(const QString &aircraftIcaoDesignator, const QString &airlineIcaoDesignator, const QString &combinedCode, const QString &modelString) :
        aircraftIcaoDesignator(aircraftIcaoDesignator.trimmed().toUpper()), airlineIcaoDesignator(airlineIcaoDesignator.trimmed().toUpper()), combinedCode(combinedCode.trimmed().toUpper()), modelString(modelString.trimmed())
    { }

} // namespace
