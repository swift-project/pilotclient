/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testblackmisc

#include "blackmisc/compare.h"
#include "blackmisc/identifier.h"
#include "blackmisc/identifiable.h"
#include "test.h"

#include <QObject>
#include <QString>
#include <QTest>

using namespace BlackMisc;

namespace BlackMiscTest
{
    //! Testing object identifier
    class CTestIdentifier : public QObject
    {
        Q_OBJECT

    private slots:
        //! Identifier tests
        void identifierBasics();

        //! Machine unique id tests
        void machineUniqueId();

        //! Conversion to dbus object path
        void dbusObjectPath();
    };

    //! Test identifiable object
    class CTestIdentifiable : public BlackMisc::CIdentifiable
    {
    public:
        //! Constructor
        CTestIdentifiable(QObject *nameObject);
    };

    void CTestIdentifier::identifierBasics()
    {
        CIdentifier o1;
        CIdentifier o2;
        CIdentifier o3("foobar");
        QVERIFY2(o1 == o2, "Two default identifiers shall be equal");
        QVERIFY2(o1 != o3, "Those identifiers shall be unequal");

        // test object name change
        QObject q;
        q.setObjectName("foo");
        CTestIdentifiable oa(&q);
        QVERIFY2(oa.identifier().getName() == q.objectName(), "Names shall be equal");
        q.setObjectName("bar");
        QVERIFY2(oa.identifier().getName() == q.objectName(), "Names shall be equal");
    }

    void CTestIdentifier::machineUniqueId()
    {
        CIdentifier o;
        QVERIFY2(!o.getMachineId().isEmpty(), "Machine id shall never be empty! If this test failed on a supported platform, get a fallback solution!");
    }

    void CTestIdentifier::dbusObjectPath()
    {
        QObject q;
        q.setObjectName("!@#$%^&*()_+");
        CTestIdentifiable id(&q);
        QString s(id.identifier().toDBusObjectPath());
        QVERIFY2(id.identifier() == CIdentifier::fromDBusObjectPath(s), "Conversion from dbus object path and back compares equal");
    }

    CTestIdentifiable::CTestIdentifiable(QObject *nameObject) : CIdentifiable(nameObject)
    { }

} //namespace

//! main
BLACKTEST_APPLESS_MAIN(BlackMiscTest::CTestIdentifier);

#include "testidentifier.moc"

//! \endcond
