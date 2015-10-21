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
            CUrl url(m_setup.get().dbHomePage());
            ui->lbl_SwiftDB->setText("<a href=\"" + url.getFullUrl() + "\">swift DB@" + url.getHost() + "</a>");
            ui->lbl_SwiftDB->setTextFormat(Qt::RichText);
            ui->lbl_SwiftDB->setTextInteractionFlags(Qt::TextBrowserInteraction);
            ui->lbl_SwiftDB->setOpenExternalLinks(true);

            connect(ui->pb_Login, &QPushButton::clicked, this, &CDbLoginComponent::ps_onLoginClicked);
        }

        CDbLoginComponent::~CDbLoginComponent()
        { }

        void CDbLoginComponent::displayOverlayMessages(const CStatusMessageList &msgs)
        {
            if (msgs.isEmpty()) { return; }
            COverlayMessagesFrame *mf = qobject_cast<COverlayMessagesFrame *>(parentWidget());
            Q_ASSERT_X(mf, Q_FUNC_INFO, "No overlay widget");
            if (!mf) { return; }
            mf->showMessages(msgs);
        }

        void CDbLoginComponent::ps_onLoginClicked()
        {
            CStatusMessageList msgs;
            static const CLogCategoryList cats(CLogCategoryList(this).join({ CLogCategory::validation()}));

            QString un(ui->le_Username->text().trimmed());
            QString pw(ui->le_Password->text().trimmed());
            if (un.isEmpty()) { msgs.push_back(CStatusMessage::CStatusMessage(cats, CStatusMessage::SeverityError, "No user name")); }
            if (pw.isEmpty()) { msgs.push_back(CStatusMessage::CStatusMessage(cats, CStatusMessage::SeverityError, "No password")); }
            if (msgs.hasWarningOrErrorMessages())
            {
                CLogMessage::preformatted(msgs);
                displayOverlayMessages(msgs);
                return;
            }
        }

    } // ns
} // ns
