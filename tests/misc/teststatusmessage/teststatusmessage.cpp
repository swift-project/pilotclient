// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testmisc

#include <QObject>
#include <QStringView>
#include <QTest>

#include "test.h"

#include "misc/statusmessage.h"

using namespace swift::misc;

namespace MiscTest
{
    //! Testing property index access
    class CTestStatusMessage : public QObject
    {
        Q_OBJECT

    private slots:
        //! Status message
        void statusMessage();
        //! Message with arguments
        void statusArgs();
    };

    void CTestStatusMessage::statusMessage()
    {
        CStatusMessage s1(CStatusMessage::SeverityDebug, u"debug msg.");
        s1.setMSecsSinceEpoch(4000);
        CStatusMessage s2(CStatusMessage::SeverityInfo, u"info msg.");
        s2.setMSecsSinceEpoch(3000);
        // CStatusMessage s3(CStatusMessage::SeverityWarning, u"warning msg.");
        // s3.setMSecsSinceEpoch(2000);
        // CStatusMessage s4(CStatusMessage::SeverityError, u"error msg.");
        // s4.setMSecsSinceEpoch(1000);

        CStatusMessage cCopy(s1);
        QVERIFY(cCopy.getMSecsSinceEpoch() == s1.getMSecsSinceEpoch());

        CStatusMessage cAssign;
        cAssign = s2;
        QVERIFY(cAssign.getMSecsSinceEpoch() == s2.getMSecsSinceEpoch());
    }

    void CTestStatusMessage::statusArgs()
    {
        auto s1 = CStatusMessage().info(u"literal percent: %1");
        auto s2 = CStatusMessage().info(u"literal percent: %a");
        auto s3 = CStatusMessage().info(u"literal percent: %");
        auto s4 = CStatusMessage().info(u"literal percent: %%");
        auto s5 = CStatusMessage().info(u"literal percents: %%%");
        auto s6 = CStatusMessage().info(u"will be expanded: %1%2") << "foo"
                                                                   << "bar";
        auto s7 = CStatusMessage().info(u"will be expanded: %1+%2") << "foo"
                                                                    << "bar";
        auto s8 = CStatusMessage().info(u"will be expanded: %012") << "foo";

        QVERIFY(s1.getMessage() == u"literal percent: %1");
        QVERIFY(s2.getMessage() == u"literal percent: %a");
        QVERIFY(s3.getMessage() == u"literal percent: %");
        QVERIFY(s4.getMessage() == u"literal percent: %");
        QVERIFY(s5.getMessage() == u"literal percents: %%");
        QVERIFY(s6.getMessage() == u"will be expanded: foobar");
        QVERIFY(s7.getMessage() == u"will be expanded: foo+bar");
        QVERIFY(s8.getMessage() == u"will be expanded: foo2");
    }
} // namespace MiscTest

//! main
SWIFTTEST_APPLESS_MAIN(MiscTest::CTestStatusMessage);

#include "teststatusmessage.moc"

//! \endcond
