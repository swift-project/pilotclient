/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "context_network.h"
#include "coreruntime.h"
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
using namespace BlackMisc::Voice;

namespace BlackCore
{

    /*
     * Read bookings
     */
    void CContextNetwork::readAtcBookingsFromSource() const
    {
        QUrl url("http://vatbook.euroutepro.com/xml2.php");
        QNetworkRequest request(url);
        this->m_networkManager->get(request);
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
            this->m_network->requestMetar(airportIcaoCode.trimmed().toUpper());

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
    CVoiceRoomList CContextNetwork::getSelectedVoiceRooms() const
    {
        CFrequency com1 = this->m_ownAircraft.getCom1System().getFrequencyActive();
        CFrequency com2 = this->m_ownAircraft.getCom2System().getFrequencyActive();

        CAtcStationList stationsCom1 = this->m_atcStationsOnline.findBy(&CAtcStation::getFrequency, com1);
        CAtcStationList stationsCom2 = this->m_atcStationsOnline.findBy(&CAtcStation::getFrequency, com2);
        stationsCom1.sortBy(&CAtcStation::getDistanceToPlane);
        stationsCom1.sortBy(&CAtcStation::getDistanceToPlane);

        CVoiceRoom vr;
        CVoiceRoomList rooms;
        rooms.push_back(stationsCom1.isEmpty() ? vr : stationsCom1[0].getVoiceRoom());
        rooms.push_back(stationsCom2.isEmpty() ? vr : stationsCom2[0].getVoiceRoom());
        return rooms;
    }

    /*
     * ATC Position update
     */
    void CContextNetwork::psFsdAtcPositionUpdate(const CCallsign &callsign, const BlackMisc::PhysicalQuantities::CFrequency &frequency, const CCoordinateGeodetic &position, const BlackMisc::PhysicalQuantities::CLength &range)
    {
        // this->log(Q_FUNC_INFO, callsign.toQString(), frequency.toQString(), position.toQString(), range.toQString());
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
            this->m_atcStationsOnline.push_back(station);
            emit this->changedAtcStationsOnline();
            emit this->m_network->sendAtisQuery(callsign); // request ATIS
            emit this->m_network->sendNameQuery(callsign);
            emit this->m_network->sendServerQuery(callsign);
        }
        else
        {
            // update
            CValueMap values;
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
        // this->log(Q_FUNC_INFO, callsign.toQString());
        this->m_atcStationsOnline.removeIf(&CAtcStation::getCallsign, callsign);
        emit this->changedAtcStationsOnline();
        this->m_atcStationsBooked.applyIf(&CAtcStation::getCallsign, callsign, CValueMap(CAtcStation::IndexIsOnline, QVariant(false)));
    }

    /*
     * ATIS received
     */
    void CContextNetwork::psFsdAtisQueryReceived(const CCallsign &callsign, const CInformationMessage &atisMessage)
    {
        // this->log(Q_FUNC_INFO, callsign.toQString(), atisMessage);

        CValueMap vm(CAtcStation::IndexAtis, atisMessage.toQVariant());
        this->m_atcStationsOnline.applyIf(&CAtcStation::getCallsign, callsign, vm);
        this->m_atcStationsBooked.applyIf(&CAtcStation::getCallsign, callsign, vm);
    }

    /*
     * ATIS (voice room part) received
     */
    void CContextNetwork::psFsdAtisVoiceRoomQueryReceived(const CCallsign &callsign, const QString &url)
    {
        QString trimmedUrl = url.trimmed();
        CValueMap vm(CAtcStation::IndexVoiceRoomUrl, trimmedUrl);
        this->m_atcStationsOnline.applyIf(&CAtcStation::getCallsign, callsign, vm);
        this->m_atcStationsBooked.applyIf(&CAtcStation::getCallsign, callsign, vm);
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
            CValueMap vm(CAtcStation::IndexBookedUntil, logoffDateTime);
            this->m_atcStationsOnline.applyIf(&CAtcStation::getCallsign, callsign, vm);
            this->m_atcStationsBooked.applyIf(&CAtcStation::getCallsign, callsign, vm);
        }
    }

