/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "context_network_impl.h"
#include "vatsimbookingreader.h"
#include "vatsimdatafilereader.h"
#include "context_runtime.h"

#include "blackmisc/avatcstationlist.h"
#include "blackmisc/predicates.h"

#include <QMetaEnum>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QXmlStreamReader>
#include <QtXml/QDomElement>

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Audio;

namespace BlackCore
{

    /*
     *  Reload bookings
     */
    void CContextNetwork::readAtcBookingsFromSource() const
    {
        Q_ASSERT(this->m_vatsimBookingReader);
        this->m_vatsimBookingReader->read();
    }

    /*
     * Update bookings
     */
    void CContextNetwork::psReceivedBookings(CAtcStationList bookedStations)
    {
        const int interval = 60 * 1000;
        if (this->m_vatsimBookingReader->interval() < interval) this->m_vatsimBookingReader->setInterval(interval);
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

    /*
     * Update data
     */
    void CContextNetwork::requestDataUpdates()
    {
        Q_ASSERT(this->m_network);
        if (!this->isConnected()) return;
        this->requestAtisUpdates();

        // other updates
        foreach(CAircraft aircraft, this->m_aircraftsInRange)
        {
            this->m_network->sendFrequencyQuery(aircraft.getCallsign());
        }
    }

    /*
     * Request new ATIS data
     */
    void CContextNetwork::requestAtisUpdates()
    {
        Q_ASSERT(this->m_network);
        if (!this->isConnected()) return;
        foreach(CAtcStation station, this->m_atcStationsOnline)
        {
            this->m_network->sendAtisQuery(station.getCallsign());
        }
    }

    /*
     * Booked stations
     */
    void CContextNetwork::setAtcStationsBooked(const BlackMisc::Aviation::CAtcStationList &newStations)
    {
        this->m_atcStationsBooked = newStations;
    }

    /*
     * Online stations
     */
    void CContextNetwork::setAtcStationsOnline(const BlackMisc::Aviation::CAtcStationList &newStations)
    {
        this->m_atcStationsOnline = newStations;
    }

    /*
     * Request METAR
     */
    BlackMisc::Aviation::CInformationMessage CContextNetwork::getMetar(const QString &airportIcaoCode)
    {
        CInformationMessage metar;
        QString icao = airportIcaoCode.trimmed().toUpper();
        if (icao.length() != 4) return metar;
        if (this->m_metarCache.contains(icao)) metar = this->m_metarCache[icao];
        if (metar.isEmpty() || metar.timeDiffReceivedMs() > 10 * 1000)
        {
            // outdated, or not in cache at all
            this->m_network->sendMetarQuery(airportIcaoCode.trimmed().toUpper());

            // with this little trick we try to make an asynchronous signal / slot
            // based approach a synchronous return value
            QTime waitForMetar = QTime::currentTime().addMSecs(1000);
            while (QTime::currentTime() < waitForMetar)
            {
                // process some other events and hope network answer is received already
                QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
                if (m_metarCache.contains(icao))
                {
                    metar = this->m_metarCache[icao];
                    break;
                }
            }
        }
        return metar;
    }

    /*
     * Selected voice rooms
     */
    CAtcStationList CContextNetwork::getSelectedAtcStations() const
    {
        CAtcStationList stationsCom1 = this->m_atcStationsOnline.findIfComUnitTunedIn25KHz(this->ownAircraft().getCom1System());
        CAtcStationList stationsCom2 = this->m_atcStationsOnline.findIfComUnitTunedIn25KHz(this->ownAircraft().getCom2System());
        stationsCom1.sortBy(&CAtcStation::getDistanceToPlane);
        stationsCom2.sortBy(&CAtcStation::getDistanceToPlane);

        CAtcStation s;
        CAtcStationList stations;
        CAtcStation com1 = stationsCom1.isEmpty() ? s : stationsCom1[0];
        CAtcStation com2 = stationsCom2.isEmpty() ? s : stationsCom2[0];

        stations.push_back(com1);
        stations.push_back(com2);

        return stations;
    }

    /*
     * Selected voice rooms
     */
    CVoiceRoomList CContextNetwork::getSelectedVoiceRooms() const
    {
        CAtcStationList stations = this->getSelectedAtcStations();
        Q_ASSERT(stations.size() == 2);
        CVoiceRoomList rooms;
        rooms.push_back(stations[0].getVoiceRoom());
        rooms.push_back(stations[1].getVoiceRoom());
        return rooms;
    }

    /*
     * ATC Position update
     */
    void CContextNetwork::psFsdAtcPositionUpdate(const CCallsign &callsign, const BlackMisc::PhysicalQuantities::CFrequency &frequency, const CCoordinateGeodetic &position, const BlackMisc::PhysicalQuantities::CLength &range)
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
            station.calculcateDistanceToPlane(this->ownAircraft().getPosition());
            this->m_vatsimDataFileReader->getAtcStations().updateFromVatsimDataFileStation(station); // prefill
            this->m_atcStationsOnline.push_back(station);

            if (this->isConnected())
            {
                emit this->m_network->sendRealNameQuery(callsign);
                emit this->m_network->sendAtisQuery(callsign); // request ATIS and voice rooms
                emit this->m_network->sendServerQuery(callsign);
            }

            emit this->changedAtcStationsOnline();
            // Remark: this->changedAtcStationOnlineConnectionStatus(station, true);
            // will be sent in psFsdAtisVoiceRoomQueryReceived
        }
        else
        {
            // update
            CIndexVariantMap values;
            values.addValue(CAtcStation::IndexFrequency, frequency);
            values.addValue(CAtcStation::IndexPosition, position);
            values.addValue(CAtcStation::IndexRange, range);
            this->m_atcStationsOnline.applyIf(BlackMisc::Predicates::MemberEqual<CAtcStation>(&CAtcStation::getCallsign, callsign), values);
            emit this->changedAtcStationsOnline();
        }
    }

    /*
     * ATC Controller disconnected
     */
    void CContextNetwork::psFsdAtcControllerDisconnected(const CCallsign &callsign)
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

    /*
     * ATIS received
     */
    void CContextNetwork::psFsdAtisQueryReceived(const CCallsign &callsign, const CInformationMessage &atisMessage)
    {
        if (callsign.isEmpty()) return;
        CIndexVariantMap vm(CAtcStation::IndexAtis, atisMessage.toQVariant());
        this->m_atcStationsOnline.applyIf(&CAtcStation::getCallsign, callsign, vm);
        this->m_atcStationsBooked.applyIf(&CAtcStation::getCallsign, callsign, vm);
        if (this->m_atcStationsOnline.contains(&CAtcStation::getCallsign, callsign)) emit this->changedAtcStationsBooked();
        if (this->m_atcStationsBooked.contains(&CAtcStation::getCallsign, callsign)) emit this->changedAtcStationsBooked();
    }

    /*
     * ATIS (voice room part) received
     */
    void CContextNetwork::psFsdAtisVoiceRoomQueryReceived(const CCallsign &callsign, const QString &url)
    {
        QString trimmedUrl = url.trimmed();
        CIndexVariantMap vm(CAtcStation::IndexVoiceRoomUrl, trimmedUrl);
        this->m_atcStationsOnline.applyIf(&CAtcStation::getCallsign, callsign, vm);
        this->m_atcStationsBooked.applyIf(&CAtcStation::getCallsign, callsign, vm);
        if (this->m_atcStationsOnline.contains(&CAtcStation::getCallsign, callsign))
        {
            CAtcStation station = this->m_atcStationsOnline.findFirstByCallsign(callsign);
            emit this->changedAtcStationsBooked();
            emit this->changedAtcStationOnlineConnectionStatus(station, true);
        }
        if (this->m_atcStationsBooked.contains(&CAtcStation::getCallsign, callsign)) emit this->changedAtcStationsBooked();
    }

    /*
     * ATIS (logoff time part) received
     */
    void CContextNetwork::psFsdAtisLogoffTimeQueryReceived(const CCallsign &callsign, const QString &zuluTime)
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

    /*
     * Metar received
     */
    void CContextNetwork::psFsdMetarReceived(const QString &metarMessage)
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
        if (this->m_atcStationsOnline.contains(&CAtcStation::getCallsign, callsignTower)) emit this->changedAtcStationsBooked();
        if (this->m_atcStationsBooked.contains(&CAtcStation::getCallsign, callsignTower)) emit this->changedAtcStationsBooked();
    }

} // namespace
