// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_NETWORK_PING_H
#define SWIFT_MISC_NETWORK_PING_H

#include <QString>
#include <QUrl>

#include "misc/swiftmiscexport.h"

namespace swift::misc::network
{
    //! Can ping the address?
    //! \note uses OS ping
    SWIFT_MISC_EXPORT bool canPing(const QString &hostAddress);

    //! Can ping the address?
    //! \note uses OS ping
    SWIFT_MISC_EXPORT bool canPing(const QUrl &url);
} // namespace swift::misc::network

#endif // SWIFT_MISC_NETWORK_PING_H
