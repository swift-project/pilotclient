// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file
//! \ingroup sampleblackmisc

#include "sampleschangeobject.h"
#include "blackmisc/aviation/atcstation.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/network/user.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/predicates.h"
#include "blackmisc/propertyindexvariantmap.h"
#include "blackmisc/variant.h"

#include <QDateTime>
#include <QtDebug>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Network;

namespace BlackSample
{
    int CSamplesChangeObject::samples()
    {
        // ATC station
        const CCoordinateGeodetic geoPos = CCoordinateGeodetic::fromWgs84("48° 21′ 13″ N", "11° 47′ 09″ E", { 1487, CLengthUnit::ft() });
        const CAtcStation station1(CCallsign("eddm_twr"), CUser("123456", "Joe Doe"),
                                   CFrequency(118.7, CFrequencyUnit::MHz()),
                                   geoPos, CLength(50, CLengthUnit::km()), false);
        const CAtcStation station2(station1);
        const CAtcStation station3(CCallsign("eddm_app"), CUser("654321", "Jen Doe"),
                                   CFrequency(120.7, CFrequencyUnit::MHz()),
                                   geoPos, CLength(100, CLengthUnit::km()), false);

        Q_ASSERT_X(station1 == station2, Q_FUNC_INFO, "Unequal stations");

        // ATC List
        CAtcStationList atcList;
        atcList.push_back(station1);
        atcList.push_back(station2);
        atcList.push_back(station3);
        atcList.push_back(station1);
        atcList.push_back(station2);
        atcList.push_back(station3);
        qDebug() << "-- before update";
        qDebug() << atcList.toQString();

        // put Jane in the tower
        CPropertyIndexVariantMap newController;
        newController.addValue(CAtcStation::IndexController, CVariant::fromValue(CUser("112233", "Jane Doe")));
        atcList.applyIf(
            BlackMisc::Predicates::MemberEqual(&CAtcStation::getCallsign, CCallsign("eddm_twr")),
            newController);
        qDebug() << "-- after update via predicates";
        qDebug() << atcList.toQString();

        // now Jane's time is over
        CPropertyIndexVariantMap anotherController;
        anotherController.addValue(CAtcStation::IndexController, CVariant::fromValue(CUser("445566", "Fuzzy")));
        atcList.applyIf([=](const auto &arg) { return newController.matches(arg); }, anotherController);

        qDebug() << "-- after update via value map";
        qDebug() << atcList.toQString();

        qDebug() << "-----------------------------------------------";
        return 0;
    }
} // namespace
