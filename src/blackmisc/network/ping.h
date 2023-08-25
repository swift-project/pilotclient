// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_NETWORK_PING_H
#define BLACKMISC_NETWORK_PING_H

#include "blackmisc/blackmiscexport.h"
#include <QString>
#include <QUrl>

namespace BlackMisc::Network
{
    //! Can ping the address?
    //! \note uses OS ping
    BLACKMISC_EXPORT bool canPing(const QString &hostAddress);

    //! Can ping the address?
    //! \note uses OS ping
    BLACKMISC_EXPORT bool canPing(const QUrl &url);
}

#endif
