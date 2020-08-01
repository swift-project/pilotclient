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
#include <QElapsedTimer>
#include <QFrame>

class QPaintEvent;
class QTimer;

namespace BlackGui
{
    //! Widget which displays a audio level meter, indicating the
    //! level and peak levels of the window of audio samples most recently analyzed
    class BLACKGUI_EXPORT CLevelMeter : public QFrame
    {
        Q_OBJECT

        //! Properties
        //! @{
        Q_PROPERTY(QColor lowColor  READ getLowColor  WRITE setLowColor)
        Q_PROPERTY(QColor highColor READ getHighColor WRITE setHighColor)
        Q_PROPERTY(QColor peakColor READ getPeakColor WRITE setPeakColor)
        //! @}

    public:
        //! Constructor
        CLevelMeter(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CLevelMeter() override;

        //! \copydoc QWidget::paintEvent
        void paintEvent(QPaintEvent *event) override;

        //! Clean up
        void reset();

        //! Values
        void levelChanged(double level);

        //! Color properties
        //! @{
        const QColor &getLowColor()  const { return m_lowColor; }
        const QColor &getHighColor() const { return m_highColor; }
        const QColor &getPeakColor() const { return m_peakColor; }
        void setLowColor(const QColor &color)  { m_lowColor = color; }
        void setHighColor(const QColor &color) { m_highColor = color; }
        void setPeakColor(const QColor &color) { m_peakColor = color; }
        //! @}

    private:
        //! Timer expired
        void redrawTimerExpired();

        const int RedrawInterval        = 100;  // ms
        const int PeakHoldLevelDuration = 1000; // ms

        //! Range 0.0 - 1.0.
        double m_level = 0.0;

        //! Most recent peak level, range 0.0 - 1.0.
        double m_peakLevel = 0.0;

        //! Time at which m_peakLevel was last changed.
        QElapsedTimer m_peakLevelChanged;

        static constexpr double High       = 0.60;
        static constexpr double DecayValue = 0.10;
        QTimer *m_redrawTimer = nullptr;
        QColor m_lowColor;  //!< color for low values
        QColor m_highColor; //!< color for high values
        QColor m_peakColor; //!< color for peak values
    };
} // ns

#endif // guard
