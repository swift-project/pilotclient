// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file
//! \ingroup samplemisc

#include "samplescontainer.h"

#include <QDateTime>
#include <QDebug>
#include <QtDebug>

#include "misc/aviation/atcstation.h"
#include "misc/aviation/atcstationlist.h"
#include "misc/aviation/callsign.h"
#include "misc/geo/coordinategeodetic.h"
#include "misc/mixin/mixincompare.h"
#include "misc/network/user.h"
#include "misc/pq/frequency.h"
#include "misc/pq/length.h"
#include "misc/pq/units.h"
#include "misc/propertyindexlist.h"
#include "misc/propertyindexvariantmap.h"
#include "misc/range.h"
#include "misc/stringutils.h"
#include "misc/test/testdata.h"
#include "misc/variant.h"

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::geo;
using namespace swift::misc::network;
using namespace swift::misc::test;

namespace swift::sample
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

        CAtcStationList atcListSort = atcList.sortedBy(&CAtcStation::getCallsign, &CAtcStation::getControllerRealName);
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
} // namespace swift::sample
