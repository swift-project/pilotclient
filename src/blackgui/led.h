/***************************************************************************
 *   Copyright (C) 2010 by P. Sereno                                       *
 *   http://www.sereno-online.com                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License           *
 *   version 2.1 as published by the Free Software Foundation              *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Lesser General Public License for more details.                   *
 *   http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.               *
 ***************************************************************************/

#ifndef BLACKGUI_LED_H
#define BLACKGUI_LED_H

#include <Qt>
#include <QWidget>
#include <QtDesigner/QDesignerExportWidget>

class QColor;
class QSvgRenderer;

namespace BlackGui
{
    //! Displaying an LED
    //! \remarks Based on qLed
    class CLed : public QWidget
    {
        Q_OBJECT
        Q_ENUMS(LedColors)
        Q_ENUMS(LedShapes)

        //! Value on/off
        Q_PROPERTY(bool value READ value WRITE setValue)
        //! Color when on
        Q_PROPERTY(LedColors onColor READ onColor WRITE setOnColor)
        //! Color when off
        Q_PROPERTY(LedColors offColor READ offColor WRITE setOffColor)
        //! Shape
        Q_PROPERTY(LedShapes shape READ shape WRITE setShape)

    public:

        //! Colors
        //! \remarks None has to be last entry
        enum LedColors { Red = 0, Green, Yellow, Grey, Orange, Purple, Blue, Black, NoColor};

        //! Shapes
        enum LedShapes { Circle = 0, Square, Triangle, Rounded};

        //! Constructor
        CLed(QWidget *parent = nullptr);

        //! Constructor
        CLed(bool on, LedColors onColor, LedColors offColor, LedShapes shape, QWidget *parent = nullptr);

        //! Destructor
        virtual ~CLed();

        //! Value
        bool value() const { return m_value; }

        //! On color
        LedColors onColor() const { return m_onColor; }

        //! Off color
        LedColors offColor() const { return m_offColor; }

        //! Shape
        LedShapes shape() const { return m_shape; }

        //! Allows to set the led value {true,false}
        void setValue(bool);

        //! Allows to change the On color {Red,Green,Yellow,Grey,Orange,Purple,blue}
        void setOnColor(LedColors color);

        //! Allows to change the Off color {Red,Green,Yellow,Grey,Orange,Purple,blue}
        void setOffColor(LedColors color);

        //! Temporary color until next value change
        void setTemporaryColor(LedColors color);

        //! Tool tips
        void setToolTips(const QString &on, const QString &off);

        //! On tool tip
        void setOnToolTip(const QString &on);

        //! Allows to change the led shape {Circle,Square,Triangle,Rounded rectangle}
        void setShape(LedShapes);

        //! Target width
        void setTargetWidth(int width) { this->m_targetWidth = width; }

        //! New values
        void setValues(LedColors onColor, LedColors offColor, LedShapes shape, const QString &toolTipOn, const QString &toolTipOff, int width = -1);

        //! Toggle on / off
        void toggleValue();

    protected:
        bool m_value = false;        //!< current value
        LedColors m_onColor = Red;   //!< On color
        LedColors m_offColor = Grey; //!< Off color
        LedShapes m_shape = Circle;  //!< shape
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
        void setLed(LedColors tempColor = NoColor); //!< Init LED
        static const QString &colorString(LedColors color); //!<Color string
    };
}
#endif
