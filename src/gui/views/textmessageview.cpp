// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/views/textmessageview.h"

#include <QtGlobal>

#include "gui/models/textmessagelistmodel.h"

using namespace swift::misc;
using namespace swift::gui::models;

namespace swift::gui::views
{
    CTextMessageView::CTextMessageView(QWidget *parent) : CViewBase(parent)
    {
        this->standardInit(new CTextMessageListModel(CTextMessageListModel::FromTo, this));
        m_menus |= MenuClear;
    }

    void CTextMessageView::setTextMessageMode(CTextMessageListModel::TextMessageMode mode)
    {
        Q_ASSERT(m_model);
        m_model->setTextMessageMode(mode);
        this->setSortIndicator();
    }

    bool CTextMessageView::isSortedByTimestampProperty() const
    {
        return m_model->isSortedByTimestampProperty();
    }

    bool CTextMessageView::isSortedByTimestampPropertyLatestLast() const
    {
        return m_model->isSortedByTimestampProperty() && this->getSortOrder() == Qt::AscendingOrder;
    }

} // namespace swift::gui::views
