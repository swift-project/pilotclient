/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "databaseauthentication.h"
#include "blackcore/application.h"
#include "blackmisc/network/networkutils.h"
#include "blackmisc/network/url.h"
#include "blackmisc/logmessage.h"

#include <QString>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QHttpPart>
#include <QHttpMultiPart>
#include <QCoreApplication>

using namespace BlackMisc;
using namespace BlackMisc::Network;

namespace BlackCore
{
    CDatabaseAuthenticationService::CDatabaseAuthenticationService(QObject *parent) :
        QObject(parent)
    {
        // void
    }

    void CDatabaseAuthenticationService::gracefulShutdown()
    {
        if (this->m_shutdown) { return; }
        this->m_shutdown = true;
        this->logoff();
    }

    CStatusMessageList CDatabaseAuthenticationService::login(const QString &username, const QString &password)
    {
        CStatusMessageList msgs;
        static const CLogCategoryList cats(CLogCategoryList(this).join({ CLogCategory::validation()}));

        if (this->m_shutdown) { msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, "Shutdown in progress")); return msgs; }

        QString un(username.trimmed());
        QString pw(password.trimmed());
        if (un.isEmpty()) { msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, "No user name/id")); }
        if (pw.isEmpty()) { msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, "No password")); }
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

        QString query = params.toString();
        const QNetworkRequest request(CNetworkUtils::getNetworkRequest(url, CNetworkUtils::PostUrlEncoded));
        sApp->postToNetwork(request, query.toUtf8(), { this, &CDatabaseAuthenticationService::ps_parseServerResponse});
        QString rm("Sent request to authentication server %1");
        msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityInfo, rm.arg(url.toQString())));
        return msgs;
    }

    void CDatabaseAuthenticationService::logoff()
    {
        CUrl url(sApp->getGlobalSetup().getDbLoginServiceUrl());
        url.setQuery("logoff=true");
        QNetworkRequest request(CNetworkUtils::getNetworkRequest(url));
        sApp->getFromNetwork(request, { this, &CDatabaseAuthenticationService::ps_parseServerResponse });
        this->m_swiftDbUser.set(CAuthenticatedUser());
    }

    void CDatabaseAuthenticationService::ps_parseServerResponse(QNetworkReply *nwReplyPtr)
    {
        // always cleanup reply
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);

        if (this->m_shutdown) { return; }
        QString urlString(nwReply->url().toString());
        if (urlString.toLower().contains("logoff"))
        {
            sApp->deleteAllCookies();
            emit logoffFinished();
            return;
        }

        if (nwReply->error() == QNetworkReply::NoError)
        {
            QString json(nwReply->readAll());
            if (json.isEmpty())
            {
                CLogMessage(this).error("Authentication failed, no response from %1") << urlString;
                return;
            }
            QJsonObject jsonObj(Json::jsonObjectFromString(json));
            CAuthenticatedUser user(CAuthenticatedUser::fromDatabaseJson(jsonObj));

            CStatusMessageList msgs;
            static const CLogCategoryList cats(CLogCategoryList(this).join({ CLogCategory::validation()}));

            if (!user.isAuthenticated() || !user.isValid())
            {
                msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, "Cannot login, user or password wrong"));
            }
            else
            {
                if (!user.isEnabled())
                {
                    msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, "User is disabled"));
                }
                if (user.getRoles().isEmpty())
                {
                    msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, "User has no roles"));
                }
            }
            this->m_swiftDbUser.set(user);
            emit userAuthenticationFinished(user, msgs);
        }
        else
        {
            CLogMessage(this).error("Authentication failed, %1") << nwReply->errorString();
            return;
        }
    }

    void CDatabaseAuthenticationService::ps_userChanged()
    {
        // code goes here
    }

} // namespace
