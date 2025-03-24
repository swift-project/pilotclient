// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "splashscreen.h"

#include <QPainter>
#include <QtMath>

using namespace swift::misc;

namespace swift::gui
{
    CSplashScreen::CSplashScreen(const QPixmap &pixmap, const QFont &font)
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
        this->setFont(font);
    }

    void CSplashScreen::showStatusMessage(const QString &msg)
    {
        if (msg.isEmpty()) { return; }
        this->showMessage(msg, Qt::AlignHCenter | Qt::AlignBottom);
    }
} // namespace swift::gui
