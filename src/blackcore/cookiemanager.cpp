/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/cookiemanager.h"
#include "blackmisc/threadutils.h"
#include <QCoreApplication>
#include <QNetworkCookieJar>
#include <QReadLocker>
#include <QWriteLocker>

using namespace BlackMisc;

namespace BlackCore
{
    CCookieManager::CCookieManager(QObject *parent) : QNetworkCookieJar(parent)
    {
        // code
    }

    bool CCookieManager::setCookiesFromUrl(const QList<QNetworkCookie> &cookies, const QUrl &url)
    {
        QWriteLocker l(&m_lock);
        return QNetworkCookieJar::setCookiesFromUrl(cookies, url);
    }

    QList<QNetworkCookie> CCookieManager::cookiesForUrl(const QUrl &url) const
    {
        QReadLocker l(&m_lock);
        const QList<QNetworkCookie> cookies(QNetworkCookieJar::cookiesForUrl(url));
        return cookies;
    }

    QList<QNetworkCookie> CCookieManager::cookiesForRequest(const QNetworkRequest &request) const
    {
        return cookiesForUrl(request.url());
    }

    bool CCookieManager::deleteCookie(const QNetworkCookie &cookie)
    {
        QWriteLocker l(&m_lock);
        return QNetworkCookieJar::deleteCookie(cookie);
    }

    bool CCookieManager::insertCookie(const QNetworkCookie &cookie)
    {
        QWriteLocker l(&m_lock);
        return QNetworkCookieJar::insertCookie(cookie);
    }

    bool CCookieManager::updateCookie(const QNetworkCookie &cookie)
    {
        QWriteLocker l(&m_lock);
        return QNetworkCookieJar::updateCookie(cookie);
    }

    void CCookieManager::deleteAllCookies()
    {
        QWriteLocker l(&m_lock);
        this->setAllCookies(QList<QNetworkCookie>());
    }

} // ns
