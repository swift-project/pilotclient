// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testmisc

#include "misc/processinfo.h"
#include "test.h"
#include <QTest>
#include <QCoreApplication>

using namespace swift::misc;

namespace MiscTest
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
SWIFTTEST_MAIN(MiscTest::CTestProcess);

#include "testprocess.moc"

//! \endcond
