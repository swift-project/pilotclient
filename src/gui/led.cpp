// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1
// Class based on qLed: Copyright (C) 2010 by P. Sereno, http://www.sereno-online.com

#include "gui/led.h"

#include <QImage>
#include <QList>
#include <QMouseEvent>
#include <QPainter>
#include <QPointer>
#include <QRgb>
#include <QSize>
#include <QStringList>
#include <QStyleOption>
#include <QSvgRenderer>
#include <Qt>
#include <QtGlobal>

namespace swift::gui
{
    CLedWidget::CLedWidget(QWidget *parent) : QWidget(parent), m_renderer(new QSvgRenderer)
    {
        this->setLed();
        this->init();
    }

    CLedWidget::CLedWidget(bool on, LedColor onColor, LedColor offColor, LedShape shape, const QString &onName,
                           const QString &offName, int targetWidth, QWidget *parent)
        : QWidget(parent), m_blinkState(on ? On : Off), m_colorOn(onColor), m_colorOff(offColor), m_shape(shape),
          m_widthTarget(targetWidth), m_tooltipOn(onName), m_tooltipOff(offName), m_renderer(new QSvgRenderer(this))
    {
        this->setLed();
        this->init();
    }

    void CLedWidget::init()
    {
        m_resetTimer.setSingleShot(true);
        m_resetTimer.setObjectName(this->objectName().isEmpty() ? "CLedWidget::ResetTimer" :
                                                                  this->objectName() + "::ResetTimer");
    }

    CLedWidget::~CLedWidget() { m_resetTimer.stop(); }

    void CLedWidget::setLed(LedColor ledColor)
    {
        Q_ASSERT_X(!m_renderer.isNull(), Q_FUNC_INFO, "no renderer");
        if (!m_renderer) { return; }

        // load image, init renderer
        QString ledShapeAndColor(shapes().at(static_cast<int>(m_shape)));
        if (ledColor == NoColor)
        {
            switch (m_blinkState)
            {
            case On:
                m_currentToolTip = m_tooltipOn;
                ledShapeAndColor.append(CLedWidget::colorString(m_colorOn));
                break;
            case TriState:
                m_currentToolTip = m_tooltipTriState;
                ledShapeAndColor.append(CLedWidget::colorString(m_colorTriState));
                break;
            case Off:
            default:
                m_currentToolTip = m_tooltipOff;
                ledShapeAndColor.append(CLedWidget::colorString(m_colorOff));
                break;
            }
        }
        else
        {
            if (ledColor == m_colorOn)
            {
                m_currentToolTip = m_tooltipOn;
                ledShapeAndColor.append(CLedWidget::colorString(m_colorOn));
            }
            else if (ledColor == m_colorOff)
            {
                m_currentToolTip = m_tooltipOff;
                ledShapeAndColor.append(CLedWidget::colorString(m_colorOff));
            }
            else
            {
                m_currentToolTip = m_tooltipTriState;
                ledShapeAndColor.append(CLedWidget::colorString(m_colorTriState));
            }
        }
        this->setToolTip(m_currentToolTip); // for widget

        // init renderer, load led.
        m_renderer->load(ledShapeAndColor); // load by filename

        // original size
        const QSize s = m_renderer->defaultSize();
        m_whRatio = s.width() / s.height();

        // size
        if (m_widthTarget < 0) { m_widthTarget = widths().at(static_cast<int>(m_shape)); }
        const double h = m_widthTarget / m_whRatio;
        m_heightCalculated = qRound(h);

        this->setFixedHeight(m_heightCalculated);
        this->setFixedWidth(m_widthTarget);
        this->update();
    }

    QPixmap CLedWidget::renderToPixmap() const
    {
        Q_ASSERT(!m_renderer.isNull());

        // Prepare a QImage with desired characteritiscs
        QImage image(QSize(m_widthTarget, m_heightCalculated), QImage::Format_ARGB32);
        image.fill(qRgba(0, 0, 0, 0)); // transparent background

        // Get QPainter that paints to the image
        QPainter painter(&image);
        m_renderer->render(&painter);
        return QPixmap::fromImage(image);
    }

    const QString &CLedWidget::colorString(CLedWidget::LedColor color)
    {
        static const QString empty;
        if (color == NoColor) { return empty; }
        return colorFiles().at(static_cast<int>(color));
    }

    void CLedWidget::resetState()
    {
        if (m_value == m_blinkState) { return; }
        m_blinkState = m_value;
        this->setLed();
    }

    void CLedWidget::setToolTips(const QString &on, const QString &off, const QString &triState)
    {
        m_tooltipOn = on;
        m_tooltipOff = off;
        m_tooltipTriState = triState;
        this->setLed();
    }

