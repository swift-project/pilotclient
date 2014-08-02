/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
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

#include "led.h"

#include <QColor>
#include <QtGlobal>
#include <QtGui>
#include <QPolygon>
#include <QtSvg>
#include <QSvgRenderer>


namespace BlackGui
{
    CLed::CLed(QWidget *parent) : QWidget(parent)
    {
        this->setLed();
    }

    CLed::CLed(bool on, CLed::LedColors onColor, CLed::LedColors offColor, CLed::LedShapes shape, QWidget *parent) :
        QWidget(parent), m_value(on), m_onColor(onColor), m_offColor(offColor), m_shape(shape)
    {
        this->setLed();
    }

    void CLed::setLed(LedColors tempColor)
    {
        // load image, init renderer
        QString ledShapeAndColor;
        ledShapeAndColor = shapes().at(static_cast<int>(this->m_shape));
        if (tempColor == NoColor)
        {
            if (m_value)
            {
                this->setToolTip(this->m_tooltipOn);
                ledShapeAndColor.append(CLed::colorString(this->m_onColor));
            }
            else
            {
                this->setToolTip(this->m_tooltipOff);
                ledShapeAndColor.append(CLed::colorString(this->m_offColor));
            }
        }
        else
        {
            this->setToolTip("transition");
            ledShapeAndColor.append(CLed::colorString(tempColor));
        }

        // init renderer, allow re-init
        bool firstTime = false;
        if (!m_renderer)
        {
            firstTime = true;
            m_renderer = new QSvgRenderer(this);
        }

        m_renderer->load(ledShapeAndColor);

        // original size
        QSize s = m_renderer->defaultSize();
        this->m_whRatio = s.width() / s.height();

        // size
        if (this->m_targetWidth < 0)
        {
            this->m_targetWidth = widths().at(static_cast<int>(m_shape));
        }
        double w = this->m_targetWidth;
        double h = w / this->m_whRatio;
        this->setFixedHeight(qRound(h));
        this->setFixedWidth(qRound(w));

        if (!firstTime)
        {
            // re-init
            this->update();
        }
    }

    const QString &CLed::colorString(CLed::LedColors color)
    {
        static const QString empty;
        if (color == NoColor) return empty;
        return colors().at(static_cast<int>(color));
    }

    CLed::~CLed()
    {
        delete m_renderer;
    }

    void CLed::setToolTips(const QString &on, const QString &off)
    {
        this->m_tooltipOn = on;
        this->m_tooltipOff = off;
        if (this->m_value)
        {
            this->setToolTip(on);
        }
        else
        {
            this->setToolTip(off);
        }
    }

    void CLed::setOnToolTip(const QString &on)
    {
        this->setToolTips(on, this->m_tooltipOff);
    }

    void CLed::paintEvent(QPaintEvent *)
    {
        // init style sheets with this widget
        QStyleOption opt;
        opt.init(this);

        // paint
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setBackgroundMode(Qt::TransparentMode);
        m_renderer->render(&painter);
    }

    void CLed::setOnColor(LedColors color)
    {
        if (color == this->m_onColor) return;
        m_onColor = color;
        setLed();
    }

    void CLed::setOffColor(LedColors color)
    {
        if (color == this->m_offColor) return;
        m_offColor = color;
        setLed();
    }

    void CLed::setTemporaryColor(CLed::LedColors color)
    {
        setLed(color);
    }

    void CLed::setShape(LedShapes newShape)
    {
        if (newShape == this->m_shape) return;
        m_shape = newShape;
        setLed();
    }

    void CLed::setValues(CLed::LedColors onColor, CLed::LedColors offColor, CLed::LedShapes shape, const QString &toolTipOn, const QString &toolTipOff, int width)
    {
        m_onColor = onColor;
        m_offColor = offColor;
        m_shape = shape;
        m_tooltipOn = toolTipOn;
        m_tooltipOff = toolTipOff;
        m_targetWidth = width;
        setLed();
    }

    void CLed::setValue(bool value)
    {
        if (value == m_value) return;
        m_value = value;
        setLed();
    }

    void CLed::toggleValue()
    {
        m_value = !m_value;
        setLed();
    }

    const QStringList &CLed::shapes()
    {
        static const QStringList shapes( {":/qled/icons/qled/circle_" , ":/qled/icons/qled/square_" , ":/qled/icons/qled/triang_" , ":/qled/icons/qled/round_"});
        return shapes;
    }

    const QStringList &CLed::colors()
    {
        static const QStringList colors( { "red.svg", "green.svg", "yellow.svg", "grey.svg", "orange.svg", "purple.svg", "blue.svg", "black.svg" });
        return colors;
    }

    const QList<int> &CLed::widths()
    {
        static const QList<int> widths({ 16 , 16, 16, 16});
        return widths;
    }
}
