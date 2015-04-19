/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "airspace_monitor.h"
#include "blackcore/blackcorefreefunctions.h"
#include "blackmisc/project.h"
#include "blackmisc/testing.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/propertyindexallclasses.h"

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackCore
{

    CAirspaceMonitor::CAirspaceMonitor(QObject *parent, const BlackMisc::Simulation::IOwnAircraftProviderReadOnly *ownAircraftProvider, INetwork *network, CVatsimBookingReader *bookings, CVatsimDataFileReader *dataFile)
        : QObject(parent),
          COwnAircraftAwareReadOnly(ownAircraftProvider),
          m_network(network), m_vatsimBookingReader(bookings), m_vatsimDataFileReader(dataFile),
          m_atcWatchdog(this), m_aircraftWatchdog(this)
    {
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
        this->connect(this->m_vatsimBookingReader, &CVatsimBookingReader::dataRead, this, &CAirspaceMonitor::ps_receivedBookings);
        this->connect(this->m_vatsimDataFileReader, &CVatsimDataFileReader::dataRead, this, &CAirspaceMonitor::ps_receivedDataFile);

        // Watchdog
        // ATC stations send updates every 25s. Configure timeout after 50s.
        this->m_atcWatchdog.setTimeout(CTime(50, CTimeUnit::s()));
        this->connect(&this->m_aircraftWatchdog, &CAirspaceWatchdog::timeout, this, &CAirspaceMonitor::ps_pilotDisconnected);
        this->connect(&this->m_atcWatchdog,      &CAirspaceWatchdog::timeout, this, &CAirspaceMonitor::ps_atcControllerDisconnected);

        this->connect(&m_interimPositionUpdateTimer, &QTimer::timeout, this, &CAirspaceMonitor::ps_sendInterimPosition);
    }

    const CSimulatedAircraftList &CAirspaceMonitor::remoteAircraft() const
    {
        // not thread safe, check
        Q_ASSERT(this->thread() == QThread::currentThread());
        return m_aircraftInRange;
    }

    CSimulatedAircraftList &CAirspaceMonitor::remoteAircraft()
    {
        // not thread safe, check
        Q_ASSERT(this->thread() == QThread::currentThread());
        return m_aircraftInRange;
    }

    const CAircraftSituationList &CAirspaceMonitor::remoteAircraftSituations() const
    {
        // not thread safe, check
        Q_ASSERT(this->thread() == QThread::currentThread());
        return m_aircraftSituations;
    }

    CAircraftSituationList &CAirspaceMonitor::remoteAircraftSituations()
    {
        // not thread safe, check
        Q_ASSERT(this->thread() == QThread::currentThread());
        return m_aircraftSituations;
    }

    CAircraftSituationList CAirspaceMonitor::getRenderedAircraftSituations() const
    {
        Q_ASSERT(this->thread() == QThread::currentThread());
        return this->m_aircraftSituations;
    }

    const CAircraftPartsList &CAirspaceMonitor::remoteAircraftParts() const
    {
        // not thread safe, check
        Q_ASSERT(this->thread() == QThread::currentThread());
        return m_aircraftParts;
    }

    CAircraftPartsList &CAirspaceMonitor::remoteAircraftParts()
    {
        // not thread safe, check
        Q_ASSERT(this->thread() == QThread::currentThread());
        return m_aircraftParts;
    }

    CAircraftPartsList CAirspaceMonitor::getRenderedAircraftParts() const
    {
        Q_ASSERT(this->thread() == QThread::currentThread());
        return this->m_aircraftParts;
    }

    bool CAirspaceMonitor::connectRemoteAircraftProviderSignals(
        std::function<void(const CAircraftSituation &)> situationSlot,
        std::function<void(const CAircraftParts &)> partsSlot,
        std::function<void(const CCallsign &)> removedAircraftSlot
    )
    {
        // bool s1 = connect(this, &CAirspaceMonitor::addedRemoteAircraftSituation, situationSlot);
        bool s1 = connect(this->m_network, &INetwork::aircraftPositionUpdate, situationSlot);
        Q_ASSERT(s1);
        bool s2 = connect(this, &CAirspaceMonitor::addedRemoteAircraftParts, partsSlot);
        Q_ASSERT(s2);
        bool s3 = connect(this, &CAirspaceMonitor::removedRemoteAircraft, removedAircraftSlot);
        Q_ASSERT(s3);
        return s1 && s2 && s3;
    }

    bool CAirspaceMonitor::updateAircraftEnabled(const CCallsign &callsign, bool enabledForRedering, const QString &originator)
    {
        CPropertyIndexVariantMap vm(CSimulatedAircraft::IndexEnabled, CVariant::fromValue(enabledForRedering));
        Q_UNUSED(originator);
        int c = m_aircraftInRange.applyIfCallsign(callsign, vm);
        return c > 0;
    }

    bool CAirspaceMonitor::updateAircraftModel(const CCallsign &callsign, const CAircraftModel &model, const QString &originator)
    {
        Q_UNUSED(originator);
        CSimulatedAircraft aircraft = m_aircraftInRange.findFirstByCallsign(callsign);
        if (!aircraft.hasValidCallsign()) { return false; }

        aircraft.setModel(model); // this consolidates all common data such as callsign, ICAO ...

        CPropertyIndexVariantMap vm(CSimulatedAircraft::IndexModel, aircraft.getModel().toCVariant());
        int c = m_aircraftInRange.applyIfCallsign(callsign, vm);
        return c > 0;
    }

    bool CAirspaceMonitor::updateFastPositionEnabled(const CCallsign &callsign, bool enableFastPositonUpdates, const QString &originator)
    {
        CPropertyIndexVariantMap vm(CSimulatedAircraft::IndexFastPositionUpdates, CVariant::fromValue(enableFastPositonUpdates));
        Q_UNUSED(originator);
        int c = m_aircraftInRange.applyIfCallsign(callsign, vm);
        return c > 0;
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
        foreach(CAircraft aircraft, this->m_aircraftInRange)
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
        if (!ownAircraft().getCallsign().isEmpty() && searchList.contains(ownAircraft().getCallsign()))
        {
            searchList.remove(ownAircraft().getCallsign());
            users.push_back(ownAircraft().getPilot());
        }

        // do aircraft first, this will handle most callsigns
        foreach(CAircraft aircraft, this->m_aircraftInRange)
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

        foreach(CAtcStation station, this->m_atcStationsOnline)
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
        // these are the ones not in range
        foreach(CCallsign callsign, searchList)
        {
            CUserList usersByCallsign = this->m_vatsimDataFileReader->getUsersForCallsign(callsign);
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
        if (callsigns.isEmpty()) return clients;
        for (const CCallsign &callsign : callsigns)
        {
            clients.push_back(this->m_otherClients.findBy(&CClient::getCallsign, callsign));
        }
        return clients;
    }

    CClientList CAirspaceMonitor::getOtherClients() const
    {
        return m_otherClients;
    }

    BlackMisc::Aviation::CInformationMessage CAirspaceMonitor::getMetar(const BlackMisc::Aviation::CAirportIcao &airportIcaoCode)
    {
        CInformationMessage metar;
        if (airportIcaoCode.isEmpty()) return metar;
        if (this->m_metarCache.contains(airportIcaoCode)) metar = this->m_metarCache[airportIcaoCode];
        if (metar.isEmpty() || metar.timeDiffReceivedMs() > 10 * 1000)
        {
            // outdated, or not in cache at all
            this->m_network->sendMetarQuery(airportIcaoCode);

            // with this little trick we try to make an asynchronous signal / slot
            // based approach a synchronous return value
            QTime waitForMetar = QTime::currentTime().addMSecs(1000);
            while (QTime::currentTime() < waitForMetar)
            {
                // process some other events and hope network answer is received already
                QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
                if (m_metarCache.contains(airportIcaoCode))
                {
                    metar = this->m_metarCache[airportIcaoCode];
                    break;
                }
            }
        }
        return metar;
    }

    CAtcStation CAirspaceMonitor::getAtcStationForComUnit(const CComSystem &comSystem)
    {
        CAtcStation station;
        CAtcStationList stations = this->m_atcStationsOnline.findIfComUnitTunedIn25KHz(comSystem);
        if (!stations.isEmpty())
        {
            stations.sortBy(&CAtcStation::getDistanceToOwnAircraft);
            station = stations.front();
        }
        return station;
    }

    void CAirspaceMonitor::requestDataUpdates()
    {
        if (!this->m_network->isConnected()) return;

        foreach(CAircraft aircraft, this->m_aircraftInRange)
        {
            const CCallsign cs(aircraft.getCallsign());
            this->m_network->sendFrequencyQuery(cs);

            // we only query ICAO if we have none yet
            // it happens sometimes with some FSD servers (e.g our testserver) a first query is skipped
            if (!aircraft.hasValidAircraftDesignator())
            {
                this->m_network->sendIcaoCodesQuery(cs);
            }
        }
    }

    void CAirspaceMonitor::requestAtisUpdates()
    {
        if (!this->m_network->isConnected()) return;

        foreach(CAtcStation station, this->m_atcStationsOnline)
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

    void CAirspaceMonitor::testAddAircraftParts(const CAircraftParts &parts, bool incremental)
    {
        this->ps_aircraftConfigReceived(parts.getCallsign(), parts.toJson(), !incremental);
    }

    void CAirspaceMonitor::clear()
    {
        m_metarCache.clear();
        m_flightPlanCache.clear();
        m_icaoCodeCache.clear();

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
        if (enable) m_interimPositionUpdateTimer.start();
        else m_interimPositionUpdateTimer.stop();
        m_sendInterimPositions = enable;
    }

    bool CAirspaceMonitor::isFastPositionSendingEnabled() const
    {
        return m_sendInterimPositions;
    }

    void CAirspaceMonitor::ps_realNameReplyReceived(const CCallsign &callsign, const QString &realname)
    {
        Q_ASSERT(this->m_vatsimDataFileReader);
        if (!this->m_connected || realname.isEmpty()) { return; }
        CPropertyIndexVariantMap vm({CAtcStation::IndexController, CUser::IndexRealName}, realname);
        this->m_atcStationsOnline.applyIf(&CAtcStation::getCallsign, callsign, vm);
        this->m_atcStationsBooked.applyIf(&CAtcStation::getCallsign, callsign, vm);

        CVoiceCapabilities caps = this->m_vatsimDataFileReader->getVoiceCapabilityForCallsign(callsign);
        vm = CPropertyIndexVariantMap({CAircraft::IndexPilot, CUser::IndexRealName}, realname);
        vm.addValue({ CSimulatedAircraft::IndexClient, CClient::IndexUser, CUser::IndexRealName }, realname);
        vm.addValue({ CSimulatedAircraft::IndexClient, CClient::IndexVoiceCapabilities }, caps);

        this->m_aircraftInRange.applyIf(&CAircraft::getCallsign, callsign, vm);

        // Client
        vm = CPropertyIndexVariantMap({CClient::IndexUser, CUser::IndexRealName}, realname);
        vm.addValue({ CClient::IndexVoiceCapabilities }, caps);
        if (!this->m_otherClients.contains(&CClient::getCallsign, callsign)) { this->m_otherClients.push_back(CClient(callsign)); }
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

        CPropertyIndexVariantMap vm(CClient::IndexCapabilities, capabilities.toCVariant());
        CVoiceCapabilities caps = m_vatsimDataFileReader->getVoiceCapabilityForCallsign(callsign);
        vm.addValue({CClient::IndexVoiceCapabilities}, caps);
        if (!this->m_otherClients.contains(&CClient::getCallsign, callsign)) { this->m_otherClients.push_back(CClient(callsign)); }
        this->m_otherClients.applyIf(&CClient::getCallsign, callsign, vm);

        // apply same to client in aircraft
        vm.prependIndex(static_cast<int>(CSimulatedAircraft::IndexClient));
        this->m_aircraftInRange.applyIf(&CSimulatedAircraft::getCallsign, callsign, vm);

        if (flags & INetwork::SupportsAircraftConfigs) m_network->sendAircraftConfigQuery(callsign);
    }

    void CAirspaceMonitor::ps_customFSinnPacketReceived(const CCallsign &callsign, const QString &airlineIcao, const QString &aircraftDesignator, const QString &combinedAircraftType, const QString &model)
    {
        if (!this->m_connected || callsign.isEmpty() || model.isEmpty()) { return; }

        // Request of other client, I can get the other's model from that
        CPropertyIndexVariantMap vm({ CClient::IndexModel, CAircraftModel::IndexModelString }, model);
        vm.addValue({ CClient::IndexModel, CAircraftModel::IndexModelType }, static_cast<int>(CAircraftModel::TypeQueriedFromNetwork));
        if (!this->m_otherClients.contains(&CClient::getCallsign, callsign))
        {
            // with custom packets it can happen,
            // the packet is received before any other packet
            this->m_otherClients.push_back(CClient(callsign));
        }
        this->m_otherClients.applyIf(&CClient::getCallsign, callsign, vm);
        vm.prependIndex(static_cast<int>(CSimulatedAircraft::IndexClient));
        this->m_aircraftInRange.applyIf(&CSimulatedAircraft::getCallsign, callsign, vm);

        // ICAO response from custom data
        if (!aircraftDesignator.isEmpty())
        {
            CAircraftIcao icao(aircraftDesignator, combinedAircraftType, airlineIcao, "", ""); // from custom packet
            if (this->m_aircraftInRange.containsCallsign(callsign))
            {
                // we have that aircraft, set straight away
                this->ps_icaoCodesReceived(callsign, icao);
            }
            else
            {
                // store in cache
                this->m_icaoCodeCache.insert(callsign, icao);
            }
        }
    }

    void CAirspaceMonitor::ps_serverReplyReceived(const CCallsign &callsign, const QString &server)
    {
        if (!this->m_connected || callsign.isEmpty() || server.isEmpty()) { return; }
        if (!this->m_otherClients.contains(&CClient::getCallsign, callsign)) { this->m_otherClients.push_back(CClient(callsign)); }
        CPropertyIndexVariantMap vm(CClient::IndexServer, server);
        this->m_otherClients.applyIf(&CClient::getCallsign, callsign, vm);
    }

    void CAirspaceMonitor::ps_metarReceived(const QString &metarMessage)
    {
        if (!this->m_connected || metarMessage.length() < 10) return; // invalid
        const QString icaoCode = metarMessage.left(4).toUpper();
        const QString icaoCodeTower = icaoCode + "_TWR";
        CCallsign callsignTower(icaoCodeTower);
        CInformationMessage metar(CInformationMessage::METAR, metarMessage);

        // add METAR to existing stations
        CPropertyIndexVariantMap vm(CAtcStation::IndexMetar, metar.toCVariant());
        this->m_atcStationsOnline.applyIf(&CAtcStation::getCallsign, callsignTower, vm);
        this->m_atcStationsBooked.applyIf(&CAtcStation::getCallsign, callsignTower, vm);
        this->m_metarCache.insert(icaoCode, metar);
        if (this->m_atcStationsOnline.contains(&CAtcStation::getCallsign, callsignTower)) { emit this->changedAtcStationsOnline(); }
        if (this->m_atcStationsBooked.contains(&CAtcStation::getCallsign, callsignTower)) { emit this->changedAtcStationsBooked(); }
    }

    void CAirspaceMonitor::ps_flightPlanReceived(const CCallsign &callsign, const CFlightPlan &flightPlan)
    {
        CFlightPlan plan(flightPlan);
        plan.setWhenLastSentOrLoaded(QDateTime::currentDateTimeUtc());
        this->m_flightPlanCache.insert(callsign, plan);
    }

    void CAirspaceMonitor::removeAllOnlineAtcStations()
    {
        m_atcWatchdog.removeAll();
        m_atcStationsOnline.clear();
    }

    void CAirspaceMonitor::removeAllAircraft()
    {
        m_aircraftWatchdog.removeAll(); // upfront
        for (CAircraft aircraft : m_aircraftInRange)
        {
            const CCallsign cs(aircraft.getCallsign());
            emit removedRemoteAircraft(cs);
        }
        m_aircraftSituations.clear();
        m_aircraftParts.clear();
        m_aircraftInRange.clear();
        m_flightPlanCache.clear();
        m_icaoCodeCache.clear();
    }

    void CAirspaceMonitor::removeAllOtherClients()
    {
        m_otherClients.clear();
    }

    void CAirspaceMonitor::removeFromAircraftCaches(const CCallsign &callsign)
    {
        if (callsign.isEmpty()) { return; }
        this->m_icaoCodeCache.remove(callsign);
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
        Q_ASSERT(BlackCore::isCurrentThreadCreatingThread(this));
        this->m_atcStationsBooked.clear();
        foreach(CAtcStation bookedStation, bookedStations)
        {
            // complete by VATSIM data file data
            this->m_vatsimDataFileReader->getAtcStations().updateFromVatsimDataFileStation(bookedStation);
            // exchange booking and online data, both sides are updated
            this->m_atcStationsOnline.mergeWithBooking(bookedStation);
            // into list
            this->m_atcStationsBooked.push_back(bookedStation);
        }
        emit this->changedAtcStationsBooked(); // all booked stations reloaded
    }

    void CAirspaceMonitor::ps_receivedDataFile()
    {
        Q_ASSERT(BlackCore::isCurrentThreadCreatingThread(this));
        for (auto i = this->m_otherClients.begin(); i != this->m_otherClients.end(); ++i)
        {
            CClient client = (*i);
            if (client.hasSpecifiedVoiceCapabilities()) continue;
            CVoiceCapabilities vc = this->m_vatsimDataFileReader->getVoiceCapabilityForCallsign(client.getCallsign());
            if (vc.isUnknown()) continue;
            client.setVoiceCapabilities(vc);
            (*i) = client;
        }
    }

    void CAirspaceMonitor::ps_sendReadyForModelMatching(const CCallsign &callsign, int trial)
    {
        // some checks for special conditions, e.g. logout -> empty list, but still signals pending
        if (!this->m_connected || this->m_aircraftInRange.isEmpty()) { return; }
        if (!this->m_aircraftInRange.containsCallsign(callsign)) { return; }

        // build simulated aircraft and crosscheck if all data are available
        CSimulatedAircraft remoteAircraft(this->m_aircraftInRange.findFirstByCallsign(callsign));
        Q_ASSERT_X(remoteAircraft.hasValidCallsign(), "ps_sendReadyForModelMatching", "Inavlid callsign");
        CClient remoteClient = this->m_otherClients.findFirstByCallsign(callsign);
        remoteAircraft.setClient(remoteClient);
        remoteAircraft.setModel(remoteClient.getAircraftModel());

        // check if the name and ICAO query went properly through
        bool dataComplete =
            remoteAircraft.hasValidAircraftDesignator() &&
            (!m_serverSupportsNameQuery || remoteAircraft.hasValidRealName());

        if (trial < 3 && !dataComplete)
        {
            // allow another period for the client data to arrive, otherwise go ahead
            this->fireDelayedReadyForModelMatching(callsign, trial + 1);
            return;
        }

        Q_ASSERT(remoteAircraft.hasValidAircraftDesignator());
        Q_ASSERT(!m_serverSupportsNameQuery || remoteAircraft.hasValidRealName());
        emit this->readyForModelMatching(remoteAircraft);
    }

    void CAirspaceMonitor::ps_atcPositionUpdate(const CCallsign &callsign, const BlackMisc::PhysicalQuantities::CFrequency &frequency, const CCoordinateGeodetic &position, const BlackMisc::PhysicalQuantities::CLength &range)
    {
        Q_ASSERT(BlackCore::isCurrentThreadCreatingThread(this));
        Q_ASSERT(CComSystem::isValidCivilAviationFrequency(frequency));
        if (!this->m_connected) { return; }
        CAtcStationList stationsWithCallsign = this->m_atcStationsOnline.findByCallsign(callsign);
        if (stationsWithCallsign.isEmpty())
        {
            // new station
            CAtcStation station;
            station.setCallsign(callsign);
            station.setRange(range);
            station.setFrequency(frequency);
            station.setPosition(position);
            station.setOnline(true);
            station.calculcateDistanceAndBearingToOwnAircraft(ownAircraft().getPosition());
            this->m_vatsimDataFileReader->getAtcStations().updateFromVatsimDataFileStation(station); // prefill
            this->m_atcStationsOnline.push_back(station);

            if (this->m_network->isConnected())
            {
                emit this->m_network->sendRealNameQuery(callsign);
                emit this->m_network->sendAtisQuery(callsign); // request ATIS and voice rooms
                emit this->m_network->sendServerQuery(callsign);
            }

            this->m_atcWatchdog.addCallsign(callsign);
            emit this->changedAtcStationsOnline();
            // Remark: this->changedAtcStationOnlineConnectionStatus(station, true);
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
            this->m_atcWatchdog.resetCallsign(callsign);
        }
    }

    void CAirspaceMonitor::ps_atcControllerDisconnected(const CCallsign &callsign)
    {
        Q_ASSERT(BlackCore::isCurrentThreadCreatingThread(this));

        this->m_atcWatchdog.removeCallsign(callsign);
        this->m_otherClients.removeByCallsign(callsign);

        if (this->m_atcStationsOnline.contains(&CAtcStation::getCallsign, callsign))
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
        Q_ASSERT(BlackCore::isCurrentThreadCreatingThread(this));
        if (!this->m_connected || callsign.isEmpty()) return;
        CPropertyIndexVariantMap vm(CAtcStation::IndexAtis, atisMessage.toCVariant());
        int changedOnline = this->m_atcStationsOnline.applyIf(&CAtcStation::getCallsign, callsign, vm);

        // receiving an ATIS means station is online, update in bookings
        vm.addValue(CAtcStation::IndexIsOnline, true);
        int changedBooked = this->m_atcStationsBooked.applyIf(&CAtcStation::getCallsign, callsign, vm);
        if (changedOnline > 0) { emit this->changedAtcStationsOnline(); }
        if (changedBooked > 0) { emit this->changedAtcStationsBooked(); }
    }

    void CAirspaceMonitor::ps_atisVoiceRoomReceived(const CCallsign &callsign, const QString &url)
    {
        Q_ASSERT(BlackCore::isCurrentThreadCreatingThread(this));
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
        vm = CPropertyIndexVariantMap(CClient::IndexVoiceCapabilities, CVoiceCapabilities::fromVoiceCapabilities(CVoiceCapabilities::Voice).toCVariant());
        this->m_otherClients.applyIf(&CClient::getCallsign, callsign, vm, false);
    }

    void CAirspaceMonitor::ps_atisLogoffTimeReceived(const CCallsign &callsign, const QString &zuluTime)
    {
        Q_ASSERT(BlackCore::isCurrentThreadCreatingThread(this));
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

    void CAirspaceMonitor::ps_icaoCodesReceived(const CCallsign &callsign, const CAircraftIcao &icaoData)
    {
        Q_ASSERT(BlackCore::isCurrentThreadCreatingThread(this));
        Q_ASSERT(!callsign.isEmpty());
        if (!this->m_connected) { return; }

        // update
        CPropertyIndexVariantMap vm(CAircraft::IndexIcao, icaoData.toCVariant());
        if (!icaoData.hasAircraftDesignator())
        {
            // empty so far, try to fetch from data file
            CLogMessage(this).warning("Empty ICAO info for %1 %2") << callsign.toQString() << icaoData.toQString();
            CAircraftIcao icaoDataFromDataFile = this->m_vatsimDataFileReader->getIcaoInfo(callsign);
            if (!icaoDataFromDataFile.hasAircraftDesignator()) { return; } // give up!
            vm = CPropertyIndexVariantMap(CAircraft::IndexIcao, icaoDataFromDataFile.toCVariant());
        }
        // ICAO code received when aircraft is already removed or not yet ready
        // We add it to cache and use it when aircraft is created
        if (!this->m_aircraftInRange.containsCallsign(callsign))
        {
            this->m_icaoCodeCache.insert(callsign, icaoData);
            return;
        }

        // update
        int c = this->m_aircraftInRange.applyIfCallsign(callsign, vm);
        if (c > 0) { ps_sendReadyForModelMatching(callsign, 1); }
    }

    void CAirspaceMonitor::ps_aircraftUpdateReceived(const CAircraftSituation &situation, const CTransponder &transponder)
    {
        Q_ASSERT_X(BlackCore::isCurrentThreadCreatingThread(this), "ps_aircraftUpdateReceived", "Called in different thread");
        if (!this->m_connected) { return; }

        CCallsign callsign(situation.getCallsign());
        Q_ASSERT_X(!callsign.isEmpty(), "ps_aircraftUpdateReceived", "Empty callsign");

        // store situation history
        this->m_aircraftSituations.push_front(situation);
        this->m_aircraftSituations.removeOlderThanNowMinusOffset(AircraftSituationsRemovedOffsetMs);
        emit this->addedRemoteAircraftSituation(situation);

        bool exists = this->m_aircraftInRange.containsCallsign(callsign);
        if (!exists)
        {
            // new aircraft
            CSimulatedAircraft aircraft;
            aircraft.setCallsign(callsign);
            aircraft.setSituation(situation);
            aircraft.setTransponder(transponder);
            aircraft.calculcateDistanceAndBearingToOwnAircraft(ownAircraft().getPosition()); // distance from myself

            // ICAO from cache if avialable
            bool setIcao = false;
            if (this->m_icaoCodeCache.contains(callsign))
            {
                CAircraftIcao icao = this->m_icaoCodeCache.value(callsign);
                this->m_icaoCodeCache.remove(callsign);
                aircraft.setIcaoInfo(icao);
                setIcao = true;
            }

            this->m_vatsimDataFileReader->updateWithVatsimDataFileData(aircraft);

            // only place where aircraft is added
            this->m_aircraftInRange.push_back(aircraft);

            // new client, there is a chance it has been already created by custom packet
            if (!this->m_otherClients.contains(&CClient::getCallsign, callsign))
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
                if (c.getAircraftModel().getModelType() != CAircraftModel::TypeQueriedFromNetwork)
                {
                    this->m_network->sendCustomFsinnQuery(callsign);
                }
                this->m_network->sendFrequencyQuery(callsign);
                this->m_network->sendRealNameQuery(callsign);
                this->m_network->sendCapabilitiesQuery(callsign);
                this->m_network->sendServerQuery(callsign);

                // keep as last
                if (setIcao)
                {
                    this->fireDelayedReadyForModelMatching(callsign);
                }
                else
                {
                    this->m_network->sendIcaoCodesQuery(callsign);
                }

                this->m_aircraftWatchdog.addCallsign(callsign);
                emit this->addedAircraft(aircraft);
            } // connected
        }
        else
        {
            // update, aircraft already exists
            CLength distance = ownAircraft().calculateGreatCircleDistance(situation.getPosition());
            distance.switchUnit(CLengthUnit::NM());
            CPropertyIndexVariantMap vm;
            vm.addValue(CAircraft::IndexTransponder, transponder);
            vm.addValue(CAircraft::IndexSituation, situation);
            vm.addValue(CAircraft::IndexDistanceToOwnAircraft, distance);

            // here I expect always a changed value
            this->m_aircraftInRange.applyIfCallsign(callsign, vm);
            this->m_aircraftWatchdog.resetCallsign(callsign);
        }

        emit this->changedAircraftInRange();
    }

    void CAirspaceMonitor::ps_aircraftInterimUpdateReceived(const CAircraftSituation &situation)
    {
        Q_ASSERT_X(BlackCore::isCurrentThreadCreatingThread(this), "ps_aircraftInterimUpdateReceived", "Called in different thread");
        if (!this->m_connected) { return; }

        CCallsign callsign(situation.getCallsign());
        Q_ASSERT_X(!callsign.isEmpty(), "ps_aircraftInterimUpdateReceived", "Empty callsign");

        // todo: Check if the timestamp is copied here as well.
        CAircraftSituation sitationCopy(situation);

        // Interim packets do not have groundspeed, hence set the last known value.
        // If there is no full position available yet, throw this interim position away.
        auto history = this->m_aircraftSituations.findByCallsign(callsign);
        if (history.empty()) return;
        sitationCopy.setGroundspeed(history.latestValue().getGroundSpeed());

        // store situation history
        this->m_aircraftSituations.push_front(situation);
        this->m_aircraftSituations.removeOlderThanNowMinusOffset(AircraftSituationsRemovedOffsetMs);
        emit this->addedRemoteAircraftSituation(situation);

        // update
        CLength distance = ownAircraft().calculateGreatCircleDistance(situation.getPosition());
        distance.switchUnit(CLengthUnit::NM());
        CPropertyIndexVariantMap vm;
        vm.addValue(CAircraft::IndexSituation, situation);
        vm.addValue(CAircraft::IndexDistanceToOwnAircraft, distance);

        // here I expect always a changed value
        this->m_aircraftInRange.applyIfCallsign(callsign, vm);
        this->m_aircraftWatchdog.resetCallsign(callsign);

        emit this->changedAircraftInRange();
    }

    void CAirspaceMonitor::ps_pilotDisconnected(const CCallsign &callsign)
    {
        Q_ASSERT(BlackCore::isCurrentThreadCreatingThread(this));
        bool contains = this->m_aircraftInRange.containsCallsign(callsign);

        // if with contains false remove here, in case of inconsistencies
        this->m_aircraftWatchdog.removeCallsign(callsign);
        this->m_otherClients.removeByCallsign(callsign);
        this->m_aircraftSituations.removeByCallsign(callsign);
        this->m_aircraftParts.removeByCallsign(callsign);
        this->removeFromAircraftCaches(callsign);

        if (contains)
        {
            this->m_aircraftInRange.removeByCallsign(callsign);
            emit this->removedRemoteAircraft(callsign);
        }
    }

    void CAirspaceMonitor::ps_frequencyReceived(const CCallsign &callsign, const CFrequency &frequency)
    {
        Q_ASSERT(BlackCore::isCurrentThreadCreatingThread(this));

        // update
        CPropertyIndexVariantMap vm({CAircraft::IndexCom1System, CComSystem::IndexActiveFrequency}, frequency.toCVariant());
        int changed = this->m_aircraftInRange.applyIf(&CAircraft::getCallsign, callsign, vm, true);
        if (changed > 0) { emit this->changedAircraftInRange(); }
    }

    void CAirspaceMonitor::ps_aircraftConfigReceived(const BlackMisc::Aviation::CCallsign &callsign,  const QJsonObject &jsonObject, bool isFull)
    {
        Q_ASSERT(BlackCore::isCurrentThreadCreatingThread(this));

        CSimulatedAircraftList list = this->m_aircraftInRange.findByCallsign(callsign);
        // Skip unknown callsigns
        if (list.isEmpty()) { return; }

        CSimulatedAircraft simAircraft = list.front();
        // If we are not yet synchronized, we throw away any incremental packet
        if (!simAircraft.isPartsSynchronized() && !isFull) { return; }

        CAircraftParts parts;
        if (isFull)
        {
            parts.convertFromJson(jsonObject);
        }
        else
        {
            // incremental update
            parts = m_aircraftParts.findFirstByCallsign(callsign);
            QJsonObject config = applyIncrementalObject(parts.toJson(), jsonObject);
            parts.convertFromJson(config);
        }

        // make sure in any case right time / callsign
        parts.setCurrentUtcTime();
        parts.setCallsign(callsign);

        // store part history
        this->m_aircraftParts.push_front(parts);
        this->m_aircraftParts.removeOlderThanNowMinusOffset(AircraftPartsRemoveOffsetMs);
        emit this->addedRemoteAircraftParts(parts);

        // here I expect always a changed value
        this->m_aircraftInRange.setAircraftParts(callsign, parts);
        this->m_aircraftWatchdog.resetCallsign(callsign);
    }

    void CAirspaceMonitor::ps_sendInterimPosition()
    {
        Q_ASSERT(BlackCore::isCurrentThreadCreatingThread(this));
        if (!this->m_connected || !m_sendInterimPositions) { return; }
        CSimulatedAircraftList aircrafts = m_aircraftInRange.findBy(&CSimulatedAircraft::fastPositionUpdates, true);
        m_network->sendInterimPosition(aircrafts.getCallsigns());
    }

} // namespace
