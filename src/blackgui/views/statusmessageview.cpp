/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
