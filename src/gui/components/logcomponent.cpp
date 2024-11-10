// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/components/logcomponent.h"
#include "gui/components/statusmessageform.h"
#include "gui/menus/menuaction.h"
#include "gui/views/statusmessageview.h"
#include "gui/views/viewbase.h"
#include "gui/guiutility.h"
#include "core/application.h"
#include "misc/icons.h"
#include "misc/logpattern.h"
#include "misc/sharedstate/datalinkdbus.h"
#include "ui_logcomponent.h"

#include <QAction>
#include <QMenu>
#include <QTabWidget>
#include <Qt>
#include <QtGlobal>

using namespace swift::misc;
using namespace swift::core;
using namespace swift::gui;
using namespace swift::gui::views;
using namespace swift::gui::menus;

namespace swift::gui::components
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

    CLogComponent::CLogComponent(QWidget *parent) : QFrame(parent), ui(new Ui::CLogComponent), m_history(this)
    {
        ui->setupUi(this);

        connect(&m_history, &CLogHistoryReplica::elementAdded, this, [this](const CStatusMessage &message) {
            ui->comp_StatusMessages->appendStatusMessageToList(message);
        });
        connect(&m_history, &CLogHistoryReplica::elementsReplaced, this, [this](const CStatusMessageList &messages) {
            ui->comp_StatusMessages->appendStatusMessagesToList(messages);
        });
        connect(ui->comp_StatusMessages, &CStatusMessagesDetail::filterChanged, this, [this](const CVariant &filter) {
            clearMessages();
            m_history.setFilter(filter.to<CLogPattern>());
        });
        m_history.setFilter(CLogPattern().withSeverityAtOrAbove(CStatusMessage::SeverityInfo));
        m_history.initialize(sApp->getDataLinkDBus());
    }

    CLogComponent::~CLogComponent()
    {}

    void CLogComponent::displayLog(bool attention)
    {
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
        ui->comp_StatusMessages->clear();
    }

    void CLogComponent::clearMessages()
    {
        ui->comp_StatusMessages->clear();
    }
} // namespace
