// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_MANAGEDSTATUSBAR_H
#define BLACKGUI_MANAGEDSTATUSBAR_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/statusmessagelist.h"
#include <QObject>
#include <QPushButton>
#include <QTimer>

class QLabel;
class QStatusBar;

namespace BlackGui
{
    //! Managed status bar
    class BLACKGUI_EXPORT CManagedStatusBar : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CManagedStatusBar(QObject *parent = nullptr);

        //! Destructor
        virtual ~CManagedStatusBar() override;

        //! Get the status bar
        QStatusBar *getStatusBar() const { return m_statusBar; }

        //! Init
        void initStatusBar(QStatusBar *statusBar = nullptr);

        //! Show warning log button
        void showWarningButton();

        //! Show warning error button
        void showErrorButton();

        //! Show
        void show();

        //! Hide
        void hide();

        //! Set the label elide mode
        void setElideMode(Qt::TextElideMode mode) { m_elideMode = mode; }

        //! Display status message
        void displayStatusMessage(const BlackMisc::CStatusMessage &statusMessage);

        //! Display status messages
        void displayStatusMessages(const BlackMisc::CStatusMessageList &statusMessages);

        //! Enabled size grip
        void setSizeGripEnabled(bool enabled);

    signals:
        //! Request to show the log page
        void requestLogPage();

    private:
        //! Clear status bar
        void clearStatusBar();

        //! Pressed the WARN button
        void pressedWarnButton();

        //! Pressed the ERROR button
        void pressedErrorButton();

        QStatusBar *m_statusBar = nullptr; //!< the status bar itself
        QLabel *m_statusBarIcon = nullptr; //!< status bar icon
        QLabel *m_statusBarLabel = nullptr; //!< status bar label
        QPushButton *m_warningButton = nullptr; //!< log warning button
        QPushButton *m_errorButton = nullptr; //!< log error button
        QTimer m_timerStatusBar { this }; //!< cleaning up status bar (own cleaning as I need to clean window / icon)
        bool m_ownedStatusBar = false; //!< own status bar or "injected" (e.g.by UI builder)
        bool m_showWarnButtonInitially = false; //!< should the button be shown initially? Might be set before the button is initialized
        bool m_showErrorButtonInitially = false; //!< should the button be shown initially? Might be set before the button is initialized
        Qt::TextElideMode m_elideMode = Qt::ElideMiddle; //!< label text elide
        BlackMisc::StatusSeverity m_currentSeverity = BlackMisc::StatusSeverity::SeverityDebug; //!< severity currently displayed
    };
} // namespace

#endif // guard
