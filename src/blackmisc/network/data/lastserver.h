// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_NETWORK_DATA_LASTSERVER_H
#define BLACKMISC_NETWORK_DATA_LASTSERVER_H

#include "../server.h"
#include "blackmisc/datacache.h"

namespace BlackMisc::Network::Data
{
    //! Trait for last (most recently) used server and user
    struct TLastServer : public TDataTrait<CServer>
    {
        //! \copydoc BlackCore::TDataTrait::key
        static const char *key() { return "lastserver"; }

        //! \copydoc BlackCore::TDataTrait::isPinned
        static constexpr bool isPinned() { return true; }

        //! \copydoc BlackCore::TDataTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Last used server");
            return name;
        }
    };
} // ns

#endif // guard
