/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "samplesaviation.h"
#include "blackmisc/pqconstants.h"
#include "blackmisc/avheading.h"
#include "blackmisc/aviocomsystem.h"
#include "blackmisc/avionavsystem.h"
#include "blackmisc/aviotransponder.h"

#include <QDebug>

using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackMiscTest
{

    /*
     * Samples
     */
    int CSamplesAviation::samples()
    {
        CHeading h1(180, CHeading::Magnetic, CAngleUnit::deg());
        CHeading h2(180, CHeading::True, CAngleUnit::deg());

        qDebug() << h1;
        qDebug() << h1 << h2 << (h1 == h2) << (h1 != h2) << (h1 == h1);

        // COM system
        CComSystem c1 = CComSystem::getCom1System(125.3);
        qDebug() << c1;
        c1.setActiveUnicom();
        qDebug() << c1;

        if (!CComSystem::tryGetComSystem(c1, "Test", -1.0))
            qDebug() << c1 << "is reset to default as expected";
        else
            qDebug() << "Something is wrong here";

        try
        {
            // uncomment to test assert
            // CFrequency f1(-1.0, CFrequencyUnit::MHz());
            // c1 = CComSystem("ups", f1, f1);
            // qDebug() << "Why do I get here??";
        }
        catch (std::range_error &ex)
        {
            qDebug() << "As expected" << ex.what();
        }

        // NAV system
        CNavSystem nav1;
        CNavSystem::tryGetNav1System(nav1, 110.0);
        qDebug() << nav1;

        // Transponder tests
        CTransponder tr1("T1", 7000, CTransponder::StateStandby);
        CTransponder tr2("T2", "4532", CTransponder::ModeMil3);
        qDebug() << tr1 << tr2;



        qDebug() << "-----------------------------------------------";
        return 0;
    }

} // namespace
