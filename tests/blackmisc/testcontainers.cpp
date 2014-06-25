/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "testcontainers.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include "blackmisc/avcallsignlist.h"
#include <QList>
#include <QVector>
#include <QSet>
#include <vector>
#include <set>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;

namespace BlackMiscTest
{

    void CTestContainers::collectionBasics()
    {
        CCollection<int> c1;
        QVERIFY2(c1.empty(), "Uninitialized collection is empty");
        auto c2 = CCollection<int>::fromImpl(QSet<int>());
        QVERIFY2(c1 == c2, "Uninitialized and empty collections are equal");
        c1.changeImpl(std::set<int>());
        QVERIFY2(c1 == c2, "Two empty collections are equal");
        c1.insert(1);
        QVERIFY2(c1 != c2, "Different collections are not equal");
        QVERIFY2(c1.size() == 1, "Collection has expected size");
        c2.insert(1);
        QVERIFY2(c1 == c2, "Collections with equal elements are equal");
        c1.changeImpl(QSet<int>());
        QVERIFY2(c1 == c2, "Collection stays equal after changing implementation");
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
        auto s2 = CSequence<int>::fromImpl(QList<int>());
        QVERIFY2(s1 == s2, "Uninitialized and empty sequence are equal");
        s1.changeImpl(std::vector<int>());
        QVERIFY2(s1 == s2, "Two empty sequences are equal");
        s1.push_back(1);
        QVERIFY2(s1 != s2, "Different sequences are not equal");
        QVERIFY2(s1.size() == 1, "Sequence has expected size");
        s2.push_back(1);
        QVERIFY2(s1 == s2, "Sequences with equal elements are equal");
        s1.changeImpl(QVector<int>());
        QVERIFY2(s1 == s2, "Sequence stays equal after changing implementation");
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
        BlackMisc::registerMetadata();
        CCallsignList callsigns;
        CSequence<CCallsign> found = callsigns.findBy(&CCallsign::asString, "Foo");
        QVERIFY2(found.isEmpty(), "Empty found");
        callsigns.push_back(CCallsign("EDDM_TWR"));
        callsigns.push_back(CCallsign("KLAX_TWR"));
        found = callsigns.findBy(&CCallsign::asString, "KLAXTWR");
        QVERIFY2(found.size() == 1, "found");
    }

} //namespace BlackMiscTest
