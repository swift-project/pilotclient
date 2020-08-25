/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file
//! \ingroup sampleblackmisc

#include "samplescontainer.h"
#include "blackmisc/test/testdata.h"
#include "blackmisc/aviation/atcstation.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/mixin/mixincompare.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/network/user.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/propertyindexlist.h"
#include "blackmisc/propertyindexvariantmap.h"
#include "blackmisc/range.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/variant.h"

#include <QDateTime>
#include <QDebug>
#include <QtDebug>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Network;
using namespace BlackMisc::Test;

namespace BlackSample
{
    int CSamplesContainer::samples()
    {
        // ATC stations
        const CAtcStation station1(CTestData::getMunichTower());
        CAtcStation station2(station1);
        CAtcStation station3(CTestData::getMunichApproach());

        // ATC List
        CAtcStationList atcList;
        atcList.push_back(station1);
        atcList.push_back(station2);
        atcList.push_back(station3);
        atcList.push_back(station1);
        atcList.push_back(station2);
        atcList.push_back(station3);
        qDebug() << "-- list:";
        qDebug() << atcList.toQString();

        CAtcStationList atcListFind = atcList.findBy(&CAtcStation::getCallsign, "eddm_twr", &CAtcStation::getFrequency, CFrequency(118.7, CFrequencyUnit::MHz()));
        qDebug() << "-- find by:";
        qDebug() << atcListFind.toQString();

        CAtcStationList atcListSort = atcList.sortedBy(&CAtcStation::getBookedFromUtc, &CAtcStation::getCallsign, &CAtcStation::getControllerRealName);
        qDebug() << "-- sort by:";
        qDebug() << atcListSort.toQString();

        qDebug() << "-----------------------------------------------";

        // Apply if tests
        atcList.clear();
        atcList.push_back(station1);
        CAtcStation station1Cpy(station1);
        CFrequency changedFrequency(118.25, CFrequencyUnit::MHz());
        CPropertyIndexVariantMap vm(CAtcStation::IndexFrequency, CVariant::from(changedFrequency));

        // demonstration apply
        CPropertyIndexList changedProperties;
        changedProperties = station1Cpy.apply(vm, true);
        qDebug() << "apply, changed" << changedProperties << vm << "expected 1";
        changedProperties = station1Cpy.apply(vm, true);
        qDebug() << "apply, changed" << changedProperties << vm << "expected 0";

        // applyIf
        int changed;
        changed = atcList.applyIf(&CAtcStation::getCallsign, CTestData::getMunichTower().getCallsign(), vm);
        qDebug() << "applyIf, changed" << changed << vm << "expected 1";
        changed = atcList.applyIf(&CAtcStation::getCallsign, CTestData::getMunichTower().getCallsign(), vm);
        qDebug() << "applyIf, changed" << changed << vm << "expected 1";
        changed = atcList.applyIf(&CAtcStation::getCallsign, CTestData::getMunichTower().getCallsign(), vm, true);
        qDebug() << "applyIf, changed" << changed << vm << "expected 0";

        return 0;
    }
} // namespace
