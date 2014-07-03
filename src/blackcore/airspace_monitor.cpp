/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "airspace_monitor.h"
#include "blackmisc/project.h"
#include "blackmisc/indexvariantmap.h"

// KB_REMOVE with debug log message
#include <QThread>

namespace BlackCore
{

    using namespace BlackMisc;
    using namespace BlackMisc::Aviation;
    using namespace BlackMisc::Network;
    using namespace BlackMisc::Geo;
    using namespace BlackMisc::PhysicalQuantities;

    CAirspaceMonitor::CAirspaceMonitor(QObject *parent, INetwork *network, CVatsimBookingReader *bookings, CVatsimDataFileReader *dataFile)
        : QObject(parent), m_network(network), m_vatsimBookingReader(bookings), m_vatsimDataFileReader(dataFile)
    {
        this->connect(this->m_network, &INetwork::atcPositionUpdate, this, &CAirspaceMonitor::atcPositionUpdate);
        this->connect(this->m_network, &INetwork::atisReplyReceived, this, &CAirspaceMonitor::atisReceived);
        this->connect(this->m_network, &INetwork::atisVoiceRoomReplyReceived, this, &CAirspaceMonitor::atisVoiceRoomReceived);
        this->connect(this->m_network, &INetwork::atisLogoffTimeReplyReceived, this, &CAirspaceMonitor::atisLogoffTimeReceived);
        this->connect(this->m_network, &INetwork::metarReplyReceived, this, &CAirspaceMonitor::metarReceived);
        this->connect(this->m_network, &INetwork::flightPlanReplyReceived, this, &CAirspaceMonitor::flightplanReceived);
        this->connect(this->m_network, &INetwork::realNameReplyReceived, this, &CAirspaceMonitor::realNameReplyReceived);
        this->connect(this->m_network, &INetwork::icaoCodesReplyReceived, this, &CAirspaceMonitor::icaoCodesReceived);
        this->connect(this->m_network, &INetwork::pilotDisconnected, this, &CAirspaceMonitor::pilotDisconnected);
        this->connect(this->m_network, &INetwork::atcDisconnected, this, &CAirspaceMonitor::atcControllerDisconnected);
        this->connect(this->m_network, &INetwork::aircraftPositionUpdate, this, &CAirspaceMonitor::aircraftUpdateReceived);
        this->connect(this->m_network, &INetwork::frequencyReplyReceived, this, &CAirspaceMonitor::frequencyReceived);
        this->connect(this->m_network, &INetwork::capabilitiesReplyReceived, this, &CAirspaceMonitor::capabilitiesReplyReceived);
        this->connect(this->m_network, &INetwork::fsipirCustomPacketReceived, this, &CAirspaceMonitor::fsipirCustomPacketReceived);
        this->connect(this->m_network, &INetwork::serverReplyReceived, this, &CAirspaceMonitor::serverReplyReceived);

        // AutoConnection: this should also avoid race conditions by updating the bookings
        this->connect(this->m_vatsimBookingReader, &CVatsimBookingReader::dataRead, this, &CAirspaceMonitor::receivedBookings);
    }

    CFlightPlan CAirspaceMonitor::loadFlightPlanFromNetwork(const CCallsign &callsign)
    {
        CFlightPlan plan;

        // use cache, but not for own callsign (always reload)
        if (this->m_flightPlanCache.contains(callsign)) plan = this->m_flightPlanCache[callsign];
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
        foreach(CAircraft aircraft, this->m_aircraftsInRange)
        {
            CUser user = aircraft.getPilot();
            users.push_back(user);
        }
        return users;
    }

