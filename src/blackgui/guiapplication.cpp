/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackconfig/buildconfig.h"
#include "blackcore/context/contextnetwork.h"
#include "blackcore/setupreader.h"
#include "blackcore/data/globalsetup.h"
#include "blackcore/webdataservices.h"
#include "blackgui/components/applicationclosedialog.h"
#include "blackgui/components/updateinfodialog.h"
#include "blackgui/components/aboutdialog.h"
#include "blackgui/components/setuploadingdialog.h"
#include "blackgui/guiapplication.h"
#include "blackgui/guiutility.h"
#include "blackgui/registermetadata.h"
#include "blackmisc/slot.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/datacache.h"
#include "blackmisc/logcategory.h"
#include "blackmisc/logcategorylist.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/metadatautils.h"
#include "blackmisc/registermetadata.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/verify.h"

#include <QAction>
#include <QCloseEvent>
#include <QApplication>
#include <QCommandLineParser>
#include <QApplication>
#include <QDesktopServices>
#include <QDir>
#include <QEventLoop>
#include <QGuiApplication>
#include <QIcon>
#include <QKeySequence>
#include <QMenu>
#include <QMessageBox>
#include <QProcess>
#include <QRegularExpression>
#include <QSettings>
#include <QSplashScreen>
#include <QStyleFactory>
#include <QStringList>
#include <QStyle>
#include <QSysInfo>
#include <QUrl>
#include <QWidget>
#include <QMainWindow>
#include <QtGlobal>

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Db;
using namespace BlackMisc::Network;
using namespace BlackGui::Components;
using namespace BlackCore;
using namespace BlackCore::Data;
using namespace BlackCore::Context;

BlackGui::CGuiApplication *sGui = nullptr; // set by constructor

namespace BlackGui
{
    CGuiApplication *CGuiApplication::instance()
    {
        return qobject_cast<CGuiApplication *>(CApplication::instance());
    }

    const BlackMisc::CLogCategoryList &CGuiApplication::getLogCategories()
    {
        static const CLogCategoryList l(CApplication::getLogCategories().join({ CLogCategory::guiComponent() }));
        return l;
    }

    CGuiApplication::CGuiApplication(const QString &applicationName, CApplicationInfo::Application application, const QPixmap &icon) :
        CApplication(applicationName, application, false)
    {
        this->addWindowModeOption();
        this->addWindowResetSizeOption();

        if (!sGui)
        {
            CGuiApplication::registerMetadata();
            CApplication::init(false); // base class without metadata
            if (this->hasSetupReader()) { this->getSetupReader()->setCheckCmdLineBootstrapUrl(false); } // no connect checks on setup reader (handled with interactive setup loading)
            CGuiApplication::adjustPalette();
            this->setWindowIcon(icon);
            this->settingsChanged();
            this->setCurrentFontValues(); // most likely the default font and not any stylesheet font at this time
            sGui = this;
            connect(&m_styleSheetUtility, &CStyleSheetUtility::styleSheetsChanged, this, &CGuiApplication::onStyleSheetsChanged);
        }
    }

    CGuiApplication::~CGuiApplication()
    {
        sGui = nullptr;
    }

    void CGuiApplication::registerMetadata()
    {
        CApplication::registerMetadata();
        BlackGui::registerMetadata();
    }

    void CGuiApplication::addWindowModeOption()
    {
        m_cmdWindowMode = QCommandLineOption(QStringList() << "w" << "window",
                                             QCoreApplication::translate("main", "Windows: (n)ormal, (f)rameless, (t)ool."),
                                             "windowtype");
        this->addParserOption(m_cmdWindowMode);
    }

    void CGuiApplication::addWindowResetSizeOption()
    {
        m_cmdWindowSizeReset = QCommandLineOption({{"r", "resetsize"}, QCoreApplication::translate("main", "Reset window size (ignore saved values).")});
        this->addParserOption(m_cmdWindowSizeReset);
    }

