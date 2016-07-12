/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "comparefunctions.h"

namespace BlackMisc
{
    namespace Compare
    {
        int compare(bool a, bool b)
        {
            if ((a && b) || (!a && !b)) return 0;
            if (a && !b) return 10;
            return -10;
        }

        int compare(int a, int b)
        {
            if (a == b) return 0;
            return a < b ? -10 : 10;
        }

        int compare(qint64 a, qint64 b)
        {
            if (a == b) return 0;
            return a < b ? -10 : 10;
        }

        int compare(double a, double b)
        {
            if (a == b) return 0;
            return a < b ? -10 : 10;
        }

        int compare(const QDateTime &a, const QDateTime &b)
        {
            return Compare::compare(a.toMSecsSinceEpoch(), b.toMSecsSinceEpoch());
        }
    } // ns
} // ns
