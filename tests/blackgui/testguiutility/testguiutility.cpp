/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
