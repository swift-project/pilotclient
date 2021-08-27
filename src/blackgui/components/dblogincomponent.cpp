/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "ui_dblogincomponent.h"
#include "blackgui/components/dblogincomponent.h"
#include "blackgui/guiapplication.h"
#include "blackgui/guiutility.h"
#include "blackgui/overlaymessagesframe.h"
#include "blackcore/data/globalsetup.h"
#include "blackmisc/network/authenticateduser.h"
#include "blackmisc/network/url.h"
#include "blackmisc/htmlutils.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/verify.h"
#include "blackmisc/crashhandler.h"
#include "blackconfig/buildconfig.h"

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
using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Network;

namespace BlackGui
{
    namespace Components
    {
        CDbLoginComponent::CDbLoginComponent(QWidget *parent) :
            QFrame(parent),
            CLoadIndicatorEnabled(this),
            ui(new Ui::CDbLoginComponent)
        {
            Q_ASSERT_X(sGui, Q_FUNC_INFO, "Missing sGui");
            ui->setupUi(this);
            this->setModeLogin(true);
            const QString urlString = asHyperlink(sGui->getGlobalSetup().getDbHomePageUrl().getFullUrl());
            QString html = ui->tbr_InfoAndHints->toHtml();
            html = html.replace("##swiftDB##", urlString, Qt::CaseInsensitive);
            html = html.replace("##swiftEnableSSO##", urlString, Qt::CaseInsensitive);

            ui->tbr_InfoAndHints->setHtml(html);
            ui->tbr_InfoAndHints->setOpenExternalLinks(true);

            const bool devEnv = sGui->isDeveloperFlagSet();
            ui->comp_DebugSetup->setVisible(devEnv);

            const QString dbUrl = sGui->getGlobalSetup().getDbHomePageUrl().toQString();
            ui->lbl_DatabaseName->setText(asHyperlink(dbUrl));
            ui->lbl_DatabaseName->setTextFormat(Qt::RichText);
            ui->lbl_DatabaseName->setTextInteractionFlags(Qt::TextBrowserInteraction);
            ui->lbl_DatabaseName->setOpenExternalLinks(true);

            connect(ui->pb_Login,  &QPushButton::clicked, this, &CDbLoginComponent::onLoginClicked);
            connect(ui->pb_Logoff, &QPushButton::clicked, this, &CDbLoginComponent::onLogoffClicked);
            connect(ui->le_Password, &QLineEdit::returnPressed, this, &CDbLoginComponent::onLoginClicked);
            connect(&m_loginService, &CDatabaseAuthenticationService::userAuthenticationFinished, this, &CDbLoginComponent::onAuthenticationFinished, Qt::QueuedConnection);

            if (CBuildConfig::isLocalDeveloperDebugBuild())
            {
                const QString url = sApp->getGlobalSetup().getDbLoginServiceUrl().toQString();
                ui->pb_Login->setToolTip(url);
            }

            // init GUI
            this->setUserInfo(this->getDbUser());
        }

        CDbLoginComponent::~CDbLoginComponent()
        { }

        CAuthenticatedUser CDbLoginComponent::getDbUser() const
        {
            return m_loginService.getDbUser();
        }

        bool CDbLoginComponent::isUserAuthenticated() const
        {
            return m_loginService.isUserAuthenticated();
        }

        void CDbLoginComponent::displayOverlayMessages(const CStatusMessageList &msgs)
        {
            if (msgs.isEmpty()) { return; }
            COverlayMessagesFrame *mf = CGuiUtility::nextOverlayMessageFrame(this);
            BLACK_VERIFY_X(mf, Q_FUNC_INFO, "No overlay widget");
            if (!mf) { return; }
            mf->showOverlayMessages(msgs);
        }

        void CDbLoginComponent::onLoginClicked()
        {
            const QString un(ui->le_Username->text().trimmed());
            const QString pw(ui->le_Password->text().trimmed());
            const CStatusMessageList msgs = m_loginService.login(un, pw);

            if (msgs.hasWarningOrErrorMessages())
            {
                CLogMessage::preformatted(msgs);
                displayOverlayMessages(msgs);
                return;
            }
            else if (!msgs.isEmpty())
            {
                CLogMessage::preformatted(msgs);
            }
            this->showLoading(5000);
        }

        void CDbLoginComponent::onLogoffClicked()
        {
            m_loginService.logoff();
            this->setModeLogin(true);
        }

        void CDbLoginComponent::onAuthenticationFinished(const CAuthenticatedUser &user, const CStatusMessageList &statusMsgs)
        {
            this->hideLoading();
            this->setUserInfo(user);
            if (statusMsgs.hasWarningOrErrorMessages())
            {
                this->displayOverlayMessages(statusMsgs);
                CLogMessage::preformatted(statusMsgs);
                ui->le_Info->setText("Authentication failed, see hints");
            }
        }

        void CDbLoginComponent::setModeLogin(bool modeLogin)
        {
            ui->sw_LoginLogoff->setCurrentIndex(modeLogin ? 0 : 1);
        }

        void CDbLoginComponent::setUserInfo(const CAuthenticatedUser &user)
        {
            if (!sGui || sGui->isShuttingDown()) { return; }
            if (user.isAuthenticated())
            {
                CLogMessage(this).info(u"User authenticated: %1") << user.toQString();
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

                // crashpad info
                CCrashHandler::instance()->crashAndLogInfoUserName(user.getRealNameAndId());
                CCrashHandler::instance()->crashAndLogAppendInfo(QStringLiteral("Login as user %1 %2").arg(user.getRealNameAndId(), user.getRolesAsString()));
            }
            else
            {
                ui->le_Name->clear();
                ui->te_Roles->clear();
                this->setModeLogin(true);
            }
        }
    } // ns
} // ns
