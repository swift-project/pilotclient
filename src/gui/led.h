// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1
// Class based on qLed: Copyright (C) 2010 by P. Sereno, http://www.sereno-online.com

//! \file

#ifndef SWIFT_GUI_LEDWIDGET_H
#define SWIFT_GUI_LEDWIDGET_H

#include <QList>
#include <QObject>
#include <QPixmap>
#include <QScopedPointer>
#include <QString>
#include <QTimer>
#include <QWidget>

#include "gui/swiftguiexport.h"

class QMouseEvent;
class QPaintEvent;
class QSvgRenderer;

namespace swift::gui
{
    //! Displaying an LED as widget. Non copyable.
    //! \remarks Based on qLed
    class SWIFT_GUI_EXPORT CLedWidget : public QWidget
    {
        Q_OBJECT

    public:
        //! Colors
        //! \remarks NoColor has to be last entry
        enum LedColor
        {
            Red = 0,
            Green,
            Yellow,
            Grey,
            Orange,
            Purple,
            Blue,
            Black,
            NoColor
        };
        Q_ENUM(LedColor)

        //! Shapes
        enum LedShape
        {
            Circle = 0,
            Square,
            Triangle,
            Rounded
        };
        Q_ENUM(LedShape)

        //! States
        enum State
        {
            On,
            Off,
            TriState
        };

        //! Default constructor
        CLedWidget(QWidget *parent = nullptr);

        //! Constructor
        CLedWidget(bool on, LedColor onColor, LedColor offColor, LedShape shape, const QString &onName = "on", const QString &offName = "off", int targetWidth = -1, QWidget *parent = nullptr);

        //! Destructor
        virtual ~CLedWidget() override;

        //! Value
        bool value() const { return m_blinkState; }

        //! Allows to set the led value {true, false}
        void setOn(bool on) { this->setOn(on, -1); }

        //! Set to on for resetTimeMs
        void blink(int resetTimeMs = 500);

        //! Sets the 3rd state
        void setTriState(int resetTimeMs = -1);

        //! Toggle on / off
        void toggleValue();

        //! Shape
        LedShape shape() const { return m_shape; }

        //! On color
        LedColor onColor() const { return m_colorOn; }

        //! Off color
        LedColor offColor() const { return m_colorOff; }

        //! Tri-state color
        LedColor triStateColor() const { return m_colorTriState; }

        //! Allows to change the On color {Red,Green,Yellow,Grey,Orange,Purple,blue}
        void setOnColor(LedColor color);

        //! Allows to change the Off color {Red,Green,Yellow,Grey,Orange,Purple,blue}
        void setOffColor(LedColor color);

        //! Temporary color until next value change
        void setTriStateColor(LedColor color);

        //! Allows to change the led shape {Circle,Square,Triangle,Rounded rectangle}
        void setShape(LedShape);

        //! Target width
        void setTargetWidth(int width)
        {
            this->m_widthTarget = width;
            this->setLed();
        }

        //! Tool tip
        QString getOnToolTip() const { return m_tooltipOn; }

        //! Tool tip
        QString getOffToolTip() const { return m_tooltipOff; }

        //! Tool tip
        QString getTriStateToolTip() const { return m_tooltipTriState; }

        //! Tool tips
        void setToolTips(const QString &on, const QString &off, const QString &triState = "tri-state");

        //! On tool tip
        void setOnToolTip(const QString &on);

        //! Off tool tip
        void setOffToolTip(const QString &off);

        //! Tri-state tool tip
        void setTriStateToolTip(const QString &triStateTooltip);

        //! Tri-state
        void setTriStateValues(LedColor color, const QString &tooltip);

        //! New values dual state
        void setValues(LedColor onColor, LedColor offColor, LedShape shape, const QString &toolTipOn, const QString &toolTipOff, int width = -1);

        //! New values tri-state
        void setValues(LedColor onColor, LedColor offColor, LedColor triStateColor, LedShape shape, const QString &toolTipOn, const QString &toolTipOff, const QString &toolTipTriState, int width = -1);

        //! Render as pixmap, so it can be used with TableViews
        QPixmap asPixmap() const;

    signals:
        //! LED clicked
        void clicked();

    private:
        State m_blinkState = Off; //!< current state, can be different from value when blinking
        State m_value = Off; //!< explicit value
        LedColor m_colorOn = Yellow; //!< On color
        LedColor m_colorOff = Black; //!< Off color
        LedColor m_colorTriState = Blue; //!< tri-state color
        LedShape m_shape = Circle; //!< shape
        double m_whRatio = 1.0; //!< width/height ratio
        int m_widthTarget = -1; //!< desired width
        int m_heightCalculated = 1; //!< calculated height

        QString m_tooltipOn = "on"; //!< tooltip when on
        QString m_tooltipOff = "off"; //!< tooltip when off
        QString m_tooltipTriState = "tri-state"; //!< tooltip tri-state
        QString m_currentToolTip = "off"; //!< currently used tooltip
        QScopedPointer<QSvgRenderer> m_renderer; //!< Renderer
        QTimer m_resetTimer; //!< reset state

        //! Init
        void init();

        //! Allows to set the led value {true, false}
        void setOn(bool on, int resetTimeMs);

        //! Reset the state
        void resetState();

        //! Paint event
        virtual void paintEvent(QPaintEvent *event) override;

        //! Mouse pressed
        virtual void mousePressEvent(QMouseEvent *event) override;

        //! Set / init LED
        void setLed(LedColor ledColor = NoColor);

        //! Render to pixmap
        QPixmap renderToPixmap() const;

        //! All shapes
        static const QStringList &shapes();

        //! All color files
        static const QStringList &colorFiles();

        //! All target widths
        static const QList<int> &widths();

        //! Color string
        static const QString &colorString(LedColor color); //!< Color string
    };
} // namespace swift::gui
#endif
