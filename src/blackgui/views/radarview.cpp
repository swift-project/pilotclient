/* Copyright (C) 2019
 * swift Project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "radarview.h"

#include <QResizeEvent>
#include <QWheelEvent>

namespace BlackGui::Views
{
    CRadarView::CRadarView(QWidget *parent) : QGraphicsView(parent)
    {
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setBackgroundBrush(Qt::black);
        setRenderHint(QPainter::Antialiasing);
    }

    void CRadarView::resizeEvent(QResizeEvent *event)
    {
        emit radarViewResized();
        QGraphicsView::resizeEvent(event);
    }

    void CRadarView::wheelEvent(QWheelEvent *event)
    {
        const QPoint delta = event->angleDelta();
        event->accept();
        const bool zoom = (delta.y() > 0);
        emit zoomEvent(zoom);
    }
} // ns
