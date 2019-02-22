/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
