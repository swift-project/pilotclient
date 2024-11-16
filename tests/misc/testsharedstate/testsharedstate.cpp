// SPDX-FileCopyrightText: Copyright (C) 2020 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE_TESTS

/*!
 * \file
 * \ingroup testmisc
 */

#include "testsharedstate.h"

#include <QDBusConnection>
#include <QProcess>
#include <QTest>

#include "test.h"

#include "misc/dbusserver.h"
#include "misc/registermetadata.h"
#include "misc/sharedstate/datalinkdbus.h"
#include "misc/sharedstate/datalinklocal.h"

using namespace QTest;
using namespace swift::misc;
using namespace swift::misc::shared_state;

namespace MiscTest
{
    //! DBus implementation classes tests
    class CTestSharedState : public QObject
    {
        Q_OBJECT

    private slots:
        //! Init test case environment
        void initTestCase();

        //! Test promise/future pair
        void testPromise();

        //! Test scalar value shared over local datalink
        void localScalar();

        //! Test list value shared over local datalink
        void localList();

        //! Test scalar value shared over dbus datalink
        void dbusScalar();

        //! Test list value shared over dbus datalink
        void dbusList();
    };

    void CTestSharedState::initTestCase()
    {
        swift::misc::registerMetadata();
        CTestFilter::registerMetadata();
    }

    void CTestSharedState::testPromise()
    {
        int result = 0;
        CPromise<int> promise;
        doAfter(promise.future(), nullptr, [&](QFuture<int> future) { result = future.result(); });
        promise.setResult(42);
        bool ok = qWaitFor([&] { return result == 42; });
        QVERIFY2(ok, "future contains expected result");
    }

    void CTestSharedState::localScalar()
    {
        CDataLinkLocal dataLink;
        CTestScalarMutator mutator(this);
        CTestScalarJournal journal(this);
        CTestScalarObserver observer(this);
        mutator.initialize(&dataLink);
        journal.initialize(&dataLink);
        observer.initialize(&dataLink);

        mutator.setValue(42);
        bool ok = qWaitFor([&] { return observer.value() == 42; });
        QVERIFY2(ok, "expected value received");
        qWait(0);
        QVERIFY2(observer.value() == 42, "still has expected value");

        CTestScalarObserver observer2(this);
        observer2.initialize(&dataLink);

        ok = qWaitFor([&] { return observer2.value() == 42; });
        QVERIFY2(ok, "new observer got existing value");
        qWait(0);
        QVERIFY2(observer2.value() == 42, "still has correct value");
    }

    void CTestSharedState::localList()
    {
        CDataLinkLocal dataLink;
        CTestListMutator mutator(this);
        CTestListJournal journal(this);
        CTestListObserver observer(this);
        mutator.initialize(&dataLink);
        journal.initialize(&dataLink);
        observer.initialize(&dataLink);

        observer.setFilter({});
        mutator.addElement(1);
        bool ok = qWaitFor([&] { return observer.allValues() == QList<int> { 1 }; });
        QVERIFY2(ok, "expected value received");
        qWait(0);
        QVERIFY2(observer.allValues() == QList<int> { 1 }, "still has expected value");

        for (int e = 2; e <= 6; ++e) { mutator.addElement(e); }
        ok = qWaitFor([&] { return observer.allValues() == QList<int> { 1, 2, 3, 4, 5, 6 }; });
        QVERIFY2(ok, "expected value received");

        observer.allValues() = {};
        observer.setFilter({ 1 });
        ok = qWaitFor([&] { return observer.allValues() == QList<int> { 1, 3, 5 }; });
        QVERIFY2(ok, "expected value received");
    }

    //! RAII wrapper
    class Server
    {
    public:
        //! ctor
        Server()
        {
            QObject::connect(&m_process, qOverload<int, QProcess::ExitStatus>(&QProcess::finished),
                             [](int code, QProcess::ExitStatus status) {
                                 qDebug() << "Server process exited" << (status ? "abnormally" : "normally")
                                          << "with exit code" << code;
                             });
            m_process.start(QCoreApplication::applicationDirPath() + "/tests_sharedstatetestserver", QStringList());
            if (!m_process.waitForStarted()) { qDebug() << "Server failed to start"; }
        }
        //! dtor
        ~Server()
        {
            m_process.disconnect();
            m_process.kill();
            m_process.waitForFinished();
        }

    private:
        QProcess m_process;
    };

    void CTestSharedState::dbusScalar()
    {
        QDBusConnection connection = QDBusConnection::sessionBus();
        if (!connection.isConnected()) { QSKIP("No session bus"); }
        Server s;

        CDataLinkDBus dataLink;
        dataLink.initializeRemote(connection, SWIFT_SERVICENAME);
        bool ok = qWaitFor([&] { return dataLink.watcher()->isConnected(); });
        QVERIFY2(ok, "Connection failed");

        CTestScalarObserver observer(this);
        CTestScalarMutator mutator(this);
        observer.initialize(&dataLink);
        mutator.initialize(&dataLink);

        ok = qWaitFor([&] { return observer.value() == 42; });
        QVERIFY2(ok, "expected value received");
        qWait(1000);
        QVERIFY2(observer.value() == 42, "still has expected value");

        mutator.setValue(69);
        ok = qWaitFor([&] { return observer.value() == 69; });
        QVERIFY2(ok, "expected value received");
        qWait(1000);
        QVERIFY2(observer.value() == 69, "still has expected value");
    }

    void CTestSharedState::dbusList()
    {
        QDBusConnection connection = QDBusConnection::sessionBus();
        if (!connection.isConnected()) { QSKIP("No session bus"); }
        Server s;

        CDataLinkDBus dataLink;
        dataLink.initializeRemote(connection, SWIFT_SERVICENAME);
        bool ok = qWaitFor([&] { return dataLink.watcher()->isConnected(); });
        QVERIFY2(ok, "Connection failed");

        CTestListObserver observer(this);
        CTestListMutator mutator(this);
        observer.initialize(&dataLink);
        mutator.initialize(&dataLink);

        observer.setFilter({});
        ok = qWaitFor([&] { return observer.allValues() == QList<int> { 1, 2, 3, 4, 5, 6 }; });
        QVERIFY2(ok, "expected value received");
        qWait(1000);
        QVERIFY2(observer.allValues() == QList<int>({ 1, 2, 3, 4, 5, 6 }), "still has expected value");

        observer.allValues() = {};
        observer.setFilter({ 1 });
        ok = qWaitFor([&] { return observer.allValues() == QList<int> { 1, 3, 5 }; });
        QVERIFY2(ok, "expected value received");

        mutator.addElement(7);
        ok = qWaitFor([&] { return observer.allValues() == QList<int> { 1, 3, 5, 7 }; });
        QVERIFY2(ok, "expected value received");
        qWait(1000);
        QVERIFY2(observer.allValues() == QList<int>({ 1, 3, 5, 7 }), "still has expected value");
    }
} // namespace MiscTest

//! main
SWIFTTEST_MAIN(MiscTest::CTestSharedState);

#include "testsharedstate.moc"

//! \endcond
