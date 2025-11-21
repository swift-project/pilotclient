// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1
// Class based on qt example: Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies)

//! \file

#ifndef SWIFT_GUI_LEVELMETER_H
#define SWIFT_GUI_LEVELMETER_H

#include <QColor>
#include <QElapsedTimer>
#include <QFrame>
#include <QObject>

#include "gui/swiftguiexport.h"

class QPaintEvent;
class QTimer;

namespace swift::gui
{
    //! Widget which displays a audio level meter, indicating the
    //! level and peak levels of the window of audio samples most recently analyzed
    class SWIFT_GUI_EXPORT CLevelMeter : public QFrame
    {
        Q_OBJECT

        //! @{
        //! Properties
        Q_PROPERTY(QColor lowColor READ getLowColor WRITE setLowColor)
        Q_PROPERTY(QColor highColor READ getHighColor WRITE setHighColor)
        Q_PROPERTY(QColor peakColor READ getPeakColor WRITE setPeakColor)
        //! @}

    public:
        //! Constructor
        CLevelMeter(QWidget *parent = nullptr);

        //! Destructor
        ~CLevelMeter() override;

        //! \copydoc QWidget::paintEvent
        void paintEvent(QPaintEvent *event) override;

        //! Clean up
        void reset();

        //! Values
        void levelChanged(double level);

        //! @{
        //! Color properties
        const QColor &getLowColor() const { return m_lowColor; }
        const QColor &getHighColor() const { return m_highColor; }
        const QColor &getPeakColor() const { return m_peakColor; }
        void setLowColor(const QColor &color) { m_lowColor = color; }
        void setHighColor(const QColor &color) { m_highColor = color; }
        void setPeakColor(const QColor &color) { m_peakColor = color; }
        //! @}

    private:
        //! Timer expired
        void redrawTimerExpired();

        const int RedrawInterval = 100; // ms
        const int PeakHoldLevelDuration = 1000; // ms

        //! Range 0.0 - 1.0.
        double m_level = 0.0;

        //! Most recent peak level, range 0.0 - 1.0.
        double m_peakLevel = 0.0;

        //! Time at which m_peakLevel was last changed.
        QElapsedTimer m_peakLevelChanged;

        static constexpr double High = 0.60;
        static constexpr double DecayValue = 0.10;
        QTimer *m_redrawTimer = nullptr;
        QColor m_lowColor; //!< color for low values
        QColor m_highColor; //!< color for high values
        QColor m_peakColor; //!< color for peak values
    };
} // namespace swift::gui

#endif // SWIFT_GUI_LEVELMETER_H
