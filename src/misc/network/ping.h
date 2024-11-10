// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_NETWORK_PING_H
#define SWIFT_MISC_NETWORK_PING_H

#include "misc/swiftmiscexport.h"
#include <QString>
#include <QUrl>

namespace swift::misc::network
{
    //! Can ping the address?
    //! \note uses OS ping
    SWIFT_MISC_EXPORT bool canPing(const QString &hostAddress);

    //! Can ping the address?
    //! \note uses OS ping
    SWIFT_MISC_EXPORT bool canPing(const QUrl &url);
}

#endif