    CUserList CAirspaceMonitor::getUsersForCallsigns(const CCallsignList &callsigns) const
    {
        CUserList users;
        if (callsigns.isEmpty()) return users;
        CCallsignList searchList(callsigns);

        // myself, which is not in the lists below
        CAircraft ownAircraft = this->m_ownAircraft;
        if (!ownAircraft.getCallsign().isEmpty() && searchList.contains(ownAircraft.getCallsign()))
        {
            searchList.remove(ownAircraft.getCallsign());
            users.push_back(ownAircraft.getPilot());
        }

        // do aircrafts first, this will handle most callsigns
        foreach(CAircraft aircraft, this->m_aircraftsInRange)
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

    CClientList CAirspaceMonitor::getOtherClientsForCallsigns(const CCallsignList &callsigns) const
    {
        CClientList clients;
        if (callsigns.isEmpty()) return clients;
        foreach(CCallsign callsign, callsigns)
        {
            clients.push_back(this->m_otherClients.findBy(&CClient::getCallsign, callsign));
        }
        return clients;
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
            stations.sortBy(&CAtcStation::getDistanceToPlane);
            station = stations.front();
        }
        return station;
    }

    void CAirspaceMonitor::requestDataUpdates()
    {
        if (!this->m_network->isConnected()) return;

        foreach(CAircraft aircraft, this->m_aircraftsInRange)
        {
            this->m_network->sendFrequencyQuery(aircraft.getCallsign());
            this->m_network->sendIcaoCodesQuery(aircraft.getCallsign());
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

    void CAirspaceMonitor::realNameReplyReceived(const CCallsign &callsign, const QString &realname)
    {
        if (realname.isEmpty()) return;
        CIndexVariantMap vm(CAtcStation::IndexControllerRealName, realname);
        this->m_atcStationsOnline.applyIf(&CAtcStation::getCallsign, callsign, vm);
        this->m_atcStationsBooked.applyIf(&CAtcStation::getCallsign, callsign, vm);

        vm = CIndexVariantMap(CAircraft::IndexPilotRealName, realname);
        this->m_aircraftsInRange.applyIf(&CAircraft::getCallsign, callsign, vm);

        vm = CIndexVariantMap(CClient::IndexRealName, realname);
        this->m_otherClients.applyIf(&CClient::getCallsign, callsign, vm);
    }

    void CAirspaceMonitor::capabilitiesReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, quint32 flags)
    {
        if (callsign.isEmpty()) return;
        CIndexVariantMap capabilities;
        capabilities.addValue(CClient::FsdAtisCanBeReceived, (flags & INetwork::AcceptsAtisResponses));
        capabilities.addValue(CClient::FsdWithInterimPositions, (flags & INetwork::SupportsInterimPosUpdates));
        capabilities.addValue(CClient::FsdWithModelDescription, (flags & INetwork::SupportsModelDescriptions));
        CIndexVariantMap vm(CClient::IndexCapabilities, capabilities.toQVariant());
        this->m_otherClients.applyIf(&CClient::getCallsign, callsign, vm);
    }

    void CAirspaceMonitor::fsipirCustomPacketReceived(const CCallsign &callsign, const QString &, const QString &, const QString &, const QString &model)
    {
        if (callsign.isEmpty() || model.isEmpty()) return;

        // Request of other client, I can get the other's model from that
        CIndexVariantMap vm(CClient::IndexQueriedModelString, QVariant(model));
        if (!this->m_otherClients.contains(&CClient::getCallsign, callsign))
        {
            // with custom packets it can happen,
            //the packet is received before any other packet
            this->m_otherClients.push_back(CClient(callsign));
        }
        this->m_otherClients.applyIf(&CClient::getCallsign, callsign, vm);
        this->sendFsipiCustomPacket(callsign); // response
    }

    void CAirspaceMonitor::serverReplyReceived(const CCallsign &callsign, const QString &server)
    {
        if (callsign.isEmpty() || server.isEmpty()) return;
        CIndexVariantMap vm(CClient::IndexServer, QVariant(server));
        this->m_otherClients.applyIf(&CClient::getCallsign, callsign, vm);
    }

    void CAirspaceMonitor::metarReceived(const QString &metarMessage)
    {
        if (metarMessage.length() < 10) return; // invalid
        const QString icaoCode = metarMessage.left(4).toUpper();
        const QString icaoCodeTower = icaoCode + "_TWR";
        CCallsign callsignTower(icaoCodeTower);
        CInformationMessage metar(CInformationMessage::METAR, metarMessage);

        // add METAR to existing stations
        CIndexVariantMap vm(CAtcStation::IndexMetar, metar.toQVariant());
        this->m_atcStationsOnline.applyIf(&CAtcStation::getCallsign, callsignTower, vm);
        this->m_atcStationsBooked.applyIf(&CAtcStation::getCallsign, callsignTower, vm);
        this->m_metarCache.insert(icaoCode, metar);
        if (this->m_atcStationsOnline.contains(&CAtcStation::getCallsign, callsignTower)) emit this->changedAtcStationsOnline();
        if (this->m_atcStationsBooked.contains(&CAtcStation::getCallsign, callsignTower)) emit this->changedAtcStationsBooked();
    }

    void CAirspaceMonitor::flightplanReceived(const CCallsign &callsign, const CFlightPlan &flightPlan)
    {
        CFlightPlan plan(flightPlan);
        plan.setWhenLastSentOrLoaded(QDateTime::currentDateTimeUtc());
        this->m_flightPlanCache.insert(callsign, plan);
    }

    void CAirspaceMonitor::sendFsipiCustomPacket(const CCallsign &recipientCallsign) const
    {
        CAircraft me = this->m_ownAircraft;
        CAircraftIcao icao = me.getIcaoInfo();
        QString modelString = this->m_ownAircraftModel.getModelString();
        if (modelString.isEmpty()) modelString = CProject::systemNameAndVersion();
        this->m_network->sendFsipiCustomPacket(recipientCallsign, icao.getAirlineDesignator(), icao.getAircraftDesignator(), icao.getAircraftCombinedType(), modelString);
    }

    void CAirspaceMonitor::sendFsipirCustomPacket(const CCallsign &recipientCallsign) const
    {
        CAircraft me = this->m_ownAircraft;
        CAircraftIcao icao = me.getIcaoInfo();
        QString modelString = this->m_ownAircraftModel.getModelString();
        if (modelString.isEmpty()) modelString = CProject::systemNameAndVersion();
        this->m_network->sendFsipirCustomPacket(recipientCallsign, icao.getAirlineDesignator(), icao.getAircraftDesignator(), icao.getAircraftCombinedType(), modelString);
    }

    void CAirspaceMonitor::receivedBookings(const CAtcStationList &bookedStations)
    {
        this->m_atcStationsBooked.clear();
        foreach(CAtcStation bookedStation, bookedStations)
        {
            // complete by VATSIM data file data
            this->m_vatsimDataFileReader->getAtcStations().updateFromVatsimDataFileStation(bookedStation);
            // exchange booking and online data
            this->m_atcStationsOnline.mergeWithBooking(bookedStation);
            // into list
            this->m_atcStationsBooked.push_back(bookedStation);
        }
    }

    void CAirspaceMonitor::atcPositionUpdate(const CCallsign &callsign, const BlackMisc::PhysicalQuantities::CFrequency &frequency, const CCoordinateGeodetic &position, const BlackMisc::PhysicalQuantities::CLength &range)
    {
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
            station.calculcateDistanceToPlane(this->m_ownAircraft.getPosition());
            this->m_vatsimDataFileReader->getAtcStations().updateFromVatsimDataFileStation(station); // prefill
            this->m_atcStationsOnline.push_back(station);

            if (this->m_network->isConnected())
            {
                emit this->m_network->sendRealNameQuery(callsign);
                emit this->m_network->sendAtisQuery(callsign); // request ATIS and voice rooms
                emit this->m_network->sendServerQuery(callsign);
            }

            emit this->changedAtcStationsOnline();
            // Remark: this->changedAtcStationOnlineConnectionStatus(station, true);
            // will be sent in psFsdAtisVoiceRoomReceived
        }
        else
        {
            // update
            CIndexVariantMap values;
            values.addValue(CAtcStation::IndexFrequency, frequency);
            values.addValue(CAtcStation::IndexPosition, position);
            values.addValue(CAtcStation::IndexRange, range);
            this->m_atcStationsOnline.applyIf(BlackMisc::Predicates::MemberEqual(&CAtcStation::getCallsign, callsign), values);
            emit this->changedAtcStationsOnline();
        }
    }

