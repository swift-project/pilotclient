/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 *
 * Class based on qt example: Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies)
 */

#include "blackgui/levelmeter.h"

#include <math.h>
#include <QPainter>
#include <QRect>
#include <QSizePolicy>
#include <QTimer>
#include <Qt>
#include <QtGlobal>

namespace BlackGui
{
    CLevelMeter::CLevelMeter(QWidget *parent)
        : QFrame(parent),
          m_redrawTimer(new QTimer(this)),
          m_lowColor(Qt::gray),
          m_highColor(Qt::green),
          m_peakColor(255, 200, 200, 255)
    {
        this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        this->setMinimumWidth(30);

        connect(m_redrawTimer, &QTimer::timeout, this, &CLevelMeter::redrawTimerExpired);
        m_redrawTimer->start(RedrawInterval);
        m_redrawTimer->setObjectName("CLevelMeter");
    }

    CLevelMeter::~CLevelMeter()
    {}

    void CLevelMeter::reset()
    {
        m_level = 0.0;
        m_peakLevel = 0.0;
        this->update();
    }

    void CLevelMeter::levelChanged(double level)
    {
        m_level = level;
        if (level > m_peakLevel)
        {
            m_peakLevel = level;
            m_peakLevelChanged.start();
        }
    }

    void CLevelMeter::redrawTimerExpired()
    {
        // decay
        if (m_peakLevelChanged.elapsed() > PeakHoldLevelDuration)
        {
            m_peakLevel -= DecayValue;
            if (m_peakLevel < m_level || m_peakLevel < 0)
            {
                m_peakLevel = 0.0;
            }
        }
        this->update();
    }

    void CLevelMeter::paintEvent(QPaintEvent *event)
    {
        Q_UNUSED(event)

        QPainter painter(this);
        QRect bar = this->contentsRect();
        painter.fillRect(bar, Qt::transparent);
        const int w = bar.width();
        const int left = rect().left();
        const int level = qRound(m_level * w);

        bar.setLeft(left);
        bar.setRight(level);
        painter.fillRect(bar, m_peakLevel >= High ? m_highColor : m_lowColor);

        if (m_peakLevel > m_level)
        {
            const int peak = qRound(m_peakLevel * w);
            bar.setLeft(level);
            bar.setRight(peak);
            painter.fillRect(bar, m_peakColor);
        }
    }

} // namespace
