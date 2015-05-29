/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "testoriginator.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/originatorlist.h"

using namespace BlackMisc;

namespace BlackMiscTest
{

    void CTestOriginator::originatorBasics()
    {
        COriginator o1;
        COriginator o2;
        COriginator o3("foobar");
        QVERIFY2(o1 == o2, "Two default originators shall be equal");
        QVERIFY2(o1 != o3, "Those originators shall be unequal");

        // test object name change
        QObject q;
        q.setObjectName("foo");
        CTestOriginatorAware oa(&q);
        QVERIFY2(oa.originator().getName() == q.objectName(), "Names shall be equal");
        q.setObjectName("bar");
        QVERIFY2(oa.originator().getName() == q.objectName(), "Names shall be equal");
    }

    CTestOriginatorAware::CTestOriginatorAware(QObject *nameObject) : COriginatorAware(nameObject)
    { }

} //namespace
