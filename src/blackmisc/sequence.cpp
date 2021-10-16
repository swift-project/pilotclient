/* Copyright (C) 2021
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
