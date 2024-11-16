// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file
//! \ingroup samplemisc

#include "sampleschangeobject.h"

#include <QDateTime>
#include <QtDebug>

#include "misc/aviation/atcstation.h"
#include "misc/aviation/atcstationlist.h"
#include "misc/aviation/callsign.h"
#include "misc/geo/coordinategeodetic.h"
#include "misc/network/user.h"
#include "misc/pq/frequency.h"
#include "misc/pq/length.h"
#include "misc/pq/units.h"
#include "misc/predicates.h"
#include "misc/propertyindexvariantmap.h"
#include "misc/variant.h"

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::geo;
using namespace swift::misc::network;

namespace swift::sample
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
            swift::misc::predicates::MemberEqual(&CAtcStation::getCallsign, CCallsign("eddm_twr")),
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
} // namespace swift::sample
