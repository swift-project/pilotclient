/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "dblogincomponent.h"
#include "ui_dblogincomponent.h"
#include "blackgui/overlaymessagesframe.h"
#include "blackmisc/network/url.h"
#include "blackmisc/logmessage.h"

using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackMisc::Network;

namespace BlackGui
{
    namespace Components
    {
        CDbLoginComponent::CDbLoginComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CDbLoginComponent)
        {
            ui->setupUi(this);
            this->setModeLogin(true);
            this->ps_setupChanged();

            connect(ui->pb_Login, &QPushButton::clicked, this, &CDbLoginComponent::ps_onLoginClicked);
            connect(ui->pb_Logoff, &QPushButton::clicked, this, &CDbLoginComponent::ps_onLogoffClicked);
            connect(&m_loginService, &CDatabaseAuthenticationService::userAuthenticationFinished, this, &CDbLoginComponent::ps_AuthenticationFinished);
        }

        CDbLoginComponent::~CDbLoginComponent()
        { }

        void CDbLoginComponent::displayOverlayMessages(const CStatusMessageList &msgs)
        {
            if (msgs.isEmpty()) { return; }
            COverlayMessagesFrame *mf = qobject_cast<COverlayMessagesFrame *>(parentWidget());
            Q_ASSERT_X(mf, Q_FUNC_INFO, "No overlay widget");
            if (!mf) { return; }
            mf->showOverlayMessages(msgs);
        }

        void CDbLoginComponent::ps_onLoginClicked()
        {
            QString un(ui->le_Username->text().trimmed());
            QString pw(ui->le_Password->text().trimmed());
            CStatusMessageList msgs = m_loginService.login(un, pw);

            if (msgs.hasWarningOrErrorMessages())
            {
                CLogMessage::preformatted(msgs);
                displayOverlayMessages(msgs);
                return;
            }
            else if (!msgs.empty())
            {
                CLogMessage::preformatted(msgs);
            }
        }

        void CDbLoginComponent::ps_onLogoffClicked()
        {
            this->m_loginService.logoff();
            this->setModeLogin(true);
        }

        void CDbLoginComponent::ps_AuthenticationFinished(const CAuthenticatedUser &user, const CStatusMessageList &status)
        {
            bool ok = !status.hasErrorMessages();
            if (ok)
            {
                CLogMessage(this).info("User authenticated: %1") << user.toQString();
                this->setModeLogin(false);
                this->ui->le_Name->setText(user.getRealNameAndId());
                this->ui->te_Roles->setPlainText(user.getRolesAsString());
            }
            else
            {
                this->setModeLogin(true);
                this->displayOverlayMessages(status);
                CLogMessage(this).preformatted(status);
            }
        }

        void CDbLoginComponent::ps_setupChanged()
        {
            CUrl url(m_setup.get().dbHomePageUrl());
            ui->lbl_SwiftDB->setText("<a href=\"" + url.getFullUrl() + "\">swift DB@" + url.getHost() + "</a>");
            ui->lbl_SwiftDB->setTextFormat(Qt::RichText);
            ui->lbl_SwiftDB->setTextInteractionFlags(Qt::TextBrowserInteraction);
            ui->lbl_SwiftDB->setOpenExternalLinks(true);
        }

        void CDbLoginComponent::setModeLogin(bool modeLogin)
        {
            this->ui->fr_Login->setVisible(modeLogin);
            this->ui->fr_Logoff->setVisible(!modeLogin);
        }

    } // ns
} // ns
