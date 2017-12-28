/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_MANAGEDSTATUSBAR_H
#define BLACKGUI_MANAGEDSTATUSBAR_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/statusmessagelist.h"
#include <QObject>

class QLabel;
class QStatusBar;
class QTimer;

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
        virtual ~CManagedStatusBar();

        //! Get the status bar
        QStatusBar *getStatusBar() const { return m_statusBar; }

        //! Init
        void initStatusBar(QStatusBar *statusBar = nullptr);

        //! Show
        void show();

        //! Hide
        void hide();

    public slots:
        //! Display status message
        void displayStatusMessage(const BlackMisc::CStatusMessage &statusMessage);

        //! Display status messages
        void displayStatusMessages(const BlackMisc::CStatusMessageList &statusMessages);

    private:
        //! Clear status bar
        void clearStatusBar();

        QStatusBar *m_statusBar  = nullptr; //!< the status bar itself
        QLabel *m_statusBarIcon  = nullptr; //!< status bar icon
        QLabel *m_statusBarLabel = nullptr; //!< status bar label
        QTimer *m_timerStatusBar = nullptr; //!< cleaning up status bar (own cleaning as I need to clean window / icon)
        bool m_ownStatusBar = false;
        BlackMisc::StatusSeverity m_currentSeverity = BlackMisc::StatusSeverity::SeverityDebug; // severity currently displayed
    };
} // namespace

#endif // guard