    void CGuiApplication::addWindowStateOption()
    {
        m_cmdWindowStateMinimized =  QCommandLineOption({{"m", "minimized"}, QCoreApplication::translate("main", "Start minimized in system tray.")});
        this->addParserOption(m_cmdWindowStateMinimized);
    }

    Qt::WindowState CGuiApplication::getWindowState() const
    {
        if (m_cmdWindowStateMinimized.valueName() == "empty") { return Qt::WindowNoState; }
        if (m_parser.isSet(m_cmdWindowStateMinimized)) { return Qt::WindowMinimized; }
        return Qt::WindowNoState;
    }

    CEnableForFramelessWindow::WindowMode CGuiApplication::getWindowMode() const
    {
        if (this->isParserOptionSet(m_cmdWindowMode))
        {
            const QString v(this->getParserValue(m_cmdWindowMode));
            return CEnableForFramelessWindow::stringToWindowMode(v);
        }
        else
        {
            return CEnableForFramelessWindow::WindowNormal;
        }
    }

    void CGuiApplication::splashScreen(const QString &resource)
    {
        if (m_splashScreen)
        {
            m_splashScreen.reset(); // delete old one
        }
        if (!resource.isEmpty())
        {
            const QPixmap pm(resource);
            this->splashScreen(pm);
        }
    }

    void CGuiApplication::splashScreen(const QPixmap &pixmap)
    {
        if (m_splashScreen)
        {
            m_splashScreen.reset(); // delete old one
        }
        m_splashScreen.reset(new QSplashScreen(pixmap.scaled(256, 256)));
        m_splashScreen->show();
        this->processEventsToRefreshGui();
    }

