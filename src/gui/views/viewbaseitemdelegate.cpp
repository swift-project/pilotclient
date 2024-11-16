// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "viewbaseitemdelegate.h"

#include "viewbase.h"

namespace swift::gui::views
{
    void CViewBaseItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        const bool isMouseOver = option.state & QStyle::State_MouseOver;
        viewBase()->mouseOverCallback(index, isMouseOver);
        QStyledItemDelegate::paint(painter, option, index);
    }

    QSize CViewBaseItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        return QStyledItemDelegate::sizeHint(option, index);
    }

    CViewBaseNonTemplate *CViewBaseItemDelegate::viewBase() const
    {
        return qobject_cast<CViewBaseNonTemplate *>(this->parent());
    }
} // namespace swift::gui::views
