/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
