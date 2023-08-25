// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/models/statusmessagelistmodel.h"
#include "blackgui/views/statusmessageview.h"
#include "blackgui/filters/statusmessagefilterdialog.h"
#include <QFlags>

using namespace BlackMisc;
using namespace BlackGui::Models;
using namespace BlackGui::Filters;

namespace BlackGui::Views
{
    CStatusMessageView::CStatusMessageView(QWidget *parent) : CViewBase(parent)
    {
        m_menus |= MenuClear;
        m_acceptRowSelection = true;
        m_enabledLoadIndicator = false; // for performance reasons
        this->menuRemoveItems(MenuRefresh | MenuBackend | MenuToggleSelectionMode);
        this->standardInit(new CStatusMessageListModel(this));
    }

    void CStatusMessageView::setMode(CStatusMessageListModel::Mode mode)
    {
        this->derivedModel()->setMode(mode);
    }

    void CStatusMessageView::setMode(CStatusMessageListModel::Mode mode, const CStatusMessageList &messages)
    {
        this->derivedModel()->setMode(mode, messages);
    }

    void CStatusMessageView::adjustOrderColumn(const CStatusMessageList &messages)
    {
        this->derivedModel()->adjustOrderColumn(messages);
    }

    bool CStatusMessageView::isSortedByTimestampOrOrder() const
    {
        return this->derivedModel()->isSortedByTimestampOrOrder();
    }

    void CStatusMessageView::addFilterDialog()
    {
        if (this->getFilterDialog()) { return; } // already existing
        this->setFilterDialog(new CStatusMessageFilterDialog(this));
    }

    void CStatusMessageView::keepLatest(int desiredSize)
    {
        if (desiredSize >= this->rowCount()) { return; }
        if (desiredSize < 1)
        {
            this->clear();
            return;
        }

        CStatusMessageList msgs = this->container();
        msgs.keepLatest(desiredSize);
        this->updateContainerMaybeAsync(msgs);
    }

    CStatusMessageFilterDialog *CStatusMessageView::getFilterDialog() const
    {
        return qobject_cast<CStatusMessageFilterDialog *>(this->getFilterWidget());
    }
} // namespace