    void CGuiApplication::processEventsToRefreshGui() const
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }

    QWidget *CGuiApplication::mainApplicationWidget()
    {
        return CGuiUtility::mainApplicationWidget();
    }

    void CGuiApplication::registerMainApplicationWidget(QWidget *mainWidget)
    {
        CGuiUtility::registerMainApplicationWidget(mainWidget);
    }

    QMainWindow *CGuiApplication::mainApplicationWindow()
    {
        return qobject_cast<QMainWindow *>(CGuiApplication::mainApplicationWidget());
    }

    IMainWindowAccess *CGuiApplication::mainWindowAccess()
    {
        IMainWindowAccess *m = qobject_cast<IMainWindowAccess *>(mainApplicationWidget());
        return m;
    }

    void CGuiApplication::initMainApplicationWidget(QWidget *mainWidget)
    {
        if (!mainWidget) { return; }
        if (m_uiSetupCompleted) { return; }
        m_uiSetupCompleted = true;

        const QString name(this->getApplicationNameVersionDetailed());
        mainWidget->setObjectName(QCoreApplication::applicationName());
        mainWidget->setWindowTitle(name);
        mainWidget->setWindowIcon(m_windowIcon);
        mainWidget->setWindowIconText(name);
        CStyleSheetUtility::setQSysInfoProperties(mainWidget, true);
        CGuiUtility::registerMainApplicationWidget(mainWidget);
        emit this->uiObjectTreeReady();
    }

    void CGuiApplication::addWindowFlags(Qt::WindowFlags flags)
    {
        QWidget *maw = this->mainApplicationWidget();
        if (maw)
        {
            Qt::WindowFlags windowFlags = maw->windowFlags();
            windowFlags |= flags;
            maw->setWindowFlags(windowFlags);
        }
        else
        {
            connectOnce(this, &CGuiApplication::uiObjectTreeReady, this, [ = ]
            {
                this->addWindowFlags(flags);
            });
        }
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

    void CGuiApplication::highDpiScreenSupport()
    {
        qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1");
    }

    bool CGuiApplication::isUsingHighDpiScreenSupport()
    {
        return CGuiUtility::isUsingHighDpiScreenSupport();
    }

    bool CGuiApplication::saveWindowGeometryAndState(const QMainWindow *window) const
    {
        if (!window) { return false; }
        QSettings settings("swift-project.org", this->getApplicationName());
        settings.setValue("geometry", window->saveGeometry());
        settings.setValue("windowState", window->saveState());
        return true;
    }

    bool CGuiApplication::restoreWindowGeometryAndState(QMainWindow *window)
    {
        if (!window) { return false; }
        const QSettings settings("swift-project.org", this->getApplicationName());
        const QByteArray g = settings.value("geometry").toByteArray();
        const QByteArray s = settings.value("windowState").toByteArray();
        window->restoreGeometry(g);
        window->restoreState(s);
        return true;
    }

    void CGuiApplication::onStartUpCompleted()
    {
        CApplication::onStartUpCompleted();
        this->setCurrentFontValues();

        const QString metricInfo = CGuiUtility::metricsInfo();
        CLogMessage(this).info(metricInfo);

        // window size
        if (m_minWidthChars > 0 || m_minHeightChars > 0)
        {
            const QSizeF s = CGuiUtility::fontMetricsEstimateSize(m_minWidthChars, m_minHeightChars);
            QWidget *mw = CGuiUtility::mainApplicationWidget();
            if (mw)
            {
                QSize cs = mw->size();
                if (m_minWidthChars > 0)  { cs.setWidth(s.width()); }
                if (m_minHeightChars > 0) { cs.setHeight(s.height()); }
                mw->resize(cs);
            }
        }
        if (m_saveMainWidgetState && !this->isSet(m_cmdWindowSizeReset))
        {
            this->restoreWindowGeometryAndState();
        }

        if (m_splashScreen)
        {
            m_splashScreen->close(); // GUI
            m_splashScreen.reset();
        }
    }

    QString CGuiApplication::beautifyHelpMessage(const QString &helpText)
    {
        // just formatting Qt help message into HTML table
        if (helpText.isEmpty()) { return ""; }
        const QStringList lines(helpText.split('\n'));
        QString html;
        bool tableMode = false;
        bool pendingTr = false;
        for (const QString &l : lines)
        {
            QString lt(l.trimmed());
            if (!tableMode && lt.startsWith("-"))
            {
                tableMode = true;
                html += "<table>\n";
            }
            if (!tableMode)
            {
                html += l.toHtmlEscaped();
                html += "<br>";
            }
            else
            {
                // in table mode
                if (lt.startsWith("-"))
                {
                    if (pendingTr)
                    {
                        html += "</td></tr>\n";
                    }
                    html += "<tr><td>";
                    thread_local const QRegularExpression reg("[ ]{2,}");
                    html += lt.replace(reg, "</td><td>");
                    pendingTr = true;
                }
                else
                {
                    html += " ";
                    html += l.simplified().toHtmlEscaped();
                }
            }
        }
        html += "</table>\n";
        return html;
    }

    bool CGuiApplication::cmdLineErrorMessage(const QString &errorMessage, bool retry) const
    {
        const QString helpText(beautifyHelpMessage(m_parser.helpText()));
        constexpr int MaxLength = 60;

        QString htmlMsg;
        if (errorMessage.length() > MaxLength)
        {
            htmlMsg = "<html><head/><body><h4>" + errorMessage.left(MaxLength) + "..." + "</h4>" +
                      "Details: " + errorMessage + "<br><br>";
        }
        else
        {
            htmlMsg = "<html><head/><body><h4>" + errorMessage + "</h4>";
        }
        htmlMsg += helpText + "</body></html>";

        const int r = QMessageBox::warning(nullptr,
                                           QGuiApplication::applicationDisplayName(),
                                           htmlMsg, QMessageBox::Abort, retry ? QMessageBox::Retry : QMessageBox::NoButton);
        return (r == QMessageBox::Retry);
    }

    bool CGuiApplication::cmdLineErrorMessage(const CStatusMessageList &msgs, bool retry) const
    {
        if (msgs.isEmpty()) { return false; }
        if (!msgs.hasErrorMessages()) { return false; }
        static const CPropertyIndexList propertiesSingle({ CStatusMessage::IndexMessage });
        static const CPropertyIndexList propertiesMulti({ CStatusMessage::IndexSeverityAsString, CStatusMessage::IndexMessage });
        const QString helpText(CGuiApplication::beautifyHelpMessage(m_parser.helpText()));
        const QString msgsHtml = msgs.toHtml(msgs.size() > 1 ? propertiesMulti : propertiesSingle);
        const int r = QMessageBox::critical(nullptr,
                                            QGuiApplication::applicationDisplayName(),
                                            "<html><head><body>" + msgsHtml + "<br><br>" + helpText + "</body></html>", QMessageBox::Abort, retry ? QMessageBox::Retry : QMessageBox::NoButton);
        return (r == QMessageBox::Retry);
    }

    bool CGuiApplication::displayInStatusBar(const CStatusMessage &message)
    {
        IMainWindowAccess *m = mainWindowAccess();
        BLACK_VERIFY_X(m, Q_FUNC_INFO, "No access interface");
        if (!m) { return false; }
        return m->displayInStatusBar(message);
    }

    bool CGuiApplication::displayInOverlayWindow(const CStatusMessage &message, int timeOutMs)
    {
        IMainWindowAccess *m = mainWindowAccess();
        BLACK_VERIFY_X(m, Q_FUNC_INFO, "No access interface");
        if (!m) { return false; }
        return m->displayInOverlayWindow(message, timeOutMs);
    }

    bool CGuiApplication::displayTextInConsole(const QString &text)
    {
        IMainWindowAccess *m = mainWindowAccess();
        BLACK_VERIFY_X(m, Q_FUNC_INFO, "No access interface");
        if (!m) { return false; }
        return m->displayTextInConsole(text);
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
                QDesktopServices::openUrl(QUrl::fromLocalFile(path));
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
                QDesktopServices::openUrl(QUrl::fromLocalFile(path));
            }
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");

        a = sm->addAction("Reset cache");
        c = connect(a, &QAction::triggered, this, [this]()
        {
            const QStringList files = CApplication::clearCaches();
            this->displayTextInConsole("Cleared caches! " + QString::number(files.size()) + " files");
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");

        a = sm->addAction("List cache files");
        c = connect(a, &QAction::triggered, this, [this]()
        {
            const QStringList files(CDataCache::instance()->enumerateStore());
            this->displayTextInConsole(files.join("\n"));
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");

        a = menu.addAction(CIcons::disk16(), "Log directory");
        c = connect(a, &QAction::triggered, this, [this]()
        {
            const QString path(QDir::toNativeSeparators(CDirectoryUtils::logDirectory()));
            if (QDir(path).exists())
            {
                QDesktopServices::openUrl(QUrl::fromLocalFile(path));
            }
        });

        a = menu.addAction(CIcons::swift24(), "Check for updates");
        c = connect(a, &QAction::triggered, this, &CGuiApplication::checkNewVersionMenu);

        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
        Q_UNUSED(c);
    }

    void CGuiApplication::addMenuForStyleSheets(QMenu &menu)
    {
        QMenu *sm = menu.addMenu("Style sheet");
        QAction *aReload = sm->addAction(CIcons::refresh16(), "Reload");
        bool c = connect(aReload, &QAction::triggered, this, [aReload, this]()
        {
            this->reloadStyleSheets();
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");

        QAction *aOpen = sm->addAction(CIcons::text16(), "Open qss file");
        c = connect(aOpen, &QAction::triggered, this, [aOpen, this]()
        {
            this->openStandardWidgetStyleSheet();
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
        Q_UNUSED(c);
    }

    void CGuiApplication::addMenuFile(QMenu &menu)
    {
        addMenuForSettingsAndCache(menu);
        addMenuForStyleSheets(menu);
        QAction *a = nullptr;
        bool c = false;
        if (this->getApplicationInfo().application() != CApplicationInfo::Laucher)
        {
            menu.addSeparator();
            a = menu.addAction(CIcons::swiftLauncher24(), "Start swift launcher");
            c = connect(a, &QAction::triggered, this, &CGuiApplication::startLauncher);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
        }

        menu.addSeparator();
        a = menu.addAction("E&xit");
        a->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Q));
        c = connect(a, &QAction::triggered, this, [a, this]()
        {
            // a close event might already trigger a shutdown
            this->mainApplicationWidget()->close();
            this->gracefulShutdown();
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
        Q_UNUSED(c);
    }

    void CGuiApplication::addMenuInternals(QMenu &menu)
    {
        QMenu *sm = menu.addMenu("Templates");
        QAction *a = sm->addAction("JSON bootstrap");
        bool c = connect(a, &QAction::triggered, this, [a, this]()
        {
            const CGlobalSetup s = this->getGlobalSetup();
            this->displayTextInConsole(s.toJsonString());
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");

        a = sm->addAction("JSON update info (for info only)");
        c = connect(a, &QAction::triggered, this, [a, this]()
        {
            const CUpdateInfo info = this->getUpdateInfo();
            this->displayTextInConsole(info.toJsonString());
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");

        if (this->hasWebDataServices())
        {
            a = menu.addAction("Services log");
            c = connect(a, &QAction::triggered, this, [a, this]()
            {
                this->displayTextInConsole(this->getWebDataServices()->getReadersLog());
            });
            Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
        }

        a = menu.addAction("Metadata (slow)");
        c = connect(a, &QAction::triggered, this, [a, this]()
        {
            this->displayTextInConsole(getAllUserMetatypesTypes());
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
        Q_UNUSED(c);
    }

    void CGuiApplication::addMenuWindow(QMenu &menu)
    {
        QWidget *w = mainApplicationWidget();
        if (!w) { return; }
        const QSize iconSize = CIcons::empty16().size();
        QPixmap icon = w->style()->standardIcon(QStyle::SP_TitleBarMaxButton).pixmap(iconSize);
        QAction *a = menu.addAction(icon.scaled(iconSize), "Fullscreen");
        bool c = connect(a, &QAction::triggered, this, [a, w]()
        {
            w->showFullScreen();
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");

        icon = w->style()->standardIcon(QStyle::SP_TitleBarMinButton).pixmap(iconSize);
        a = menu.addAction(icon.scaled(iconSize), "Minimize");
        c = connect(a, &QAction::triggered, this, [a, w]()
        {
            w->showMinimized();
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");

        icon = w->style()->standardIcon(QStyle::SP_TitleBarNormalButton).pixmap(iconSize);
        a = menu.addAction(icon.scaled(iconSize), "Normal");
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
        Q_UNUSED(c);
    }

    void CGuiApplication::addMenuHelp(QMenu &menu)
    {
        QWidget *w = mainApplicationWidget();
        if (!w) { return; }
        QAction *a = menu.addAction(w->style()->standardIcon(QStyle::SP_TitleBarContextHelpButton), "Online help");
        bool c = connect(a, &QAction::triggered, this, [this]()
        {
            this->showHelp();
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");

        a = menu.addAction(QApplication::windowIcon(), "About swift");
        c = connect(a, &QAction::triggered, this, [w]()
        {
            CAboutDialog dialog(w);
            dialog.exec();
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
        Q_UNUSED(c);

        // https://joekuan.wordpress.com/2015/09/23/list-of-qt-icons/
        a = menu.addAction(QApplication::style()->standardIcon(QStyle::SP_TitleBarMenuButton), "About Qt");
        c = connect(a, &QAction::triggered, this, []()
        {
            QApplication::aboutQt();
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
        Q_UNUSED(c);
    }

    void CGuiApplication::showHelp(const QString &context) const
    {
        const CGlobalSetup gs = this->getGlobalSetup();
        const CUrl helpPage = gs.getHelpPageUrl(context);
        if (helpPage.isEmpty())
        {
            CLogMessage(this).warning("No help page");
            return;
        }
        QDesktopServices::openUrl(helpPage);
    }

    void CGuiApplication::showHelp(const QObject *qObject) const
    {
        if (!qObject || qObject->objectName().isEmpty()) { return this->showHelp(); }
        return this->showHelp(qObject->objectName());
    }

    const CStyleSheetUtility &CGuiApplication::getStyleSheetUtility() const
    {
        return m_styleSheetUtility;
    }

    QString CGuiApplication::getWidgetStyle() const
    {
        QString currentWidgetStyle(QApplication::style()->metaObject()->className());
        if (currentWidgetStyle.startsWith('Q')) { currentWidgetStyle.remove(0, 1); }
        return currentWidgetStyle.replace("Style", "");
    }

    bool CGuiApplication::reloadStyleSheets()
    {
        return m_styleSheetUtility.read();
    }

    bool CGuiApplication::openStandardWidgetStyleSheet()
    {
        const QString fn = CStyleSheetUtility::fileNameAndPathStandardWidget();
        return QDesktopServices::openUrl(QUrl::fromLocalFile(fn));
    }

    bool CGuiApplication::updateFont(const QString &fontFamily, const QString &fontSize, const QString &fontStyle, const QString &fontWeight, const QString &fontColor)
    {
        return m_styleSheetUtility.updateFont(fontFamily, fontSize, fontStyle, fontWeight, fontColor);
    }

    bool CGuiApplication::updateFont(const QString &qss)
    {
        return m_styleSheetUtility.updateFont(qss);
    }

    bool CGuiApplication::resetFont()
    {
        return m_styleSheetUtility.resetFont();
    }

    void CGuiApplication::setMinimumSizeInCharacters(int widthChars, int heightChars)
    {
        m_minWidthChars = widthChars;
        m_minHeightChars = heightChars;
    }

    bool CGuiApplication::interactivelySynchronizeSetup(int timeoutMs)
    {
        bool ok = false;
        do
        {
            const CStatusMessageList msgs = this->synchronizeSetup(timeoutMs);
            if (msgs.hasErrorMessages())
            {
                static const QString style = sGui->getStyleSheetUtility().styles(
                {
                    CStyleSheetUtility::fileNameFonts(),
                    CStyleSheetUtility::fileNameStandardWidget()
                });
                CSetupLoadingDialog dialog(msgs, this->mainApplicationWidget());
                dialog.setStyleSheet(style);
                const int r = dialog.exec();
                if (r == QDialog::Rejected)
                {
                    ok = false;
                    break;
                }
            }
            else
            {
                ok = true;
                break;
            }
        }
        while (true);
        return ok;
    }

    bool CGuiApplication::parseAndSynchronizeSetup(int timeoutMs)
    {
        if (!this->parseAndStartupCheck()) return false;
        return this->interactivelySynchronizeSetup(timeoutMs);
    }

    QDialog::DialogCode CGuiApplication::showCloseDialog(QMainWindow *mainWindow, QCloseEvent *closeEvent)
    {
        this->saveSettingsOnShutdown(false); // saving itself will be handled in dialog
        const bool needsDialog = this->hasUnsavedSettings();
        if (!needsDialog) { return QDialog::Accepted; }
        if (!m_closeDialog)
        {
            m_closeDialog = new CApplicationCloseDialog(mainWindow);
            if (mainWindow && !mainWindow->windowTitle().isEmpty())
            {
                m_closeDialog->setWindowTitle(mainWindow->windowTitle());
                m_closeDialog->setModal(true);
            }
        }

        // dialog will handle the saving
        const QDialog::DialogCode c = static_cast<QDialog::DialogCode>(m_closeDialog->exec());

        // settings already saved when reaching here
        switch (c)
        {
        case QDialog::Rejected:
            if (closeEvent) { closeEvent->ignore(); }
            break;
        default:
            break;
        }
        return c;
    }

    void CGuiApplication::cmdLineHelpMessage()
    {
        if (CBuildConfig::isRunningOnWindowsNtPlatform())
        {
            const QString helpText(CGuiApplication::beautifyHelpMessage(m_parser.helpText()));
            QMessageBox::information(nullptr, QGuiApplication::applicationDisplayName(),
                                     "<html><head/><body>" + helpText + "</body></html>");
        }
        else
        {
            CApplication::cmdLineHelpMessage();
        }
    }

    void CGuiApplication::cmdLineVersionMessage() const
    {
        if (CBuildConfig::isRunningOnWindowsNtPlatform())
        {
            QMessageBox::information(nullptr,
                                     QGuiApplication::applicationDisplayName(),
                                     QGuiApplication::applicationDisplayName() + ' ' +
                                     QCoreApplication::applicationVersion());
        }
        else
        {
            CApplication::cmdLineVersionMessage();
        }
    }

    bool CGuiApplication::parsingHookIn()
    {
        return true;
    }

    void CGuiApplication::onCoreFacadeStarted()
    {
        if (this->supportsContexts())
        {
            connect(this->getIContextApplication(), &IContextApplication::requestDisplayOnConsole, this, &CGuiApplication::displayTextInConsole);
        }
    }

    void CGuiApplication::checkNewVersion(bool onlyIfNew)
    {
        if (!m_updateDialog)
        {
            // without parent stylesheet is not inherited
            m_updateDialog = new CUpdateInfoDialog(this->mainApplicationWidget());
        }

        if (onlyIfNew && !m_updateDialog->isNewVersionAvailable()) return;
        const int result = m_updateDialog->exec();
        if (result != QDialog::Accepted) { return; }
    }

    QString CGuiApplication::getFontInfo() const
    {
        static const QString info("Family: '%1', average width: %2");
        const QWidget *w = this->mainApplicationWidget();
        if (!w) { return QStringLiteral("Font info not available"); }
        return info.
               arg(w->font().family()).
               arg(w->fontMetrics().averageCharWidth());
    }

    void CGuiApplication::triggerNewVersionCheck(int delayedMs)
    {
        if (!m_updateSetting.get()) { return; }
        QTimer::singleShot(delayedMs, this, [ = ]
        {
            if (m_updateDialog) { return; }
            this->checkNewVersion(true);
        });
    }

    void CGuiApplication::gracefulShutdown()
    {
        if (m_saveMainWidgetState)
        {
            this->saveWindowGeometryAndState();
        }
        CApplication::gracefulShutdown();
    }

    void CGuiApplication::settingsChanged()
    {
        // changing widget style is slow, so I try to prevent setting it when nothing changed
        const QString widgetStyle = m_guiSettings.get().getWidgetStyle();
        const QString currentWidgetStyle(this->getWidgetStyle());
        if (!(currentWidgetStyle.length() == widgetStyle.length() && currentWidgetStyle.startsWith(widgetStyle, Qt::CaseInsensitive)))
        {
            const auto availableStyles = QStyleFactory::keys();
            if (availableStyles.contains(widgetStyle))
            {
                // changing style freezes the application, so it must not be done in flight mode
                if (this->getIContextNetwork() && this->getIContextNetwork()->isConnected())
                {
                    CLogMessage(this).validationError("Cannot change style while connected to network");
                }
                else
                {
                    QApplication::setStyle(QStyleFactory::create(widgetStyle));
                }
            }
        }
    }

    void CGuiApplication::checkNewVersionMenu()
    {
        this->checkNewVersion(false);
    }

    void CGuiApplication::adjustPalette()
    {
        // only way to change link color
        // https://stackoverflow.com/q/5497799/356726
        // Ref T84
        QPalette newPalette(qApp->palette());
        const QColor linkColor(135, 206, 250);
        newPalette.setColor(QPalette::Link, linkColor);
        newPalette.setColor(QPalette::LinkVisited, linkColor);
        qApp->setPalette(newPalette);
    }

    void CGuiApplication::onStyleSheetsChanged()
    {
        emit this->styleSheetsChanged();
        const QFont f = CGuiUtility::currentFont();
        if (f.pointSize() != m_fontPointSize || f.family() != m_fontFamily)
        {
            emit this->fontChanged();
            CLogMessage(this).info(this->getFontInfo());
        }
    }

    void CGuiApplication::setCurrentFontValues()
    {
        const QFont font = CGuiUtility::currentFont();
        m_fontFamily = font.family();
        m_fontPointSize = font.pointSize();
    }
} // ns
