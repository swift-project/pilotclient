// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE_TESTS

/*!
 * \file
 * \ingroup testmisc
 */

#include <QByteArray>
#include <QTest>

#include "test.h"

#include "misc/registermetadata.h"
#include "misc/simulation/simulatedaircraftlist.h"
#include "misc/test/testservice.h"
#include "misc/test/testserviceinterface.h"

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::simulation;
using namespace swift::misc::test;

namespace MiscTest
{
    //! QDataStream serialization tests
    class CTestDataStream : public QObject
    {
        Q_OBJECT

    private slots:
        //! Init test case data
        void initTestCase();

        //! Test marshaling/unmarshaling
        void marshalUnmarshal();
    };

    void CTestDataStream::initTestCase() { swift::misc::registerMetadata(); }

    void CTestDataStream::marshalUnmarshal()
    {
        CSimulatedAircraftList testData { { CCallsign("BAW123"), {}, {} },
                                          { CCallsign("DLH456"), {}, {} },
                                          { CCallsign("AAL789"), {}, {} } };

        QByteArray bytes;
        {
            QDataStream writer(&bytes, QIODevice::WriteOnly);
            writer << testData;
        }
        {
            QDataStream reader(bytes);
            CSimulatedAircraftList result;
            reader >> result;
            QVERIFY2(result == testData, "roundtrip marshal/unmarshal compares equal");
        }
    }
} // namespace MiscTest

//! main
SWIFTTEST_MAIN(MiscTest::CTestDataStream);

#include "testdatastream.moc"

//! \endcond
