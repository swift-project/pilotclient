/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 *
 * Copyright (C) 2010 by P. Sereno
 * http://www.sereno-online.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation
 */

//! \file

#ifndef BLACKGUI_LED_H
#define BLACKGUI_LED_H

#include <Qt>
#include <QWidget>

class QColor;
class QSvgRenderer;

namespace BlackGui
{
    //! Displaying an LED
    //! \remarks Based on qLed
    class CLed : public QWidget
    {
        Q_OBJECT
        Q_ENUMS(LedColor)
        Q_ENUMS(LedShape)

        //! Value on/off
        Q_PROPERTY(bool value READ value WRITE setOn)
        //! Color when on
        Q_PROPERTY(LedColor onColor READ onColor WRITE setOnColor)
        //! Color when off
        Q_PROPERTY(LedColor offColor READ offColor WRITE setOffColor)
        //! Shape
        Q_PROPERTY(LedShape shape READ shape WRITE setShape)

    public:

        //! Colors
        //! \remarks None has to be last entry
        enum LedColor { Red = 0, Green, Yellow, Grey, Orange, Purple, Blue, Black, NoColor};

        //! Shapes
        enum LedShape { Circle = 0, Square, Triangle, Rounded};

        //! States
        enum State { On, Off, Temporary };

        //! Constructor
        CLed(QWidget *parent = nullptr);

        //! Constructor
        CLed(bool on, LedColor onColor, LedColor offColor, LedShape shape, QWidget *parent = nullptr);

        //! Destructor
        virtual ~CLed();

        //! Value
        bool value() const { return m_value; }

        //! On color
        LedColor onColor() const { return m_onColor; }

        //! Off color
        LedColor offColor() const { return m_offColor; }

        //! Shape
        LedShape shape() const { return m_shape; }

        //! Allows to set the led value {true,false}
        void setOn(bool on);

        //! Allows to change the On color {Red,Green,Yellow,Grey,Orange,Purple,blue}
        void setOnColor(LedColor color);

        //! Allows to change the Off color {Red,Green,Yellow,Grey,Orange,Purple,blue}
        void setOffColor(LedColor color);

        //! Temporary color until next value change
        void setTemporaryColor(LedColor color);

        //! Tool tips
        void setToolTips(const QString &on, const QString &off);

        //! On tool tip
        void setOnToolTip(const QString &on);

        //! Allows to change the led shape {Circle,Square,Triangle,Rounded rectangle}
        void setShape(LedShape);

        //! Target width
        void setTargetWidth(int width) { this->m_targetWidth = width; }

        //! New values
        void setValues(LedColor onColor, LedColor offColor, LedShape shape, const QString &toolTipOn, const QString &toolTipOff, int width = -1);

        //! Toggle on / off
        void toggleValue();

    protected:
        State m_value = Off;         //!< current value
        LedColor m_onColor = Red;    //!< On color
        LedColor m_offColor = Grey;  //!< Off color
        LedShape m_shape = Circle;   //!< shape
        double m_whRatio = 1.0;      //!< width/height ratio
        int m_targetWidth = -1;      //!< TargetWidth

        //! All shapes
        static const QStringList &shapes();

        //! All colors
        static const QStringList &colors();

        //! All target widths
        static const QList<int> &widths();

        //! Paint event
        void paintEvent(QPaintEvent *event);

    private:
        QSvgRenderer *m_renderer = nullptr;         //!< Renderer
        QString m_tooltipOn;
        QString m_tooltipOff;
        void setLed(LedColor ledColor = NoColor); //!< Init LED
        static const QString &colorString(LedColor color); //!<Color string
    };
}
#endif
