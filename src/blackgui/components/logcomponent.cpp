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
            bool c = connect(this, &CConsoleTextEdit::customContextMenuRequested, this, &CConsoleTextEdit::customMenuRequested);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Custom menu");
            Q_UNUSED(c);
            this->setContextMenuPolicy(Qt::CustomContextMenu);
        }

        void CConsoleTextEdit::customMenuRequested(const QPoint &pos)
        {
            QMenu *menu = QPlainTextEdit::createStandardContextMenu();
            menu->addAction(CIcons::delete16(), "Clear console", this, &QPlainTextEdit::clear);
            menu->exec(this->mapToGlobal(pos));
        }

        CLogComponent::CLogComponent(QWidget *parent) :
            QFrame(parent), ui(new Ui::CLogComponent)
        {
            ui->setupUi(this);
            connect(ui->comp_StatusMessages, &CStatusMessagesDetail::modelDataChangedDigest, this, &CLogComponent::onStatusMessageDataChanged);
        }

        CLogComponent::~CLogComponent()
        { }

        void CLogComponent::displayLog(bool attention)
        {
            ui->tw_StatusPage->setCurrentIndex(0);
            if (attention) { emit this->requestAttention(); }
        }

        void CLogComponent::displayConsole(bool attention)
        {
            ui->tw_StatusPage->setCurrentIndex(1);
            if (attention) { emit this->requestAttention(); }
        }

        void CLogComponent::filterUseRadioButtonDescriptiveIcons(bool oneLetterText)
        {
            ui->comp_StatusMessages->filterUseRadioButtonDescriptiveIcons(oneLetterText);
        }

        void CLogComponent::showFilterDialog()
        {
            ui->comp_StatusMessages->showFilterDialog();
        }

        void CLogComponent::showFilterBar()
        {
            ui->comp_StatusMessages->showFilterBar();
        }

        void CLogComponent::hideFilterBar()
        {
            ui->comp_StatusMessages->hideFilterBar();
        }

        void CLogComponent::showDetails(bool details)
        {
            ui->comp_StatusMessages->showDetails(details);
        }

        void CLogComponent::setMaxLogMessages(int max)
        {
            ui->comp_StatusMessages->setMaxLogMessages(max);
        }

        void CLogComponent::setNoSorting()
        {
            ui->comp_StatusMessages->setNoSorting();
        }

        void CLogComponent::setSorting(const CPropertyIndex &propertyIndex, Qt::SortOrder order)
        {
            ui->comp_StatusMessages->setSorting(propertyIndex, order);
        }

        void CLogComponent::clear()
        {
            ui->tep_StatusPageConsole->clear();
            ui->comp_StatusMessages->clear();
        }

        void CLogComponent::clearConsole()
        {
            ui->tep_StatusPageConsole->clear();
        }

        void CLogComponent::clearMessages()
        {
            ui->comp_StatusMessages->clear();
        }

        void CLogComponent::appendStatusMessageToConsole(const CStatusMessage &statusMessage)
        {
            if (statusMessage.isEmpty()) return;
            ui->tep_StatusPageConsole->appendHtml(statusMessage.toHtml(false, true));
        }

        void CLogComponent::appendPlainTextToConsole(const QString &text)
        {
            ui->tep_StatusPageConsole->appendPlainText(text);
        }

        void CLogComponent::appendStatusMessageToList(const CStatusMessage &statusMessage)
        {
            ui->comp_StatusMessages->appendStatusMessageToList(statusMessage);
        }

        void CLogComponent::appendStatusMessagesToList(const CStatusMessageList &statusMessages)
        {
            ui->comp_StatusMessages->appendStatusMessagesToList(statusMessages);
        }

        void CLogComponent::onStatusMessageDataChanged(int count, bool withFilter)
        {
            Q_UNUSED(count);
            Q_UNUSED(withFilter);
            const int i = ui->tw_StatusPage->indexOf(ui->pg_LogPage);
            QString o = ui->tw_StatusPage->tabText(i);
            const QString f = withFilter ? "F" : "";
            o = CGuiUtility::replaceTabCountValue(o, ui->comp_StatusMessages->rowCount()) + f;
            ui->tw_StatusPage->setTabText(i, o);
        }
    }
} // namespace
