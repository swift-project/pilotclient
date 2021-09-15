/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "statusmessagesdetail.h"
#include "ui_statusmessagesdetail.h"
#include "blackgui/filters/statusmessagefilterbar.h"
#include "blackgui/filters/statusmessagefilterdialog.h"

using namespace BlackMisc;
using namespace BlackGui::Menus;
using namespace BlackGui::Views;
using namespace BlackGui::Filters;

namespace BlackGui::Components
{
    CStatusMessagesDetail::CStatusMessagesDetail(QWidget *parent) :
        QFrame(parent),
        ui(new Ui::CStatusMessagesDetail)
    {
        ui->setupUi(this);
        connect(ui->tvp_StatusMessages, &CStatusMessageView::objectSelected, ui->form_StatusMessage, &CStatusMessageForm::setVariant);
        connect(ui->tvp_StatusMessages, &CStatusMessageView::modelDataChangedDigest, this, &CStatusMessagesDetail::modelDataChangedDigest);
        ui->tvp_StatusMessages->setAutoResizeFrequency(3);
        ui->tvp_StatusMessages->setSorting(CStatusMessage::IndexUtcTimestamp, Qt::DescendingOrder);
        ui->tvp_StatusMessages->setCustomMenu(new CMessageMenu(this));
        ui->tvp_StatusMessages->menuAddItems(CStatusMessageView::MenuSave);
        this->showFilterBar(); // default

        connect(ui->filter_LogMessages, &CStatusMessageFilterBar::changeFilter, this, [this](bool enable)
        {
            emit filterChanged(enable ? ui->filter_LogMessages->createModelFilter()->getAsValueObject() : CVariant{});
        });
    }

    CStatusMessagesDetail::~CStatusMessagesDetail()
    { }

    void CStatusMessagesDetail::appendStatusMessageToList(const CStatusMessage &message)
    {
        if (message.isEmpty()) { return; }
        m_pending.push_front(message); // in many cases we want to have the latest "on top"
        m_dsDeferredUpdate.inputSignal();
    }

    void CStatusMessagesDetail::appendStatusMessagesToList(const CStatusMessageList &messages)
    {
        if (messages.isEmpty()) { return; }
        m_pending.push_front(messages);
        m_dsDeferredUpdate.inputSignal();
    }

    void CStatusMessagesDetail::showDetails(bool details)
    {
        ui->form_StatusMessage->setVisible(details);
    }

    void CStatusMessagesDetail::showFilterDialog()
    {
        ui->tvp_StatusMessages->addFilterDialog();
        ui->filter_LogMessages->hide();

        connect(ui->tvp_StatusMessages->getFilterDialog(), &QDialog::accepted, [this]
        {
            emit filterChanged(ui->tvp_StatusMessages->getFilterDialog()->createModelFilter()->getAsValueObject());
        });
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

    void CStatusMessagesDetail::setSorting(const CPropertyIndex &propertyIndex, Qt::SortOrder order)
    {
        ui->tvp_StatusMessages->setSorting(propertyIndex, order);
    }

    void CStatusMessagesDetail::setNoSorting()
    {
        ui->tvp_StatusMessages->setNoSorting();
    }

    void CStatusMessagesDetail::deferredUpdate()
    {
        if (m_pending.isEmpty()) { return; }
        CStatusMessageList newMsgs(ui->tvp_StatusMessages->container());
        newMsgs.push_front(m_pending); // default in many cases, latest first
        m_pending.clear();

        // cleanup outdated: do not remove every time, but when a threshold is reached
        if (m_maxLogMessages < 0)
        {
            // do not restrict
        }
        else  if (m_maxLogMessages < 100 && ui->tvp_StatusMessages->rowCount() > (m_maxLogMessages + 10))
        {
            newMsgs.keepLatest(m_maxLogMessages);
        }
        else if (ui->tvp_StatusMessages->rowCount() > (m_maxLogMessages * 1.1))
        {
            newMsgs.keepLatest(m_maxLogMessages);
        }

        ui->tvp_StatusMessages->adjustOrderColumn(newMsgs);
        ui->tvp_StatusMessages->updateContainerMaybeAsync(newMsgs);
    }

    void CStatusMessagesDetail::CMessageMenu::customMenu(CMenuActions &menuActions)
    {
        CStatusMessagesDetail *messagesDetail = qobject_cast<CStatusMessagesDetail *>(this->parent());
        Q_ASSERT_X(messagesDetail, Q_FUNC_INFO, "Missing parent");

        const bool v = messagesDetail->ui->form_StatusMessage->isVisible();
        const QString formString(v ? "Hide log details" : "Show log details");
        m_action = menuActions.addAction(m_action,
                                            CIcons::databaseTable16(), formString,
                                            CMenuAction::pathLog(), { messagesDetail->ui->form_StatusMessage, &CStatusMessageForm::toggleVisibility});
        m_action->setCheckable(true);
        m_action->setChecked(v);
    }
} // ns
