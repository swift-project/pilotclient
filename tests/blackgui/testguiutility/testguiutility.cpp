// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testblackgui

#include "blackgui/guiutility.h"
#include "test.h"

using namespace BlackGui;

namespace BlackGuiTest
{
    //! Test GUI utilities
    class CTestGuiUtility : public QObject
    {
        Q_OBJECT

    private slots:
        //! Test the lenientTitleComparison
        void testLenientTitleComparison();
    };

    void CTestGuiUtility::testLenientTitleComparison()
    {
        QVERIFY2(CGuiUtility::lenientTitleComparison("foo", "foo"), "wrong title match");
        QVERIFY2(CGuiUtility::lenientTitleComparison("foo&", "&Foo"), "wrong title match");
        QVERIFY2(!CGuiUtility::lenientTitleComparison("foo", "bar"), "wrong title mismatch");
    }
} // ns

//! main
BLACKTEST_APPLESS_MAIN(BlackGuiTest::CTestGuiUtility);

#include "testguiutility.moc"

//! \endcond
