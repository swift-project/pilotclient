/* Copyright (C) 2015
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_DATA_AUTHENTICATEDUSER_H
#define BLACKCORE_DATA_AUTHENTICATEDUSER_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/datacache.h"
#include "blackmisc/network/authenticateduser.h"

namespace BlackCore
{
    namespace Data
    {
        //! Trait for for global setup data
        struct AuthenticatedUser : public BlackCore::CDataTrait<BlackMisc::Network::CAuthenticatedUser>
        {
            //! Key in data cache
            static const char *key() { return "readers/global/user"; }

            //! Default value
            static const BlackMisc::Network::CAuthenticatedUser &defaultValue()
            {
                static const BlackMisc::Network::CAuthenticatedUser user;
                return user;
            }
        };

    } // ns
} // ns

#endif // guard
