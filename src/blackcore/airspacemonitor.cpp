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
#include "blackcore/network.h"
#include "blackcore/vatsim/vatsimbookingreader.h"
#include "blackcore/vatsim/vatsimdatafilereader.h"
#include "blackcore/webdataservices.h"
#include "blackmisc/audio/voiceroom.h"
#include "blackmisc/aviation/aircraftparts.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/aviation/modulator.h"
#include "blackmisc/aviation/transponder.h"
#include "blackmisc/compare.h"
#include "blackmisc/iterator.h"
#include "blackmisc/json.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/network/client.h"
#include "blackmisc/network/user.h"
#include "blackmisc/network/voicecapabilities.h"
#include "blackmisc/pq/units.h"
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
          m_network(network),
          m_analyzer(new CAirspaceAnalyzer(ownAircraftProvider, this, network, this))
    {
        this->setObjectName("CAirspaceMonitor");

        this->connect(this->m_network, &INetwork::atcPositionUpdate, this, &CAirspaceMonitor::ps_atcPositionUpdate);
        this->connect(this->m_network, &INetwork::atisReplyReceived, this, &CAirspaceMonitor::ps_atisReceived);
        this->connect(this->m_network, &INetwork::atisVoiceRoomReplyReceived, this, &CAirspaceMonitor::ps_atisVoiceRoomReceived);
        this->connect(this->m_network, &INetwork::atisLogoffTimeReplyReceived, this, &CAirspaceMonitor::ps_atisLogoffTimeReceived);
        this->connect(this->m_network, &INetwork::metarReplyReceived, this, &CAirspaceMonitor::ps_metarReceived);
        this->connect(this->m_network, &INetwork::flightPlanReplyReceived, this, &CAirspaceMonitor::ps_flightPlanReceived);
        this->connect(this->m_network, &INetwork::realNameReplyReceived, this, &CAirspaceMonitor::ps_realNameReplyReceived);
        this->connect(this->m_network, &INetwork::icaoCodesReplyReceived, this, &CAirspaceMonitor::ps_icaoCodesReceived);
        this->connect(this->m_network, &INetwork::pilotDisconnected, this, &CAirspaceMonitor::ps_pilotDisconnected);
        this->connect(this->m_network, &INetwork::atcDisconnected, this, &CAirspaceMonitor::ps_atcControllerDisconnected);
        this->connect(this->m_network, &INetwork::aircraftPositionUpdate, this, &CAirspaceMonitor::ps_aircraftUpdateReceived);
        this->connect(this->m_network, &INetwork::aircraftInterimPositionUpdate, this, &CAirspaceMonitor::ps_aircraftInterimUpdateReceived);
        this->connect(this->m_network, &INetwork::frequencyReplyReceived, this, &CAirspaceMonitor::ps_frequencyReceived);
        this->connect(this->m_network, &INetwork::capabilitiesReplyReceived, this, &CAirspaceMonitor::ps_capabilitiesReplyReceived);
        this->connect(this->m_network, &INetwork::customFSinnPacketReceived, this, &CAirspaceMonitor::ps_customFSinnPacketReceived);
        this->connect(this->m_network, &INetwork::serverReplyReceived, this, &CAirspaceMonitor::ps_serverReplyReceived);
        this->connect(this->m_network, &INetwork::aircraftConfigPacketReceived, this, &CAirspaceMonitor::ps_aircraftConfigReceived);

        // AutoConnection: this should also avoid race conditions by updating the bookings
        Q_ASSERT_X(sApp->getWebDataServices(), Q_FUNC_INFO, "Missing data reader");
        this->connect(sApp->getWebDataServices()->getBookingReader(), &CVatsimBookingReader::atcBookingsRead, this, &CAirspaceMonitor::ps_receivedBookings);
        this->connect(sApp->getWebDataServices()->getDataFileReader(), &CVatsimDataFileReader::dataFileRead, this, &CAirspaceMonitor::ps_receivedDataFile);

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
        QMetaObject::Connection c1 = connect(this, &CAirspaceMonitor::addedAircraftSituation, receiver, situationSlot);
        Q_ASSERT_X(c1, Q_FUNC_INFO, "connect failed");
        QMetaObject::Connection c2 = connect(this, &CAirspaceMonitor::addedAircraftParts, receiver, partsSlot);
        Q_ASSERT_X(c2, Q_FUNC_INFO, "connect failed");
        QMetaObject::Connection c3 = connect(this, &CAirspaceMonitor::removedAircraft, receiver, removedAircraftSlot);
        Q_ASSERT_X(c3, Q_FUNC_INFO, "connect failed");
        // trick is to use the Queued signal here
        // analyzer (own thread) -> airspaceAircraftSnapshot -> AirspaceMonitor -> airspaceAircraftSnapshot queued in main thread
        QMetaObject::Connection c4 = this->connect(this->m_analyzer, &CAirspaceAnalyzer::airspaceAircraftSnapshot, receiver, aircraftSnapshotSlot, Qt::QueuedConnection);
        Q_ASSERT_X(c4, Q_FUNC_INFO, "connect failed");
        return QList<QMetaObject::Connection>({ c1, c2, c3, c4});
    }

    bool CAirspaceMonitor::updateAircraftEnabled(const CCallsign &callsign, bool enabledForRedering, const BlackMisc::CIdentifier &originator)
    {
        Q_UNUSED(originator);
        CPropertyIndexVariantMap vm(CSimulatedAircraft::IndexEnabled, CVariant::fromValue(enabledForRedering));
        QWriteLocker l(&m_lockAircraft);
        const int c = m_aircraftInRange.applyIfCallsign(callsign, vm);
        return c > 0;
    }

    bool CAirspaceMonitor::updateAircraftModel(const CCallsign &callsign, const CAircraftModel &model, const BlackMisc::CIdentifier &originator)
    {
        Q_UNUSED(originator);
        QWriteLocker l(&m_lockAircraft);
        CPropertyIndexVariantMap vm(CSimulatedAircraft::IndexModel, CVariant::from(model));
        const int c = m_aircraftInRange.applyIfCallsign(callsign, vm);
        return c > 0;
    }

    bool CAirspaceMonitor::updateFastPositionEnabled(const CCallsign &callsign, bool enableFastPositonUpdates, const BlackMisc::CIdentifier &originator)
    {
        //! \fixme CAirspaceMonitor: If in the long term originator is not used, remove from signature
        Q_UNUSED(originator);
        CPropertyIndexVariantMap vm(CSimulatedAircraft::IndexFastPositionUpdates, CVariant::fromValue(enableFastPositonUpdates));
        QWriteLocker l(&m_lockAircraft);
        int c = m_aircraftInRange.applyIfCallsign(callsign, vm);
        CSimulatedAircraftList enabledAircrafts = m_aircraftInRange.findBy(&CSimulatedAircraft::fastPositionUpdates, true);
        m_network->setInterimPositionReceivers(enabledAircrafts.getCallsigns());
        return c > 0;
    }

    bool CAirspaceMonitor::updateAircraftRendered(const CCallsign &callsign, bool rendered, const BlackMisc::CIdentifier &originator)
    {
        //! \fixme CAirspaceMonitor: If in the long term originator is not used, remove from signature
        Q_UNUSED(originator);
        CPropertyIndexVariantMap vm(CSimulatedAircraft::IndexRendered, CVariant::fromValue(rendered));
        QWriteLocker l(&m_lockAircraft);
        int c = m_aircraftInRange.applyIfCallsign(callsign, vm);
        return c > 0;
    }

    void CAirspaceMonitor::updateMarkAllAsNotRendered(const BlackMisc::CIdentifier &originator)
    {
        //! \fixme CAirspaceMonitor: If in the long term originator is not used, remove from signature
        Q_UNUSED(originator);
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
            QTime waitForFlightPlan = QTime::currentTime().addMSecs(1000);
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
        CSimulatedAircraft myAircraft(getOwnAircraft());
        if (!myAircraft.getCallsign().isEmpty() && searchList.contains(myAircraft.getCallsign()))
        {
            searchList.remove(myAircraft.getCallsign());
            users.push_back(myAircraft.getPilot());
        }

        // do aircraft first, this will handle most callsigns
        for (const CSimulatedAircraft &aircraft : this->getAircraftInRange())
        {
            if (searchList.isEmpty()) break;
            CCallsign callsign = aircraft.getCallsign();
            if (searchList.contains(callsign))
            {
                CUser user = aircraft.getPilot();
                users.push_back(user);
                searchList.remove(callsign);
            }
        }

        for (const CAtcStation &station : this->m_atcStationsOnline)
        {
            if (searchList.isEmpty()) break;
            CCallsign callsign = station.getCallsign();
            if (searchList.contains(callsign))
            {
                CUser user = station.getController();
                users.push_back(user);
                searchList.remove(callsign);
            }
        }

        // we might have unresolved callsigns
        // those are the ones not in range
        for (const CCallsign &callsign : searchList)
        {
            CUserList usersByCallsign = sApp->getWebDataServices()->getUsersForCallsign(callsign);
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

    bool CAirspaceMonitor::isInRange(const CCallsign &callsign) const
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

    void CAirspaceMonitor::requestDataUpdates()
    {
        if (!this->m_network->isConnected()) { return; }
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
        if (!this->m_network->isConnected()) { return; }
        for (const CAtcStation &station : this->m_atcStationsOnline)
        {
            this->m_network->sendAtisQuery(station.getCallsign());
        }
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
        m_modelTemporaryCache.clear();

        removeAllOnlineAtcStations();
        removeAllAircraft();
        removeAllOtherClients();
    }

    void CAirspaceMonitor::setConnected(bool connected)
    {
        this->m_connected = connected;
        if (!connected)
        {
            this->clear();
        }
    }

    void CAirspaceMonitor::gracefulShutdown()
    {
        if (this->m_analyzer) { this->m_analyzer->gracefulShutdown(); }
        QObject::disconnect(this);
    }

    void CAirspaceMonitor::ps_realNameReplyReceived(const CCallsign &callsign, const QString &realname)
    {
        if (!this->m_connected || realname.isEmpty()) { return; }

        CPropertyIndexVariantMap vm;
        int wasAtc = false;
        if (callsign.hasSuffix())
        {
            // very likely and ATC callsign
            vm = CPropertyIndexVariantMap({CAtcStation::IndexController, CUser::IndexRealName}, realname);
            int c1 = this->m_atcStationsOnline.applyIf(&CAtcStation::getCallsign, callsign, vm);
            int c2 = this->m_atcStationsBooked.applyIf(&CAtcStation::getCallsign, callsign, vm);
            wasAtc = c1 > 0 || c2 > 0;
        }

        if (!wasAtc)
        {
            vm = CPropertyIndexVariantMap({CSimulatedAircraft::IndexPilot, CUser::IndexRealName}, realname);

            // lock block
            {
                QWriteLocker l(&m_lockAircraft);
                this->m_aircraftInRange.applyIf(&CSimulatedAircraft::getCallsign, callsign, vm);
            }
        }

        // Client
        const CVoiceCapabilities caps = sApp->getWebDataServices()->getVoiceCapabilityForCallsign(callsign);
        vm = CPropertyIndexVariantMap({CClient::IndexUser, CUser::IndexRealName}, realname);
        vm.addValue({ CClient::IndexVoiceCapabilities }, caps);
        {
            QWriteLocker l(&m_lockClient);
            if (!this->m_otherClients.containsCallsign(callsign)) { this->m_otherClients.push_back(CClient(callsign)); }
            this->m_otherClients.applyIf(&CClient::getCallsign, callsign, vm);
        }
    }

    void CAirspaceMonitor::ps_capabilitiesReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, quint32 flags)
    {
        if (!this->m_connected || callsign.isEmpty()) { return; }
        CPropertyIndexVariantMap capabilities;
        capabilities.addValue(CClient::FsdAtisCanBeReceived, (flags & INetwork::AcceptsAtisResponses));
        capabilities.addValue(CClient::FsdWithInterimPositions, (flags & INetwork::SupportsInterimPosUpdates));
        capabilities.addValue(CClient::FsdWithIcaoCodes, (flags & INetwork::SupportsIcaoCodes));
        capabilities.addValue(CClient::FsdWithAircraftConfig, (flags & INetwork::SupportsAircraftConfigs));

        CPropertyIndexVariantMap vm(CClient::IndexCapabilities, CVariant::from(capabilities));
        const CVoiceCapabilities caps = sApp->getWebDataServices()->getVoiceCapabilityForCallsign(callsign);
        vm.addValue({CClient::IndexVoiceCapabilities}, caps);
        {
            QWriteLocker l(&m_lockClient);
            if (!this->m_otherClients.containsCallsign(callsign)) { this->m_otherClients.push_back(CClient(callsign)); }
            this->m_otherClients.applyIf(&CClient::getCallsign, callsign, vm);
        }

        // foraircraft parts
        if (flags & INetwork::SupportsAircraftConfigs) m_network->sendAircraftConfigQuery(callsign);
    }

    void CAirspaceMonitor::ps_serverReplyReceived(const CCallsign &callsign, const QString &server)
    {
        if (!this->m_connected || callsign.isEmpty() || server.isEmpty()) { return; }
        CPropertyIndexVariantMap vm(CClient::IndexServer, server);
        {
            QWriteLocker l(&m_lockClient);
            if (!this->m_otherClients.containsCallsign(callsign)) { this->m_otherClients.push_back(CClient(callsign)); }
            this->m_otherClients.applyIf(&CClient::getCallsign, callsign, vm);
        }
    }
    void CAirspaceMonitor::ps_metarReceived(const QString & /** metarMessage **/)
    {
        // deprecated
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
        m_modelTemporaryCache.clear();
    }

    void CAirspaceMonitor::removeAllOtherClients()
    {
        QWriteLocker l1(&m_lockClient);
        m_otherClients.clear();
    }

    void CAirspaceMonitor::removeFromAircraftCachesAndLogs(const CCallsign &callsign)
    {
        if (callsign.isEmpty()) { return; }
        this->m_modelTemporaryCache.remove(callsign);
        this->m_flightPlanCache.remove(callsign);

        QWriteLocker l(&m_lockMessages);
        this->m_reverseLookupMessages.remove(callsign);
    }

    void CAirspaceMonitor::fireDelayedReadyForModelMatching(const CCallsign &callsign, int trial, int delayMs)
    {
        Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "missing callsign");
        BlackMisc::singleShot(delayMs, QThread::currentThread(), [ = ]()
        {
            this->addReverseLookupMessage(callsign, QString("Waiting for FSD packets, %1 trial").arg(trial));
            this->ps_sendReadyForModelMatching(callsign, trial);
        });
    }

    void CAirspaceMonitor::ps_receivedBookings(const CAtcStationList &bookedStations)
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
                this->m_atcStationsOnline.syncronizeWithBookedStation(bookedStation);
            }
            this->m_atcStationsBooked = newBookedStations;
        }
        emit this->changedAtcStationsBooked(); // all booked stations reloaded
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

        // some checks for special conditions, e.g. logout -> empty list, but still signals pending
        if (!this->m_connected) { return; }
        CSimulatedAircraft remoteAircraft = this->getAircraftInRangeForCallsign(callsign);

        // check if the name and ICAO query went properly through, those usually mean the aircraft are in range and can be used
        // this here is part of the reverse lookup process, where we turn FSD data into a model we assume the other user is flying
        const bool maxTrialsReached = trial >= 2;
        const bool inRange = remoteAircraft.hasValidCallsign(); // found? otherwise we need to wait for some data
        bool validData = inRange && remoteAircraft.hasAircraftDesignator();
        if (!maxTrialsReached && validData && !remoteAircraft.getModel().hasModelString())
        {
            // do we have to wait for model string?
            // 1) we have currently no good way to detect if model strings are supported, so
            //    we can only try, there is no crtiertia to decide if a client supports FSInn extensions
            // 2) it is still possible to receive a model string, so we wait and try again
            // 3) disadvantage, we try also when no model string is supported which just delays the process
            // 4) another option would be to fire again when the model string eventually arrives,
            //    which would on the orhter hand cause more effort in model matching part
            this->fireDelayedReadyForModelMatching(callsign, trial + 1);
            return;
        }

        if (!inRange)
        {
            // here we assume user has logged out, incomplete data because of traffic sim, etc.
            // aircraft is no longer in range, or ws never added to range (no position updates)
            const QString msg = QString("Aircraft '%1' not in range anymore or never added to range, designator '%2', airline '%3'").arg(callsign.toQString()).arg(remoteAircraft.getAircraftIcaoCodeDesignator()).arg(remoteAircraft.getAirlineIcaoCodeDesignator());
            const CStatusMessage m = CMatchingUtils::logMessage(callsign, msg, getLogCategories(), CStatusMessage::SeverityWarning);
            this->addReverseLookupMessage(callsign, m);
            CLogMessage::preformatted(m);
            return;
        }

        if (!validData)
        {
            // even after all the trials, still no valid data, something is wrong here
            const QString msg = QString("Cannot retrieve correct data for '%1', designator '%2', airline '%3'").arg(callsign.toQString()).arg(remoteAircraft.getAircraftIcaoCodeDesignator()).arg(remoteAircraft.getAirlineIcaoCodeDesignator());
            const CStatusMessage m = CMatchingUtils::logMessage(callsign, msg, getLogCategories(), CStatusMessage::SeverityWarning);
            this->addReverseLookupMessage(callsign, m);
            CLogMessage::preformatted(m);
            return; // ignore this model (crashed, logoff, ....)
        }
        Q_ASSERT_X(remoteAircraft.getCallsign() == remoteAircraft.getModel().getCallsign(), Q_FUNC_INFO, "wrong model callsign");
        emit this->readyForModelMatching(remoteAircraft);

        // log message
        {
            const QString msg = QString("Ready for matching '%1' with model type '%2'").arg(callsign.toQString()).arg(remoteAircraft.getModel().getModelTypeAsString());
            const CStatusMessage m = CMatchingUtils::logMessage(callsign, msg, getLogCategories());
            this->addReverseLookupMessage(callsign, m);
            // CLogMessage::preformatted(m);
        }
    }

    void CAirspaceMonitor::ps_atcPositionUpdate(const CCallsign &callsign, const BlackMisc::PhysicalQuantities::CFrequency &frequency, const CCoordinateGeodetic &position, const BlackMisc::PhysicalQuantities::CLength &range)
    {
        Q_ASSERT(CThreadUtils::isCurrentThreadObjectThread(this));
        if (!this->m_connected) { return; }
        CAtcStationList stationsWithCallsign = this->m_atcStationsOnline.findByCallsign(callsign);
        if (stationsWithCallsign.isEmpty())
        {
            // new station, init with data from data file
            CAtcStation station(sApp->getWebDataServices()->getAtcStationsForCallsign(callsign).frontOrDefault());
            station.setCallsign(callsign);
            station.setRange(range);
            station.setFrequency(frequency);
            station.setPosition(position);
            station.setOnline(true);
            station.calculcateDistanceAndBearingToOwnAircraft(getOwnAircraftPosition());

            // sync with bookings
            if (this->m_atcStationsBooked.containsCallsign(callsign))
            {
                CAtcStation bookedStation(this->m_atcStationsBooked.findFirstByCallsign(callsign));
                station.syncronizeWithBookedStation(bookedStation);
                this->m_atcStationsBooked.replaceIf(&CAtcStation::getCallsign, callsign, bookedStation);
            }

            this->m_atcStationsOnline.push_back(station);

            // subsequent queries
            if (this->m_network->isConnected())
            {
                emit this->m_network->sendRealNameQuery(callsign);
                emit this->m_network->sendAtisQuery(callsign); // request ATIS and voice rooms
                emit this->m_network->sendServerQuery(callsign);
            }

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
            int changed = this->m_atcStationsOnline.applyIf(&CAtcStation::getCallsign, callsign, vm, true);
            if (changed > 0) { emit this->changedAtcStationsOnline(); }
        }
    }

    void CAirspaceMonitor::ps_atcControllerDisconnected(const CCallsign &callsign)
    {
        Q_ASSERT(CThreadUtils::isCurrentThreadObjectThread(this));

        this->m_otherClients.removeByCallsign(callsign);
        if (this->m_atcStationsOnline.containsCallsign(callsign))
        {
            CAtcStation removedStation = this->m_atcStationsOnline.findFirstByCallsign(callsign);
            this->m_atcStationsOnline.removeByCallsign(callsign);
            emit this->changedAtcStationsOnline();
            emit this->changedAtcStationOnlineConnectionStatus(removedStation, false);
        }

        // booked
        this->m_atcStationsBooked.applyIf(&CAtcStation::getCallsign, callsign, CPropertyIndexVariantMap(CAtcStation::IndexIsOnline, CVariant::from(false)));
    }

    void CAirspaceMonitor::ps_atisReceived(const CCallsign &callsign, const CInformationMessage &atisMessage)
    {
        Q_ASSERT(CThreadUtils::isCurrentThreadObjectThread(this));
        if (!this->m_connected || callsign.isEmpty()) return;
        CPropertyIndexVariantMap vm(CAtcStation::IndexAtis, CVariant::from(atisMessage));
        int changedOnline = this->m_atcStationsOnline.applyIf(&CAtcStation::getCallsign, callsign, vm);

        // receiving an ATIS means station is online, update in bookings
        vm.addValue(CAtcStation::IndexIsOnline, true);
        int changedBooked = this->m_atcStationsBooked.applyIf(&CAtcStation::getCallsign, callsign, vm);
        if (changedOnline > 0) { emit this->changedAtcStationsOnline(); }
        if (changedBooked > 0) { emit this->changedAtcStationsBooked(); }
    }

    void CAirspaceMonitor::ps_atisVoiceRoomReceived(const CCallsign &callsign, const QString &url)
    {
        Q_ASSERT(CThreadUtils::isCurrentThreadObjectThread(this));
        if (!this->m_connected) { return; }
        QString trimmedUrl = url.trimmed();
        CPropertyIndexVariantMap vm({ CAtcStation::IndexVoiceRoom, CVoiceRoom::IndexUrl }, trimmedUrl);
        int changedOnline = this->m_atcStationsOnline.applyIf(&CAtcStation::getCallsign, callsign, vm, true);
        if (changedOnline < 1) { return; }

        Q_ASSERT(changedOnline == 1);
        CAtcStation station = this->m_atcStationsOnline.findFirstByCallsign(callsign);
        emit this->changedAtcStationsOnline();
        emit this->changedAtcStationOnlineConnectionStatus(station, true); // send when voice room url is available

        vm.addValue(CAtcStation::IndexIsOnline, true); // with voice room ATC is online
        int changedBooked = this->m_atcStationsBooked.applyIf(&CAtcStation::getCallsign, callsign, vm, true);
        if (changedBooked > 0)
        {
            // receiving a voice room means station is online, update in bookings
            emit this->changedAtcStationsBooked();
        }

        // receiving voice room means ATC has voice
        vm = CPropertyIndexVariantMap(CClient::IndexVoiceCapabilities, CVariant::from(CVoiceCapabilities::fromVoiceCapabilities(CVoiceCapabilities::Voice)));
        {
            QWriteLocker l(&m_lockClient);
            this->m_otherClients.applyIf(&CClient::getCallsign, callsign, vm, false);
        }
    }

    void CAirspaceMonitor::ps_atisLogoffTimeReceived(const CCallsign &callsign, const QString &zuluTime)
    {
        Q_ASSERT(CThreadUtils::isCurrentThreadObjectThread(this));
        if (!this->m_connected) { return; }
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

            CPropertyIndexVariantMap vm(CAtcStation::IndexBookedUntil, CVariant(logoffDateTime));
            const int changedOnline = this->m_atcStationsOnline.applyIf(&CAtcStation::getCallsign, callsign, vm, true);
            const int changedBooked = this->m_atcStationsBooked.applyIf(&CAtcStation::getCallsign, callsign, vm, true);

            if (changedOnline > 0) { emit changedAtcStationsOnline(); }
            if (changedBooked > 0) { emit changedAtcStationsBooked(); }
        }
    }

    void CAirspaceMonitor::ps_customFSinnPacketReceived(const CCallsign &callsign, const QString &airlineIcaoDesignator, const QString &aircraftIcaoDesignator, const QString &combinedAircraftType, const QString &modelString)
    {
        if (!this->m_connected || callsign.isEmpty() || modelString.isEmpty()) { return; }

        // Request of other client, I can get the other's model from that
        Q_UNUSED(combinedAircraftType);

        // update clients
        {
            QWriteLocker l(&m_lockClient);
            if (!this->m_otherClients.containsCallsign(callsign))
            {
                this->m_otherClients.push_back(CClient(callsign, modelString));
            }
            else
            {
                const CPropertyIndexVariantMap vm(CClient::IndexModelString, modelString);
                this->m_otherClients.applyIf(&CClient::getCallsign, callsign, vm); // update client info
            }
        }

        // ICAO response from custom data
        if (!aircraftIcaoDesignator.isEmpty())
        {
            // hand over, same functionality as would be needed here
            this->icaoOrFsdDataReceived(callsign, aircraftIcaoDesignator, airlineIcaoDesignator, "", modelString,  CAircraftModel::TypeFsdData);
        }
    }

    void CAirspaceMonitor::ps_icaoCodesReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &aircraftIcaoDesignator, const QString &airlineIcaoDesignator, const QString &livery)
    {
        this->icaoOrFsdDataReceived(callsign, aircraftIcaoDesignator, airlineIcaoDesignator, livery, "", CAircraftModel::TypeQueriedFromNetwork);
    }

    void CAirspaceMonitor::icaoOrFsdDataReceived(const CCallsign &callsign, const QString &aircraftIcaoDesignator, const QString &airlineIcaoDesignator, const QString &livery, const QString &modelString, CAircraftModel::ModelType type)
    {
        Q_ASSERT_X(CThreadUtils::isCurrentThreadObjectThread(this), Q_FUNC_INFO, "not in main thread");
        Q_ASSERT_X(type == CAircraftModel::TypeFsdData || type == CAircraftModel::TypeQueriedFromNetwork, Q_FUNC_INFO, "Wrong type");
        BLACK_VERIFY_X(callsign.isValid(), Q_FUNC_INFO, "invalid callsign");
        if (!callsign.isValid()) { return; }
        if (!this->m_connected) { return; }
        if (aircraftIcaoDesignator.isEmpty() && airlineIcaoDesignator.isEmpty() && livery.isEmpty() && modelString.isEmpty()) { return; }

        CStatusMessageList reverseLookupMessages;
        CStatusMessageList *pReverseLookupMessages = this->isReverseLookupMessagesEnabled() ? &reverseLookupMessages : nullptr;
        CMatchingUtils::addLogDetailsToList(pReverseLookupMessages, callsign, QString("Data from network (%1): aircraft '%2', airline '%3', livery '%4', model '%5'").
                                            arg(CAircraftModel::modelTypeToString(type)).
                                            arg(aircraftIcaoDesignator).arg(airlineIcaoDesignator).
                                            arg(livery).arg(modelString));

        const CSimulatedAircraft remoteAircraft(this->getAircraftInRangeForCallsign(callsign));
        const bool existingAircraft = !remoteAircraft.getCallsign().isEmpty();

        CAircraftModel model; // generate a model for that aircraft
        model.setCallsign(callsign);
        if (existingAircraft)
        {
            model = remoteAircraft.getModel();
            this->m_modelTemporaryCache.remove(callsign); // normally already removed
        }
        else if (m_modelTemporaryCache.contains(callsign))
        {
            model = this->m_modelTemporaryCache[callsign];
        }

        // already matched with DB? Means we already have DB data in cache or existing model
        if (!model.canInitializeFromFsd()) { return; }

        // update type to FSD if applicable
        if (model.getModelType() == CAircraftModel::TypeFsdData)
        {
            model.setModelType(CAircraftModel::TypeFsdData); // update type if no type yet
        }

        //
        // --- now I try to fill in as many DB data as possible ---
        // 1) This will unify data where possible
        // 2) I have full information of what the other pilot flies where possible
        // 3) This is not model matching here (!), it is a process of getting the most accurate data from that fuzzy information I get via FSD

        // we have no DB model yet, but do we have model string?
        if (!model.hasModelString() && !modelString.isEmpty()) { model.setModelString(modelString); }

        // reverse lookup, use DB data wherever possible
        // 1) If I cannot resolce the ICAO codes here, they are either wrong (most likely in most cases) or
        // 2) not in the DB yet
        if (!model.hasAircraftDesignator() && !aircraftIcaoDesignator.isEmpty())
        {
            const CAircraftIcaoCode reverseIcao = CAircraftMatcher::reverseLookupAircraftIcao(aircraftIcaoDesignator, callsign, pReverseLookupMessages);
            if (reverseIcao.hasValidDbKey())
            {
                model.setAircraftIcaoCode(reverseIcao);
            }
            else
            {
                model.setAircraftIcaoCode(aircraftIcaoDesignator.trimmed().toUpper());
                CMatchingUtils::addLogDetailsToList(pReverseLookupMessages, callsign, QString("No DB data for aircraft %1").arg(aircraftIcaoDesignator), getLogCategories());
            }
        }

        // Derive airline from callsign
        CAirlineIcaoCode airlineIcaoDesignatorReviewed(airlineIcaoDesignator);
        if (type == CAircraftModel::TypeQueriedFromNetwork && airlineIcaoDesignator.isEmpty())
        {
            // try to conclude from callsign
            airlineIcaoDesignatorReviewed = CAirspaceMonitor::callsignToAirline(callsign);
            if (airlineIcaoDesignatorReviewed.hasValidDesignator())
            {
                CMatchingUtils::addLogDetailsToList(pReverseLookupMessages, callsign, QString("Turned callsign %1 into airline %2").arg(callsign.asString()).arg(airlineIcaoDesignatorReviewed.getDesignator()), getLogCategories());
            }
            else
            {
                CMatchingUtils::addLogDetailsToList(pReverseLookupMessages, callsign, QString("Cannot turn callsign %1 into airline").arg(callsign.asString()), getLogCategories());
            }
        }

        // Set livery / airline
        if (!model.getLivery().hasValidDbKey())
        {
            if (!model.hasAirlineDesignator() && airlineIcaoDesignatorReviewed.hasValidDesignator())
            {
                const CAirlineIcaoCode reverseIcao = CAircraftMatcher::reverseLookupAirlineIcao(airlineIcaoDesignatorReviewed.getDesignator(), callsign, pReverseLookupMessages);
                CLivery defaultLivery = CAircraftMatcher::reverseLookupStandardLivery(reverseIcao, callsign, pReverseLookupMessages);
                if (!defaultLivery.hasValidDbKey())
                {
                    // No DB livery, create one
                    if (reverseIcao.hasValidDbKey())
                    {
                        defaultLivery.setAirlineIcaoCode(reverseIcao);
                    }
                    else
                    {
                        defaultLivery.setAirlineIcaoCode(airlineIcaoDesignatorReviewed);
                        CMatchingUtils::addLogDetailsToList(pReverseLookupMessages, callsign, QString("No DB data for airline %1").arg(aircraftIcaoDesignator), getLogCategories());
                    }
                }
            }
        }

        // now try resolution to model from DB
        model = CAircraftMatcher::reverselLookupModel(model, livery, &reverseLookupMessages);

        // messages
        if (this->m_enableReverseLookupMsgs && !reverseLookupMessages.isEmpty())
        {
            this->addReverseLookupMessages(callsign, reverseLookupMessages);
        }

        {
            QWriteLocker l(&m_lockAircraft);
            if (this->m_aircraftInRange.containsCallsign(callsign))
            {
                // we know the aircraft, so we update it
                this->m_aircraftInRange.setAircraftModel(callsign, model);
            }
            else
            {
                // keep in cache, as aircraft is not already known
                this->m_modelTemporaryCache.insert(callsign, model);
            }
        } // lock
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

    CAirlineIcaoCode CAirspaceMonitor::callsignToAirline(const CCallsign &callsign)
    {
        if (callsign.isEmpty() || !sApp || !sApp->getWebDataServices()) { return CAirlineIcaoCode(); }
        const CAirlineIcaoCode icao = sApp->getWebDataServices()->findBestMatchByCallsign(callsign);
        return icao;
    }

    void CAirspaceMonitor::ps_aircraftUpdateReceived(const CAircraftSituation &situation, const CTransponder &transponder)
    {
        Q_ASSERT_X(CThreadUtils::isCurrentThreadObjectThread(this), Q_FUNC_INFO, "Called in different thread");
        if (!this->m_connected) { return; }

        CCallsign callsign(situation.getCallsign());
        Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "Empty callsign");

        // store situation history
        this->storeAircraftSituation(situation);
        emit this->addedAircraftSituation(situation);

        const bool existsInRange = this->isInRange(callsign);
        if (!existsInRange)
        {
            // new aircraft
            CSimulatedAircraft aircraft;
            aircraft.setCallsign(callsign);
            aircraft.setSituation(situation);
            aircraft.setTransponder(transponder);
            aircraft.calculcateDistanceAndBearingToOwnAircraft(getOwnAircraftPosition()); // distance from myself
            sApp->getWebDataServices()->updateWithVatsimDataFileData(aircraft);

            // ICAO from cache if avialable
            bool setModelFromCache = false;
            {
                CAircraftModel model;
                if (this->m_modelTemporaryCache.contains(callsign))
                {
                    model = this->m_modelTemporaryCache.value(callsign);
                    this->m_modelTemporaryCache.remove(callsign);
                    aircraft.setModel(model);
                    setModelFromCache = true;
                }

                //
                // only place where aircraft is added
                //
                {
                    QWriteLocker l(&m_lockAircraft);
                    this->m_aircraftInRange.push_back(aircraft);
                }

                // new client, there is a chance it has been already created by custom packet
                {
                    QWriteLocker l(&m_lockClient);
                    if (!this->m_otherClients.containsCallsign(callsign))
                    {
                        const CClient c(callsign);
                        this->m_otherClients.push_back(c); // initial, will be filled by data later
                    }
                }
            }

            // only if still connected
            if (this->m_network->isConnected())
            {
                // the order here makes some sense, as we hope to receive ICAO codes last, and everthing else already in place

                // Send a custom FSinn query only if we don't have the exact model yet
                const CClient c = this->getOtherClientOrDefaultForCallsign(callsign);
                if (!c.hasQueriedModelString() && c.hasCapability(CClient::FsdWithIcaoCodes))
                {
                    this->m_network->sendCustomFsinnQuery(callsign);
                }
                this->m_network->sendFrequencyQuery(callsign);
                this->m_network->sendRealNameQuery(callsign);
                this->m_network->sendCapabilitiesQuery(callsign);
                this->m_network->sendServerQuery(callsign);

                // do this as last thing
                if (setModelFromCache)
                {
                    // we have had at least some information,
                    // means either ICAO codes or FSInn package has already been received
                    this->ps_sendReadyForModelMatching(callsign, 1);
                }
                else
                {
                    // no info yet, query ICAO codes at least
                    // allow some time for the data to arrive before for model matching
                    this->m_network->sendIcaoCodesQuery(callsign);
                    this->fireDelayedReadyForModelMatching(callsign);
                }
                emit this->addedAircraft(aircraft);
            } // connected
        }
        else
        {
            // update, aircraft already exists
            CLength distance = getOwnAircraft().calculateGreatCircleDistance(situation.getPosition());
            distance.switchUnit(CLengthUnit::NM());
            CPropertyIndexVariantMap vm;
            vm.addValue(CSimulatedAircraft::IndexTransponder, transponder);
            vm.addValue(CSimulatedAircraft::IndexSituation, situation);
            vm.addValue(CSimulatedAircraft::IndexDistanceToOwnAircraft, distance);

            // here I expect always a changed value
            {
                QWriteLocker l(&m_lockAircraft);
                this->m_aircraftInRange.applyIfCallsign(callsign, vm);
            }
        }

        emit this->changedAircraftInRange();
    }

    void CAirspaceMonitor::ps_aircraftInterimUpdateReceived(const CAircraftSituation &situation)
    {
        Q_ASSERT_X(CThreadUtils::isCurrentThreadObjectThread(this), Q_FUNC_INFO, "Called in different thread");
        if (!this->m_connected) { return; }

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
        if (lastSituation.getPosition() == interimSituation.getPosition()) { return; } // save position, ignore

        // changed position, continue and copy values
        interimSituation.setCurrentUtcTime();
        interimSituation.setGroundSpeed(lastSituation.getGroundSpeed());

        // store situation history
        this->storeAircraftSituation(interimSituation);
        emit this->addedAircraftSituation(interimSituation);

        // update aircraft
        CLength distance = getOwnAircraft().calculateGreatCircleDistance(interimSituation.getPosition());
        distance.switchUnit(CLengthUnit::NM()); // looks nicer
        CPropertyIndexVariantMap vm;
        vm.addValue(CSimulatedAircraft::IndexSituation, interimSituation);
        vm.addValue(CSimulatedAircraft::IndexDistanceToOwnAircraft, distance);

        // here I expect always a changed value
        {
            QWriteLocker l(&m_lockAircraft);
            this->m_aircraftInRange.applyIfCallsign(callsign, vm);
        }
        emit this->changedAircraftInRange();
    }

    void CAirspaceMonitor::ps_pilotDisconnected(const CCallsign &callsign)
    {
        Q_ASSERT(CThreadUtils::isCurrentThreadObjectThread(this));

        // in case of inconsistencies I always remove here
        this->removeFromAircraftCachesAndLogs(callsign);

        { QWriteLocker l1(&m_lockParts); m_partsByCallsign.remove(callsign); m_aircraftSupportingParts.remove(callsign); }
        { QWriteLocker l2(&m_lockSituations); m_situationsByCallsign.remove(callsign); }
        { QWriteLocker l(&m_lockClient); m_otherClients.removeByCallsign(callsign); }

        bool containsCallsign;
        {
            QWriteLocker l(&m_lockAircraft);
            containsCallsign = this->m_aircraftInRange.containsCallsign(callsign);
            if (containsCallsign) { this->m_aircraftInRange.removeByCallsign(callsign); }
        }
        if (containsCallsign) { emit this->removedAircraft(callsign); }
    }

    void CAirspaceMonitor::ps_frequencyReceived(const CCallsign &callsign, const CFrequency &frequency)
    {
        Q_ASSERT(CThreadUtils::isCurrentThreadObjectThread(this));

        // update
        int changed;
        CPropertyIndexVariantMap vm({CSimulatedAircraft::IndexCom1System, CComSystem::IndexActiveFrequency}, CVariant::from(frequency));
        {
            QWriteLocker l(&m_lockAircraft);
            changed = this->m_aircraftInRange.applyIf(&CSimulatedAircraft::getCallsign, callsign, vm, true);
        }
        if (changed > 0) { emit this->changedAircraftInRange(); }
    }

    void CAirspaceMonitor::ps_aircraftConfigReceived(const BlackMisc::Aviation::CCallsign &callsign, const QJsonObject &jsonObject, bool isFull)
    {
        Q_ASSERT(CThreadUtils::isCurrentThreadObjectThread(this));

        CSimulatedAircraft simAircraft(getAircraftInRangeForCallsign(callsign));

        // If we are not yet synchronized, we throw away any incremental packet
        if (!simAircraft.hasValidCallsign()) { return; }
        if (!simAircraft.isPartsSynchronized() && !isFull) { return; }

        CAircraftParts parts;
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

        // make sure in any case right time
        parts.setCurrentUtcTime();

        // store part history (parts always absolute)
        this->storeAircraftParts(callsign, parts);
        emit this->addedAircraftParts(callsign, parts);

        // here I expect always a changed value
        QWriteLocker l(&m_lockAircraft);
        this->m_aircraftInRange.setAircraftParts(callsign, parts);
    }

    void CAirspaceMonitor::storeAircraftSituation(const CAircraftSituation &situation)
    {
        const CCallsign callsign(situation.getCallsign());
        Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "empty callsign");
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
        if (callsign.isEmpty()) { return; }

        // list sorted from new to old
        QWriteLocker lock(&m_lockParts);
        CAircraftPartsList &partsList = this->m_partsByCallsign[callsign];
        partsList.push_frontMaxElements(parts, MaxPartsPerCallsign);

        if (!m_aircraftSupportingParts.contains(callsign))
        {
            m_aircraftSupportingParts.push_back(callsign); // mark as callsign which supports parts
        }

        // check sort order
        Q_ASSERT_X(partsList.size() < 2 || partsList[0].getMSecsSinceEpoch() >= partsList[1].getMSecsSinceEpoch(), Q_FUNC_INFO, "wrong sort order");
    }
} // namespace
