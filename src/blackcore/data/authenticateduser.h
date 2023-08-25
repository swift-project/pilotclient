// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_DATA_AUTHENTICATEDUSER_H
#define BLACKCORE_DATA_AUTHENTICATEDUSER_H

#include "blackcore/blackcoreexport.h"
#include "blackmisc/datacache.h"
#include "blackmisc/network/authenticateduser.h"

namespace BlackCore::Data
{
    //! Trait for for global cached DB user.
    //! This user has authenticated with the swift DB.
    struct TAuthenticatedDbUser : public BlackMisc::TDataTrait<BlackMisc::Network::CAuthenticatedUser>
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
} // ns

#endif // guard
