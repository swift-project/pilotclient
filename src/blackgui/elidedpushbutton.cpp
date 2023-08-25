// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "elidedpushbutton.h"

#include <QPainter>
#include <QTextLine>
#include <QFontMetrics>

namespace BlackGui
{
    CElidedPushButton::CElidedPushButton(QWidget *parent) : QPushButton(parent)
    {}

    CElidedPushButton::CElidedPushButton(const QString &text, QWidget *parent)
        : QPushButton(parent), m_elided(false), m_content(text)
    {
        this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    }

    void CElidedPushButton::setText(const QString &newText)
    {
        m_content = newText;
        this->update();
    }

    void CElidedPushButton::paintEvent(QPaintEvent *event)
    {
        QPainter painter(this);
        const QFontMetrics fontMetrics = painter.fontMetrics();
        const int usableWidth = qRound(0.9 * this->width());

        const QString elidedText = fontMetrics.elidedText(m_content, Qt::ElideRight, usableWidth);
        m_elided = (elidedText != m_content);
        QPushButton::setText(elidedText);
        QPushButton::paintEvent(event);
    }
} // ns
