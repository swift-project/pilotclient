// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/components/dblogincomponent.h"

#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QString>
#include <QWidget>
#include <Qt>
#include <QtGlobal>

#include "ui_dblogincomponent.h"

#include "config/buildconfig.h"
#include "core/data/globalsetup.h"
#include "gui/guiapplication.h"
#include "gui/guiutility.h"
#include "gui/overlaymessagesframe.h"
#include "misc/crashhandler.h"
#include "misc/htmlutils.h"
#include "misc/logmessage.h"
#include "misc/network/authenticateduser.h"
#include "misc/network/url.h"
#include "misc/statusmessage.h"
#include "misc/verify.h"

using namespace swift::core;
using namespace swift::core::db;
using namespace swift::gui;
using namespace swift::config;
using namespace swift::misc;
using namespace swift::misc::network;

namespace swift::gui::components
{
    CDbLoginComponent::CDbLoginComponent(QWidget *parent) : QFrame(parent),
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

        const QString dbUrl = sGui->getGlobalSetup().getDbHomePageUrl().toQString();
        ui->lbl_DatabaseName->setText(asHyperlink(dbUrl));
        ui->lbl_DatabaseName->setTextFormat(Qt::RichText);
        ui->lbl_DatabaseName->setTextInteractionFlags(Qt::TextBrowserInteraction);
        ui->lbl_DatabaseName->setOpenExternalLinks(true);

        connect(ui->pb_Login, &QPushButton::clicked, this, &CDbLoginComponent::onLoginClicked);
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
    {}

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
        SWIFT_VERIFY_X(mf, Q_FUNC_INFO, "No overlay widget");
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
} // namespace swift::gui::components