    void CLedWidget::setOnToolTip(const QString &on)
    {
        m_tooltipOn = on;
        this->setLed();
    }

    void CLedWidget::setOffToolTip(const QString &off)
    {
        m_tooltipOff = off;
        this->setLed();
    }

    void CLedWidget::setTriStateToolTip(const QString &triStateTooltip)
    {
        m_tooltipTriState = triStateTooltip;
        this->setLed();
    }

    void CLedWidget::setTriStateValues(CLedWidget::LedColor color, const QString &tooltip)
    {
        m_tooltipTriState = tooltip;
        m_colorTriState = color;
    }

    void CLedWidget::setOnColor(LedColor color)
    {
        if (color == m_colorOn) return;
        m_colorOn = color;
        this->setLed();
    }

    void CLedWidget::setOffColor(LedColor color)
    {
        if (color == m_colorOff) return;
        m_colorOff = color;
        this->setLed();
    }

    void CLedWidget::setTriStateColor(CLedWidget::LedColor color)
    {
        if (color == m_colorOff) return;
        m_colorTriState = color;
        this->setLed();
    }

    void CLedWidget::setShape(LedShape newShape)
    {
        if (newShape == m_shape) return;
        m_shape = newShape;
        this->setLed();
    }

    void CLedWidget::setValues(LedColor onColor, LedColor offColor, LedShape shape, const QString &toolTipOn,
                               const QString &toolTipOff, int width)
    {
        m_colorOn = onColor;
        m_colorOff = offColor;
        m_shape = shape;
        m_tooltipOn = toolTipOn;
        m_tooltipOff = toolTipOff;
        m_widthTarget = width;
        this->setLed();
    }

    void CLedWidget::setValues(LedColor onColor, LedColor offColor, LedColor triStateColor, LedShape shape,
                               const QString &toolTipOn, const QString &toolTipOff, const QString &toolTipTriState,
                               int width)
    {
        m_colorOn = onColor;
        m_colorOff = offColor;
        m_colorTriState = triStateColor;
        m_shape = shape;
        m_tooltipOn = toolTipOn;
        m_tooltipOff = toolTipOff;
        m_tooltipTriState = toolTipTriState;
        m_widthTarget = width;
        this->setLed();
    }

    QPixmap CLedWidget::asPixmap() const { return this->renderToPixmap(); }

    void CLedWidget::setOn(bool on, int resetTimeMs)
    {
        State s = on ? On : Off;
        if (resetTimeMs > 0)
        {
            QPointer<CLedWidget> myself(this);
            m_resetTimer.singleShot(resetTimeMs, this, [=] {
                if (!myself) { return; }
                this->resetState();
            });
        }
        else
        {
            m_resetTimer.stop();
            m_value = s;
        }
        if (m_blinkState == s) { return; }
        m_blinkState = s;
        this->setLed();
    }

    void CLedWidget::blink(int resetTimeMs)
    {
        m_value = Off;
        this->setOn(true, resetTimeMs);
    }

    void CLedWidget::setTriState(int resetTimeMs)
    {
        if (resetTimeMs > 0) { m_resetTimer.singleShot(resetTimeMs, this, &CLedWidget::resetState); }
        else
        {
            m_resetTimer.stop();
            m_value = TriState;
        }
        if (m_blinkState == TriState) { return; }
        m_blinkState = TriState;
        this->setLed();
    }

    void CLedWidget::toggleValue()
    {
        m_blinkState = (m_blinkState == Off) ? On : Off;
        this->setLed();
    }

    void CLedWidget::paintEvent(QPaintEvent *)
    {
        // init style sheets with this widget
        QStyleOption opt;
        opt.initFrom(this);

        // paint
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setBackgroundMode(Qt::TransparentMode);
        m_renderer->render(&painter);
    }

    void CLedWidget::mousePressEvent(QMouseEvent *event)
    {
        if (event->button() == Qt::LeftButton)
        {
            emit clicked();
            event->accept();
        }
        else { QWidget::mousePressEvent(event); }
    }

    const QStringList &CLedWidget::shapes()
    {
        static const QStringList shapes({ ":/qled/icons/qled/circle_", ":/qled/icons/qled/square_",
                                          ":/qled/icons/qled/triang_", ":/qled/icons/qled/round_" });
        return shapes;
    }

    const QStringList &CLedWidget::colorFiles()
    {
        static const QStringList colors(
            { "red.svg", "green.svg", "yellow.svg", "grey.svg", "orange.svg", "purple.svg", "blue.svg", "black.svg" });
        return colors;
    }

    const QList<int> &CLedWidget::widths()
    {
        static const QList<int> widths({ 16, 16, 16, 16 });
        return widths;
    }
} // namespace swift::gui
