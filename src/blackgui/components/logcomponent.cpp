/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/components/logcomponent.h"
#include "blackgui/components/statusmessageform.h"
#include "blackgui/menus/menuaction.h"
#include "blackgui/views/statusmessageview.h"
#include "blackgui/views/viewbase.h"
#include "blackgui/guiutility.h"
#include "blackmisc/icons.h"
#include "ui_logcomponent.h"

#include <QAction>
#include <QMenu>
#include <QTabWidget>
#include <Qt>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackGui;
using namespace BlackGui::Views;
using namespace BlackGui::Menus;

namespace BlackGui
{
    namespace Components
    {
        CConsoleTextEdit::CConsoleTextEdit(QWidget *parent) : QPlainTextEdit(parent)
        {
            bool c = connect(this, &CConsoleTextEdit::customContextMenuRequested, this, &CConsoleTextEdit::ps_customMenuRequested);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Custom menu");
            Q_UNUSED(c);
            this->setContextMenuPolicy(Qt::CustomContextMenu);
        }

        void CConsoleTextEdit::ps_customMenuRequested(const QPoint &pos)
        {
            QMenu *menu = QPlainTextEdit::createStandardContextMenu();
            menu->addAction(CIcons::delete16(), "Clear console", this, SLOT(clear()));
            menu->exec(this->mapToGlobal(pos));
        }

        CLogComponent::CLogComponent(QWidget *parent) :
            QFrame(parent), ui(new Ui::CLogComponent)
        {
            ui->setupUi(this);
            connect(ui->tvp_StatusMessages, &CStatusMessageView::objectSelected, ui->form_StatusMessage, &CStatusMessageForm::setVariant);
            connect(ui->tvp_StatusMessages, &CStatusMessageView::modelDataChangedDigest, this, &CLogComponent::onStatusMessageDataChanged);
            ui->tvp_StatusMessages->setAutoResizeFrequency(3);
            ui->tvp_StatusMessages->setCustomMenu(new CLogMenu(this));
            ui->tvp_StatusMessages->menuAddItems(CStatusMessageView::MenuSave);
            this->showFilterBar(); // default
        }

        CLogComponent::~CLogComponent()
        { }

        void CLogComponent::displayLog(bool attention)
        {
            ui->tw_StatusPage->setCurrentIndex(0);
            if (attention) { emit requestAttention(); }
        }

        void CLogComponent::displayConsole(bool attention)
        {
            ui->tw_StatusPage->setCurrentIndex(1);
            if (attention) { emit requestAttention(); }
        }

        void CLogComponent::showDetails(bool details)
        {
            ui->form_StatusMessage->setVisible(details);
        }

        void CLogComponent::showFilterDialog()
        {
            ui->tvp_StatusMessages->addFilterDialog();
            ui->filter_LogMessages->hide();
        }

        void CLogComponent::showFilterBar()
        {
            ui->tvp_StatusMessages->setFilterWidget(ui->filter_LogMessages);
            ui->filter_LogMessages->show();
        }

        void CLogComponent::filterUseRadioButtonDescriptiveIcons(bool oneCharacterText)
        {
            ui->filter_LogMessages->useRadioButtonDescriptiveIcons(oneCharacterText);
        }

        void CLogComponent::clear()
        {
            ui->tvp_StatusMessages->clear();
            ui->form_StatusMessage->clear();
            ui->tep_StatusPageConsole->clear();
        }

        int CLogComponent::rowCount() const
        {
            return ui->tvp_StatusMessages->rowCount();
        }

        void CLogComponent::appendStatusMessageToConsole(const CStatusMessage &statusMessage)
        {
            if (statusMessage.isEmpty()) return;
            ui->tep_StatusPageConsole->appendHtml(statusMessage.toHtml());
        }

        void CLogComponent::appendPlainTextToConsole(const QString &text)
        {
            ui->tep_StatusPageConsole->appendPlainText(text);
        }

        void CLogComponent::appendStatusMessageToList(const CStatusMessage &statusMessage)
        {
            if (statusMessage.isEmpty()) { return; }
            ui->tvp_StatusMessages->insert(statusMessage);
            this->removeOldest();
        }

        void CLogComponent::appendStatusMessagesToList(const CStatusMessageList &statusMessages)
        {
            if (statusMessages.isEmpty()) { return; }
            ui->tvp_StatusMessages->insert(statusMessages);
            this->removeOldest();
        }

        void CLogComponent::onStatusMessageDataChanged(int count, bool withFilter)
        {
            Q_UNUSED(count);
            Q_UNUSED(withFilter);
            const int i = ui->tw_StatusPage->indexOf(ui->pg_LogPage);
            QString o = ui->tw_StatusPage->tabText(i);
            const QString f = ui->tvp_StatusMessages->hasFilter() ? "F" : "";
            o = CGuiUtility::replaceTabCountValue(o, ui->tvp_StatusMessages->rowCount()) + f;
            ui->tw_StatusPage->setTabText(i, o);
        }

        void CLogComponent::removeOldest()
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

        void CLogComponent::CLogMenu::customMenu(CMenuActions &menuActions)
        {
            CLogComponent *logComp = qobject_cast<CLogComponent *>(this->parent());
            Q_ASSERT_X(logComp, Q_FUNC_INFO, "Missing parent");

            bool v = logComp->ui->form_StatusMessage->isVisible();
            QString formString(v ? "Hide log details" : "Show log details");
            this->m_action = menuActions.addAction(this->m_action, BlackMisc::CIcons::databaseTable16(), formString,
                                                   CMenuAction::pathLog(), { logComp->ui->form_StatusMessage, &CStatusMessageForm::toggleVisibility});
            this->m_action->setCheckable(true);
            this->m_action->setChecked(v);
        }
    }
} // namespace
