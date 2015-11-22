/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "airspace_monitor.h"
#include "blackcore/registermetadata.h"
#include "blackcore/webdataservices.h"
#include "blackcore/vatsimbookingreader.h"
#include "blackcore/vatsimdatafilereader.h"
#include "blackmisc/project.h"
#include "blackmisc/testing.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/propertyindexallclasses.h"
#include "blackmisc/threadutils.h"

using namespace BlackMisc;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Json;
using namespace BlackMisc::Network;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Weather;

namespace BlackCore
{

    CAirspaceMonitor::CAirspaceMonitor(BlackMisc::Simulation::IOwnAircraftProvider *ownAircraftProvider, INetwork *network, CWebDataServices *webDataReader, QObject *parent)
        : QObject(parent),
          COwnAircraftAware(ownAircraftProvider),
          CWebDataServicesAware(webDataReader),
          m_network(network),
          m_analyzer(new CAirspaceAnalyzer(ownAircraftProvider, this, network, this))
    {
        this->setObjectName("CAirspaceMonitor");
        m_interimPositionUpdateTimer.setObjectName(this->objectName().append(":m_interimPositionUpdateTimer"));

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
        this->connect(&m_interimPositionUpdateTimer, &QTimer::timeout, this, &CAirspaceMonitor::ps_sendInterimPositions);

        // AutoConnection: this should also avoid race conditions by updating the bookings
        Q_ASSERT_X(webDataReader, Q_FUNC_INFO, "Missing data reader");
        if (webDataReader)
        {
            this->connect(webDataReader->getBookingReader(), &CVatsimBookingReader::atcBookingsRead, this, &CAirspaceMonitor::ps_receivedBookings);
            this->connect(webDataReader->getDataFileReader(), &CVatsimDataFileReader::dataFileRead, this, &CAirspaceMonitor::ps_receivedDataFile);
        }

        // Force snapshot in the main event loop
        this->connect(this->m_analyzer, &CAirspaceAnalyzer::airspaceAircraftSnapshot, this, &CAirspaceMonitor::airspaceAircraftSnapshot, Qt::QueuedConnection);

        // Analyzer
        this->connect(this->m_analyzer, &CAirspaceAnalyzer::timeoutAircraft, this, &CAirspaceMonitor::ps_pilotDisconnected, Qt::QueuedConnection);
        this->connect(this->m_analyzer, &CAirspaceAnalyzer::timeoutAtc, this, &CAirspaceMonitor::ps_atcControllerDisconnected, Qt::QueuedConnection);
    }

    CSimulatedAircraftList CAirspaceMonitor::getAircraftInRange() const
    {
        QReadLocker l(&m_lockAircraft);
        return m_aircraftInRange;
    }

    CSimulatedAircraft CAirspaceMonitor::getAircraftInRangeForCallsign(const CCallsign &callsign) const
    {
        QReadLocker l(&m_lockAircraft);
        return m_aircraftInRange.findFirstByCallsign(callsign);
    }

    CAircraftModel CAirspaceMonitor::getAircraftInRangeModelForCallsign(const CCallsign &callsign) const
    {
        CSimulatedAircraft aircraft(getAircraftInRangeForCallsign(callsign)); // threadsafe
        return aircraft.getModel();
    }

    int CAirspaceMonitor::getAircraftInRangeCount() const
    {
        QReadLocker l(&m_lockAircraft);
        return m_aircraftInRange.size();
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
        //! \todo remove old workaround if new version with receiver works
        // trick is to use the Queued signal here
        // analyzer (own thread) -> airspaceAircraftSnapshot -> AirspaceMonitor -> airspaceAircraftSnapshot queued in main thread
        // QMetaObject::Connection c4 = this->connect(this, &CAirspaceMonitor::airspaceAircraftSnapshot, receiver, aircraftSnapshotSlot);
        QMetaObject::Connection c4 = this->connect(this->m_analyzer, &CAirspaceAnalyzer::airspaceAircraftSnapshot, receiver, aircraftSnapshotSlot, Qt::QueuedConnection);
        Q_ASSERT_X(c4, Q_FUNC_INFO, "connect failed");
        return QList<QMetaObject::Connection>({ c1, c2, c3, c4});
    }

