// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "radarview.h"

#include <QResizeEvent>
#include <QWheelEvent>

namespace swift::gui::views
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
