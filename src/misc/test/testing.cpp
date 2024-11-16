// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/test/testing.h"

#include <tuple>

#include <QDateTime>
#include <QList>
#include <QStringList>
#include <QtGlobal>

#include "misc/aviation/callsign.h"
#include "misc/geo/coordinategeodetic.h"
#include "misc/iterator.h"
#include "misc/network/user.h"
#include "misc/pq/frequency.h"
#include "misc/pq/length.h"
#include "misc/pq/physicalquantity.h"
#include "misc/pq/units.h"
#include "misc/propertyindexvariantmap.h"
#include "misc/sequence.h"
#include "misc/variant.h"

using namespace swift::misc::aviation;
using namespace swift::misc::geo;
using namespace swift::misc::network;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::simulation::fscommon;

namespace swift::misc::test
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
            station.setPropertyByIndex(CAtcStation::IndexRelativeDistance, CVariant::from(CLength(index + 1, CLengthUnit::NM())));
            station.setPropertyByIndex(CAtcStation::IndexAtis, CVariant::from(atis));
            station.setPropertyByIndex(CAtcStation::IndexMetar, CVariant::from(metar));
        }
        else
        {
            station = CAtcStation(CCallsign(cs, CCallsign::Atc), user,
                                  CFrequency(f, CFrequencyUnit::MHz()),
                                  geoPos, CLength(50, CLengthUnit::km()), false);
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
        swift::misc::aviation::CAirportList list;
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
} // namespace swift::misc::test
