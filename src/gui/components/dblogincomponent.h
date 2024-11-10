// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_DBLOGINCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_DBLOGINCOMPONENT_H

#include "core/db/databaseauthentication.h"
#include "core/data/globalsetup.h"
#include "gui/swiftguiexport.h"
#include "gui/loadindicator.h"
#include "misc/statusmessagelist.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

namespace swift::misc::network
{
    class CAuthenticatedUser;
}
namespace Ui
{
    class CDbLoginComponent;
}
namespace swift::gui::components
{
    /*!
     * Login to DB
     * \sa swift::core::db::CDatabaseAuthenticationService
     */
    class SWIFT_GUI_EXPORT CDbLoginComponent :
        public QFrame,
        public CLoadIndicatorEnabled
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CDbLoginComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CDbLoginComponent() override;

        //! DB user
        swift::misc::network::CAuthenticatedUser getDbUser() const;

        //! Is user authenticated?
        bool isUserAuthenticated() const;

    private:
        QScopedPointer<Ui::CDbLoginComponent> ui;
        swift::core::db::CDatabaseAuthenticationService m_loginService { this }; //!< login service

        //! Overlay messages
        void displayOverlayMessages(const swift::misc::CStatusMessageList &msgs);

        //! Mode login
        void setModeLogin(bool modeLogin);

        //! Set the user fields
        void setUserInfo(const swift::misc::network::CAuthenticatedUser &user);

        //! Login
        void onLoginClicked();

        //! Logoff
        void onLogoffClicked();

        //! User authentication completed
        void onAuthenticationFinished(const swift::misc::network::CAuthenticatedUser &user, const swift::misc::CStatusMessageList &statusMsgs);
    };
} // ns

#endif // guard
