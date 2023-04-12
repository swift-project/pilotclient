/* Copyright (C) 2016
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testblackmisc

#include "blackmisc/processinfo.h"
#include "test.h"
#include <QTest>
#include <QCoreApplication>

using namespace BlackMisc;

namespace BlackMiscTest
{
    //! Testing process tools
    class CTestProcess : public QObject
    {
        Q_OBJECT

    private slots:
        //! Process info tests
        void processInfo();
    };

    void CTestProcess::processInfo()
    {
        CProcessInfo invalid;
        QVERIFY2(!invalid.exists(), "Invalid process shall not exist");

        CProcessInfo current1(CProcessInfo::currentProcess());
        CProcessInfo current2(QCoreApplication::applicationPid());
        CProcessInfo current3(QCoreApplication::applicationPid(), QCoreApplication::applicationName());
        QVERIFY2(current1.exists(), "Current process shall exist");
        QVERIFY2(current1 == current2, "Current process equals process with current PID");
        QVERIFY2(current1 == current3, "Current process equals process with current PID and name");
    }
}

//! main
BLACKTEST_MAIN(BlackMiscTest::CTestProcess);

#include "testprocess.moc"

//! \endcond
