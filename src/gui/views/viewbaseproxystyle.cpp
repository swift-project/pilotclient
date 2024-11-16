// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "viewbaseproxystyle.h"

#include "gui/views/viewbase.h"

namespace swift::gui::views
{
    CViewBaseProxyStyle::CViewBaseProxyStyle(CViewBaseNonTemplate *view, QStyle *style) : QProxyStyle(style), m_view(view) {}

    void CViewBaseProxyStyle::drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
    {
        const bool indicator = (element == QStyle::PE_IndicatorItemViewItemDrop);
        if (indicator)
        {
            Q_UNUSED(painter);
            Q_UNUSED(option);
            Q_UNUSED(widget);
            QPainter viewPainter(this->m_view->viewport());
            QPoint point = this->m_view->mapFromGlobal(QCursor::pos());
            if (!point.isNull())
            {
                const QPoint topLeft(0, point.y());
                const QPoint topRight(this->m_view->width(), point.y());
                viewPainter.drawLine(topLeft, topRight);
            }
        }
    }
} // namespace swift::gui::views
