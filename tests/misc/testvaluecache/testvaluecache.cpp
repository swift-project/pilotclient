// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testmisc

#include "misc/valuecache.h"
#include "misc/aviation/atcstation.h"
#include "misc/aviation/atcstationlist.h"
#include "misc/dictionary.h"
#include "misc/identifier.h"
#include "misc/registermetadata.h"
#include "misc/simulation/simulatedaircraft.h"
#include "misc/simulation/simulatedaircraftlist.h"
#include "misc/statusmessage.h"
#include "misc/variant.h"
#include "misc/variantmap.h"
#include "misc/worker.h"
#include "test.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QFlags>
#include <QJsonObject>
#include <QJsonValue>
#include <QList>
#include <QMetaObject>
#include <QRegularExpression>
#include <QString>
#include <QTest>
#include <QThread>
#include <QTimer>
#include <QtDebug>
#include <chrono>
#include <future>
#include <ratio>

namespace MiscTest
{
    using namespace swift::misc;
    using namespace swift::misc::aviation;
    using namespace swift::misc::simulation;

    //! Unit tests for value cache system.
    class CTestValueCache : public QObject
    {
        Q_OBJECT

    private slots:
        //! Init test case data
        void initTestCase();

        //! Test insert and getAll functions of CValueCache.
        void insertAndGet();

        //! Test using CCached locally in one process.
        void localOnly();

        //! Test using CCached locally in one process, with multiple threads.
        void localOnlyWithThreads();

        //! Test using CCached distributed among two processes.
        void distributed();

        //! Test using batched changes.
        void batched();

        //! Test Json serialization.
        void json();

        //! Test saving to and loading from files.
        void saveAndLoad();
    };

    //! Simple class which uses CCached, for testing.
    class CValueCacheUser : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor.
        explicit CValueCacheUser(swift::misc::CValueCache *cache);

        //! Slot to be called when a cached value changes.
        void ps_valueChanged();

        //! Detect whether the slot was called, for verification.
        bool slotFired();

