/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 *
 * Class based on qLed: Copyright (C) 2010 by P. Sereno, http://www.sereno-online.com
 */

#include "loadindicator.h"
#include "guiapplication.h"

#include <QColor>
#include <QPainter>
#include <QRect>
#include <QSizePolicy>
#include <QtGlobal>

namespace BlackGui
{
    CLoadIndicator::CLoadIndicator(int width, int height, QWidget *parent)
        : QWidget(parent)
    {
        this->resize(width, height);
        this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        this->setFocusPolicy(Qt::NoFocus);
        this->setAutoFillBackground(true);
        this->setStyleSheet("background-color: transparent;");
    }

    bool CLoadIndicator::isAnimated() const
    {
        return (this->m_timerId != -1);
    }

    void CLoadIndicator::setDisplayedWhenStopped(bool state)
    {
        this->m_displayedWhenStopped = state;
        this->update();
    }

    bool CLoadIndicator::isDisplayedWhenStopped() const
    {
        return m_displayedWhenStopped;
    }

    void CLoadIndicator::startAnimation(bool processEvents)
    {
        this->m_angle = 0;
        this->show();
        this->setEnabled(true);
        if (this->m_timerId == -1) { this->m_timerId = startTimer(m_delayMs); }
        if (processEvents && sGui)
        {
            sGui->processEventsToRefreshGui();
        }
    }

    void CLoadIndicator::stopAnimation()
    {
        if (this->m_timerId != -1) { killTimer(this->m_timerId); }
        this->m_timerId = -1;
        this->hide();
        this->setEnabled(false);
        this->update();
    }

    void CLoadIndicator::setAnimationDelay(int delay)
    {
        if (this->m_timerId != -1) { killTimer(this->m_timerId); }
        this->m_delayMs = delay;
        if (this->m_timerId != -1) { this->m_timerId = startTimer(this->m_delayMs); }
    }

    void CLoadIndicator::setColor(const QColor &color)
    {
        this->m_color = color;
        update();
    }

    QSize CLoadIndicator::sizeHint() const
    {
        return QSize(64, 64);
    }

    int CLoadIndicator::heightForWidth(int w) const
    {
        return w;
    }

    void CLoadIndicator::timerEvent(QTimerEvent *event)
    {
        Q_UNUSED(event);
        m_angle = (m_angle + 30) % 360;
        update();
        emit updatedAnimation();
    }

    void CLoadIndicator::paintEvent(QPaintEvent *event)
    {
        Q_UNUSED(event);
        QPainter p(this);
        this->paint(p);
    }

    bool CLoadIndicator::isParentVisible() const
    {
        if (this->parentWidget()) { return parentWidget()->isVisible(); }
        return false;
    }

    void CLoadIndicator::paint(QPainter &painter) const
    {
        if (!m_displayedWhenStopped && !isAnimated()) { return; }
        if (!this->isVisible() || !this->isEnabled()) { return; }
        if (!isParentVisible()) { return; }

        int width = qMin(this->width(), this->height());
        painter.setRenderHint(QPainter::Antialiasing);

        // painter.setBrush(QBrush(QColor(0, 0, 255)));
        // painter.drawEllipse(0, 0, width, width);

        int outerRadius = (width - 1) * 0.5;
        int innerRadius = (width - 1) * 0.5 * 0.38;

        int capsuleHeight = outerRadius - innerRadius;
        int capsuleWidth  = (width > 32) ? capsuleHeight * .23 : capsuleHeight * .35;
        int capsuleRadius = capsuleWidth / 2;

        for (int i = 0; i < 12; i++)
        {
            QColor color = m_color;
            color.setAlphaF(1.0f - (i / 12.0f));
            painter.setPen(Qt::NoPen);
            painter.setBrush(color);
            painter.save();
            painter.translate(rect().center());
            painter.rotate(m_angle - i * 30.0f);
            painter.drawRoundedRect(-capsuleWidth * 0.5, -(innerRadius + capsuleHeight), capsuleWidth, capsuleHeight, capsuleRadius, capsuleRadius);
            painter.restore();
        }
    }

    void CLoadIndicator::centerLoadIndicator(const QPoint &middle)
    {
        const int w = this->width();
        const int h = this->height();
        const int x = middle.x() - w / 2;
        const int y = middle.y() - h / 2;
        this->setGeometry(x, y, w, h);
    }
} // ns
