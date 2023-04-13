/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/test/testing.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/iterator.h"
#include "blackmisc/network/user.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/physicalquantity.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/propertyindexvariantmap.h"
#include "blackmisc/sequence.h"
#include "blackmisc/variant.h"

#include <QDateTime>
#include <QList>
#include <QStringList>
#include <QtGlobal>
#include <tuple>

using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Network;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation::FsCommon;

namespace BlackMisc::Test
{
    CAtcStationList CTesting::createAtcStations(int number, bool byPropertyIndex)
    {
        CAtcStationList atcs;
        for (int i = 0; i < number; i++)
        {
            atcs.push_back(createStation(i, byPropertyIndex));
        }
        return atcs;
    }

    CAtcStation CTesting::createStation(int index, bool byPropertyIndex)
    {
        // from WGS is slow, so static const (only 1 time init)
        // https://dev.vatsim-germany.org/issues/322#note-2
        static const CCoordinateGeodetic geoPos = CCoordinateGeodetic::fromWgs84("48° 21′ 13″ N", "11° 47′ 09″ E", CAltitude(index, CLengthUnit::ft()));
        const QString cs = QStringLiteral("%1MI-SNO_TWR").arg(index);
        const QString usr = QStringLiteral("Joe %1").arg(index);
        const QString id = QStringLiteral("00000%1").arg(index).right(6);
        const double f = 118.0 + (index % 30) * 0.25;

        const QDateTime dtFrom = QDateTime::currentDateTimeUtc();
        const QDateTime dtUntil = dtFrom.addSecs(60 * 60); // 1 hour
        const CUser user(id, usr);
        const CInformationMessage atis(CInformationMessage::ATIS, "ATIS for " + QString::number(index));
        const CInformationMessage metar(CInformationMessage::METAR, "Metar for " + QString::number(index));

        CAtcStation station;
        if (byPropertyIndex)
        {
            station.setPropertyByIndex(CAtcStation::IndexCallsign, CVariant::from(CCallsign(cs)));
            station.setPropertyByIndex(CAtcStation::IndexController, CVariant::from(user));
            station.setPropertyByIndex(CAtcStation::IndexFrequency, CVariant::from(CFrequency(f, CFrequencyUnit::MHz())));
            station.setPropertyByIndex(CAtcStation::IndexRange, CVariant::from(CLength(50, CLengthUnit::km())));
            station.setPropertyByIndex(CAtcStation::IndexPosition, CVariant::from(geoPos));
            station.setPropertyByIndex(CAtcStation::IndexIsOnline, CVariant::from(false));
            station.setPropertyByIndex(CAtcStation::IndexBookedFrom, CVariant::from(dtFrom));
            station.setPropertyByIndex(CAtcStation::IndexBookedUntil, CVariant::from(dtUntil));
            station.setPropertyByIndex(CAtcStation::IndexRelativeDistance, CVariant::from(CLength(index + 1, CLengthUnit::NM())));
            station.setPropertyByIndex(CAtcStation::IndexAtis, CVariant::from(atis));
            station.setPropertyByIndex(CAtcStation::IndexMetar, CVariant::from(metar));
        }
        else
        {
            station = CAtcStation(CCallsign(cs, CCallsign::Atc), user,
                                  CFrequency(f, CFrequencyUnit::MHz()),
                                  geoPos, CLength(50, CLengthUnit::km()), false, dtFrom, dtUntil);
            station.setRelativeDistance(CLength(index + 1, CLengthUnit::NM()));
            station.setAtis(atis);
            station.setMetar(metar);
        }
        return station;
    }

    CAircraftCfgEntriesList CTesting::getAircraftCfgEntries(int number)
    {
        CAircraftCfgEntriesList list;
        for (int i = 0; i < number; i++)
        {
            CAircraftCfgEntries e;
            e.setAtcModel("atc model");
            e.setAtcParkingCode(QString::number(i));
            e.setIndex(i);
            e.setFileName("this will be the file path and pretty long");
            e.setTitle("I am the aircraft title foobar");
            e.setAtcType("B737");
            list.push_back(e);
        }
        return list;
    }

    CAirportList CTesting::getAirports(int number)
    {
        BlackMisc::Aviation::CAirportList list;
        for (int i = 0; i < number; i++)
        {
            const char cc = 65 + (i % 26);
            const QString icao = QStringLiteral("EXX%1").arg(QLatin1Char(cc));
            CAirport a(icao);
            a.setPosition(CCoordinateGeodetic(i, i, i));
            list.push_back(a);
        }
        return list;
    }

    CClientList CTesting::getClients(int number)
    {
        CClientList list;
        for (int i = 0; i < number; i++)
        {
            const CCallsign cs(QStringLiteral("DXX%1").arg(i));
            const QString rn = QStringLiteral("Joe Doe%1").arg(i);
            CUser user(QString::number(i), rn, cs);
            user.setCallsign(cs);
            CClient client(user);
            client.addCapability(CClient::FsdWithInterimPositions);
            client.addCapability(CClient::FsdWithIcaoCodes);
            const QString myFooModel = QStringLiteral("fooModel %1").arg(i);
            client.setQueriedModelString(myFooModel);
            list.push_back(client);
        }
        return list;
    }
} // namespace
