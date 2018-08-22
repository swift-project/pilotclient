/* Copyright (C) 2018
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

#include "blackmisc/statusmessage.h"
#include "test.h"
#include <QObject>
#include <QTest>

using namespace BlackMisc;

namespace BlackMiscTest
{
    //! Testing property index access
    class CTestStatusMessage : public QObject
    {
        Q_OBJECT

    private slots:
        //! Status message
        void statusMessage();
    };

    void CTestStatusMessage::statusMessage()
    {
        CStatusMessage s1(CStatusMessage::SeverityDebug, "debug msg.");
        s1.setMSecsSinceEpoch(4000);
        CStatusMessage s2(CStatusMessage::SeverityInfo, "info msg.");
        s2.setMSecsSinceEpoch(3000);
        // CStatusMessage s3(CStatusMessage::SeverityWarning, "warning msg.");
        // s3.setMSecsSinceEpoch(2000);
        // CStatusMessage s4(CStatusMessage::SeverityError, "error msg.");
        // s4.setMSecsSinceEpoch(1000);

        CStatusMessage cCopy(s1);
        QVERIFY(cCopy.getMSecsSinceEpoch() == s1.getMSecsSinceEpoch());

        CStatusMessage cAssign;
        cAssign = s2;
        QVERIFY(cAssign.getMSecsSinceEpoch() == s2.getMSecsSinceEpoch());
    }
} // namespace

//! main
BLACKTEST_APPLESS_MAIN(BlackMiscTest::CTestStatusMessage);

#include "teststatusmessage.moc"

//! \endcond
