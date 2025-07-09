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

        const QString td = tempDir.path();
        const QString compressedFile(CFileUtils::appendFilePaths(CSwiftDirectories::shareTestDirectory(), "test.zip"));
        const QString unCompressedFile(CFileUtils::appendFilePaths(td, "1.txt"));
        const bool c = CCompressUtils::zipUncompress(compressedFile, td);

        QVERIFY2(c, "Uncompressing failed");

        const QFileInfo check(unCompressedFile);
        QVERIFY2(check.exists(), "Uncompressed file does not exist");
        QVERIFY2(check.isReadable(), "Not readable");

        qDebug() << "Uncompressed" << compressedFile << "to" << unCompressedFile << "with size" << check.size();
    }
} // namespace MiscTest

//! main
SWIFTTEST_MAIN(MiscTest::CTestCompress);

#include "testcompress.moc"

//! \endcond
