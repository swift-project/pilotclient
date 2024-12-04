// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1
// Class based on qLed: Copyright (C) 2010 by P. Sereno, http://www.sereno-online.com

#include "loadindicator.h"

#include <QColor>
#include <QPainter>
#include <QPointer>
#include <QRect>
#include <QSizePolicy>
#include <QtGlobal>

#include "gui/guiapplication.h"
#include "misc/threadutils.h"
#include "misc/verify.h"

using namespace swift::misc;

namespace swift::gui
{
    CLoadIndicator::CLoadIndicator(int width, int height, QWidget *parent) : QWidget(parent)
    {
        this->setObjectName("CLoadIndicator");
        this->resize(width, height);
        this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        this->setFocusPolicy(Qt::NoFocus);
        this->setAutoFillBackground(true);
        this->setStyleSheet("background-color: transparent;");
    }

    bool CLoadIndicator::isAnimated() const { return (m_timerId != -1); }

    void CLoadIndicator::setDisplayedWhenStopped(bool state)
    {
        m_displayedWhenStopped = state;
        this->update();
    }

    bool CLoadIndicator::isDisplayedWhenStopped() const { return m_displayedWhenStopped; }

    int CLoadIndicator::startAnimation(std::chrono::milliseconds timeout, bool processEvents)
    {
        using namespace std::chrono_literals;
        m_angle = 0;
        this->show();
        this->setEnabled(true);

        QPointer<CLoadIndicator> myself(this);
        if (m_timerId == -1) { m_timerId = startTimer(m_delayMs); }
        if (processEvents && sGui)
        {
            sGui->processEventsToRefreshGui();
            if (!myself) // cppcheck-suppress knownConditionTrueFalse
            {
                return -1; // deleted in meantime (process events)
            }
        }

        const int stopId = m_currentId++; // copy
        if (timeout > 0ms)
        {
            QTimer::singleShot(timeout, this, [=] {
                if (!myself) { return; }

                // only timeout myself id
                this->stopAnimation(stopId);
                emit this->timedOut();
            });
        }
        m_pendingIds.push_back(stopId);
        return stopId;
    }

    void CLoadIndicator::stopAnimation(int indicatorId)
    {
        if (indicatorId > 0)
        {
            m_pendingIds.removeOne(indicatorId);
            // if others pending do not stop
            if (!m_pendingIds.isEmpty()) { return; }
        }
        m_pendingIds.clear();
        if (m_timerId != -1)
        {
            SWIFT_AUDIT_X(CThreadUtils::isInThisThread(this), Q_FUNC_INFO, "Try to kill timer from another thread");
            this->killTimer(m_timerId);
        }
        m_timerId = -1;
        this->hide();
        this->setEnabled(false);
        this->update();
    }

    void CLoadIndicator::setAnimationDelay(int delay)
    {
        m_delayMs = delay;
        if (m_timerId != -1)
        {
            SWIFT_AUDIT_X(CThreadUtils::isInThisThread(this), Q_FUNC_INFO, "Try to kill timer from another thread");
            this->killTimer(m_timerId);
        }
        m_timerId = this->startTimer(m_delayMs);
    }

    void CLoadIndicator::setColor(const QColor &color)
    {
        m_color = color;
        update();
    }

    QSize CLoadIndicator::sizeHint() const { return QSize(64, 64); }

    int CLoadIndicator::heightForWidth(int w) const { return w; }

    void CLoadIndicator::timerEvent(QTimerEvent *event)
    {
        Q_UNUSED(event)
        m_angle = (m_angle + 30) % 360;
        this->update();
    }

    void CLoadIndicator::paintEvent(QPaintEvent *event)
    {
        Q_UNUSED(event)
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

        int outerRadius = qRound((width - 1) * 0.5);
        int innerRadius = qRound((width - 1) * 0.5 * 0.38);

        int capsuleHeight = outerRadius - innerRadius;
        int capsuleWidth = (width > 32) ? qRound(capsuleHeight * .23) : qRound(capsuleHeight * .35);
        int capsuleRadius = capsuleWidth / 2;

        for (int i = 0; i < 12; i++)
        {
            QColor color = m_color;
            color.setAlphaF(qRound(1.0f - (i / 12.0f)));
            painter.setPen(Qt::NoPen);
            painter.setBrush(color);
            painter.save();
            painter.translate(rect().center());
            painter.rotate(m_angle - qRound(i * 30.0f));
            painter.drawRoundedRect(-qRound(capsuleWidth * 0.5), -(innerRadius + capsuleHeight), capsuleWidth,
                                    capsuleHeight, capsuleRadius, capsuleRadius);
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

    CLoadIndicatorEnabled::CLoadIndicatorEnabled(QWidget *usingWidget) : m_usingWidget(usingWidget)
    {
        Q_ASSERT_X(usingWidget, Q_FUNC_INFO, "need widget");
    }

    bool CLoadIndicatorEnabled::isShowingLoadIndicator() const
    {
        return m_loadIndicator && m_usingWidget->isVisible() && m_loadIndicator->isAnimated();
    }

    bool CLoadIndicatorEnabled::isLoadInProgress() const { return m_loadInProgress; }

    void CLoadIndicatorEnabled::showLoading(std::chrono::milliseconds timeout, bool processEvents)
    {
        if (!m_loadIndicator)
        {
            m_loadIndicator = new CLoadIndicator(64, 64, m_usingWidget);
            QObject::connect(m_loadIndicator, &CLoadIndicator::timedOut, [this] { this->indicatorTimedOut(); });
        }

        this->centerLoadIndicator();
        m_indicatorId = m_loadIndicator->startAnimation(timeout, processEvents);
    }

    void CLoadIndicatorEnabled::hideLoading()
    {
        if (m_loadIndicator) { m_loadIndicator->stopAnimation(); }
    }

    void CLoadIndicatorEnabled::centerLoadIndicator()
    {
        if (!m_loadIndicator) { return; }
        const QPoint middle = m_usingWidget->visibleRegion().boundingRect().center();
        m_loadIndicator->centerLoadIndicator(middle);
    }

    void CLoadIndicatorEnabled::indicatorTimedOut()
    {
        // to be overridden
    }
} // namespace swift::gui
