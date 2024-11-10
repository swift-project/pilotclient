// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE_TESTS

/*!
 * \file
 * \ingroup testmisc
 */

#include "misc/worker.h"
#include "misc/eventloop.h"
#include <QObject>
#include <QTest>

using namespace swift::misc;

namespace MiscTest
{
    //! Aviation classes basic tests
    class CTestWorker : public QObject
    {
        Q_OBJECT

    public:
        //! Standard test case constructor
        explicit CTestWorker(QObject *parent = nullptr);

    private slots:
        //! Testing single shot
        void singleShot();
    };

    CTestWorker::CTestWorker(QObject *parent) : QObject(parent)
    {
        // void
    }

    void CTestWorker::singleShot()
    {
        QFuture<int> future = swift::misc::singleShot(0, this, []() { return 123; });
        CEventLoop::processEventsFor(0);
        QVERIFY2(future.isFinished(), "Future is finished after slot has returned");
        QVERIFY2(future.result() == 123, "Future provides access to slot's return value");
    }

} // namespace

//! main
SWIFTTEST_APPLESS_MAIN(MiscTest::CTestWorker);

#include "testworker.moc"

//! \endcond
