// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testmisc

#include "misc/slot.h"
#include "test.h"

#include <QTest>

using namespace swift::misc;

namespace MiscTest
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
        CSlot<void(const QString &)> slot1 = { obj, [&](const QString &name) { obj->setObjectName(name); } };
        QVERIFY2(slot1, "Slot has valid object and function - can be called.");

        // KB 8/17 T125, CSlot can no longer be constructed with null object
        // CSlot<void(const QString &)> slot2 = { static_cast<QObject*>(nullptr), &QObject::setObjectName };
        // QVERIFY2(!slot2, "Slot has an invalid pointer - cannot be called.");

        CSlot<void(const QString &)> slot3;
        QVERIFY2(!slot3, "Slot has an invalid pointer and invalid function - cannot be called.");
    }

} // namespace

//! main
BLACKTEST_APPLESS_MAIN(MiscTest::CTestSlot);

#include "testslot.moc"

//! \endcond
