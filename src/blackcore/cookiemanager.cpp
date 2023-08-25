// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackcore/cookiemanager.h"
#include "blackmisc/restricted.h"

#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QNetworkRequest>
#include <QReadLocker>
#include <QWriteLocker>

using namespace BlackMisc;

namespace BlackCore
{
    CCookieManager::CCookieManager(BlackMisc::Restricted<CApplication>, QObject *parent) : QNetworkCookieJar(parent)
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
