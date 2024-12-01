// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_DATA_AUTHENTICATEDUSER_H
#define SWIFT_CORE_DATA_AUTHENTICATEDUSER_H

#include "misc/datacache.h"
#include "misc/network/authenticateduser.h"

namespace swift::core::data
{
    //! Trait for for global cached DB user.
    //! This user has authenticated with the swift DB.
    struct TAuthenticatedDbUser : public swift::misc::TDataTrait<swift::misc::network::CAuthenticatedUser>
    {
        //! Key in data cache
        static const char *key() { return "dbuser"; }

        //! First load is synchronous
        static constexpr bool isPinned() { return true; }

        //! Session object
        static constexpr bool isSession() { return true; }

        //! Cache lifetime
        static int timeToLive() { return 18 * 60 * 60 * 1000; }
    };
} // namespace swift::core::data

#endif // guard
