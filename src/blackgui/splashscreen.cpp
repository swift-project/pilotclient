// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "splashscreen.h"

using namespace BlackMisc;

namespace BlackGui
{
    CSplashScreen::CSplashScreen(const QPixmap &pixmap) : QSplashScreen(pixmap)
    {
        const int heightTe = 60;
        const int height = pixmap.height();
        const int width = qRound(0.9 * pixmap.width());
        const int yPos = (height - heightTe) / 2;
        const int xPos = (pixmap.width() - width) / 2;

        m_label = new QLabel(this);
        m_label->setGeometry(xPos, yPos, width, 80);
        m_label->setAlignment(Qt::AlignCenter);
        m_label->setStyleSheet("background: rgba(0,0,0,0); color: white;");
        m_label->setWordWrap(true);
        m_label->setVisible(false);

        m_hideTextTimer.setSingleShot(true);
        connect(&m_hideTextTimer, &QTimer::timeout, this, &CSplashScreen::hideText);
    }

    void CSplashScreen::showStatusMessage(const QString &html)
    {
        if (html.isEmpty()) { return; }
        m_label->setVisible(true);
        m_label->setText(html);
        m_hideTextTimer.start(2000);
    }

    void CSplashScreen::showStatusMessage(const BlackMisc::CStatusMessage &message)
    {
        this->showStatusMessage(message.toHtml(true, true));
    }

    void CSplashScreen::setSplashFont(const QFont &font)
    {
        this->setFont(font);
        m_label->setFont(font);
    }

    void CSplashScreen::hideText()
    {
        m_label->setVisible(false);
    };
}
