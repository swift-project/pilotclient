/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "context_network_impl.h"
#include "vatsimbookingreader.h"
#include "vatsimdatafilereader.h"
#include "context_runtime.h"

#include "blackmisc/avatcstationlist.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/predicates.h"
#include "blackcore/context_application.h"

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
    void CContextNetwork::psReceivedBookings(const CAtcStationList &)
    {
        // TODO (MS) no test for if (this->getIContextApplication()) here?
        this->getIContextApplication()->sendStatusMessage(CStatusMessage::getInfoMessage("Read bookings from network", CStatusMessage::TypeTrafficNetwork));
    }

    /*
     * Update data
     */
    void CContextNetwork::requestDataUpdates()
    {
        Q_ASSERT(this->m_network);
        if (!this->isConnected()) return;

        this->requestAtisUpdates();
        this->m_airspace->requestDataUpdates();
    }

    /*
     * Request new ATIS data
     */
    void CContextNetwork::requestAtisUpdates()
    {
        Q_ASSERT(this->m_network);
        if (!this->isConnected()) return;

        this->m_airspace->requestAtisUpdates();
    }

    /*
     * Request METAR
     */
    BlackMisc::Aviation::CInformationMessage CContextNetwork::getMetar(const BlackMisc::Aviation::CAirportIcao &airportIcaoCode)
    {
        this->getRuntime()->logSlot(c_logContext, Q_FUNC_INFO, airportIcaoCode.toQString());
        return m_airspace->getMetar(airportIcaoCode);
    }

    /*
     * Selected voice rooms
     */
    CAtcStationList CContextNetwork::getSelectedAtcStations() const
    {
        CAtcStation com1Station = this->m_airspace->getAtcStationForComUnit(this->ownAircraft().getCom1System());
        CAtcStation com2Station = this->m_airspace->getAtcStationForComUnit(this->ownAircraft().getCom2System());

        CAtcStationList selectedStations;
        selectedStations.push_back(com1Station);
        selectedStations.push_back(com2Station);
        return selectedStations;
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

        CAtcStation s1 = stations[0];
        CAtcStation s2 = stations[1];

        // KB_REMOVE
        qDebug() << this->ownAircraft().getCom1System().getFrequencyActive() << s1.getCallsign() << s1.getFrequency() << s1.getVoiceRoom().getVoiceRoomUrl();
        qDebug() << this->ownAircraft().getCom2System().getFrequencyActive() << s2.getCallsign() << s2.getFrequency() << s2.getVoiceRoom().getVoiceRoomUrl();

        return rooms;
    }
} // namespace
