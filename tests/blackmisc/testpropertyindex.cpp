/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testblackmisc

#include "testpropertyindex.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include <QTest>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackMiscTest
{
    CTestPropertyIndex::CTestPropertyIndex(QObject *parent): QObject(parent)
    {
        // void
    }

    void CTestPropertyIndex::propertyIndexCSimulatedAircraft()
    {
        const CFrequency f(123.50, CFrequencyUnit::MHz());
        const CPropertyIndex i({ CSimulatedAircraft::IndexCom1System, CComSystem::IndexActiveFrequency });
        CSimulatedAircraft aircraft;
        aircraft.setCallsign("DEIHL");
        aircraft.setCom1ActiveFrequency(f);
        CVariant vf = aircraft.propertyByIndex(i);
        const CFrequency pf = vf.value<CFrequency>();
        QVERIFY2(pf == f, "Frequencies should have same value");
    }
} // namespace

//! \endcond
