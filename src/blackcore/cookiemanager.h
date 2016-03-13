/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_COOKIEMANAGER_H
#define BLACKCORE_COOKIEMANAGER_H

#include "blackcore/blackcoreexport.h"
#include <QNetworkCookieJar>
#include <QNetworkCookie>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QReadWriteLock>

namespace BlackCore
{
    class CApplication;

    /*!
     * Cookie manager, which allows thread safe sharing of cookies
     */
    class BLACKCORE_EXPORT CCookieManager : public QNetworkCookieJar
    {
        Q_OBJECT

        friend class CApplication;

    public:
        //! \copydoc QNetworkCookieJar::setCookiesFromUrl
        //! \threadsafe
        virtual bool setCookiesFromUrl(const QList<QNetworkCookie> &cookies, const QUrl &url) override;

        //! \copydoc QNetworkCookieJar::cookiesForUrl
        //! \threadsafe
        virtual QList<QNetworkCookie> cookiesForUrl(const QUrl &url) const override;

        //! Cookies for request
        //! \threadsafe
        QList<QNetworkCookie> cookiesForRequest(const QNetworkRequest &request) const;

        //! \copydoc QNetworkCookieJar::deleteCookie
        //! \threadsafe
        virtual bool deleteCookie(const QNetworkCookie &cookie) override;

        //! Delete all cookies
        //! \threadsafe
        void deleteAllCookies();

        //! \copydoc QNetworkCookieJar::insertCookie
        //! \threadsafe
        virtual bool insertCookie(const QNetworkCookie &cookie) override;

        //! \copydoc QNetworkCookieJar::updateCookie
        //! \threadsafe
        virtual bool updateCookie(const QNetworkCookie &cookie) override;

    private:
        //! Constructor
        CCookieManager(QObject *parent = nullptr);

        mutable QReadWriteLock m_lock { QReadWriteLock::Recursive };
    };

} // ns

#endif // guard
