/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_NETWORK_PING_H
#define BLACKMISC_NETWORK_PING_H

#include "blackmisc/blackmiscexport.h"
#include <QString>
#include <QUrl>

namespace BlackMisc
{
    namespace Network
    {
        //! Can ping the address?
        //! \note uses OS ping
        BLACKMISC_EXPORT bool canPing(const QString &hostAddress);

        //! Can ping the address?
        //! \note uses OS ping
        BLACKMISC_EXPORT bool canPing(const QUrl &url);
    }
}

#endif
