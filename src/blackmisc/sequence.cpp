// SPDX-FileCopyrightText: Copyright (C) 2021 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#include "blackmisc/sequence.h"
#include <numeric>

namespace BlackMisc::Private
{
    QVector<int> findIndices(int size, const std::function<bool(int)> &predicate)
    {
        QVector<int> result(size);
        std::iota(result.begin(), result.end(), 0);
        result.erase(std::remove_if(result.begin(), result.end(), std::not_fn(predicate)), result.end());
        return result;
    }

    QVector<int> sortIndices(int size, const std::function<bool(int, int)> &cmp)
    {
        QVector<int> result(size);
        std::iota(result.begin(), result.end(), 0);
        std::sort(result.begin(), result.end(), cmp);
        return result;
    }
}
