// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_SPLASHSCREEN_H
#define BLACKGUI_SPLASHSCREEN_H

#include "misc/statusmessage.h"
#include <QSplashScreen>
#include <QPainter>
#include <QPixmap>
#include <QLabel>
#include <QFont>
#include <QTimer>

namespace BlackGui
{
    /*!
     * Own splash screen
     */
    class CSplashScreen : public QSplashScreen
    {
    public:
        //! Ctor
        CSplashScreen(const QPixmap &pixmap);

        //! Show status message
        void showStatusMessage(const QString &html);

        //! Show status message
        void showStatusMessage(const swift::misc::CStatusMessage &message);

        //! Set the font for the splash screen
        void setSplashFont(const QFont &font);

    private:
        QString m_message;
        QTimer m_clearTextTimer;
    };
} // ns

#endif // guard
