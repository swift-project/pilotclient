/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "guiapplication.h"
#include "guiutility.h"
#include "stylesheetutility.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/project.h"
#include "blackmisc/verify.h"
#include "blackmisc/metadatautils.h"
#include "blackmisc/settingscache.h"
#include <QApplication>
#include <QMessageBox>
#include <QDesktopServices>
#include <QDir>
#include <QKeySequence>

using namespace BlackMisc;

BlackGui::CGuiApplication *sGui = nullptr; // set by constructor

namespace BlackGui
{
    CGuiApplication *CGuiApplication::instance()
    {
        return qobject_cast<CGuiApplication *>(CApplication::instance());
    }

    const BlackMisc::CLogCategoryList &CGuiApplication::getLogCategories()
    {
        static const CLogCategoryList l(CApplication::getLogCategories().join( { CLogCategory::guiComponent() }));
        return l;
    }

    CGuiApplication::CGuiApplication(const QString &applicationName, const QPixmap &icon) : CApplication(applicationName)
    {
        setWindowIcon(icon);
        connect(&CStyleSheetUtility::instance(), &CStyleSheetUtility::styleSheetsChanged, this, &CGuiApplication::styleSheetsChanged);
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
            const QString v(this->getParserValue(this->m_cmdWindowMode));
            return CEnableForFramelessWindow::stringToWindowMode(v);
        }
        else
        {
            return CEnableForFramelessWindow::WindowNormal;
        }
    }

    void CGuiApplication::processEventsToRefreshGui() const
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }

    QWidget *CGuiApplication::mainApplicationWindow()
    {
        return CGuiUtility::mainApplicationWindow();
    }

    IMainWindowAccess *CGuiApplication::mainWindowAccess()
    {
        IMainWindowAccess *m = dynamic_cast<IMainWindowAccess *>(mainApplicationWindow());
        return m;
    }

    void CGuiApplication::initMainApplicationWindow(QWidget *mainWindow) const
    {
        if (!mainWindow) { return; }
        const QString name(this->getApplicationNameVersionBetaDev());
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

    bool CGuiApplication::displayInStatusBar(const CStatusMessage &message)
    {
        IMainWindowAccess *m = mainWindowAccess();
        BLACK_VERIFY_X(m, Q_FUNC_INFO, "No access interface");
        if (!m) { return false; }
        return m->displayInStatusBar(message);
    }

    bool CGuiApplication::displayInOverlayWindow(const CStatusMessage &message)
    {
        IMainWindowAccess *m = mainWindowAccess();
        BLACK_VERIFY_X(m, Q_FUNC_INFO, "No access interface");
        if (!m) { return false; }
        return m->displayInOverlayWindow(message);
    }

    bool CGuiApplication::displayTextInConsole(const QString &text)
    {
        IMainWindowAccess *m = mainWindowAccess();
        BLACK_VERIFY_X(m, Q_FUNC_INFO, "No access interface");
        if (!m) { return false; }
        return m->displayTextInConsole(text);
    }

    bool CGuiApplication::reloadStyleSheets() const
    {
        return CStyleSheetUtility::instance().read();
    }

    void CGuiApplication::addMenuForSettingsAndCache(QMenu &menu)
    {
        QMenu *sm = menu.addMenu(CIcons::appSettings16(), "Settings");
        sm->setIcon(CIcons::appSettings16());
        QAction *a = sm->addAction(CIcons::disk16(), "Settings directory");
        bool c = connect(a, &QAction::triggered, this, [a, this]()
        {
            const QString path(QDir::toNativeSeparators(CSettingsCache::persistentStore()));
            if (QDir(path).exists())
            {
                QDesktopServices::openUrl(QUrl("file:///" + path));
            }
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");

        a = sm->addAction("Reset settings");
        c = connect(a, &QAction::triggered, this, [this]()
        {
            CSettingsCache::instance()->clearAllValues();
            this->displayTextInConsole("Cleared all settings!");
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");

        a = sm->addAction("List settings files");
        c = connect(a, &QAction::triggered, this, [this]()
        {
            const QStringList files(CSettingsCache::instance()->enumerateStore());
            this->displayTextInConsole(files.join("\n"));
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");

        sm = menu.addMenu("Cache");
        sm->setIcon(CIcons::appSettings16());
        a = sm->addAction(CIcons::disk16(), "Cache directory");
        c = connect(a, &QAction::triggered, this, [this]()
        {
            const QString path(QDir::toNativeSeparators(CDataCache::persistentStore()));
            if (QDir(path).exists())
            {
                QDesktopServices::openUrl(QUrl("file:///" + path));
            }
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");

        a = sm->addAction("Reset cache");
        c = connect(a, &QAction::triggered, this, [this]()
        {
            CDataCache::instance()->clearAllValues();
            this->displayTextInConsole("Cleared cache!");
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");

        a = sm->addAction("List cache files");
        c = connect(a, &QAction::triggered, this, [this]()
        {
            const QStringList files(CDataCache::instance()->enumerateStore());
            this->displayTextInConsole(files.join("\n"));
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
    }

    void CGuiApplication::addMenuForStyleSheets(QMenu &menu)
    {
        QMenu *sm = menu.addMenu("Style sheet");
        QAction *a = sm->addAction(CIcons::refresh16(), "Reload");
        bool c = connect(a, &QAction::triggered, this, [a, this]()
        {
            this->reloadStyleSheets();
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
    }

    void CGuiApplication::addMenuFile(QMenu &menu)
    {
        addMenuForSettingsAndCache(menu);
        addMenuForStyleSheets(menu);

        menu.addSeparator();
        QAction *a = menu.addAction("E&xit");
        a->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Q));
        bool c = connect(a, &QAction::triggered, this, [a, this]()
        {
            // a close event might already trigger a shutdown
            this->mainApplicationWindow()->close();
            this->gracefulShutdown();
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
    }

    void CGuiApplication::addMenuInternals(QMenu &menu)
    {
        QMenu *sm = menu.addMenu("Templates");
        QAction *a = sm->addAction("JSON bootstrap");
        bool c = connect(a, &QAction::triggered, this, [a, this]()
        {
            this->displayTextInConsole(this->getGlobalSetup().toJsonString());
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");

        a = sm->addAction("JSON update info");
        c = connect(a, &QAction::triggered, this, [a, this]()
        {
            this->displayTextInConsole(this->getUpdateInfo().toJsonString());
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");

        a = menu.addAction("Metadata (slow)");
        c = connect(a, &QAction::triggered, this, [a, this]()
        {
            this->displayTextInConsole(getAllUserMetatypesTypes());
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");

        a = menu.addAction("Setup");
        c = connect(a, &QAction::triggered, this, [a, this]()
        {
            this->displayTextInConsole(this->getGlobalSetup().convertToQString("\n", true));
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");

        a = menu.addAction("Compile info");
        c = connect(a, &QAction::triggered, this, [a, this]()
        {
            this->displayTextInConsole(this->getInfoString("\n"));
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
    }

    void CGuiApplication::addMenuWindow(QMenu &menu)
    {
        QWidget *w = mainApplicationWindow();
        if (!w) { return; }
        QAction *a = menu.addAction(w->style()->standardIcon(QStyle::SP_TitleBarMaxButton), "Fullscreen");
        bool c = connect(a, &QAction::triggered, this, [a, w]()
        {
            w->showFullScreen();
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");

        a = menu.addAction(w->style()->standardIcon(QStyle::SP_TitleBarMinButton), "Minimize");
        c = connect(a, &QAction::triggered, this, [a, w]()
        {
            w->showMinimized();

        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");

        a = menu.addAction(w->style()->standardIcon(QStyle::SP_TitleBarNormalButton), "Normal");
        c = connect(a, &QAction::triggered, this, [a, w]()
        {
            w->showNormal();

        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");

        a = menu.addAction("Toggle stay on top");
        c = connect(a, &QAction::triggered, this, [a, w]()
        {
            if (CGuiUtility::toggleStayOnTop(w))
            {
                CLogMessage(w).info("Window on top");
            }
            else
            {
                CLogMessage(w).info("Window not always on top");
            }
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
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
