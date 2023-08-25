// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

/*                                         ---------
 *                                        ! WARNING !
 *      ---------------------------------------------------------------------------------
 * >>>> CHANGES IN ANY HEADERS INCLUDED HEREIN WILL TRIGGER A FULL REBUILD OF EVERYTHING! <<<<
 *      ---------------------------------------------------------------------------------
 */

#ifdef BLACKCORE_PCH_H
#    error "Don't #include this file"
#else
#    define BLACKCORE_PCH_H

#    include "blackmisc/pch/pch.h"

#    ifdef __cplusplus

#        include <QDBusAbstractInterface>
#        include <QDBusConnection>
#        include <QDBusConnectionInterface>
#        include <QDBusError>
#        include <QDBusMessage>
#        include <QDBusPendingCall>
#        include <QDBusPendingReply>
#        include <QDBusReply>
#        include <QDBusServer>
#        include <QDBusServiceWatcher>
#        include <QNetworkAccessManager>
#        include <QNetworkCookieJar>
#        include <QNetworkInterface>
#        include <QNetworkReply>
#        include <QNetworkRequest>
#        include <QUrl>

#    endif // __cplusplus
#endif // guard
