/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_COMPAREFUNCTIONS_H
#define BLACKMISC_COMPAREFUNCTIONS_H

#include <QFlags>
#include <QDateTime>
#include <type_traits>

namespace BlackMisc
{
    namespace Compare
    {
        //! Compare arithmetic values
        template <typename T, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
        int compare(T a, T b)
        {
            if (a < b) { return -1; }
            if (b < a) { return 1; }
            return 0;
        }

        //! Compare enumerators
        template <typename T, std::enable_if_t<std::is_enum_v<T>, int> = 0>
        int compare(T a, T b)
        {
            using UT = std::underlying_type_t<T>;
            return compare(static_cast<UT>(a), static_cast<UT>(b));
        }

        //! Compare QFlags
        template <typename T>
        int compare(QFlags<T> a, QFlags<T> b)
        {
            using UT = typename QFlags<T>::Int;
            return compare(static_cast<UT>(a), static_cast<UT>(b));
        }

        //! Compare dates
        inline int compare(const QDateTime &a, const QDateTime &b)
        {
            return compare(a.toMSecsSinceEpoch(), b.toMSecsSinceEpoch());
        }
    } // ns
} // ns

#endif
