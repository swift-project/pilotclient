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

#include "led.h"

#include <QColor>
#include <QtGlobal>
#include <QtGui>
#include <QPolygon>
#include <QtSvg>
#include <QSvgRenderer>
#include <QImage>

namespace BlackGui
{
    CLedWidget::CLedWidget(QWidget *parent) : QWidget(parent), m_renderer(new QSvgRenderer)
    {
        this->setLed();
    }

    CLedWidget::CLedWidget(bool on, LedColor onColor, LedColor offColor, LedShape shape, const QString &onName, const QString &offName, QWidget *parent) :
        m_value(on ? On : Off), m_colorOn(onColor), m_colorOff(offColor),
        m_shape(shape), m_tooltipOn(onName), m_tooltipOff(offName), m_renderer(new QSvgRenderer(parent))
    {
        this->setLed();
    }

    CLedWidget::~CLedWidget()
    { }

    void CLedWidget::setLed(LedColor ledColor)
    {
        Q_ASSERT(!this->m_renderer.isNull());

        // load image, init renderer
        QString ledShapeAndColor;
        ledShapeAndColor = shapes().at(static_cast<int>(this->m_shape));
        if (ledColor == NoColor)
        {
            if (m_value == On)
            {
                this->m_currentToolTip = this->m_tooltipOn;
                ledShapeAndColor.append(CLedWidget::colorString(this->m_colorOn));
            }
            else
            {
                this->m_currentToolTip = this->m_tooltipOff;
                ledShapeAndColor.append(CLedWidget::colorString(this->m_colorOff));
            }
        }
        else
        {
            this->m_currentToolTip = "transition";
            ledShapeAndColor.append(CLedWidget::colorString(ledColor));
        }
        this->setToolTip(this->m_currentToolTip); // for widget

        // init renderer, load led.
        m_renderer->load(ledShapeAndColor);

        // original size
        QSize s = m_renderer->defaultSize();
        this->m_whRatio = s.width() / s.height();

        // size
        if (this->m_widthTarget < 0)
        {
            this->m_widthTarget = widths().at(static_cast<int>(m_shape));
        }
        double h = this->m_widthTarget / this->m_whRatio;
        this->m_heightCalculated = qRound(h);

        this->setFixedHeight(this->m_heightCalculated);
        this->setFixedWidth(this->m_widthTarget);
        this->update();
    }

    QPixmap CLedWidget::renderToPixmap() const
    {
        Q_ASSERT(!this->m_renderer.isNull());

        // Prepare a QImage with desired characteritiscs
        QImage image(QSize(this->m_widthTarget, this->m_heightCalculated), QImage::Format_ARGB32);
        image.fill(qRgba(0, 0, 0, 0)); // transparent background

        // Get QPainter that paints to the image
        QPainter painter(&image);
        this->m_renderer->render(&painter);
        return QPixmap::fromImage(image);
    }

    const QString &CLedWidget::colorString(CLedWidget::LedColor color)
    {
        static const QString empty;
        if (color == NoColor) return empty;
        return colors().at(static_cast<int>(color));
    }

    void CLedWidget::setToolTips(const QString &on, const QString &off)
    {
        this->m_tooltipOn = on;
        this->m_tooltipOff = off;
        this->setLed();
    }

    void CLedWidget::setOnToolTip(const QString &on)
    {
        this->setToolTips(on, this->m_tooltipOff);
    }

    void CLedWidget::setOnColor(LedColor color)
    {
        if (color == this->m_colorOn) return;
        m_colorOn = color;
        setLed();
    }

    void CLedWidget::setOffColor(LedColor color)
    {
        if (color == this->m_colorOff) return;
        m_colorOff = color;
        setLed();
    }

    void CLedWidget::setTemporaryColor(CLedWidget::LedColor color)
    {
        m_value = Temporary;
        setLed(color);
    }

    void CLedWidget::setShape(LedShape newShape)
    {
        if (newShape == this->m_shape) return;
        m_shape = newShape;
        setLed();
    }

    void CLedWidget::setValues(LedColor onColor, LedColor offColor, LedShape shape, const QString &toolTipOn, const QString &toolTipOff, int width)
    {
        m_colorOn = onColor;
        m_colorOff = offColor;
        m_shape = shape;
        m_tooltipOn = toolTipOn;
        m_tooltipOff = toolTipOff;
        m_widthTarget = width;
        setLed();
    }

    QPixmap CLedWidget::asPixmap() const
    {
        return this->renderToPixmap();
    }

    void CLedWidget::setOn(bool on)
    {
        State s = on ? On : Off;
        if (m_value == s) return;
        m_value = s;
        setLed();
    }

    void CLedWidget::toggleValue()
    {
        m_value = (m_value == Off) ? m_value = On : m_value = Off;
        setLed();
    }

    void CLedWidget::paintEvent(QPaintEvent *)
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

    const QStringList &CLedWidget::shapes()
    {
        static const QStringList shapes( {":/qled/icons/qled/circle_" , ":/qled/icons/qled/square_" , ":/qled/icons/qled/triang_" , ":/qled/icons/qled/round_"});
        return shapes;
    }

    const QStringList &CLedWidget::colors()
    {
        static const QStringList colors( { "red.svg", "green.svg", "yellow.svg", "grey.svg", "orange.svg", "purple.svg", "blue.svg", "black.svg" });
        return colors;
    }

    const QList<int> &CLedWidget::widths()
    {
        static const QList<int> widths({ 16, 16, 16, 16});
        return widths;
    }
}