    void CAirspaceMonitor::atcControllerDisconnected(const CCallsign &callsign)
    {
        if (this->m_atcStationsOnline.contains(&CAtcStation::getCallsign, callsign))
        {
            CAtcStation removeStation = this->m_atcStationsOnline.findByCallsign(callsign).front();
            this->m_atcStationsOnline.removeIf(&CAtcStation::getCallsign, callsign);
            emit this->changedAtcStationsOnline();
            emit this->changedAtcStationOnlineConnectionStatus(removeStation, false);
        }

        // booked
        this->m_atcStationsBooked.applyIf(&CAtcStation::getCallsign, callsign, CIndexVariantMap(CAtcStation::IndexIsOnline, QVariant(false)));
    }

    void CAirspaceMonitor::atisReceived(const CCallsign &callsign, const CInformationMessage &atisMessage)
    {
        if (callsign.isEmpty()) return;
        CIndexVariantMap vm(CAtcStation::IndexAtis, atisMessage.toQVariant());
        this->m_atcStationsOnline.applyIf(&CAtcStation::getCallsign, callsign, vm);
        this->m_atcStationsBooked.applyIf(&CAtcStation::getCallsign, callsign, vm);
        if (this->m_atcStationsOnline.contains(&CAtcStation::getCallsign, callsign)) emit this->changedAtcStationsBooked();
        if (this->m_atcStationsBooked.contains(&CAtcStation::getCallsign, callsign)) emit this->changedAtcStationsBooked();
    }

