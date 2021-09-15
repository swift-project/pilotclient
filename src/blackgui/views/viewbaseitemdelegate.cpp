/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "viewbaseitemdelegate.h"
#include "viewbase.h"

namespace BlackGui::Views
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
} // namespace
