/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
