/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "samplesalgorithm.h"
#include "blackmisc/algorithm.h"
#include <QDebug>
#include <QString>

namespace BlackMiscTest
{

    /*
     * Samples
     */
    int CSamplesAlgorithm::samples()
    {
        QStringList src { "a1", "a2", "a3", "b1", "b2", "b3", "c1", "c2", "c3" };
        std::random_shuffle(src.begin(), src.end());
        qDebug() << src;
        qDebug() << "topologicallySortedInsert";
        QStringList dst;
        int count = 0;
        auto cmp = [ & ](const QString &a, const QString &b) { count++; return a[0] == b[0] && a[1] < b[1]; };
        for (const auto &s : src) { BlackMisc::topologicallySortedInsert(dst, s, cmp); }
        qDebug() << count << "comparisons";
        qDebug() << dst;
        qDebug() << "topologicalSort";
        count = 0;
        BlackMisc::topologicalSort(dst.begin(), dst.end(), cmp);
        qDebug() << count << "comparisons";
        qDebug() << dst;

        return 0;
    }

} // namespace
