// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE_TESTS

/*!
 * \file
 * \ingroup testmisc
 */

#include <QFileInfo>
#include <QObject>
#include <QTemporaryDir>
#include <QTest>

#include "test.h"

#include "config/buildconfig.h"
#include "misc/compressutils.h"
#include "misc/directoryutils.h"
#include "misc/fileutils.h"
#include "misc/swiftdirectories.h"

using namespace swift::misc;
using namespace swift::config;

namespace MiscTest
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
} // namespace MiscTest

//! main
SWIFTTEST_MAIN(MiscTest::CTestCompress);

#include "testcompress.moc"

//! \endcond
