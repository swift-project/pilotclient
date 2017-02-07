/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#include "testutils.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include <QString>
#include <QStringBuilder>
#include <QVariant>
#include <typeinfo>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Network;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::FsCommon;

namespace BlackCore
{
    namespace Test
    {
        QString CTestUtils::getQDBusArgumentSignature(const QDBusArgument &arg, int level)
        {
            arg.beginArray();
            QVariant qv;
            const QString indent(level * 2, ' ');
            QString out;

            while (!arg.atEnd())
            {
                const QString type = CTestUtils::dbusTypeAsString(arg.currentType());
                const QString signature = arg.currentSignature();
                qv = arg.asVariant(); // this advances in the stream
                if (qv.canConvert<QDBusArgument>())
                {
                    out += indent % type % QLatin1Literal("signature ") % signature % QLatin1Char('\n');
                    out += CTestUtils::getQDBusArgumentSignature(qv.value<QDBusArgument>(), level + 1) % QLatin1Char('\n');
                }
                else
                {
                    out += indent % QLatin1Literal("type: ") % type % QLatin1Literal("signature ") % signature % QLatin1Literal(" value ") % qv.toString() % QLatin1Char('\n');
                }
            }
            arg.endArray();
            return out;
        }

        QString CTestUtils::dbusTypeAsString(QDBusArgument::ElementType type)
        {
            switch (type)
            {
            case QDBusArgument::BasicType: return QLatin1Literal("BasicType");
            case QDBusArgument::VariantType: return QLatin1Literal("VariantType");
            case QDBusArgument::ArrayType: return QLatin1Literal("ArrayType");
            case QDBusArgument::StructureType: return QLatin1Literal("StructureType");
            case QDBusArgument::MapType: return QLatin1Literal("MapType");
            case QDBusArgument::MapEntryType: return QLatin1Literal("MapEntryType");
            case QDBusArgument::UnknownType:
            default:
                return QLatin1Literal("Unknown type");
            }
        }

        void CTestUtils::showDBusSignatures(QTextStream &out)
        {
            const CCallsign cs;
            const CLength l;
            const CAircraftIcaoCode icao;
            const CAircraftModel model;
            const CCountry country;
            const CAirportIcaoCode airportIcao;
            const CLivery livery;
            const CAirport airport;
            const CSimulatedAircraft aircraft;
            const CSimulatedAircraftList aircraftList;
            const CVariant var;
            QString s;
            s = CTestUtils::dBusSignature(cs);
            out << "CCallsign" << " size: " << s.size() << " sig: " << s << endl;
            s = CTestUtils::dBusSignature(l);
            out << "CLength" << " size: " << s.size() << " sig: " << s << endl;
            s = CTestUtils::dBusSignature(icao);
            out << "CAircraftIcaoCode" << " size: " << s.size() << " sig: " << s << endl;
            s = CTestUtils::dBusSignature(airportIcao);
            out << "CAirportIcaoCode" << " size: " << s.size() << " sig: " << s << endl;
            s = CTestUtils::dBusSignature(livery);
            out << "CLivery" << " size: " << s.size() << " sig: " << s << endl;
            s = CTestUtils::dBusSignature(country);
            out << "CCountry" << " size: " << s.size() << " sig: " << s << endl;
            s = CTestUtils::dBusSignature(airport);
            out << "CAirport" << " size: " << s.size() << " sig: " << s << endl;
            s = CTestUtils::dBusSignature(model);
            out << "CAircraftModel" << " size: " << s.size() << " sig: " << s << endl;
            s = CTestUtils::dBusSignature(aircraft);
            out << "CSimulatedAircraft" << " size: " << s.size() << " sig: " << s << endl;
            s = CTestUtils::dBusSignature(aircraftList);
            out << "CSimulatedAircraftList" << " size: " << s.size() << " sig: " << s << endl;
            s = CTestUtils::dBusSignature(var);
            out << "CVariant" << " size: " << s.size() << " sig: " << s << endl;
        }

        CCallsign CTestUtils::getRandomAtcCallsign()
        {
            static QList<CCallsign> callsigns;
            if (callsigns.isEmpty())
            {
                callsigns << CCallsign("EDDM_TWR");
                callsigns << CCallsign("EDDM_APP");
                callsigns << CCallsign("EDDM_GND");
                callsigns << CCallsign("EDDF_TWR");
                callsigns << CCallsign("EDDF_APP");
                callsigns << CCallsign("EDDF_GND");
            }
            int i = (rand() % (callsigns.size()));
            CCallsign cs = callsigns.at(i);
            return cs;
        }

        CAtcStation CTestUtils::getAtcStation()
        {
            const CCoordinateGeodetic geoPos = CCoordinateGeodetic::fromWgs84("48° 21′ 13″ N", "11° 47′ 09″ E", { 1487, CLengthUnit::ft() }); // Munich
            const CAtcStation station(CCallsign("eddm_twr"), CUser("654321", "client"),
                                      CFrequency(118.7, CFrequencyUnit::MHz()),
                                      geoPos, CLength(50, CLengthUnit::km()));
            return station;
        }

        CSpeed CTestUtils::getSpeed()
        {
            return CSpeed(666, CSpeedUnit::km_h());
        }

        CAtcStationList CTestUtils::getStations(int number)
        {
            BlackMisc::Aviation::CAtcStationList list;
            for (int i = 0; i < number; i++)
            {
                BlackMisc::Aviation::CAtcStation s;
                s.setCallsign(QString::number(i));
                s.setFrequency(BlackMisc::PhysicalQuantities::CFrequency(i, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz()));
                s.setController(CUser(QString::number(i), "FooBar", "", ""));
                s.setOnline(i % 2 == 0 ? true : false);
                s.setPosition(CCoordinateGeodetic(i, i, i));
                list.push_back(s);
            }
            return list;
        }

        CAircraftCfgEntriesList CTestUtils::getAircraftCfgEntries(int number)
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

        CAirportList CTestUtils::getAirports(int number)
        {
            BlackMisc::Aviation::CAirportList list;
            for (int i = 0; i < number; i++)
            {
                const char cc = 65 + (i % 26);
                QString icao = QString("EXX%1").arg(QLatin1Char(cc));
                BlackMisc::Aviation::CAirport a(icao);
                a.setPosition(CCoordinateGeodetic(i, i, i));
                list.push_back(a);
            }
            return list;
        }

        CClientList CTestUtils::getClients(int number)
        {
            BlackMisc::Network::CClientList list;
            for (int i = 0; i < number; i++)
            {
                CCallsign cs(QString("DXX%1").arg(i));
                QString rn = QString("Joe Doe%1").arg(i);
                CUser user(QString::number(i), rn, cs);
                user.setCallsign(cs);
                CClient client(user);
                client.setCapability(true, CClient::FsdWithInterimPositions);
                client.setCapability(true, CClient::FsdWithIcaoCodes);
                const QString myFooModel = QString("fooModel %1").arg(i);
                client.setQueriedModelString(myFooModel);
                list.push_back(client);
            }
            return list;
        }
    } // ns
} // ns
