/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "statusmessagesdetail.h"
#include "ui_statusmessagesdetail.h"

using namespace BlackMisc;
using namespace BlackGui::Menus;
using namespace BlackGui::Views;

namespace BlackGui
{
    namespace Components
    {
        CStatusMessagesDetail::CStatusMessagesDetail(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CStatusMessagesDetail)
        {
            ui->setupUi(this);
            connect(ui->tvp_StatusMessages, &CStatusMessageView::objectSelected, ui->form_StatusMessage, &CStatusMessageForm::setVariant);
            connect(ui->tvp_StatusMessages, &CStatusMessageView::modelDataChangedDigest, this, &CStatusMessagesDetail::modelDataChangedDigest);
            ui->tvp_StatusMessages->setAutoResizeFrequency(3);
            ui->tvp_StatusMessages->setCustomMenu(new CMessageMenu(this));
            ui->tvp_StatusMessages->menuAddItems(CStatusMessageView::MenuSave);
            this->showFilterBar(); // default
        }

        CStatusMessagesDetail::~CStatusMessagesDetail()
        { }

        void CStatusMessagesDetail::appendStatusMessageToList(const CStatusMessage &message, bool resize)
        {
            if (message.isEmpty()) { return; }
            ui->tvp_StatusMessages->insert(message, resize);
            this->removeOldest();
        }

        void CStatusMessagesDetail::appendStatusMessagesToList(const CStatusMessageList &messages, bool resize)
        {
            if (messages.isEmpty()) { return; }
            ui->tvp_StatusMessages->insert(messages, resize);
            this->removeOldest();
        }

        void CStatusMessagesDetail::showDetails(bool details)
        {
            ui->form_StatusMessage->setVisible(details);
        }

        void CStatusMessagesDetail::showFilterDialog()
        {
            ui->tvp_StatusMessages->addFilterDialog();
            ui->filter_LogMessages->hide();
        }

        void CStatusMessagesDetail::showFilterBar()
        {
            ui->tvp_StatusMessages->setFilterWidget(ui->filter_LogMessages);
            ui->filter_LogMessages->show();
        }

        void CStatusMessagesDetail::hideFilterBar()
        {
            ui->filter_LogMessages->hide();
        }

        void CStatusMessagesDetail::clear()
        {
            ui->tvp_StatusMessages->clear();
            ui->form_StatusMessage->clear();
        }

        int CStatusMessagesDetail::rowCount() const
        {
            return ui->tvp_StatusMessages->rowCount();
        }

        void CStatusMessagesDetail::filterUseRadioButtonDescriptiveIcons(bool oneCharacterText)
        {
            ui->filter_LogMessages->useRadioButtonDescriptiveIcons(oneCharacterText);
        }

        void CStatusMessagesDetail::removeOldest()
        {
            // do not remove every time, but when a threshold is reached
            if (m_maxLogMessages < 1) { return; }
            if (m_maxLogMessages < 100 && ui->tvp_StatusMessages->rowCount() > (m_maxLogMessages + 10))
            {
                ui->tvp_StatusMessages->keepLatest(m_maxLogMessages);
            }
            else if (ui->tvp_StatusMessages->rowCount() > (m_maxLogMessages * 1.1))
            {
                ui->tvp_StatusMessages->keepLatest(m_maxLogMessages);
            }
        }

        void CStatusMessagesDetail::CMessageMenu::customMenu(CMenuActions &menuActions)
        {
            CStatusMessagesDetail *messagesDetail = qobject_cast<CStatusMessagesDetail *>(this->parent());
            Q_ASSERT_X(messagesDetail, Q_FUNC_INFO, "Missing parent");

            bool v = messagesDetail->ui->form_StatusMessage->isVisible();
            const QString formString(v ? "Hide log details" : "Show log details");
            this->m_action = menuActions.addAction(this->m_action, BlackMisc::CIcons::databaseTable16(), formString,
                                                   CMenuAction::pathLog(), { messagesDetail->ui->form_StatusMessage, &CStatusMessageForm::toggleVisibility});
            this->m_action->setCheckable(true);
            this->m_action->setChecked(v);
        }
    } // ns
} // ns
