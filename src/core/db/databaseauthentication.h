// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_DATABASE_CDATABASEUATHENTICATIONSERVICE_H
#define SWIFT_CORE_DATABASE_CDATABASEUATHENTICATIONSERVICE_H

#include "core/data/authenticateduser.h"
#include "core/swiftcoreexport.h"
#include "misc/datacache.h"
#include "misc/logcategories.h"
#include "misc/statusmessagelist.h"

#include <QObject>
#include <QString>

class QNetworkReply;

namespace swift::misc::network
{
    class CAuthenticatedUser;
}
namespace swift::core::db
{
    //! Database user used with swift DB. Features role and cookie handling.
    class SWIFT_CORE_EXPORT CDatabaseAuthenticationService : public QObject
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
        swift::misc::network::CAuthenticatedUser getDbUser() const;

        //! User authenticated
        bool isUserAuthenticated() const;

        //! Try to login to authentication web service
        swift::misc::CStatusMessageList login(const QString &id, const QString &password);

        //! Logoff
        void logoff();

    signals:
        //! User authenticated
        void userAuthenticationFinished(const swift::misc::network::CAuthenticatedUser &user, const swift::misc::CStatusMessageList &loginStatus);

        //! Logoff completed
        void logoffFinished();

    private:
        //! Parse login answer
        void parseServerResponse(QNetworkReply *nwReplyPtr);

        //! User object changed
        void userChanged();

        swift::misc::CData<swift::core::data::TAuthenticatedDbUser> m_swiftDbUser { this, &CDatabaseAuthenticationService::userChanged };
        bool m_shutdown = false;
    };
} // ns

#endif // guard
