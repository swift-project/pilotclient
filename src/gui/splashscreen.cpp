// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "splashscreen.h"

#include <QtMath>

using namespace swift::misc;

namespace swift::gui
{
    CSplashScreen::CSplashScreen(const QPixmap &pixmap)
    {
        const int height = qFloor(pixmap.height() * 1.2);
        const int width = pixmap.width();
        QPixmap splash(pixmap.width(), height);
        splash.fill(Qt::transparent);

        QPainter painter(&splash);
        painter.drawPixmap(0, 0, pixmap);

        const int statusbar_height = qFloor((height - pixmap.height()) / 2.0);
        QPixmap statusbar(width, statusbar_height);
        statusbar.fill(QColor(200, 200, 200));
        painter.drawPixmap(0, height - statusbar_height, statusbar);

        this->setPixmap(splash);

        m_clearTextTimer.setSingleShot(true);
        connect(&m_clearTextTimer, &QTimer::timeout, this, &CSplashScreen::clearMessage);
    }

    void CSplashScreen::showStatusMessage(const QString &html)
    {
        if (html.isEmpty()) { return; }
        this->showMessage(html, Qt::AlignHCenter | Qt::AlignBottom);
        m_clearTextTimer.start(2000);
    }

    void CSplashScreen::showStatusMessage(const swift::misc::CStatusMessage &message)
    {
        this->showStatusMessage(message.toHtml(true, true));
    }

    void CSplashScreen::setSplashFont(const QFont &font) { this->setFont(font); }
} // namespace swift::gui
