// SPDX-FileCopyrightText: Copyright (C) 2022 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_MATH_CONSTANTS_H
#define SWIFT_MISC_MATH_CONSTANTS_H

#include <numbers>

namespace swift::misc::math
{
    //! PI
    constexpr double c_pi = std::numbers::pi;

    //! PI / 2
    constexpr double c_pihalf = c_pi / 2;

    //! PI * 2
    constexpr double c_pi2 = c_pi * 2;

} // namespace swift::misc::math

#endif // SWIFT_MISC_MATH_CONSTANTS_H
