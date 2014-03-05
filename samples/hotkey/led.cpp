/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "led.h"
#include <QPen>
#include <QPainter>
#include <QPointF>
#include <QRadialGradient>

CLed::CLed(QWidget *parent) :
    QWidget(parent),
    m_isChecked(false)
{
}

void CLed::paintEvent(QPaintEvent * /* event */)
{
    qint32 diameter = qMin(width(), height());

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(width() / 2, height() / 2);
    painter.scale(diameter / 50.0, diameter / 50.0);

    drawLed(painter);
    drawShine(painter);
}

QSize CLed::minimumSizeHint() const
{
    return QSize(50, 50);
}

void CLed::drawLed(QPainter &painter)
{
    // Draw circle
    QColor backgroundColor;

    if (isChecked())
        backgroundColor = Qt::red;
    else
        backgroundColor = Qt::gray;

    // Black border color
    // Fill color depends on the state
    QPen penCircle;
    penCircle.setColor(Qt::black);
    penCircle.setWidthF(10.0);
    painter.setBrush(backgroundColor);
    painter.drawEllipse(QPointF(0.0, 0.0), 24.5, 24.5);
}

void CLed::drawShine(QPainter &painter)
{
    // Draw shine
    QColor white1(255,255,255,200);
    QColor white0(255,255,255,0);
    QRadialGradient shine(QPointF(-10.0,-10.0),30.0,QPointF(-10,-10));
    shine.setColorAt(0.0, white1);
    shine.setColorAt(1.0, white0);
    painter.setBrush(shine);
    painter.drawEllipse(QPointF(0.0, 0.0), 24.0, 24.0);
}


