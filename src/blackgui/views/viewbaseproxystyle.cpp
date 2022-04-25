/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "viewbaseproxystyle.h"
#include "blackgui/views/viewbase.h"

namespace BlackGui::Views
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
} // namespace
