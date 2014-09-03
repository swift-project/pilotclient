/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "testing.h"

using namespace BlackMisc;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Network;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc
{
    namespace Aviation
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
            static const CCoordinateGeodetic geoPos = CCoordinateGeodetic::fromWgs84("48° 21′ 13″ N", "11° 47′ 09″ E", CLength(index, CLengthUnit::ft()));

            QString cs = QString("%1_TWR").arg(index);
            QString usr = QString("Joe %1").arg(index);
            QString id = QString("00000%1").arg(index).right(6);
            double f = 118.0 + (index % 30) * 0.25;

            QDateTime dtFrom = QDateTime::currentDateTimeUtc();
            QDateTime dtUntil = dtFrom.addSecs(60 * 60.0); // 1 hour

            if (byPropertyIndex)
            {
                CAtcStation station;
                station.setPropertyByIndex(CCallsign(cs).toQVariant(), CAtcStation::IndexCallsign);
                station.setPropertyByIndex(CUser(id, usr).toQVariant(), CAtcStation::IndexController);
                station.setPropertyByIndex(CFrequency(f, CFrequencyUnit::MHz()).toQVariant(), CAtcStation::IndexFrequency);
                station.setPropertyByIndex(CLength(50, CLengthUnit::km()).toQVariant(), CAtcStation::IndexRange);
                station.setPropertyByIndex(geoPos.toQVariant(), CAtcStation::IndexPosition);
                station.setPropertyByIndex(false, CAtcStation::IndexIsOnline);
                station.setPropertyByIndex(dtFrom, CAtcStation::IndexBookedFrom);
                station.setPropertyByIndex(dtUntil, CAtcStation::IndexBookedUntil);
                station.setPropertyByIndex(CLength(index + 1, CLengthUnit::NM()).toQVariant(), CAtcStation::IndexDistance);
                return station;
            }
            else
            {
                CAtcStation station(CCallsign(cs), CUser(id, usr),
                                    CFrequency(f, CFrequencyUnit::MHz()),
                                    geoPos, CLength(50, CLengthUnit::km()), false, dtFrom, dtUntil);
                station.setDistanceToPlane(CLength(index + 1, CLengthUnit::NM()));
                return station;
            }
        }

        void CTesting::readStations(const CAtcStationList &stations, bool byPropertyIndex)
        {
            foreach(const CAtcStation station, stations)
            {
                readStation(station, byPropertyIndex);
            }
        }

        QString CTesting::readStation(const CAtcStation &station, bool byPropertyIndex)
        {
            QString r;
            if (byPropertyIndex)
            {
                r.append(station.propertyByIndex({ CAtcStation::IndexCallsign, CCallsign::IndexString}).toString());
                r.append(station.propertyByIndex({ CAtcStation::IndexController, CUser::IndexRealName}).toString());
                r.append(station.propertyByIndex({ CAtcStation::IndexPosition, CCoordinateGeodetic::IndexLatitudeAsString}).toString());
                r.append(station.propertyByIndex({ CAtcStation::IndexPosition, CCoordinateGeodetic::IndexLongitudeAsString}).toString());
                r.append(station.propertyByIndex({ CAtcStation::IndexDistance, CLength::IndexValueRounded2DigitsWithUnit}).toString());
                r.append(station.propertyByIndex({ CAtcStation::IndexBookedFrom}).toDateTime().toString("YYYY-mm-dd hh:mm"));
                r.append(station.propertyByIndex({ CAtcStation::IndexBookedUntil}).toDateTime().toString("YYYY-mm-dd hh:mm"));
            }
            else
            {
                r.append(station.getCallsignAsString());
                r.append(station.getController().getRealName());
                r.append(station.getPosition().latitudeAsString());
                r.append(station.getPosition().longitudeAsString());
                r.append(station.getDistanceToPlane().toQString(true));
                r.append(station.getBookedFromUtc().toString("YYYY-mm-dd hh:mm"));
                r.append(station.getBookedUntilUtc().toString("YYYY-mm-dd hh:mm"));
            }
            return r;
        }

        void CTesting::calculateDistance(int n)
        {
            if (n < 1) return;
            CAtcStation atc = createStation(1);
            QList<CCoordinateGeodetic> pos(
            {
                CCoordinateGeodetic(10.0, 10.0, 10.0),
                CCoordinateGeodetic(20.0, 20.0, 20.0),
                CCoordinateGeodetic(30.0, 30.0, 30.0),
                CCoordinateGeodetic(40.0, 40.0, 40.0),
                CCoordinateGeodetic(50.0, 50.0, 50.0),
                CCoordinateGeodetic(60.0, 60.0, 60.0),
                CCoordinateGeodetic(70.0, 70.0, 70.0)
            }
            );
            const int s = pos.size();
            for (int i = 0; i < n; i++)
            {
                int p = i % s;
                atc.calculcateDistanceToPlane(pos.at(p));
            }
        }

        void CTesting::copy10kStations(int times)
        {
            CAtcStationList stations;
            for (int i = 0; i < times; i++)
            {
                stations = stations10k();
                stations.pop_back(); // make sure stations are really copied (copy-on-write)
            }
        }


        void CTesting::parseWgs(int times)
        {
            static QStringList wgsLatLng(
            {
                "12° 11′ 10″ N", "11° 22′ 33″ W",
                "48° 21′ 13″ N", "11° 47′ 09″ E",
                " 8° 21′ 13″ N", "11° 47′ 09″ W",
                "18° 21′ 13″ S", "11° 47′ 09″ E",
                "09° 12′ 13″ S", "11° 47′ 09″ W"
            }
            );

            CCoordinateGeodetic c;
            const CLength h(333, CLengthUnit::m());
            for (int i = 0; i < times; i++)
            {
                int idx = (i % 5) * 2;
                c = CCoordinateGeodetic::fromWgs84(wgsLatLng.at(idx), wgsLatLng.at(idx + 1), h);
            }
        }

    } // namespace
} // namespace
