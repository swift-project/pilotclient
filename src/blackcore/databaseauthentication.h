/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKCORE_DATABASE_CDATABASEUATHENTICATIONSERVICE_H
#define BLACKCORE_DATABASE_CDATABASEUATHENTICATIONSERVICE_H

//! \file

#include "blackcore/blackcoreexport.h"
#include "blackmisc/network/authenticateduser.h"
#include "blackcore/data/globalsetup.h"
#include "blackcore/data/authenticateduser.h"
#include "blackmisc/statusmessagelist.h"

#include <QString>
#include <QNetworkReply>
#include <QNetworkCookieJar>
#include <QSharedPointer>

namespace BlackCore
{
    //! Databse user used with swift DB. Features role and cookie handling.
    class BLACKCORE_EXPORT  CDatabaseAuthenticationService: public QObject
    {
        Q_OBJECT

    public:
        //! Constructor
        CDatabaseAuthenticationService(QObject *parent = nullptr);

        //! Shutdown
        void gracefulShutdown();

    public slots:
        //! Try to login to authentication web service
        BlackMisc::CStatusMessageList login(const QString &id, const QString &password);

        //! Logoff
        void logoff();

    signals:
        //! User authenticated
        void userAuthenticationFinished(const BlackMisc::Network::CAuthenticatedUser &user, const BlackMisc::CStatusMessageList &loginStatus);

        //! Logoff completed
        void logoffFinished();

    private slots:
        //! Parse login answer
        void ps_parseServerResponse(QNetworkReply *nwReplyPtr);

        //! User object changed
        void ps_userChanged();

    private:
        BlackMisc::CData<BlackCore::Data::GlobalSetup>         m_setup {this}; //!< data cache
        BlackMisc::CData<BlackCore::Data::AuthenticatedDbUser> m_swiftDbUser {this, &CDatabaseAuthenticationService::ps_userChanged};
        bool m_shutdown = false;
    };
} // namespace

#endif // guard
