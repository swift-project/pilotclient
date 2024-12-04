// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_NETWORK_DATA_LASTSERVER_H
#define SWIFT_MISC_NETWORK_DATA_LASTSERVER_H

#include "../server.h"

#include "misc/datacache.h"

namespace swift::misc::network::data
{
    //! Trait for last (most recently) used server and user
    struct TLastServer : public TDataTrait<CServer>
    {
        //! \copydoc swift::core::TDataTrait::key
        static const char *key() { return "lastserver"; }

        //! \copydoc swift::core::TDataTrait::isPinned
        static constexpr bool isPinned() { return true; }

        //! \copydoc swift::core::TDataTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Last used server");
            return name;
        }
    };
} // namespace swift::misc::network::data

#endif // SWIFT_MISC_NETWORK_DATA_LASTSERVER_H