    void CAirspaceMonitor::atisVoiceRoomReceived(const CCallsign &callsign, const QString &url)
    {
        QString trimmedUrl = url.trimmed();
        CIndexVariantMap vm(CAtcStation::IndexVoiceRoomUrl, trimmedUrl);
        if (this->m_atcStationsBooked.contains(&CAtcStation::getCallsign, callsign))
        {
            this->m_atcStationsBooked.applyIf(&CAtcStation::getCallsign, callsign, vm);
            emit this->changedAtcStationsBooked();
        }
        if (this->m_atcStationsOnline.contains(&CAtcStation::getCallsign, callsign))
        {
            this->m_atcStationsOnline.applyIf(&CAtcStation::getCallsign, callsign, vm);
            CAtcStation station = this->m_atcStationsOnline.findFirstByCallsign(callsign);
            emit this->changedAtcStationsBooked();
            emit this->changedAtcStationOnlineConnectionStatus(station, true);
        }
        vm = CIndexVariantMap(CClient::IndexVoiceCapabilities, CVoiceCapabilities(CVoiceCapabilities::Voice).toQVariant());
        this->m_otherClients.applyIf(&CClient::getCallsign, callsign, vm);
    }

    void CAirspaceMonitor::atisLogoffTimeReceived(const CCallsign &callsign, const QString &zuluTime)
    {
        if (zuluTime.length() == 4)
        {
            // Logic to set logoff time
            bool ok;
            int h = zuluTime.left(2).toInt(&ok);
            if (!ok) return;
            int m = zuluTime.right(2).toInt(&ok);
            if (!ok) return;
            QDateTime logoffDateTime = QDateTime::currentDateTimeUtc();
            logoffDateTime.setTime(QTime(h, m));
            CIndexVariantMap vm(CAtcStation::IndexBookedUntil, logoffDateTime);
            this->m_atcStationsOnline.applyIf(&CAtcStation::getCallsign, callsign, vm);
            this->m_atcStationsBooked.applyIf(&CAtcStation::getCallsign, callsign, vm);
            if (this->m_atcStationsOnline.contains(&CAtcStation::getCallsign, callsign)) emit this->changedAtcStationsBooked();
            if (this->m_atcStationsBooked.contains(&CAtcStation::getCallsign, callsign)) emit this->changedAtcStationsBooked();
        }
    }