    bool CAirspaceMonitor::updateAircraftEnabled(const CCallsign &callsign, bool enabledForRedering, const BlackMisc::CIdentifier &originator)
    {
        Q_UNUSED(originator);
        CPropertyIndexVariantMap vm(CSimulatedAircraft::IndexEnabled, CVariant::fromValue(enabledForRedering));
        QWriteLocker l(&m_lockAircraft);
        int c = m_aircraftInRange.applyIfCallsign(callsign, vm);
        return c > 0;
    }

    bool CAirspaceMonitor::updateAircraftModel(const CCallsign &callsign, const CAircraftModel &model, const BlackMisc::CIdentifier &originator)
    {
        Q_UNUSED(originator);
        QWriteLocker l(&m_lockAircraft);
        CPropertyIndexVariantMap vm(CSimulatedAircraft::IndexModel, CVariant::from(model));
        int c = m_aircraftInRange.applyIfCallsign(callsign, vm);
        return c > 0;
    }

    bool CAirspaceMonitor::updateFastPositionEnabled(const CCallsign &callsign, bool enableFastPositonUpdates, const BlackMisc::CIdentifier &originator)
    {
        //! \fixme CAirspaceMonitor: If in the long term originator is not used, remove from signature
        Q_UNUSED(originator);
        CPropertyIndexVariantMap vm(CSimulatedAircraft::IndexFastPositionUpdates, CVariant::fromValue(enableFastPositonUpdates));
        QWriteLocker l(&m_lockAircraft);
        int c = m_aircraftInRange.applyIfCallsign(callsign, vm);
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
        foreach(CAtcStation station, this->m_atcStationsOnline)
        {
            CUser user = station.getController();
            users.push_back(user);
        }
        for (const CSimulatedAircraft &aircraft : this->getAircraftInRange())
        {
            CUser user = aircraft.getPilot();
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
            CUserList usersByCallsign = this->getUsersForCallsign(callsign);
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
        clients.push_back(this->m_otherClients.findByCallsigns(callsigns));
        return clients;
    }

    CClientList CAirspaceMonitor::getOtherClients() const
    {
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
        m_modelCache.clear();

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

    void CAirspaceMonitor::enableFastPositionSending(bool enable)
    {
        if (enable)
        {
            m_interimPositionUpdateTimer.start();
        }
        else
        {
            m_interimPositionUpdateTimer.stop();
        }
        m_sendInterimPositions = enable;
    }

    void CAirspaceMonitor::gracefulShutdown()
    {
        if (this->m_analyzer) { this->m_analyzer->gracefulShutdown(); }
        QObject::disconnect(this);
        this->enableFastPositionSending(false);
    }

    bool CAirspaceMonitor::isFastPositionSendingEnabled() const
    {
        return m_sendInterimPositions;
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
        CVoiceCapabilities caps = this->getVoiceCapabilityForCallsign(callsign);
        vm = CPropertyIndexVariantMap({CClient::IndexUser, CUser::IndexRealName}, realname);
        vm.addValue({ CClient::IndexVoiceCapabilities }, caps);
        if (!this->m_otherClients.containsCallsign(callsign)) { this->m_otherClients.push_back(CClient(callsign)); }
        this->m_otherClients.applyIf(&CClient::getCallsign, callsign, vm);
    }

    void CAirspaceMonitor::ps_capabilitiesReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, quint32 flags)
    {
        if (!this->m_connected || callsign.isEmpty()) { return; }
        CPropertyIndexVariantMap capabilities;
        capabilities.addValue(CClient::FsdAtisCanBeReceived, (flags & INetwork::AcceptsAtisResponses));
        capabilities.addValue(CClient::FsdWithInterimPositions, (flags & INetwork::SupportsInterimPosUpdates));
        capabilities.addValue(CClient::FsdWithModelDescription, (flags & INetwork::SupportsModelDescriptions));
        capabilities.addValue(CClient::FsdWithAircraftConfig, (flags & INetwork::SupportsAircraftConfigs));

        CPropertyIndexVariantMap vm(CClient::IndexCapabilities, CVariant::from(capabilities));
        CVoiceCapabilities caps = this->getVoiceCapabilityForCallsign(callsign);
        vm.addValue({CClient::IndexVoiceCapabilities}, caps);
        if (!this->m_otherClients.containsCallsign(callsign)) { this->m_otherClients.push_back(CClient(callsign)); }
        this->m_otherClients.applyIf(&CClient::getCallsign, callsign, vm);

        // foraircraft parts
        if (flags & INetwork::SupportsAircraftConfigs) m_network->sendAircraftConfigQuery(callsign);
    }

    void CAirspaceMonitor::ps_customFSinnPacketReceived(const CCallsign &callsign, const QString &airlineIcaoDesignator, const QString &aircraftIcaoDesignator, const QString &combinedAircraftType, const QString &model)
    {
        if (!this->m_connected || callsign.isEmpty() || model.isEmpty()) { return; }

        // Request of other client, I can get the other's model from that
        Q_UNUSED(combinedAircraftType);
        CPropertyIndexVariantMap vm({ CClient::IndexModel, CAircraftModel::IndexModelString }, model);
        vm.addValue({ CClient::IndexModel, CAircraftModel::IndexModelType }, static_cast<int>(CAircraftModel::TypeQueriedFromNetwork));
        if (!this->m_otherClients.containsCallsign(callsign))
        {
            // with custom packets it can happen,
            // the packet is received before any other packet
            this->m_otherClients.push_back(CClient(callsign));
        }
        this->m_otherClients.applyIf(&CClient::getCallsign, callsign, vm);

        // ICAO response from custom data
        if (!aircraftIcaoDesignator.isEmpty())
        {
            // hand over, same functionality as would be needed here
            this->ps_icaoCodesReceived(callsign, aircraftIcaoDesignator, airlineIcaoDesignator, "");
        }
    }

    void CAirspaceMonitor::ps_serverReplyReceived(const CCallsign &callsign, const QString &server)
    {
        if (!this->m_connected || callsign.isEmpty() || server.isEmpty()) { return; }
        if (!this->m_otherClients.containsCallsign(callsign)) { this->m_otherClients.push_back(CClient(callsign)); }
        CPropertyIndexVariantMap vm(CClient::IndexServer, server);
        this->m_otherClients.applyIf(&CClient::getCallsign, callsign, vm);
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

        QWriteLocker l1(&m_lockParts);
        QWriteLocker l2(&m_lockSituations);
        m_situationsByCallsign.clear();
        m_partsByCallsign.clear();
        m_aircraftSupportingParts.clear();
        m_aircraftInRange.clear();
        m_flightPlanCache.clear();
        m_modelCache.clear();
    }

    void CAirspaceMonitor::removeAllOtherClients()
    {
        m_otherClients.clear();
    }

    void CAirspaceMonitor::removeFromAircraftCaches(const CCallsign &callsign)
    {
        if (callsign.isEmpty()) { return; }
        this->m_modelCache.remove(callsign);
        this->m_flightPlanCache.remove(callsign);
    }

    void CAirspaceMonitor::fireDelayedReadyForModelMatching(const CCallsign &callsign, int trial, int delayMs)
    {
        BlackMisc::singleShot(delayMs, QThread::currentThread(), [ = ]()
        {
            this->ps_sendReadyForModelMatching(callsign, trial + 1);
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
        for (auto client = this->m_otherClients.begin(); client != this->m_otherClients.end(); ++client)
        {
            if (client->hasSpecifiedVoiceCapabilities()) { continue; } // we already have voice caps
            CVoiceCapabilities vc = this->getVoiceCapabilityForCallsign(client->getCallsign());
            if (vc.isUnknown()) { continue; }
            client->setVoiceCapabilities(vc);
        }
    }

    void CAirspaceMonitor::ps_sendReadyForModelMatching(const CCallsign &callsign, int trial)
    {
        // some checks for special conditions, e.g. logout -> empty list, but still signals pending
        CSimulatedAircraft remoteAircraft;
        {
            QReadLocker l(&m_lockAircraft);
            if (!this->m_connected || this->m_aircraftInRange.isEmpty()) { return; }
            if (!this->m_aircraftInRange.containsCallsign(callsign)) { return; }

            // build simulated aircraft and crosscheck if all data are available
            remoteAircraft = CSimulatedAircraft(this->m_aircraftInRange.findFirstByCallsign(callsign));
            Q_ASSERT_X(remoteAircraft.hasValidCallsign(), Q_FUNC_INFO, "Invalid callsign");
        }

        // check if the name and ICAO query went properly through
        bool dataComplete =
            remoteAircraft.hasAircraftDesignator() &&
            (!m_serverSupportsNameQuery || remoteAircraft.getModel().hasModelString());

        if (trial < 3 && !dataComplete)
        {
            // allow another period for the data to arrive, otherwise go ahead
            this->fireDelayedReadyForModelMatching(callsign, trial + 1);
            return;
        }

        Q_ASSERT(remoteAircraft.hasAircraftDesignator());
        Q_ASSERT(!m_serverSupportsNameQuery || remoteAircraft.hasValidRealName());
        emit this->readyForModelMatching(remoteAircraft);
    }

    void CAirspaceMonitor::ps_atcPositionUpdate(const CCallsign &callsign, const BlackMisc::PhysicalQuantities::CFrequency &frequency, const CCoordinateGeodetic &position, const BlackMisc::PhysicalQuantities::CLength &range)
    {
        Q_ASSERT(CThreadUtils::isCurrentThreadObjectThread(this));
        if (!this->m_connected) { return; }
        CAtcStationList stationsWithCallsign = this->m_atcStationsOnline.findByCallsign(callsign);
        if (stationsWithCallsign.isEmpty())
        {
            // new station, init with data from data file
            CAtcStation station(this->getAtcStationsForCallsign(callsign).frontOrDefault());
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
        this->m_otherClients.applyIf(&CClient::getCallsign, callsign, vm, false);
    }

    void CAirspaceMonitor::ps_atisLogoffTimeReceived(const CCallsign &callsign, const QString &zuluTime)
    {
        Q_ASSERT(CThreadUtils::isCurrentThreadObjectThread(this));
        if (!this->m_connected) { return; }
        if (zuluTime.length() == 4)
        {
            // Logic to set logoff time
            bool ok;
            int h = zuluTime.left(2).toInt(&ok);
            if (!ok) { return; }
            int m = zuluTime.right(2).toInt(&ok);
            if (!ok) { return; }
            QDateTime logoffDateTime = QDateTime::currentDateTimeUtc();
            logoffDateTime.setTime(QTime(h, m));

            CPropertyIndexVariantMap vm(CAtcStation::IndexBookedUntil, CVariant(logoffDateTime));
            int changedOnline = this->m_atcStationsOnline.applyIf(&CAtcStation::getCallsign, callsign, vm, true);
            int changedBooked = this->m_atcStationsBooked.applyIf(&CAtcStation::getCallsign, callsign, vm, true);

            if (changedOnline > 0) { emit changedAtcStationsOnline(); }
            if (changedBooked > 0) { emit changedAtcStationsBooked(); }
        }
    }

    void CAirspaceMonitor::ps_icaoCodesReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &aircraftIcaoDesignator, const QString &airlineIcaoDesignator, const QString &livery)
    {
        Q_ASSERT_X(CThreadUtils::isCurrentThreadObjectThread(this), Q_FUNC_INFO, "not in main thread");
        Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "no callsign");
        if (!this->m_connected) { return; }

        if (aircraftIcaoDesignator.isEmpty() && airlineIcaoDesignator.isEmpty() && livery.isEmpty()) { return; }

        CSimulatedAircraft remoteAircraft(this->getAircraftInRangeForCallsign(callsign));
        bool existingAircraft = !remoteAircraft.getCallsign().isEmpty();

        CAircraftModel model; // generate a model for that aircraft
        if (existingAircraft)
        {
            model = remoteAircraft.getModel();
            m_modelCache.remove(callsign);
        }
        else if (m_modelCache.contains(callsign))
        {
            model  = m_modelCache[callsign];
        }

        // already matched with DB?
        if (model.getModelType() != CAircraftModel::TypeQueriedFromNetwork && model.getModelType() != CAircraftModel::TypeFsdData) { return; }

        // we have no DB model yet, but do we have model string?
        if (model.hasModelString())
        {
            CAircraftModel modelFromDb(this->getModelForModelString(model.getModelString()));
            if (modelFromDb.hasValidDbKey()) { model = modelFromDb; }
        }

        // only if not yet matched with DB
        if (!model.hasValidDbKey() && CLivery::isValidCombinedCode(livery))
        {
            CAircraftModelList models(this->getModelsForAircraftDesignatorAndLiveryCombinedCode(aircraftIcaoDesignator, livery));
            if (models.isEmpty())
            {
                // no models for that livery
                CLivery databaseLivery(this->getLiveryForCombinedCode(livery));
                if (databaseLivery.hasValidDbKey())
                {
                    // we have found a livery in the DB
                    model.setLivery(databaseLivery);
                }
                else
                {
                    // create a pseudo livery, try to find airline first
                    CAirlineIcaoCode airlineIcao(this->getAirlineIcaoCodeForDesignator(airlineIcaoDesignator));
                    if (!airlineIcao.hasValidDbKey()) { airlineIcao = CAirlineIcaoCode(airlineIcaoDesignator); }
                    CLivery liveryDummy(livery, airlineIcao, "Generated");
                    model.setLivery(liveryDummy);
                }

                CAircraftIcaoCode aircraftIcao(this->getAircraftIcaoCodeForDesignator(aircraftIcaoDesignator));
                if (!aircraftIcao.hasValidDbKey()) { aircraftIcao = CAircraftIcaoCode(aircraftIcaoDesignator); }
                model.setModelType(CAircraftModel::TypeFsdData);
            }
            else
            {
                // model by livery data
                model = models.front();
            }
        }

        int c = 0;
        {
            QWriteLocker l(&m_lockAircraft);
            if (!this->m_aircraftInRange.containsCallsign(callsign))
            {
                this->m_modelCache.insert(callsign, model);
                return;
            }

            // we know the aircraft, so we update
            c = this->m_aircraftInRange.setAircraftModel(callsign, model);
        }
        if (c > 0) { ps_sendReadyForModelMatching(callsign, 1); }
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

        QWriteLocker l(&m_lockAircraft);
        bool exists = this->m_aircraftInRange.containsCallsign(callsign);
        if (!exists)
        {
            // new aircraft
            CSimulatedAircraft aircraft;
            aircraft.setCallsign(callsign);
            aircraft.setSituation(situation);
            aircraft.setTransponder(transponder);
            aircraft.calculcateDistanceAndBearingToOwnAircraft(getOwnAircraftPosition()); // distance from myself
            this->updateWithVatsimDataFileData(aircraft);

            // ICAO from cache if avialable
            bool setModel = false;
            if (this->m_modelCache.contains(callsign))
            {
                CAircraftModel model = this->m_modelCache.value(callsign);
                this->m_modelCache.remove(callsign);
                aircraft.setModel(model);
                setModel = true;
            }

            // only place where aircraft is added
            this->m_aircraftInRange.push_back(aircraft);

            // new client, there is a chance it has been already created by custom packet
            if (!this->m_otherClients.containsCallsign(callsign))
            {
                CClient c(callsign);
                this->m_otherClients.push_back(c); // initial, will be filled by data later
            }

            // only if still connected
            if (this->m_network->isConnected())
            {
                // the order here makes some sense, as we hope to receive ICAO codes last, and everthing else already in place

                // Send a custom FSinn query only if we don't have the exact model yet
                CClient c = this->m_otherClients.findByCallsign(callsign).frontOrDefault();
                CAircraftModel::ModelType modelType = c.getAircraftModel().getModelType();
                if (modelType != CAircraftModel::TypeQueriedFromNetwork && modelType != CAircraftModel::TypeDatabaseEntry)
                {
                    this->m_network->sendCustomFsinnQuery(callsign);
                }
                this->m_network->sendFrequencyQuery(callsign);
                this->m_network->sendRealNameQuery(callsign);
                this->m_network->sendCapabilitiesQuery(callsign);
                this->m_network->sendServerQuery(callsign);

                // keep as last
                if (setModel)
                {
                    this->fireDelayedReadyForModelMatching(callsign);
                }
                else
                {
                    this->m_network->sendIcaoCodesQuery(callsign);
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
            this->m_aircraftInRange.applyIfCallsign(callsign, vm);
        }

        emit this->changedAircraftInRange();
    }

    void CAirspaceMonitor::ps_aircraftInterimUpdateReceived(const CAircraftSituation &situation)
    {
        Q_ASSERT_X(CThreadUtils::isCurrentThreadObjectThread(this), Q_FUNC_INFO, "Called in different thread");
        if (!this->m_connected) { return; }

        CCallsign callsign(situation.getCallsign());
        Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "Empty callsign");

        // todo: Check if the timestamp is copied here as well.

        // Interim packets do not have groundspeed, hence set the last known value.
        // If there is no full position available yet, throw this interim position away.
        CAircraftSituation iterimSituation(situation);
        {
            QReadLocker l(&m_lockSituations);
            auto history = this->m_situationsByCallsign[callsign];
            if (history.empty()) { return; } // we need one full situation
            iterimSituation.setCurrentUtcTime();
            iterimSituation.setGroundspeed(history.latestValue().getGroundSpeed());
        }

        // store situation history
        this->storeAircraftSituation(iterimSituation);
        emit this->addedAircraftSituation(iterimSituation);

        // update aircraft
        //! \todo skip aircraft updates for interim positions as for performance reasons
        CLength distance = getOwnAircraft().calculateGreatCircleDistance(iterimSituation.getPosition());
        distance.switchUnit(CLengthUnit::NM()); // lloks nicer
        CPropertyIndexVariantMap vm;
        vm.addValue(CSimulatedAircraft::IndexSituation, iterimSituation);
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
        this->m_otherClients.removeByCallsign(callsign);
        this->removeFromAircraftCaches(callsign);

        // block for lock scope
        {
            QWriteLocker l1(&m_lockParts);
            QWriteLocker l2(&m_lockSituations);
            m_situationsByCallsign.remove(callsign);
            m_partsByCallsign.remove(callsign);
            m_aircraftSupportingParts.remove(callsign);
        }

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

    void CAirspaceMonitor::ps_sendInterimPositions()
    {
        Q_ASSERT(CThreadUtils::isCurrentThreadObjectThread(this));
        if (!this->m_connected || !m_sendInterimPositions) { return; }
        CSimulatedAircraftList aircrafts = m_aircraftInRange.findBy(&CSimulatedAircraft::fastPositionUpdates, true);
        m_network->sendInterimPositions(aircrafts.getCallsigns());
    }

    void CAirspaceMonitor::storeAircraftSituation(const CAircraftSituation &situation)
    {
        const CCallsign callsign(situation.getCallsign());
        Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "empty callsign");
        if (callsign.isEmpty()) { return; }

        // list from new to old
        QWriteLocker lock(&m_lockSituations);
        CAircraftSituationList &l = this->m_situationsByCallsign[callsign];
        l.push_frontMaxElements(situation, MaxSituationsPerCallsign);

        // check sort order
        Q_ASSERT_X(l.size() < 2 || l[0].getMSecsSinceEpoch() >= l[1].getMSecsSinceEpoch(), Q_FUNC_INFO, "wrong sort order");
    }

    void CAirspaceMonitor::storeAircraftParts(const CCallsign &callsign, const CAircraftParts &parts)
    {
        if (callsign.isEmpty()) { return; }

        // list sorted from new to old
        QWriteLocker lock(&m_lockParts);
        CAircraftPartsList &l = this->m_partsByCallsign[callsign];
        l.push_frontMaxElements(parts, MaxPartsPerCallsign);

        if (!m_aircraftSupportingParts.contains(callsign))
        {
            m_aircraftSupportingParts.push_back(callsign); // mark as callsign which supports parts
        }

        // check sort order
        Q_ASSERT_X(l.size() < 2 || l[0].getMSecsSinceEpoch() >= l[1].getMSecsSinceEpoch(), Q_FUNC_INFO, "wrong sort order");
    }

} // namespace
