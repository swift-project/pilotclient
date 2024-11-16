// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file
//! \ingroup samplemisc

#include "samplesalgorithm.h"

#include <algorithm>
#include <cmath>
#include <ctime>
#include <numeric>

#include <QChar>
#include <QDebug>
#include <QList>
#include <QString>
#include <QStringList>
#include <QTypeInfo>
#include <QtDebug>

#include "misc/algorithm.h"
#include "misc/iterator.h"
#include "misc/sequence.h"
#include "misc/stringutils.h"

namespace swift::sample
{
    int CSamplesAlgorithm::samples()
    {
        swift::misc::CSequence<int> seq;
        for (int i = 1; i <= 100; ++i)
        {
            seq.push_back(i);
        }
        const int samples = 200;
        swift::misc::CSequence<int> means;
        {
            for (int i = 0; i < samples; ++i)
            {
                auto randoms = seq.randomElements(10);
                means.push_back(std::accumulate(randoms.cbegin(), randoms.cend(), 0) / 10);
            }
            int mean = std::accumulate(means.cbegin(), means.cend(), 0) / samples;
            int stdDev = std::sqrt(std::accumulate(means.cbegin(), means.cend(), 0, [&](int a, int n) { return a + (n - mean) * (n - mean); }) / samples);
            qDebug() << "randomElements";
            qDebug() << "means:" << means;
            qDebug() << "mean of the means:" << mean;
            qDebug() << "std deviation of the means:" << stdDev;
        }
        means.clear();
        {
            for (int i = 0; i < samples; ++i)
            {
                auto randoms = seq.sampleElements(10);
                means.push_back(std::accumulate(randoms.cbegin(), randoms.cend(), 0) / 10);
            }
            int mean = std::accumulate(means.cbegin(), means.cend(), 0) / samples;
            int stdDev = std::sqrt(std::accumulate(means.cbegin(), means.cend(), 0, [&](int a, int n) { return a + (n - mean) * (n - mean); }) / samples);
            qDebug() << "sampleElements";
            qDebug() << "means:" << means;
            qDebug() << "mean of the means:" << mean;
            qDebug() << "std deviation of the means:" << stdDev;
        }

        QStringList src { "a1", "a2", "a3", "b1", "b2", "b3", "c1", "c2", "c3" };
        std::shuffle(src.begin(), src.end(), std::mt19937(static_cast<unsigned>(std::time(nullptr))));
        qDebug() << src;
        qDebug() << "topologicallySortedInsert";
        QStringList dst;
        int count = 0;
        auto cmp = [&](const QString &a, const QString &b) { count++; return a[0] == b[0] && a[1] < b[1]; };
        for (const auto &s : src) { swift::misc::topologicallySortedInsert(dst, s, cmp); }
        qDebug() << count << "comparisons";
        qDebug() << dst;
        qDebug() << "topologicalSort";
        count = 0;
        swift::misc::topologicalSort(dst.begin(), dst.end(), cmp);
        qDebug() << count << "comparisons";
        qDebug() << dst;

        return 0;
    }

} // namespace swift::sample
