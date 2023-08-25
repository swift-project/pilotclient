// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_MANAGEDSTATUSBAR_H
#define BLACKGUI_MANAGEDSTATUSBAR_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/statusmessagelist.h"
#include <QObject>
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

    private:
        //! Clear status bar
        void clearStatusBar();

        QStatusBar *m_statusBar = nullptr; //!< the status bar itself
        QLabel *m_statusBarIcon = nullptr; //!< status bar icon
        QLabel *m_statusBarLabel = nullptr; //!< status bar label
        QTimer m_timerStatusBar { this }; //!< cleaning up status bar (own cleaning as I need to clean window / icon)
        bool m_ownedStatusBar = false; //!< own status bar or "injected" (e.g.by UI builder)
        Qt::TextElideMode m_elideMode = Qt::ElideMiddle; //!< label text elide
        BlackMisc::StatusSeverity m_currentSeverity = BlackMisc::StatusSeverity::SeverityDebug; //!< severity currently displayed
    };
} // namespace

#endif // guard
