/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/systemtraywindow.h"

#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QEvent>
#include <QMenu>
#include <QMessageBox>
#include <QObject>
#include <QWidget>
#include <QtGlobal>

namespace BlackGui
{

    CSystemTrayWindow::CSystemTrayWindow(const QIcon &icon, QWidget *parent) : QMainWindow(parent)
    {
        createActions();
        createTrayIcon(icon);
    }

    void CSystemTrayWindow::setSystemTrayMode(SystemTrayMode mode)
    {
        m_systemTrayMode = mode;
    }

    void CSystemTrayWindow::setSystemTrayIcon(const QIcon &icon)
    {
        Q_ASSERT(m_systemTrayIcon);
        m_systemTrayIcon->setIcon(icon);
    }

    void CSystemTrayWindow::setSystemTrayToolTip(const QString &tooltip)
    {
        Q_ASSERT(m_systemTrayIcon);
        m_systemTrayIcon->setToolTip(tooltip);
    }

    void CSystemTrayWindow::ps_activateWindow(QSystemTrayIcon::ActivationReason reason)
    {
        switch (reason)
        {
        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::DoubleClick:
            showNormal();
            activateWindow();
            break;
        case QSystemTrayIcon::MiddleClick:
            break;
        default:
            break;
        }
    }

    void CSystemTrayWindow::ps_showMessage(const QString &title, const QString &message, QSystemTrayIcon::MessageIcon icon,
                                           int millisecondsTimeoutHint)
    {
        Q_ASSERT(m_systemTrayIcon);
        m_systemTrayIcon->showMessage(title, message, icon, millisecondsTimeoutHint);
    }

    void CSystemTrayWindow::closeEvent(QCloseEvent *event)
    {
        int result = QMessageBox::Close;

        if (m_systemTrayMode.testFlag(AskOnClose))
        {
            QMessageBox msgBox;
            msgBox.setText("Are you sure you want to close? This will quit swiftcore!");
            msgBox.setStandardButtons(QMessageBox::Close | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Close);
            msgBox.setIcon(QMessageBox::Warning);
        }

        result = QMessageBox::Close; // msgBox.exec();
        switch (result)
        {
        case QMessageBox::Close:
            event->accept();
            break;
        case QMessageBox::Cancel:
            event->ignore();
            break;
        default:
            closeEvent(event);
            break;
        }
    }

    void CSystemTrayWindow::changeEvent(QEvent *event)
    {
        QMainWindow::changeEvent(event);
        if (event->type() == QEvent::WindowStateChange)
        {
            if (isMinimized()) { hide(); }
        }
    }

    void CSystemTrayWindow::createActions()
    {
        m_actionRestore = new QAction(tr("&Restore"), this);
        connect(m_actionRestore, &QAction::triggered, this, &QWidget::showNormal);

        m_actionQuit = new QAction(tr("&Quit"), this);
        connect(m_actionQuit, &QAction::triggered, qApp, &QApplication::quit);
    }

    void CSystemTrayWindow::createTrayIcon(const QIcon &icon)
    {
        m_trayIconMenu = new QMenu(this);
        m_trayIconMenu->addAction(m_actionRestore);
        m_trayIconMenu->addSeparator();
        m_trayIconMenu->addAction(m_actionQuit);

        m_systemTrayIcon = new QSystemTrayIcon(this);
        m_systemTrayIcon->setContextMenu(m_trayIconMenu);
        m_systemTrayIcon->setIcon(icon);
        m_systemTrayIcon->show();

        connect(m_systemTrayIcon, &QSystemTrayIcon::activated, this, &CSystemTrayWindow::ps_activateWindow);
    }

} // namespace
