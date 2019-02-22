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
        :   QWidget(parent)
        ,   m_peakDecayRate(PeakDecayRate)
        ,   m_redrawTimer(new QTimer(this))
        ,   m_rmsColor(Qt::red)
        ,   m_peakColor(255, 200, 200, 255)
    {
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        setMinimumWidth(30);

        connect(m_redrawTimer, &QTimer::timeout, this, &CLevelMeter::ps_redrawTimerExpired);
        m_redrawTimer->start(RedrawInterval);
        m_redrawTimer->setObjectName("CLevelMeter");
    }

    CLevelMeter::~CLevelMeter()
    { }

    void CLevelMeter::reset()
    {
        m_rmsLevel = 0.0;
        m_peakLevel = 0.0;
        update();
    }

    void CLevelMeter::levelChanged(double rmsLevel, double peakLevel, int numSamples)
    {
        // Smooth the RMS signal
        const double smooth = pow(double(0.9), static_cast<double>(numSamples) / 256); // TODO: remove this magic number
        m_rmsLevel = (m_rmsLevel * smooth) + (rmsLevel * (1.0 - smooth));

        if (peakLevel > m_decayedPeakLevel)
        {
            m_peakLevel = peakLevel;
            m_decayedPeakLevel = peakLevel;
            m_peakLevelChanged.start();
        }

        if (peakLevel > m_peakHoldLevel)
        {
            m_peakHoldLevel = peakLevel;
            m_peakHoldLevelChanged.start();
        }

        update();
    }

    void CLevelMeter::ps_redrawTimerExpired()
    {
        // Decay the peak signal
        const int elapsedMs = m_peakLevelChanged.elapsed();
        const double decayAmount = m_peakDecayRate * elapsedMs;
        if (decayAmount < m_peakLevel)
        {
            m_decayedPeakLevel = m_peakLevel - decayAmount;
        }
        else
        {
            m_decayedPeakLevel = 0.0;
        }

        // Check whether to clear the peak hold level
        if (m_peakHoldLevelChanged.elapsed() > PeakHoldLevelDuration)
            m_peakHoldLevel = 0.0;

        update();
    }

    void CLevelMeter::paintEvent(QPaintEvent *event)
    {
        Q_UNUSED(event)

        QPainter painter(this);
        painter.fillRect(rect(), Qt::black);

        QRect bar = rect();

        bar.setTop(rect().top() + (1.0 - m_peakHoldLevel) * rect().height());
        bar.setBottom(bar.top() + 5);
        painter.fillRect(bar, m_rmsColor);
        bar.setBottom(rect().bottom());

        bar.setTop(rect().top() + (1.0 - m_decayedPeakLevel) * rect().height());
        painter.fillRect(bar, m_peakColor);

        bar.setTop(rect().top() + (1.0 - m_rmsLevel) * rect().height());
        painter.fillRect(bar, m_rmsColor);
    }

} // namespace
