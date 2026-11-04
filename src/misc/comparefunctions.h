// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_COMPAREFUNCTIONS_H
#define SWIFT_MISC_COMPAREFUNCTIONS_H

#include <type_traits>

#include <QDateTime>
#include <QFlags>

namespace swift::misc::Compare
{
    //! Compare arithmetic values
    template <typename T>
    int compare(T a, T b) requires(std::is_arithmetic_v<T>)
    {
        if (a < b)
        {
            return -1;
        }
        if (b < a)
        {
            return 1;
        }
        return 0;
    }

    //! Compare enumerators
    template <typename T>
    int compare(T a, T b) requires(std::is_enum_v<T>)
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
} // namespace swift::misc::Compare

#endif // SWIFT_MISC_COMPAREFUNCTIONS_H
