// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/cookiemanager.h"

#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QNetworkRequest>
#include <QReadLocker>
#include <QWriteLocker>

namespace swift::core
{
    CCookieManager::CCookieManager(QObject *parent) : QNetworkCookieJar(parent) {}

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

} // namespace swift::core
