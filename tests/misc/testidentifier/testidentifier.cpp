// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testmisc

#include <QObject>
#include <QString>
#include <QTest>

#include "test.h"

#include "misc/identifiable.h"
#include "misc/identifier.h"
#include "misc/mixin/mixincompare.h"

using namespace swift::misc;

namespace MiscTest
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
    class CTestIdentifiable : public swift::misc::CIdentifiable
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
        q.setObjectName(QString::fromUtf16(u"!@#$%^&*()_+\u263a"));
        CTestIdentifiable id(&q);
        QString s(id.identifier().toDBusObjectPath());
        QVERIFY2(id.identifier() == CIdentifier::fromDBusObjectPath(s), "Conversion from dbus object path and back compares equal");
    }

    CTestIdentifiable::CTestIdentifiable(QObject *nameObject) : CIdentifiable(nameObject)
    {}

} // namespace MiscTest

//! main
SWIFTTEST_APPLESS_MAIN(MiscTest::CTestIdentifier);

#include "testidentifier.moc"

//! \endcond
