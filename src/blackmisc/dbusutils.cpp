/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/dbusutils.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/aviation/airport.h"
#include <QString>
#include <QStringBuilder>
#include <QVariant>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Network;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;

namespace BlackMisc
{
    QString CDBusUtils::getQDBusArgumentSignature(const QDBusArgument &arg, int level)
    {
        arg.beginArray();
        QVariant qv;
        const QString indent(level * 2, ' ');
        QString out;

        while (!arg.atEnd())
        {
            const QString type = CDBusUtils::dbusTypeAsString(arg.currentType());
            const QString signature = arg.currentSignature();
            qv = arg.asVariant(); // this advances in the stream
            if (qv.canConvert<QDBusArgument>())
            {
                out += indent % type % u"signature " % signature % u'\n';
                out += CDBusUtils::getQDBusArgumentSignature(qv.value<QDBusArgument>(), level + 1) % u'\n';
            }
            else
            {
                out += indent % u"type: " % type % u"signature " % signature % u" value " % qv.toString() % u'\n';
            }
        }
        arg.endArray();
        return out;
    }

    QString CDBusUtils::dbusTypeAsString(QDBusArgument::ElementType type)
    {
        switch (type)
        {
        case QDBusArgument::BasicType: return QLatin1String("BasicType");
        case QDBusArgument::VariantType: return QLatin1String("VariantType");
        case QDBusArgument::ArrayType: return QLatin1String("ArrayType");
        case QDBusArgument::StructureType: return QLatin1String("StructureType");
        case QDBusArgument::MapType: return QLatin1String("MapType");
        case QDBusArgument::MapEntryType: return QLatin1String("MapEntryType");
        case QDBusArgument::UnknownType:
        default:
            return QLatin1String("Unknown type");
        }
    }

    void CDBusUtils::showDBusSignatures(QTextStream &out)
    {
        const CCallsign cs;
        const CLength l;
        const CAircraftIcaoCode icao;
        const CAircraftModel model;
        const CCountry country;
        const CAirportIcaoCode airportIcao;
        const CLivery livery;
        const CAirport airport;
        const CAircraftSituation situation;
        const CSimulatedAircraft aircraft;
        const CSimulatedAircraftList aircraftList;
        const CVariant var;
        QString s;
        s = CDBusUtils::dBusSignature(cs);
        out << "CCallsign" << " size: " << s.size() << " sig: " << s << Qt::endl;
        s = CDBusUtils::dBusSignature(l);
        out << "CLength" << " size: " << s.size() << " sig: " << s << Qt::endl;
        s = CDBusUtils::dBusSignature(icao);
        out << "CAircraftIcaoCode" << " size: " << s.size() << " sig: " << s << Qt::endl;
        s = CDBusUtils::dBusSignature(airportIcao);
        out << "CAirportIcaoCode" << " size: " << s.size() << " sig: " << s << Qt::endl;
        s = CDBusUtils::dBusSignature(livery);
        out << "CLivery" << " size: " << s.size() << " sig: " << s << Qt::endl;
        s = CDBusUtils::dBusSignature(situation);
        out << "CAircraftSituation" << " size: " << s.size() << " sig: " << s << Qt::endl;
        s = CDBusUtils::dBusSignature(country);
        out << "CCountry" << " size: " << s.size() << " sig: " << s << Qt::endl;
        s = CDBusUtils::dBusSignature(airport);
        out << "CAirport" << " size: " << s.size() << " sig: " << s << Qt::endl;
        s = CDBusUtils::dBusSignature(model);
        out << "CAircraftModel" << " size: " << s.size() << " sig: " << s << Qt::endl;
        s = CDBusUtils::dBusSignature(aircraft);
        out << "CSimulatedAircraft" << " size: " << s.size() << " sig: " << s << Qt::endl;
        s = CDBusUtils::dBusSignature(aircraftList);
        out << "CSimulatedAircraftList" << " size: " << s.size() << " sig: " << s << Qt::endl;
        s = CDBusUtils::dBusSignature(var);
        out << "CVariant" << " size: " << s.size() << " sig: " << s << Qt::endl;
    }
} // ns
