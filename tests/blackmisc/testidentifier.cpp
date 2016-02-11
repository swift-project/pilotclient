/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \cond PRIVATE_TESTS

/*!
 * \file
 * \ingroup testblackmisc
 */

#include "testidentifier.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/identifierlist.h"

using namespace BlackMisc;

namespace BlackMiscTest
{

    void CTestIdentifier::identifierBasics()
    {
        CIdentifier o1;
        CIdentifier o2;
        CIdentifier o3("foobar");
        QVERIFY2(o1 == o2, "Two default identifiers shall be equal");
        QVERIFY2(o1 != o3, "Those identifiers shall be unequal");

        // test object name change
        QObject q;
        q.setObjectName("foo");
        CTestIdentifiable oa(&q);
        QVERIFY2(oa.identifier().getName() == q.objectName(), "Names shall be equal");
        q.setObjectName("bar");
        QVERIFY2(oa.identifier().getName() == q.objectName(), "Names shall be equal");
    }

    CTestIdentifiable::CTestIdentifiable(QObject *nameObject) : CIdentifiable(nameObject)
    { }

} //namespace

//! \endcond