        std::promise<void> m_slotFired; //!< Flag marking whether the slot was called.
        swift::misc::CCached<int> m_value1; //!< First cached value.
        swift::misc::CCached<int> m_value2; //!< Second cached value.
    };

    void CTestValueCache::initTestCase()
    {
        swift::misc::registerMetadata();
    }

    void CTestValueCache::insertAndGet()
    {
        CVariantMap testData {
            { "value1", CVariant::from(1) },
            { "value2", CVariant::from(2) },
            { "value3", CVariant::from(3) }
        };
        CVariantMap testData2 {
            { "value2", CVariant::from(42) },
            { "value4", CVariant::from(4) }
        };
        CVariantMap testDataCombined {
            { "value1", CVariant::from(1) },
            { "value2", CVariant::from(42) },
            { "value3", CVariant::from(3) },
            { "value4", CVariant::from(4) }
        };

        CValueCache cache(1);
        QVERIFY(cache.getAllValues() == CVariantMap());
        cache.insertValues({ testData, QDateTime::currentMSecsSinceEpoch() });
        QVERIFY(cache.getAllValues() == testData);
        cache.insertValues({ testData2, QDateTime::currentMSecsSinceEpoch() });
        QVERIFY(cache.getAllValues() == testDataCombined);
    }

    //! \cond PRIVATE
    void waitForQueueOf(QObject *object)
    {
        if (object->thread() != QThread::currentThread())
        {
            std::promise<void> promise;
            QTimer::singleShot(0, object, [&] { promise.set_value(); });
            promise.get_future().wait();
        }
    }

    template <typename F>
    void singleShotAndWait(QObject *object, F task)
    {
        if (object->thread() == QThread::currentThread())
        {
            task();
        }
        else
        {
            QTimer::singleShot(0, object, task);
            waitForQueueOf(object);
        }
    }

    void testCommon(CValueCacheUser &user1, CValueCacheUser &user2)
    {
        user1.m_value1.set(42);
        QVERIFY(user2.slotFired());
        QVERIFY(!user1.slotFired());
        singleShotAndWait(&user2, [&] { QVERIFY(user2.m_value1.get() == 42); });
        QVERIFY(user1.m_value1.get() == 42);

        user1.m_value2.set(42);
        user2.slotFired();
        auto status = user1.m_value2.set(-1337);
        QVERIFY(status.isFailure());
        QVERIFY(!user1.slotFired());
        QVERIFY(!user2.slotFired());
        singleShotAndWait(&user2, [&] { QVERIFY(user2.m_value2.get() == 42); });
        QVERIFY(user1.m_value2.get() == 42);
    }
    //! \endcond

    void CTestValueCache::localOnly()
    {
        CValueCache cache(1);
        for (int i = 0; i < 2; ++i) { QTest::ignoreMessage(QtDebugMsg, QRegularExpression("Empty cache value")); }
        CValueCacheUser user1(&cache);
        CValueCacheUser user2(&cache);
        testCommon(user1, user2);
    }

    void CTestValueCache::localOnlyWithThreads()
    {
        CValueCache cache(1);
        for (int i = 0; i < 2; ++i) { QTest::ignoreMessage(QtDebugMsg, QRegularExpression("Empty cache value")); }
        CValueCacheUser user1(&cache);
        CValueCacheUser user2(&cache);
        CRegularThread thread;
        user2.moveToThread(&thread);
        thread.start();
        testCommon(user1, user2);
    }

    void CTestValueCache::distributed()
    {
        CIdentifier thisProcess;
        CIdentifier otherProcess;
        auto json = otherProcess.toJson();
        json.insert("processId", otherProcess.getProcessId() + 1);
        otherProcess.convertFromJson(json);

        CValueCache thisCache(1);
        CValueCache otherCache(1);
        connect(&thisCache, &CValueCache::valuesChangedByLocal, &thisCache, [&](const CValueCachePacket &values) {
            QMetaObject::invokeMethod(&thisCache, [=, &thisCache] { thisCache.changeValuesFromRemote(values, thisProcess); });
            QMetaObject::invokeMethod(&otherCache, [=, &otherCache] { otherCache.changeValuesFromRemote(values, otherProcess); });
        });
        connect(&otherCache, &CValueCache::valuesChangedByLocal, &thisCache, [&](const CValueCachePacket &values) {
            QMetaObject::invokeMethod(&thisCache, [=, &thisCache] { thisCache.changeValuesFromRemote(values, otherProcess); });
            QMetaObject::invokeMethod(&otherCache, [=, &otherCache] { otherCache.changeValuesFromRemote(values, thisProcess); });
        });

        for (int i = 0; i < 4; ++i) { QTest::ignoreMessage(QtDebugMsg, QRegularExpression("Empty cache value")); }
        CValueCacheUser thisUser(&thisCache);
        CValueCacheUser otherUser(&otherCache);

        CRegularThread thread;
        otherCache.moveToThread(&thread);
        otherUser.moveToThread(&thread);
        thread.start();

        singleShotAndWait(&otherUser, [&] { otherUser.m_value1.set(99); });
        thisUser.m_value1.set(100);
        QCoreApplication::processEvents();
        waitForQueueOf(&otherUser);
        QVERIFY(thisUser.slotFired() != otherUser.slotFired());
        auto thisValue = thisUser.m_value1.get();
        singleShotAndWait(&otherUser, [&] { QVERIFY(thisValue == otherUser.m_value1.get()); });
    }

    void CTestValueCache::batched()
    {
        CValueCache cache(1);
        for (int i = 0; i < 2; ++i) { QTest::ignoreMessage(QtDebugMsg, QRegularExpression("Empty cache value")); }
        CValueCacheUser user1(&cache);
        CValueCacheUser user2(&cache);

        {
            auto batch = cache.batchChanges(&user1);
            user1.m_value1.set(42);
            user1.m_value2.set(42);
        }
        QVERIFY(!user1.slotFired());
        QVERIFY(user2.slotFired());
        singleShotAndWait(&user2, [&] {
            QVERIFY(user2.m_value1.get() == 42);
            QVERIFY(user2.m_value2.get() == 42);
        });
    }

    void CTestValueCache::json()
    {
        QJsonObject testJson {
            { "value1", CVariant::from(1).toJson() },
            { "value2", CVariant::from(2).toJson() },
            { "value3", CVariant::from(3).toJson() }
        };
        CVariantMap testData {
            { "value1", CVariant::from(1) },
            { "value2", CVariant::from(2) },
            { "value3", CVariant::from(3) }
        };

        CValueCache cache(1);
        cache.loadFromJson(testJson);
        QVERIFY(cache.getAllValues() == testData);
        QVERIFY(cache.saveToJson() == testJson);
    }

    void CTestValueCache::saveAndLoad()
    {
        CSimulatedAircraftList aircraft({ CSimulatedAircraft("BAW001", {}, {}) });
        CAtcStationList atcStations({ CAtcStation("EGLL_TWR") });
        const CVariantMap testData {
            { "namespace1/value1", CVariant::from(1) },
            { "namespace1/value2", CVariant::from(2) },
            { "namespace1/value3", CVariant::from(3) },
            { "namespace2/aircraft", CVariant::from(aircraft) },
            { "namespace2/atcstations", CVariant::from(atcStations) }
        };
        CValueCache cache(1);
        cache.insertValues({ testData, QDateTime::currentMSecsSinceEpoch() });

        QDir dir(QDir::currentPath() + "/testcache");
        if (dir.exists()) { dir.removeRecursively(); }

        auto status = cache.saveToFiles(dir.absolutePath());
        QVERIFY(status.isSuccess());

        auto files = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot, QDir::Name);
        QCOMPARE(files.size(), 2);
        QCOMPARE(files[0].fileName(), QString("namespace1.json"));
        QCOMPARE(files[1].fileName(), QString("namespace2.json"));

        CValueCache cache2(1);
        status = cache2.loadFromFiles(dir.absolutePath());
        QVERIFY(status.isSuccess());
        const CVariantMap test2Values = cache2.getAllValues();
        QCOMPARE(test2Values, testData);
    }

    //! Is value between 0 - 100?
    bool validator(int value, QString &)
    {
        return value >= 0 && value <= 100;
    }

    CValueCacheUser::CValueCacheUser(CValueCache *cache) : m_value1(cache, "value1", "", validator, 0, this),
                                                           m_value2(cache, "value2", "", validator, 0, this)
    {
        m_value1.setNotifySlot(&CValueCacheUser::ps_valueChanged);
        m_value2.setNotifySlot(&CValueCacheUser::ps_valueChanged);
    }

    void CValueCacheUser::ps_valueChanged()
    {
        m_slotFired.set_value();
    }

    bool CValueCacheUser::slotFired()
    {
        auto status = m_slotFired.get_future().wait_for(std::chrono::milliseconds(250));
        m_slotFired = std::promise<void>();
        switch (status)
        {
        case std::future_status::ready: return true;
        case std::future_status::timeout: return false;
        case std::future_status::deferred:
        default: QTEST_ASSERT(false);
        }
        return false;
    }
} // ns

//! main
BLACKTEST_MAIN(MiscTest::CTestValueCache);

#include "testvaluecache.moc"

//! \endcond
