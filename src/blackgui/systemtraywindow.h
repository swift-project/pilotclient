/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_ENABLEFORSYSTEMTRAY_H
#define BLACKGUI_ENABLEFORSYSTEMTRAY_H

#include "blackgui/blackguiexport.h"

#include <QFlag>
#include <QFlags>
#include <QIcon>
#include <QMainWindow>
#include <QObject>
#include <QString>
#include <QSystemTrayIcon>

class QAction;
class QCloseEvent;
class QEvent;
class QMenu;
class QWidget;

namespace BlackGui
{

    //! QDialog which can minimize to system tray
    class BLACKGUI_EXPORT CSystemTrayWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        //! System Tray Flag
        enum SystemTrayFlag
        {
            MinimizeToTray = 0x0,
            QuitOnClose = 0x1,
            AskOnClose = 0x2
        };
        Q_DECLARE_FLAGS(SystemTrayMode, SystemTrayFlag)

        //! Constructor
        CSystemTrayWindow(const QIcon &icon, QWidget *parent = nullptr);

        //! System tray mode
        void setSystemTrayMode(SystemTrayMode mode);

        //! Set icon
        void setSystemTrayIcon(const QIcon &icon);

        //! Set tool tip
        void setSystemTrayToolTip(const QString &tooltip);

    protected slots:

        //! Activate window from system tray
        void ps_activateWindow(QSystemTrayIcon::ActivationReason reason);

        //! Show message in the system tray
        void ps_showMessage(const QString &title, const QString &message,
                            QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::Information,
                            int millisecondsTimeoutHint = 10000);

    protected:
        //! \copydoc QMainWindow::changeEvent
        virtual void changeEvent(QEvent *event) override;

        //! \copydoc QMainWindow::closeEvent
        virtual void closeEvent(QCloseEvent *event) override;

    private:
        void createActions();
        void createTrayIcon(const QIcon &icon);

        SystemTrayMode m_systemTrayMode = SystemTrayMode { MinimizeToTray | QuitOnClose }; //!< Minimize mode, \sa MinimizeMode

        // System Tray actions
        QAction *m_actionRestore = nullptr;
        QAction *m_actionQuit = nullptr;
        QSystemTrayIcon *m_systemTrayIcon = nullptr;
        QMenu *m_trayIconMenu = nullptr;
    };

} // namespace

Q_DECLARE_OPERATORS_FOR_FLAGS(BlackGui::CSystemTrayWindow::SystemTrayMode)

#endif // guard
