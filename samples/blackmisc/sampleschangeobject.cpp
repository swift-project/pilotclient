/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
        const QDateTime dtFrom = QDateTime::currentDateTimeUtc();
        const QDateTime dtUntil = dtFrom.addSecs(60 * 60); // 1 hour
        const QDateTime dtFrom2 = dtUntil;
        const QDateTime dtUntil2 = dtUntil.addSecs(60 * 60);
        const CCoordinateGeodetic geoPos = CCoordinateGeodetic::fromWgs84("48° 21′ 13″ N", "11° 47′ 09″ E", { 1487, CLengthUnit::ft() });
        const CAtcStation station1(CCallsign("eddm_twr"), CUser("123456", "Joe Doe"),
                                   CFrequency(118.7, CFrequencyUnit::MHz()),
                                   geoPos, CLength(50, CLengthUnit::km()), false, dtFrom, dtUntil);
        const CAtcStation station2(station1);
        const CAtcStation station3(CCallsign("eddm_app"), CUser("654321", "Jen Doe"),
                             CFrequency(120.7, CFrequencyUnit::MHz()),
                             geoPos, CLength(100, CLengthUnit::km()), false, dtFrom2, dtUntil2);

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
        atcList.applyIf([ = ](const auto &arg) { return newController.matches(arg); }, anotherController);

        qDebug() << "-- after update via value map";
        qDebug() << atcList.toQString();

        qDebug() << "-----------------------------------------------";
        return 0;
    }
} // namespace
