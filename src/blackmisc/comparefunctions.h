// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_COMPAREFUNCTIONS_H
#define BLACKMISC_COMPAREFUNCTIONS_H

#include <QFlags>
#include <QDateTime>
#include <type_traits>

namespace BlackMisc::Compare
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

#endif
