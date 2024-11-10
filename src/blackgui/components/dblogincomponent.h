// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_DBLOGINCOMPONENT_H
#define BLACKGUI_COMPONENTS_DBLOGINCOMPONENT_H

#include "blackcore/db/databaseauthentication.h"
#include "blackcore/data/globalsetup.h"
#include "blackgui/blackguiexport.h"
#include "blackgui/loadindicator.h"
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
namespace BlackGui::Components
{
    /*!
     * Login to DB
     * \sa BlackCore::Db::CDatabaseAuthenticationService
     */
    class BLACKGUI_EXPORT CDbLoginComponent :
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
        BlackCore::Db::CDatabaseAuthenticationService m_loginService { this }; //!< login service

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
