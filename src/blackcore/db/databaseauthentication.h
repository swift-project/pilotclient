// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_DATABASE_CDATABASEUATHENTICATIONSERVICE_H
#define BLACKCORE_DATABASE_CDATABASEUATHENTICATIONSERVICE_H

#include "blackcore/data/authenticateduser.h"
#include "blackcore/blackcoreexport.h"
#include "blackmisc/datacache.h"
#include "blackmisc/logcategories.h"
#include "blackmisc/statusmessagelist.h"

#include <QObject>
#include <QString>

class QNetworkReply;

namespace BlackMisc::Network
{
    class CAuthenticatedUser;
}
namespace BlackCore::Db
{
    //! Database user used with swift DB. Features role and cookie handling.
    class BLACKCORE_EXPORT CDatabaseAuthenticationService : public QObject
    {
        Q_OBJECT

    public:
        //! Log categories
        static const QStringList &getLogCategories();

        //! Constructor
        CDatabaseAuthenticationService(QObject *parent = nullptr);

        //! Shutdown
        void gracefulShutdown();

        //! DB user
        BlackMisc::Network::CAuthenticatedUser getDbUser() const;

        //! User authenticated
        bool isUserAuthenticated() const;

        //! Try to login to authentication web service
        BlackMisc::CStatusMessageList login(const QString &id, const QString &password);

        //! Logoff
        void logoff();

    signals:
        //! User authenticated
        void userAuthenticationFinished(const BlackMisc::Network::CAuthenticatedUser &user, const BlackMisc::CStatusMessageList &loginStatus);

        //! Logoff completed
        void logoffFinished();

    private:
        //! Parse login answer
        void parseServerResponse(QNetworkReply *nwReplyPtr);

        //! User object changed
        void userChanged();

        BlackMisc::CData<BlackCore::Data::TAuthenticatedDbUser> m_swiftDbUser { this, &CDatabaseAuthenticationService::userChanged };
        bool m_shutdown = false;
    };
} // ns

#endif // guard
