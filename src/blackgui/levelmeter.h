/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 *
 * Class based on qt example: Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies)
 */

//! \file

#ifndef BLACKGUI_LEVELMETER_H
#define BLACKGUI_LEVELMETER_H

#include "blackgui/blackguiexport.h"

#include <QColor>
#include <QObject>
#include <QTime>
#include <QWidget>

class QPaintEvent;
class QTimer;

namespace BlackGui
{
    //! Widget which displays a vertical audio level meter, indicating the
    //! RMS and peak levels of the window of audio samples most recently analyzed
    //! by the Engine.
    class BLACKGUI_EXPORT CLevelMeter : public QWidget
    {
        Q_OBJECT
    public:
        //! Constructor
        CLevelMeter(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CLevelMeter();

        //! \copydoc QWidget::paintEvent
        void paintEvent(QPaintEvent *event) override;

    public slots:
        //! Clean up
        void reset();

        //! Values
        void levelChanged(double rmsLevel, double peakLevel, int numSamples);

    private slots:
        void ps_redrawTimerExpired();

    private:
        const int RedrawInterval        = 100; // ms
        const double PeakDecayRate      = 0.001;
        const int PeakHoldLevelDuration = 2000; // ms

        //! Height of RMS level bar, range 0.0 - 1.0.
        double m_rmsLevel = 0.0;

        //! Most recent peak level, range 0.0 - 1.0.
        double m_peakLevel = 0.0;

        //! Height of peak level bar.
        //! This is calculated by decaying m_peakLevel depending on the elapsed time since m_peakLevelChanged, and the value of m_decayRate.
        double m_decayedPeakLevel = 0.0;

        //! Time at which m_peakLevel was last changed.
        QTime m_peakLevelChanged;

        //! Rate at which peak level bar decays.  Expressed in level units / millisecond.
        double m_peakDecayRate;

        //! High watermark of peak level. Range 0.0 - 1.0.
        double m_peakHoldLevel = 0.0;

        //! Time at which m_peakHoldLevel was last changed.
        QTime m_peakHoldLevelChanged;

        QTimer *m_redrawTimer = nullptr;
        QColor m_rmsColor;
        QColor m_peakColor;
    };
}

#endif // guard