    void CAirspaceMonitor::icaoCodesReceived(const CCallsign &callsign, const CAircraftIcao &icaoData)
    {
        // update
        CIndexVariantMap vm(CAircraft::IndexIcao, icaoData.toQVariant());
        if (!icaoData.hasAircraftDesignator())
        {
            // empty so far, try to fetch from data file
            qDebug() << "Empty ICAO info for " << callsign << icaoData;
            CAircraftIcao icaoDataDataFile = this->m_vatsimDataFileReader->getIcaoInfo(callsign);
            if (!icaoDataDataFile.hasAircraftDesignator()) return; // give up!
            vm = CIndexVariantMap(CAircraft::IndexIcao, icaoData.toQVariant());
        }
        this->m_aircraftsInRange.applyIf(BlackMisc::Predicates::MemberEqual(&CAircraft::getCallsign, callsign), vm);
        emit this->changedAircraftsInRange();
    }

    void CAirspaceMonitor::aircraftUpdateReceived(const CCallsign &callsign, const CAircraftSituation &situation, const CTransponder &transponder)
    {
        CAircraftList list = this->m_aircraftsInRange.findByCallsign(callsign);
        if (list.isEmpty())
        {
            // new aircraft
            CAircraft aircraft;
            aircraft.setCallsign(callsign);
            aircraft.setSituation(situation);
            aircraft.setTransponder(transponder);
            aircraft.setCalculcatedDistanceToPosition(this->m_ownAircraft.getPosition()); // distance from myself
            this->m_vatsimDataFileReader->updateWithVatsimDataFileData(aircraft);
            this->m_aircraftsInRange.push_back(aircraft);

            // and new client, there is a chance it has been created by
            // custom packet first
            if (!this->m_otherClients.contains(&CClient::getCallsign, callsign))
            {
                CClient c(callsign);
                this->m_otherClients.push_back(c); // initial, will be filled by data later
            }

            if (this->m_network->isConnected())
            {
                // only if still connected
                this->m_network->sendFrequencyQuery(callsign);
                this->m_network->sendRealNameQuery(callsign);
                this->m_network->sendIcaoCodesQuery(callsign);
                this->m_network->sendCapabilitiesQuery(callsign);
                this->m_network->sendServerQuery(callsign);
                this->sendFsipirCustomPacket(callsign); // own aircraft model
            }
        }
        else
        {
            // update
            CLength distance = this->m_ownAircraft.calculcateDistanceToPosition(situation.getPosition());
            distance.switchUnit(CLengthUnit::NM());
            CIndexVariantMap vm;
            vm.addValue(CAircraft::IndexTransponder, transponder);
            vm.addValue(CAircraft::IndexSituation, situation);
            vm.addValue(CAircraft::IndexDistance, distance);
            this->m_aircraftsInRange.applyIf(BlackMisc::Predicates::MemberEqual(&CAircraft::getCallsign, callsign), vm);
        }

        emit this->changedAircraftsInRange();
        emit changedAircraftSituation(callsign, situation);
    }

    void CAirspaceMonitor::pilotDisconnected(const CCallsign &callsign)
    {
        this->m_aircraftsInRange.removeIf(&CAircraft::getCallsign, callsign);
        this->m_otherClients.removeIf(&CClient::getCallsign, callsign);
    }

    void CAirspaceMonitor::frequencyReceived(const CCallsign &callsign, const CFrequency &frequency)
    {
        // update
        CIndexVariantMap vm(CAircraft::IndexFrequencyCom1, frequency.toQVariant());
        this->m_aircraftsInRange.applyIf(BlackMisc::Predicates::MemberEqual(&CAircraft::getCallsign, callsign), vm);
        emit this->changedAircraftsInRange();
    }

} // namespace
