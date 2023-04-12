/* Copyright (C) 2018
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \cond PRIVATE_TESTS

/*!
 * \file
 * \ingroup testblackmisc
 */

#include "blackconfig/buildconfig.h"
#include "blackmisc/compressutils.h"
#include "blackmisc/swiftdirectories.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/fileutils.h"
#include "test.h"

#include <QObject>
#include <QFileInfo>
#include <QTemporaryDir>
#include <QTest>

using namespace BlackMisc;
using namespace BlackConfig;

namespace BlackMiscTest
{
    //! Testing process tools
    class CTestCompress : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CTestCompress(QObject *parent = nullptr) : QObject(parent) {}

        //! Destructor
        virtual ~CTestCompress() {}

    private slots:
        //! Uncompress file
        void uncompressFile();
    };

    void CTestCompress::uncompressFile()
    {
        QTemporaryDir tempDir;
        tempDir.setAutoRemove(true);
        QVERIFY2(tempDir.isValid(), "Invalid directory");

        const bool win = CBuildConfig::isRunningOnWindowsNtPlatform();
        const bool zip7Exists = CCompressUtils::hasZip7();
        if (!win && !zip7Exists)
        {
            QSKIP("No 7zip, skipping");
            return;
        }

        QVERIFY2(zip7Exists, "No 7zip");

        const QString td = tempDir.path();
        const QString compressedFile(CFileUtils::appendFilePaths(CSwiftDirectories::shareTestDirectory(), "countries.json.gz"));
        const QString unCompressedFile(CFileUtils::appendFilePaths(td, "countries.json"));
        const bool c = CCompressUtils::zip7Uncompress(compressedFile, td);

        QVERIFY2(c, "Uncompressing failed");

        const QFileInfo check(unCompressedFile);
        QVERIFY2(check.size() > 1000, "Uncompressing yielded not data");
        QVERIFY2(check.exists(), "Uncompressed file does not exist");
        QVERIFY2(check.isReadable(), "Not readable");

        qDebug() << "Uncompressed" << compressedFile << "to" << unCompressedFile << "with size" << check.size();
    }
}

//! main
BLACKTEST_MAIN(BlackMiscTest::CTestCompress);

#include "testcompress.moc"

//! \endcond
