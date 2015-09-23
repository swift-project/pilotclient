/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "logcomponent.h"
#include "ui_logcomponent.h"
#include "../stylesheetutility.h"

using namespace BlackMisc;
using namespace BlackGui;
using namespace BlackGui::Views;

namespace BlackGui
{
    namespace Components
    {
        CLogComponent::CLogComponent(QWidget *parent) :
            QFrame(parent), ui(new Ui::CLogComponent)
        {
            ui->setupUi(this);
            this->ui->tvp_StatusMessages->setAutoResizeFrequency(3);
            connect(this->ui->tvp_StatusMessages, &CStatusMessageView::messageSelected,
                    this->ui->form_StatusMessage, &CStatusMessageForm::setValue);
            this->ui->tvp_StatusMessages->setCustomMenu(new CLogMenu(this));
        }

        CLogComponent::~CLogComponent()
        { }

        void CLogComponent::appendStatusMessageToConsole(const CStatusMessage &statusMessage)
        {
            if (statusMessage.isEmpty()) return;
            this->ui->te_StatusPageConsole->appendHtml(statusMessage.toHtml());
        }

        void CLogComponent::appendPlainTextToConsole(const QString &text)
        {
            this->ui->te_StatusPageConsole->appendPlainText(text);
        }

        void CLogComponent::appendStatusMessageToList(const CStatusMessage &statusMessage)
        {
            if (statusMessage.isEmpty()) return;
            this->ui->tvp_StatusMessages->insert(statusMessage);
        }

        void CLogComponent::CLogMenu::customMenu(QMenu &menu) const
        {
            CLogComponent *logComp = qobject_cast<CLogComponent *>(this->parent());
            Q_ASSERT_X(logComp, Q_FUNC_INFO, "Missing parent");

            bool v = logComp->ui->form_StatusMessage->isVisible();
            QString formString(v ? "Hide details" : "Show details");
            QAction *a = menu.addAction(BlackMisc::CIcons::databaseTable16(), formString, logComp->ui->form_StatusMessage, SLOT(toggleVisibility()));
            a->setCheckable(true);
            a->setChecked(v);
        }
    }
} // namespace
