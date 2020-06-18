/* Copyright (C) 2013
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

#include "../testvalueobject.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/aircraftsituationlist.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/callsignset.h"
#include "blackmisc/collection.h"
#include "blackmisc/dictionary.h"
#include "blackmisc/iterator.h"
#include "blackmisc/range.h"
#include "blackmisc/registermetadata.h"
#include "blackmisc/sequence.h"
#include "blackmisc/math/mathutils.h"
#include "test.h"

#include <QDateTime>
#include <QJsonObject>
#include <QList>
#include <QSet>
#include <QString>
#include <QTest>
#include <QVector>
#include <QtGlobal>
#include <algorithm>
#include <iterator>
#include <set>
#include <vector>


using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Math;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackMiscTest
{
    //! Testing containers
    class CTestContainers : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CTestContainers(QObject *parent = nullptr) : QObject(parent) {}

    private slots:
        void initTestCase();

        void collectionBasics();
        void sequenceBasics();
        void joinAndSplit();
        void findTests();
        void sortTests();
        void removeTests();
        void dictionaryBasics();
        void timestampList();
        void offsetTimestampList();
    };

    void CTestContainers::initTestCase()
    {
        BlackMisc::registerMetadata();
    }

    void CTestContainers::collectionBasics()
    {
        CCollection<int> c1;
        QVERIFY2(c1.empty(), "Uninitialized collection is empty");
        auto c2 = c1;
        QVERIFY2(c1 == c2, "Copy of collection is equal");
        c1.insert(1);
        QVERIFY2(c1 != c2, "Different collections are not equal");
        QVERIFY2(c1.size() == 1, "Collection has expected size");
        c2.insert(1);
        QVERIFY2(c1 == c2, "Collections with equal elements are equal");
        c1.clear();
        QVERIFY2(c1.empty(), "Cleared collection is empty");
        c1.insert(2);
        QVERIFY2(c1 != c2, "Collections with different elements are not equal");
        c1 = c2;
        QVERIFY2(c1 == c2, "Copied collection is equal");
    }

    void CTestContainers::sequenceBasics()
    {
        CSequence<int> s1;
        QVERIFY2(s1.empty(), "Uninitialized sequence is empty");
        auto s2 = s1;
        QVERIFY2(s1 == s2, "Copy of sequence is equal");
        s1.push_back(1);
        QVERIFY2(s1 != s2, "Different sequences are not equal");
        QVERIFY2(s1.size() == 1, "Sequence has expected size");
        s2.push_back(1);
        QVERIFY2(s1 == s2, "Sequences with equal elements are equal");
        s1.clear();
        QVERIFY2(s1.empty(), "Cleared sequence is empty");
        s1.push_back(2);
        QVERIFY2(s1 != s2, "Sequences with different elements are not equal");
        s1 = s2;
        QVERIFY2(s1 == s2, "Copied sequence is equal");

        QVERIFY2((s1[0] = 1), "Subscripted element mutation");
        QVERIFY2(s1[0] == 1, "Subscripted element has expected value");
        QVERIFY2(s1.back() == 1, "Last element has expected value");
    }

    void CTestContainers::joinAndSplit()
    {
        CSequence<int> s1, s2;
        s1.push_back(1);
        s1.push_back(2);
        s1.push_back(3);
        s2.push_back(4);
        s2.push_back(5);
        s2.push_back(6);
        auto joined = s1.join(s2);
        s1.push_back(s2);
        QVERIFY2(s1.size() == 6, "Combine sequences");
        QVERIFY2(s1 == joined, "Combine sequences");

        CCollection<int> c1, c2, c3, c4;
        c1.push_back(1);
        c1.push_back(2);
        c1.push_back(3);
        c1.push_back(4);
        c1.push_back(5);
        c1.push_back(6);
        c2.push_back(1);
        c2.push_back(2);
        c2.push_back(3);
        c3.push_back(4);
        c3.push_back(5);
        c3.push_back(6);
        c4.push_back(10);
        c4.push_back(20);
        c4.push_back(30);
        QVERIFY2(c1.makeUnion(c2) == c1, "Combine collections");
        QVERIFY2(c2.makeUnion(c3) == c1, "Combine collections");
        QVERIFY2(c1.intersection(c2) == c2, "Combine collections");
        QVERIFY2(c1.difference(c2) == c3, "Split collections");
        c1.insert(c4);
        QVERIFY2(c1.size() == 9, "Combine collections");
        c1.remove(c4);
        QVERIFY2(c1.size() == 6, "Split collections");
        c1.remove(c2);
        QVERIFY2(c1 == c3, "Split collections");
    }

    void CTestContainers::findTests()
    {
        CCallsignSet callsigns;
        CSequence<CCallsign> found = callsigns.findBy(&CCallsign::asString, "Foo");
        QVERIFY2(found.isEmpty(), "Empty found");
        callsigns.push_back(CCallsign("EDDM_TWR"));
        callsigns.push_back(CCallsign("KLAX_TWR"));
        found = callsigns.findBy(&CCallsign::asString, "KLAX_TWR");
        QVERIFY2(found.size() == 1, "found");
    }

    void CTestContainers::sortTests()
    {
        struct Person
        {
            const QString &getName() const { return name; }
            int getAge() const { return age; }
            bool operator==(const Person &other) const { return name == other.name && age == other.age; }
            QString name;
            int age;
        };
        CSequence<Person> list
        {
            { "Alice", 33 },
            { "Bob", 32 },
            { "Cathy", 32 },
            { "Dave", 31 },
            { "Emily", 31 }
        };
        CSequence<Person> sorted
        {
            { "Dave", 31 },
            { "Emily", 31 },
            { "Bob", 32 },
            { "Cathy", 32 },
            { "Alice", 33 }
        };
        QVERIFY2(list.sortedBy(&Person::getAge, &Person::getName) == sorted, "sort by multiple members");
    }

    void CTestContainers::removeTests()
    {
        const CSequence<int> base { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        const CSequence<CSequence<int>> subsets
        {
            {}, { 1 }, { 9 }, { 5 }, { 1, 9 }, { 1, 5 }, { 5, 9 }, { 1, 2 },
            { 8, 9 }, { 4, 5, 6 }, { 1, 5, 9 }, { 3, 7 }, { 3, 5, 7 }, base
        };
        for (const auto &subset : subsets)
        {
            auto copy1 = base, copy2 = base;
            copy1.removeIfIn(subset);
            copy2.removeIfInSubset(subset);
            QVERIFY2(copy1 == copy2, "removeIfInSubset");
        }
    }

    void CTestContainers::dictionaryBasics()
    {
        CTestValueObject key1("Key1", "This is key object 1");
        CTestValueObject key2("Key2", "This is key object 2");

        CTestValueObject value1("Value1", "This is value object 1");
        CTestValueObject value2("Value2", "This is value object 2");

        CValueObjectDictionary d1;
        d1.insert(key1, value1);
        d1.insert(key2, value2);

        CValueObjectDictionary d3;
        d3.insert(key1, value1);
        d3.insert(key2, value2);

        CValueObjectDictionary d2;

        // Operators
        QVERIFY2(d1 != d2, "Inequality operator failed");
        QVERIFY2(d1 == d3, "Equality operator failed");

        // size
        QVERIFY2(d1.size() == 2, "size() wrong");
        QVERIFY2(d1.size() == d1.count(), "size() is different to count()");

        // clear/empty
        d1.clear();
        QVERIFY2(d1.isEmpty(), "clear failed");
        d1.insert(key1, value1);
        d1.insert(key2, value2);

        // keys range
        auto keys = d1.keys();
        QVERIFY2(std::distance(keys.begin(), keys.end()) == 2, "keys range size wrong");

        // keys collection
        CCollection<CTestValueObject> keyCollection = d1.keys();
        QVERIFY2(keyCollection.size() == 2, "keys collection size wrong");

        // keys sequence
        CSequence<CTestValueObject> keySequence = d1.keys();
        QVERIFY2(keySequence.size() == 2, "keys sequence size wrong");

        // findKeyBy
        d2 = d1.findKeyBy(&CTestValueObject::getName, QString("Key1"));
        QVERIFY2(d2.size() == 1, "findKeyBy returned wrong container");
        CTestValueObject o1 = d2.value(key1);
        QVERIFY2(o1.getName() == "Value1", "findKeyBy returned wrong container");

        // findValueBy
        d2 = d1.findValueBy(&CTestValueObject::getName, QString("Value1"));
        QVERIFY2(d2.size() == 1, "findValueBy returned wrong container");
        o1 = d2.value(key1);
        QVERIFY2(o1.getName() == "Value1", "findKeyBy returned wrong container");

        // containsByKey
        QVERIFY2(d1.containsByKey(&CTestValueObject::getName, QString("Key1")), "containsByKey failed");
        QVERIFY2(!d1.containsByKey(&CTestValueObject::getName, QString("Key5")), "containsByKey failed");

        // containsByValue
        QVERIFY2(d1.containsByValue(&CTestValueObject::getName, QString("Value1")), "containsByValue failed");
        QVERIFY2(!d1.containsByValue(&CTestValueObject::getName, QString("Value5")), "containsByValue failed");

        // removeByKeyIf
        d2 = d1;
        d2.removeByKeyIf(&CTestValueObject::getName, "Key2");
        QVERIFY2(d2.size() == 1, "size() wrong");

        // removeByValueIf
        d2 = d1;
        d2.removeByValueIf(&CTestValueObject::getName, "Value2");
        QVERIFY2(d2.size() == 1, "size() wrong");

        // JSON
        QJsonObject jsonObject = d1.toJson();
        CValueObjectDictionary d4;
        d4.convertFromJson(jsonObject);
        QVERIFY2(d1 == d4, "JSON serialization/deserialization failed");
    }

    void CTestContainers::timestampList()
    {
        CAircraftSituationList situations;
        const qint64 ts = QDateTime::currentMSecsSinceEpoch();
        int no = 10;
        for (int i = 0; i < no; ++i)
        {
            CAircraftSituation s;
            s.setCallsign("CS" + QString::number(i));
            s.setMSecsSinceEpoch(ts - 10 * i);
            situations.push_back(s);
        }

        // test sorting
        situations.sortOldestFirst();
        qint64 ms = situations.front().getMSecsSinceEpoch();
        QVERIFY2(ms == ts - 10 * (no - 1), "Oldest value not first");

        situations.sortLatestFirst();
        ms = situations.front().getMSecsSinceEpoch();
        QVERIFY2(ms == ts, "Latest value not first");

        // test shifting
        situations.clear();
        const int maxElements = 8;
        QVERIFY(situations.isEmpty());
        for (int i = 0; i < no; ++i)
        {
            qint64 cTs = ts - 10 * i;
            CAircraftSituation s;
            s.setCallsign("CS" + QString::number(i));
            s.setMSecsSinceEpoch(cTs);
            situations.push_frontMaxElements(s, maxElements);
            if (i > maxElements - 1)
            {
                QVERIFY2(situations.size() == maxElements, "Situations must only contain max.elements");
            }
            else
            {
                QVERIFY2(situations.size() == i + 1, "Element size does not match");
                QVERIFY2(situations.front().getMSecsSinceEpoch() == cTs, "Wrong front element");
            }
        }

        situations.sortLatestFirst();
        QVERIFY2(situations.isSortedLatestFirst(), "Espect sorted latest first");
        no = situations.size();
        for (int i = 0; i < no; ++i)
        {
            const CAircraftSituation current = situations[i];
            const qint64 cTs = current.getMSecsSinceEpoch();
            const CAircraftSituation expectedBefore = situations.findObjectBeforeOrDefault(cTs);
            const CAircraftSituation expectedAfter = situations.findObjectAfterOrDefault(cTs);
            const qint64 beforeTs = expectedBefore.getMSecsSinceEpoch();
            const qint64 afterTs = expectedAfter.getMSecsSinceEpoch();

            if (i > 0)
            {
                const qint64 t1 = situations[i - 1].getMSecsSinceEpoch();
                QVERIFY2(t1 == afterTs, "Wrong expected after");
            }
            if (i < (no - 1))
            {
                const qint64 t1 = situations[i + 1].getMSecsSinceEpoch();
                QVERIFY2(t1 == beforeTs, "Wrong expected before");
            }
        }
    }

    void CTestContainers::offsetTimestampList()
    {
        CAircraftSituationList situations;
        static const CCoordinateGeodetic geoPos = CCoordinateGeodetic::fromWgs84("48° 21′ 13″ N", "11° 47′ 09″ E", { 1487, CLengthUnit::ft() });
        qint64 ts = 1000000;
        int no = 10;
        const int max = 6;

        for (int i = 0; i < no; ++i)
        {
            CAircraftSituation s;
            s.setPosition(geoPos);
            s.setMSecsSinceEpoch(ts);
            s.setCallsign("CS" + QString::number(i));

            if (CMathUtils::randomBool())
            {
                ts += CMathUtils::randomInteger(4500, 5500);
                s.setTimeOffsetMs(6000);
            }
            else
            {
                ts += CMathUtils::randomInteger(900, 1100);
                s.setTimeOffsetMs(2000);
            }

            situations.push_frontKeepLatestFirstAdjustOffset(s, true, max);

            QVERIFY2(situations.size() <= max, "Wrong size");
            QVERIFY2(situations.isSortedAdjustedLatestFirstWithoutNullPositions(), "Wrong sort order");
            QVERIFY2(!situations.hasInvalidTimestamps(), "Missing timestamps");
            QVERIFY2(!situations.containsZeroOrNegativeOffsetTime(), "Missing offset time");
        }

        no = situations.size();
        for (int i = 0; i < no; ++i)
        {
            const CAircraftSituation current = situations[i];
            const qint64 cTs = current.getAdjustedMSecsSinceEpoch();
            const CAircraftSituation expectedBefore = situations.findObjectBeforeAdjustedOrDefault(cTs);
            const CAircraftSituation expectedAfter = situations.findObjectAfterAdjustedOrDefault(cTs);
            const qint64 beforeTs = expectedBefore.getAdjustedMSecsSinceEpoch();
            const qint64 afterTs = expectedAfter.getAdjustedMSecsSinceEpoch();

            if (i > 0)
            {
                const qint64 t1 = situations[i - 1].getAdjustedMSecsSinceEpoch();
                QVERIFY2(t1 == afterTs, "Wrong expected after");
            }
            if (i < (no - 1))
            {
                const qint64 t1 = situations[i + 1].getAdjustedMSecsSinceEpoch();
                QVERIFY2(t1 == beforeTs, "Wrong expected before");
            }
        }
    }
} //namespace

//! main
BLACKTEST_APPLESS_MAIN(BlackMiscTest::CTestContainers);

#include "testcontainers.moc"

//! \endcond
