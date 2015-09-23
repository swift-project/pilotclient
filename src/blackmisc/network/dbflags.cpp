/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/network/dbflags.h"
#include <QStringList>

namespace BlackMisc
{
    namespace Network
    {
        QString CDbFlags::flagToString(CDbFlags::EntityFlags flag)
        {
            switch (flag)
            {
            case NoEntity: return "no data";
            case VatsimBookings: return "VATSIM bookings";
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

        QString CDbFlags::flagToString(Entity flag)
        {
            QStringList list;
            if (flag.testFlag(NoEntity)) list << "no data";
            if (flag.testFlag(VatsimBookings)) list << "VATSIM bookings";
            if (flag.testFlag(VatsimDataFile)) list << "VATSIM data file";
            if (flag.testFlag(AircraftIcaoEntity)) list << "Aircraft ICAO";
            if (flag.testFlag(AirlineIcaoEntity)) list << "Airline ICAO";
            if (flag.testFlag(CountryEntity)) list << "Country";
            if (flag.testFlag(DistributorEntity)) list << "Distributor";
            if (flag.testFlag(LiveryEntity)) list << "Livery";
            if (flag.testFlag(ModelEntity)) list << "Model";
            return list.join(',');
        }

        QString CDbFlags::flagToString(CDbFlags::ReadState flag)
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
    } // namespace
} // namespace
