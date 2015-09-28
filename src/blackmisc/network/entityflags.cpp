/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/network/entityflags.h"
#include "blackmisc/dbus.h"
#include <QtDBus/QDBusMetaType>
#include <QStringList>

namespace BlackMisc
{
    namespace Network
    {
        QString CEntityFlags::flagToString(CEntityFlags::EntityFlag flag)
        {
            switch (flag)
            {
            case NoEntity: return "no data";
            case BookingEntity: return "VATSIM bookings";
            case VatsimDataFile: return "VATSIM data file";
            case AircraftIcaoEntity: return "Aircraft ICAO";
            case AirlineIcaoEntity: return "Airline ICAO";
            case CountryEntity: return "Country";
            case DistributorEntity: return "Distributor";
            case LiveryEntity: return "Livery";
            case ModelEntity: return "Model";
            case AllIcaoEntities: return "All ICAO";
            case AllIcaoAndCountries: return "All ICAO + country";
            case AllEntities: return "All";
            default:
                Q_ASSERT_X(false, Q_FUNC_INFO, "wrong flags");
                return "wrong flags";
            }
        }

        QString CEntityFlags::flagToString(Entity flag)
        {
            QStringList list;
            if (flag.testFlag(NoEntity)) list << "no data";
            if (flag.testFlag(BookingEntity)) list << "VATSIM bookings";
            if (flag.testFlag(VatsimDataFile)) list << "VATSIM data file";
            if (flag.testFlag(AircraftIcaoEntity)) list << "Aircraft ICAO";
            if (flag.testFlag(AirlineIcaoEntity)) list << "Airline ICAO";
            if (flag.testFlag(CountryEntity)) list << "Country";
            if (flag.testFlag(DistributorEntity)) list << "Distributor";
            if (flag.testFlag(LiveryEntity)) list << "Livery";
            if (flag.testFlag(ModelEntity)) list << "Model";
            return list.join(',');
        }

        QString CEntityFlags::flagToString(CEntityFlags::ReadState flag)
        {
            switch (flag)
            {
            case ReadFinished: return "finished";
            case ReadFailed: return "failed";
            case StartRead: return "read started";
            default:
                Q_ASSERT_X(false, Q_FUNC_INFO, "wrong flags");
                return "wrong flags";
            }
        }

        void CEntityFlags::registerMetadata()
        {
            // this is no value class and I register enums here,
            // that's why I do not use the Mixins
            int id = qRegisterMetaType<CEntityFlags::Entity>();
            // int idb = qDBusRegisterMetaType<CEntityFlags::Entity>();
            // Q_ASSERT_X(id >= 1024 && id == idb, Q_FUNC_INFO, "wrong id for metatype");
            Q_ASSERT_X(id >= 1024, Q_FUNC_INFO, "wrong id for metatype");

            id = qRegisterMetaType<CEntityFlags::EntityFlag>();
            // idb = qDBusRegisterMetaType<CEntityFlags::EntityFlag>();
            Q_ASSERT_X(id >= 1024, Q_FUNC_INFO, "wrong id for metatype");

            id = qRegisterMetaType<CEntityFlags::ReadState>();
            // idb = qDBusRegisterMetaType<CEntityFlags::ReadState>();
            Q_ASSERT_X(id >= 1024, Q_FUNC_INFO, "wrong id for metatype");
            Q_UNUSED(id);
            // Q_UNUSED(idb);
        }
    } // namespace
} // namespace
