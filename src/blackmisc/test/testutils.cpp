/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "testutils.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/aviation/airport.h"
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

namespace BlackMisc
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
    } // ns
} // ns
