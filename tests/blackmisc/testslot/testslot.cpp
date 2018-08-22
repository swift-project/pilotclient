/* Copyright (C) 2016
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

#include "blackmisc/slot.h"
#include "test.h"

#include <QTest>

using namespace BlackMisc;

namespace BlackMiscTest
{
    //! CSlot tests
    class CTestSlot : public QObject
    {
        Q_OBJECT

    private slots:
        //! Basic unit tests for CSlot
        void slotBasics();
    };

    void CTestSlot::slotBasics()
    {
        QObject *obj = new QObject(this);
        CSlot<void(const QString &)> slot1 = { obj, &QObject::setObjectName };
        QVERIFY2(slot1, "Slot has valid object and function - can be called.");

        // KB 8/17 T125, CSlot can no longer be constructed with null object
        // CSlot<void(const QString &)> slot2 = { static_cast<QObject*>(nullptr), &QObject::setObjectName };
        // QVERIFY2(!slot2, "Slot has an invalid pointer - cannot be called.");

        CSlot<void(const QString &)> slot3;
        QVERIFY2(!slot3, "Slot has an invalid pointer and invalid function - cannot be called.");
    }

} // namespace

//! main
BLACKTEST_APPLESS_MAIN(BlackMiscTest::CTestSlot);

#include "testslot.moc"

//! \endcond
