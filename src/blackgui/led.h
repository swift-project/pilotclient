/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 *
 * Class based on qLed: Copyright (C) 2010 by P. Sereno, http://www.sereno-online.com
 */

//! \file

#ifndef BLACKGUI_LED_H
#define BLACKGUI_LED_H

#include <Qt>
#include <QWidget>
#include <QSvgRenderer>
#include <QColor>

namespace BlackGui
{
    //! Displaying an LED as widget. Non copyable.
    //! \remarks Based on qLed
    class CLedWidget : public QWidget
    {
        Q_OBJECT
        Q_ENUMS(LedColor)
        Q_ENUMS(LedShape)

    public:

        //! Colors
        //! \remarks None has to be last entry
        enum LedColor { Red = 0, Green, Yellow, Grey, Orange, Purple, Blue, Black, NoColor};

        //! Shapes
        enum LedShape { Circle = 0, Square, Triangle, Rounded};

        //! States
        enum State { On, Off, TriState };

        //! Default constructor
        CLedWidget(QWidget *parent = nullptr);

        //! Constructor
        CLedWidget(bool on, LedColor onColor, LedColor offColor, LedShape shape, const QString &onName = "on", const QString &offName = "off", QWidget *parent = nullptr);

        //! Destructor
        virtual ~CLedWidget();

        //! Value
        bool value() const { return m_value; }

        //! Allows to set the led value {true, false}
        void setOn(bool on);

        //! Sets the 3rd state
        void setTriState();

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
        void setTargetWidth(int width) { this->m_widthTarget = width; }

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
        void setTriStateToolTip(const QString &triState);

        //! New values dual state
        void setValues(LedColor onColor, LedColor offColor, LedShape shape, const QString &toolTipOn, const QString &toolTipOff, int width = -1);

        //! New values tri-state
        void setValues(LedColor onColor, LedColor offColor, LedColor triStateColor, LedShape shape, const QString &toolTipOn, const QString &toolTipOff, const QString &toolTipTriState, int width = -1);

        //! Render as pixmap, so it can be used with TableViews
        QPixmap asPixmap() const;

    protected:
        State m_value = Off;         //!< current value
        LedColor m_colorOn = Yellow; //!< On color
        LedColor m_colorOff = Black; //!< Off color
        LedColor m_colorTriState = Blue; //!< tri-state color
        LedShape m_shape = Circle;   //!< shape
        double m_whRatio = 1.0;      //!< width/height ratio
        int m_widthTarget = -1;      //!< desired width
        int m_heightCalculated = 1;  //!< calculated height

        QString m_tooltipOn = "on";        //!< tooltip when on
        QString m_tooltipOff = "off";      //!< tooltip when off
        QString m_tooltipTriState = "tri-state"; //!< tooltip tri state
        QString m_currentToolTip = "off";  //!< currently used tooltip
        QScopedPointer<QSvgRenderer> m_renderer; //!< Renderer

        //! Paint event
        virtual void paintEvent(QPaintEvent *event) override;

        //! Set / init LED
        void setLed(LedColor ledColor = NoColor);

        //! Render to pixmap
        QPixmap renderToPixmap() const;

        //! All shapes
        static const QStringList &shapes();

        //! All colors
        static const QStringList &colors();

        //! All target widths
        static const QList<int> &widths();

        //! Color string
        static const QString &colorString(LedColor color); //!<Color string

    private:
        //! Fix widget after widths calculated
        void firstTimeReInit();

    };
}
#endif
