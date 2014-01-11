/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "testcontainers.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include <QList>
#include <QString>
#include <vector>

using namespace BlackMisc;

namespace BlackMiscTest
{

    void CTestContainers::collectionBasics()
    {
        CCollection<int> c1;
        QVERIFY2(c1.empty(), "Uninitialized collection is empty");
        auto c2 = CCollection<int>::fromImpl(QList<int>());
        QVERIFY2(c1 == c2, "Uninitialized and empty collections are equal");
        c1.changeImpl(std::vector<int>());
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

        QVERIFY2(s1[0] = 1, "Subscripted element mutation");
        QVERIFY2(s1[0] == 1, "Subscripted element has expected value");
        QVERIFY2(s1.back() == 1, "Last element has expected value");
    }

} //namespace BlackMiscTest