/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "guiapplication.h"
#include "blackmisc/project.h"
#include <QApplication>
#include <QMessageBox>

using namespace BlackMisc;

BlackGui::CGuiApplication *sGui = nullptr; // set by constructor

namespace BlackGui
{
    CGuiApplication *CGuiApplication::instance()
    {
        return qobject_cast<CGuiApplication *>(CApplication::instance());
    }

    CGuiApplication::CGuiApplication(const QString &applicationName, const QPixmap &icon) : CApplication(applicationName)
    {
        setWindowIcon(icon);
        sGui = this;
    }

    CGuiApplication::~CGuiApplication()
    {
        sGui = nullptr;
    }

    void CGuiApplication::addWindowModeOption()
    {
        this->m_cmdWindowMode = QCommandLineOption(QStringList() << "w" << "window",
                                QCoreApplication::translate("main", "Windows: (n)ormal, (f)rameless, (t)ool."),
                                "windowtype");
        this->addParserOption(this->m_cmdWindowMode);
    }

    void CGuiApplication::addWindowStateOption()
    {
        this->m_cmdWindowStateMinimized =  QCommandLineOption({{"m", "minimized"}, QCoreApplication::translate("main", "Start minimized in system tray.")});
        this->addParserOption(this->m_cmdWindowStateMinimized);
    }

    Qt::WindowState CGuiApplication::getWindowState() const
    {
        if (this->m_cmdWindowStateMinimized.valueName() == "empty") { return Qt::WindowNoState; }
        if (this->m_parser.isSet(this->m_cmdWindowStateMinimized)) { return Qt::WindowMinimized; }
        return Qt::WindowNoState;
    }

    CEnableForFramelessWindow::WindowMode CGuiApplication::getWindowMode() const
    {
        if (this->isParserOptionSet(m_cmdWindowMode))
        {
            const QString v(this->getParserOptionValue(this->m_cmdWindowMode));
            return CEnableForFramelessWindow::stringToWindowMode(v);
        }
        else
        {
            return CEnableForFramelessWindow::WindowNormal;
        }
    }

    void CGuiApplication::initMainApplicationWindow(QWidget *mainWindow) const
    {
        if (!mainWindow) { return; }
        const QString name(this->getApplicationNameAndVersion());
        mainWindow->setObjectName(QCoreApplication::applicationName());
        mainWindow->setWindowTitle(name);
        mainWindow->setWindowIcon(m_windowIcon);
        mainWindow->setWindowIconText(name);
    }

    void CGuiApplication::setWindowIcon(const QPixmap &icon)
    {
        instance()->m_windowIcon = icon;
        QApplication::setWindowIcon(icon);
    }

    void CGuiApplication::exit(int retcode)
    {
        CApplication::exit(retcode);
    }

    void CGuiApplication::cmdLineErrorMessage(const QString &errorMessage) const
    {
        if (CProject::isRunningOnWindowsNtPlatform())
        {
            QMessageBox::warning(nullptr,
                                 QGuiApplication::applicationDisplayName(),
                                 "<html><head/><body><h2>" + errorMessage + "</h2><pre>" + this->m_parser.helpText() + "</pre></body></html>");
        }
        else
        {
            CApplication::cmdLineErrorMessage(errorMessage);
        }
    }

    void CGuiApplication::cmdLineHelpMessage()
    {
        if (CProject::isRunningOnWindowsNtPlatform())
        {
            QMessageBox::information(nullptr,
                                     QGuiApplication::applicationDisplayName(),
                                     "<html><head/><body><pre>" + this->m_parser.helpText() + "</pre></body></html>");
        }
        else
        {
            CApplication::cmdLineHelpMessage();
        }
    }

    void CGuiApplication::cmdLineVersionMessage() const
    {
        if (CProject::isRunningOnWindowsNtPlatform())
        {
            QMessageBox::information(nullptr,
                                     QGuiApplication::applicationDisplayName(),
                                     QGuiApplication::applicationDisplayName() + ' ' + QCoreApplication::applicationVersion());
        }
        else
        {
            CApplication::cmdLineVersionMessage();
        }
    }

    bool CGuiApplication::parsingHookIn()
    {
        // void
        return true;
    }

} // ns
