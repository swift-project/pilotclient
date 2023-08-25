// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testblackmisc

#include "blackconfig/buildconfig.h"
#include "blackmisc/librarypath.h"
#include "test.h"

#include <QTest>

namespace BlackMiscTest
{
    //! Geo classes tests
    class CTestLibraryPath : public QObject
    {
        Q_OBJECT

    private slots:
        //! Basic unit tests for library path
        void libraryPath();
    };

    void CTestLibraryPath::libraryPath()
    {
        if (!BlackConfig::CBuildConfig::isRunningOnWindowsNtPlatform()) { return; }

        auto emptyPath = BlackMisc::getCustomLibraryPath();
        QVERIFY2(emptyPath.isEmpty(), "Default path should be empty");

        QString customLibraryPath("c:\test");
        BlackMisc::setCustomLibraryPath(customLibraryPath);
        QVERIFY2(BlackMisc::getCustomLibraryPath() == customLibraryPath, "setLibraryPath failed");

        BlackMisc::setCustomLibraryPath(emptyPath);
        QVERIFY2(emptyPath.isEmpty(), "Path should be empty again.");
    }

} // namespace

//! main
BLACKTEST_MAIN(BlackMiscTest::CTestLibraryPath);

#include "testlibrarypath.moc"

//! \endcond
