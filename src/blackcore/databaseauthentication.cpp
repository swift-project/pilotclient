/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "databaseauthentication.h"
#include "blackmisc/network/networkutils.h"
#include "blackmisc/network/url.h"
#include "blackmisc/logmessage.h"

#include <QString>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QHttpPart>
#include <QHttpMultiPart>

using namespace BlackMisc;
using namespace BlackMisc::Network;

namespace BlackCore
{
    CDatabaseAuthenticationService::CDatabaseAuthenticationService(QObject *parent) :
        QObject(parent),
        m_networkManager(new QNetworkAccessManager(this))
    {
        this->connect(this->m_networkManager, &QNetworkAccessManager::finished, this, &CDatabaseAuthenticationService::ps_parseServerResponse);
    }

    const Network::CAuthenticatedUser &CDatabaseAuthenticationService::getUser() const
    {
        return m_user;
    }

    void CDatabaseAuthenticationService::gracefulShutdown()
    {
        if (this->m_shutdown) { return; }
        this->m_shutdown = true;
        this->logoff();
    }

    BlackMisc::CStatusMessageList CDatabaseAuthenticationService::login(const QString &username, const QString &password)
    {
        CStatusMessageList msgs;
        static const CLogCategoryList cats(CLogCategoryList(this).join({ CLogCategory::validation()}));

        if (this->m_shutdown) { msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, "Shutdown in progress")); return msgs; }

        QString un(username.trimmed());
        QString pw(password.trimmed());
        if (un.isEmpty()) { msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, "No user name/id")); }
        if (pw.isEmpty()) { msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, "No password")); }
        if (!msgs.isEmpty()) { return msgs; }

        CUrl url(this->m_setup.get().dbLoginService());
        QString msg;
        if (!CNetworkUtils::canConnect(url, msg))
        {
            msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, msg));
            return msgs;
        }

        QUrlQuery params;
        params.addQueryItem("username", un);
        params.addQueryItem("password", pw);
        if (m_setup.get().dbDebugFlag()) { CNetworkUtils::addDebugFlag(params); }

        QString query = params.toString();
        QNetworkRequest request(CNetworkUtils::getNetworkRequest(url, CNetworkUtils::PostUrlEncoded));
        QNetworkReply *r = this->m_networkManager->post(request, query.toUtf8());
        if (!r)
        {
            QString rm("Cannot send request to authentication server %1");
            msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, rm.arg(url.toQString())));
        }
        else
        {
            QString rm("Sent request to authentication server %1");
            msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityInfo, rm.arg(url.toQString())));
        }
        return msgs;
    }

    void CDatabaseAuthenticationService::logoff()
    {
        CUrl url(this->m_setup.get().dbLoginService());
        url.setQuery("logoff=true");
        QNetworkRequest request(CNetworkUtils::getNetworkRequest(url));
        this->m_networkManager->get(request);
    }

    void CDatabaseAuthenticationService::ps_parseServerResponse(QNetworkReply *nwReplyPtr)
    {
        // always cleanup reply
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);

        if (this->m_shutdown) { return; }
        QString urlString(nwReply->url().toString());
        if (urlString.toLower().contains("logoff"))
        {
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
            emit userAuthenticationFinished(user, msgs);
        }
        else
        {
            CLogMessage(this).error("Authentication failed, %1") << nwReply->errorString();
            return;
        }
    }

} // namespace
