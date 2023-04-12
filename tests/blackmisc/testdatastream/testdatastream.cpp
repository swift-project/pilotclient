/* Copyright (C) 2019
 * swift project Community / Contributors
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

#include "blackmisc/registermetadata.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/test/testservice.h"
#include "blackmisc/test/testserviceinterface.h"
#include "test.h"
#include <QTest>
#include <QByteArray>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Test;

namespace BlackMiscTest
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

    void CTestDataStream::initTestCase()
    {
        BlackMisc::registerMetadata();
    }

    void CTestDataStream::marshalUnmarshal()
    {
        CSimulatedAircraftList testData {
            { CCallsign("BAW123"), {}, {} },
            { CCallsign("DLH456"), {}, {} },
            { CCallsign("AAL789"), {}, {} }
        };

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
}

//! main
BLACKTEST_MAIN(BlackMiscTest::CTestDataStream);

#include "testdatastream.moc"

//! \endcond
