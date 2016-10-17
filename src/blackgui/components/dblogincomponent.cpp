/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/data/globalsetup.h"
#include "blackgui/components/dblogincomponent.h"
#include "blackgui/guiapplication.h"
#include "blackgui/guiutility.h"
#include "blackgui/overlaymessagesframe.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/network/authenticateduser.h"
#include "blackmisc/network/url.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/verify.h"
#include "ui_dblogincomponent.h"

#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QString>
#include <QWidget>
#include <Qt>
#include <QtGlobal>

using namespace BlackCore;
using namespace BlackCore::Db;
using namespace BlackGui;
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
            CUrl url(sGui->getGlobalSetup().getDbHomePageUrl());
            QString html = ui->tbr_InfoAndHints->toHtml();
            html = html.replace("##swiftDB##", url.getFullUrl(), Qt::CaseInsensitive);
            html = html.replace("##swiftEnableSSO##", url.getFullUrl(), Qt::CaseInsensitive);

            ui->tbr_InfoAndHints->setHtml(html);
            ui->tbr_InfoAndHints->setOpenExternalLinks(true);

            const bool devEnv = sGui->isRunningInDeveloperEnvironment();
            ui->comp_DebugSetup->setVisible(devEnv);

            connect(ui->pb_Login, &QPushButton::clicked, this, &CDbLoginComponent::ps_onLoginClicked);
            connect(ui->pb_Logoff, &QPushButton::clicked, this, &CDbLoginComponent::ps_onLogoffClicked);
            connect(&m_loginService, &CDatabaseAuthenticationService::userAuthenticationFinished, this, &CDbLoginComponent::ps_authenticationFinished);
            connect(ui->le_Password, &QLineEdit::returnPressed, this, &CDbLoginComponent::ps_onLoginClicked);
        }

        CDbLoginComponent::~CDbLoginComponent()
        { }

        void CDbLoginComponent::displayOverlayMessages(const CStatusMessageList &msgs)
        {
            if (msgs.isEmpty()) { return; }
            COverlayMessagesFrame *mf = CGuiUtility::nextOverlayMessageFrame(this);
            BLACK_VERIFY_X(mf, Q_FUNC_INFO, "No overlay widget");
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

        void CDbLoginComponent::ps_authenticationFinished(const CAuthenticatedUser &user, const CStatusMessageList &status)
        {
            bool ok = !status.hasErrorMessages();
            if (ok)
            {
                CLogMessage(this).info("User authenticated: %1") << user.toQString();
                this->setModeLogin(false);
                ui->le_Name->setText(user.getRealNameAndId());
                ui->te_Roles->setPlainText(user.getRolesAsString());
                if (user.canDirectlyWriteModels())
                {
                    ui->le_Info->setText("You can directly update models");
                }
                else
                {
                    ui->le_Info->setText("You can create model change requests");
                }
            }
            else
            {
                this->setModeLogin(true);
                this->displayOverlayMessages(status);
                CLogMessage::preformatted(status);
                ui->le_Info->setText("Authentication failed, see hints");
            }
        }

        void CDbLoginComponent::setModeLogin(bool modeLogin)
        {
            ui->sw_LoginLogoff->setCurrentIndex(modeLogin ? 0 : 1);
        }
    } // ns
} // ns
