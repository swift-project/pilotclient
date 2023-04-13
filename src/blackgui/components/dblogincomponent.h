/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_DBLOGINCOMPONENT_H
#define BLACKGUI_COMPONENTS_DBLOGINCOMPONENT_H

#include "blackcore/db/databaseauthentication.h"
#include "blackcore/data/globalsetup.h"
#include "blackgui/blackguiexport.h"
#include "blackgui/loadindicator.h"
#include "blackmisc/statusmessagelist.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

namespace BlackMisc::Network
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
        BlackMisc::Network::CAuthenticatedUser getDbUser() const;

        //! Is user authenticated?
        bool isUserAuthenticated() const;

    private:
        QScopedPointer<Ui::CDbLoginComponent> ui;
        BlackCore::Db::CDatabaseAuthenticationService m_loginService { this }; //!< login service

        //! Overlay messages
        void displayOverlayMessages(const BlackMisc::CStatusMessageList &msgs);

        //! Mode login
        void setModeLogin(bool modeLogin);

        //! Set the user fields
        void setUserInfo(const BlackMisc::Network::CAuthenticatedUser &user);

        //! Login
        void onLoginClicked();

        //! Logoff
        void onLogoffClicked();

        //! User authentication completed
        void onAuthenticationFinished(const BlackMisc::Network::CAuthenticatedUser &user, const BlackMisc::CStatusMessageList &statusMsgs);
    };
} // ns

#endif // guard