    /*
     * Metar received
     */
    void CContextNetwork::psFsdMetarReceived(const QString &metarMessage)
    {
        // this->log(Q_FUNC_INFO, metarMessage);
        if (metarMessage.length() < 10) return; // invalid
        const QString icaoCode = metarMessage.left(4).toUpper();
        const QString icaoCodeTower = icaoCode + "_TWR";
        CCallsign callsignTower(icaoCodeTower);
        CInformationMessage metar(CInformationMessage::METAR, metarMessage);

        // add METAR to existing stations
        CValueMap vm(CAtcStation::IndexMetar, metar.toQVariant());
        this->m_atcStationsOnline.applyIf(&CAtcStation::getCallsign, callsignTower, vm);
        this->m_atcStationsBooked.applyIf(&CAtcStation::getCallsign, callsignTower, vm);
        this->m_metarCache.insert(icaoCode, metar);
    }

    /*
     * Bookings read from XML
     * TODO: encapsulate reading from WWW in some class
     */
    void CContextNetwork::psAtcBookingsRead(QNetworkReply *nwReply)
    {
        if (nwReply->error() == QNetworkReply::NoError)
        {
            QString xmlData = nwReply->readAll();
            QDomDocument doc;

            if (doc.setContent(xmlData))
            {
                QDomNode atc = doc.elementsByTagName("atcs").at(0);
                QDomNodeList bookingNodes = atc.toElement().elementsByTagName("booking");
                int size = bookingNodes.size();
                CSequence<CAtcStation> stations;
                for (int i = 0; i < size; i++)
                {
                    QDomNode bookingNode = bookingNodes.at(i);
                    QDomNodeList bookingNodeValues = bookingNode.childNodes();
                    CAtcStation bookedStation;
                    CUser user;
                    for (int v = 0; v < bookingNodeValues.size(); v++)
                    {
                        QDomNode bookingNodeValue = bookingNodeValues.at(v);
                        QString name = bookingNodeValue.nodeName().toLower();
                        QString value = bookingNodeValue.toElement().text();
                        if (name == "id")
                        {
                            // could be used as unique key
                        }
                        else if (name == "callsign")
                        {
                            bookedStation.setCallsign(CCallsign(value));
                        }
                        else if (name == "name")
                        {
                            user.setRealName(value);
                        }
                        else if (name == "cid")
                        {
                            user.setId(value);
                        }
                        else if (name == "time_end")
                        {
                            QDateTime t = QDateTime::fromString(value, "yyyy-MM-dd HH:mm:ss");
                            bookedStation.setBookedUntilUtc(t);
                        }
                        else if (name == "time_start")
                        {
                            QDateTime t = QDateTime::fromString(value, "yyyy-MM-dd HH:mm:ss");
                            bookedStation.setBookedFromUtc(t);
                        }
                    }
                    // time checks
                    QDateTime now = QDateTime::currentDateTimeUtc();
                    if (now.msecsTo(bookedStation.getBookedUntilUtc()) < (1000 * 60 * 15)) continue; // until n mins in past
                    if (now.msecsTo(bookedStation.getBookedFromUtc()) > (1000 * 60 * 60 * 24)) continue; // to far in the future, n hours

                    // booking does not have position, so distance cannot be calculated
                    // bookedStation.calculcateDistanceToPlane(this->m_ownAircraft.getPosition());

                    bookedStation.setController(user);

                    // consolidate and append
                    this->m_atcStationsOnline.mergeWithBooking(bookedStation);
                    stations.push_back(bookedStation);
                }
                nwReply->close();
                nwReply->deleteLater();

                // set the new values
                if (this->getAtcStationsBooked() != stations)
                {
                    this->atcStationsBooked() = stations;
                    emit this->changedAtcStationsBooked();
                }
            } // node
        } // content
    } // method
} // namespace
