// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/db/databaseauthentication.h"

#include <QByteArray>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QScopedPointer>
#include <QScopedPointerDeleteLater>
#include <QString>
#include <QUrl>
#include <QUrlQuery>

#include "core/application.h"
#include "core/data/globalsetup.h"
#include "misc/json.h"
#include "misc/logcategories.h"
#include "misc/logmessage.h"
#include "misc/network/authenticateduser.h"
#include "misc/network/networkutils.h"
#include "misc/network/rolelist.h"
#include "misc/network/url.h"
#include "misc/statusmessage.h"

using namespace swift::misc;
using namespace swift::misc::network;

namespace swift::core::db
{
    const QStringList &CDatabaseAuthenticationService::getLogCategories()
    {
        static const QStringList cats { CLogCategories::swiftDbWebservice() };
        return cats;
    }

    CDatabaseAuthenticationService::CDatabaseAuthenticationService(QObject *parent) : QObject(parent)
    {
        // void
    }

    void CDatabaseAuthenticationService::gracefulShutdown()
    {
        if (m_shutdown) { return; }
        m_shutdown = true;
        this->logoff();
    }

    CAuthenticatedUser CDatabaseAuthenticationService::getDbUser() const { return m_swiftDbUser.get(); }

    bool CDatabaseAuthenticationService::isUserAuthenticated() const
    {
        const CAuthenticatedUser user(this->getDbUser());
        return user.isAuthenticated();
    }

    CStatusMessageList CDatabaseAuthenticationService::login(const QString &username, const QString &password)
    {
        CStatusMessageList msgs;
        static const CLogCategoryList cats(CLogCategoryList(this).withValidation());

        if (!sApp || m_shutdown)
        {
            msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, u"Shutdown in progress"));
            return msgs;
        }

        const QString un(username.trimmed());
        const QString pw(password.trimmed());
        if (un.isEmpty()) { msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, u"No user name/id")); }
        if (pw.isEmpty()) { msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, u"No password")); }
        if (!msgs.isEmpty()) { return msgs; }

        const CUrl url(sApp->getGlobalSetup().getDbLoginServiceUrl());
        QString msg;
        if (!CNetworkUtils::canConnect(url, msg))
        {
            msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, msg));
            return msgs;
        }

        QUrlQuery params;
        params.addQueryItem("username", un);
        params.addQueryItem("password", pw);
        if (sApp->getGlobalSetup().dbDebugFlag()) { CNetworkUtils::addDebugFlag(params); }

        const QString query = params.toString();
        const QNetworkRequest request(CNetworkUtils::getSwiftNetworkRequest(url, CNetworkUtils::PostUrlEncoded,
                                                                            sApp->getApplicationNameAndVersion()));
        sApp->postToNetwork(request, CApplication::NoLogRequestId, query.toUtf8(),
                            { this, &CDatabaseAuthenticationService::parseServerResponse });
        static const QString rm("Sent request to authentication server '%1'");
        msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityInfo, rm.arg(url.toQString())));
        return msgs;
    }

    void CDatabaseAuthenticationService::logoff()
    {
        if (!sApp) { return; }
        CUrl url(sApp->getGlobalSetup().getDbLoginServiceUrl());
        url.setQuery("logoff=true");
        QNetworkRequest request(CNetworkUtils::getSwiftNetworkRequest(url));
        sApp->getFromNetwork(request, { this, &CDatabaseAuthenticationService::parseServerResponse });
        m_swiftDbUser.set(CAuthenticatedUser());
    }

    void CDatabaseAuthenticationService::parseServerResponse(QNetworkReply *nwReplyPtr)
    {
        // always cleanup reply
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);

        if (m_shutdown) { return; }
        QString urlString(nwReply->url().toString());
        if (urlString.contains("logoff", Qt::CaseInsensitive))
        {
            sApp->deleteAllCookies();
            emit this->logoffFinished();
            return;
        }

        if (nwReply->error() == QNetworkReply::NoError)
        {
            const QString json(nwReply->readAll().trimmed());
            if (json.isEmpty())
            {
                CLogMessage(this).error(u"Authentication failed, no response from '%1'") << urlString;
                return;
            }
            if (!json::looksLikeJson(json))
            {
                CLogMessage(this).error(u"Illegal JSON object: %1")
                    << CNetworkUtils::removeHtmlPartsFromPhpErrorMessage(json);
                return;
            }

            static const CLogCategoryList cats(CLogCategoryList(this).withValidation());
            const QJsonObject jsonObj(json::jsonObjectFromString(json));
            CAuthenticatedUser user =
                CAuthenticatedUser::fromDatabaseJson(jsonObj.contains("user") ? jsonObj["user"].toObject() : jsonObj);
            CStatusMessageList msgs;
            if (jsonObj.contains("messages"))
            {
                msgs = CStatusMessageList::fromDatabaseJson(jsonObj["messages"].toArray());
                msgs.setCategories(cats);
            }

            // allow auto enabled for SSO users
            if (user.isValid() && !user.isEnabled())
            {
                if (user.getRoles().hasRole("VATSIMUSER"))
                {
                    user.setEnabled(true);
                    msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityInfo, u"Auto enabled SSO user"));
                }
            }

            if (!user.isAuthenticated() || !user.isValid())
            {
                if (!msgs.hasErrorMessages())
                {
                    msgs.push_back(
                        CStatusMessage(cats, CStatusMessage::SeverityError, u"Cannot login, user or password wrong"));
                }
            }
            else
            {
                if (!user.isEnabled())
                {
                    msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, u"User is disabled"));
                }
                if (user.getRoles().isEmpty())
                {
                    msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, u"User has no roles"));
                }
            }
            m_swiftDbUser.set(user);
            emit userAuthenticationFinished(user, msgs);
        }
        else
        {
            CLogMessage(this).error(u"Authentication failed, %1") << nwReply->errorString();
            return;
        }
    }

    void CDatabaseAuthenticationService::userChanged()
    {
        // this->logoff();
    }
} // namespace swift::core::db
