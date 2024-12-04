// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_COOKIEMANAGER_H
#define SWIFT_CORE_COOKIEMANAGER_H

#include <QList>
#include <QNetworkCookieJar>
#include <QObject>
#include <QReadWriteLock>

#include "core/swiftcoreexport.h"

class QNetworkCookie;
class QUrl;

namespace swift::core
{
    /*!
     * Threadsafe version of QNetworkCookieJar
     */
    class SWIFT_CORE_EXPORT CCookieManager : public QNetworkCookieJar
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CCookieManager(QObject *parent = nullptr);

        //! \copydoc QNetworkCookieJar::setCookiesFromUrl
        //! \threadsafe
        bool setCookiesFromUrl(const QList<QNetworkCookie> &cookies, const QUrl &url) override;

        //! \copydoc QNetworkCookieJar::cookiesForUrl
        //! \threadsafe
        QList<QNetworkCookie> cookiesForUrl(const QUrl &url) const override;

        //! \copydoc QNetworkCookieJar::deleteCookie
        //! \threadsafe
        bool deleteCookie(const QNetworkCookie &cookie) override;

        //! Delete all cookies
        //! \threadsafe
        void deleteAllCookies();

        //! \copydoc QNetworkCookieJar::insertCookie
        //! \threadsafe
        bool insertCookie(const QNetworkCookie &cookie) override;

        //! \copydoc QNetworkCookieJar::updateCookie
        //! \threadsafe
        bool updateCookie(const QNetworkCookie &cookie) override;

    private:
        mutable QReadWriteLock m_lock { QReadWriteLock::Recursive };
    };

} // namespace swift::core

#endif // SWIFT_CORE_COOKIEMANAGER_H
