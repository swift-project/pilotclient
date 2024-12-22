// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_SPLASHSCREEN_H
#define SWIFT_GUI_SPLASHSCREEN_H

#include <QFont>
#include <QPixmap>
#include <QSplashScreen>
#include <QTimer>

#include "misc/statusmessage.h"

namespace swift::gui
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
} // namespace swift::gui

#endif // SWIFT_GUI_SPLASHSCREEN_H
