// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/components/applicationclosedialog.h"
#include "blackgui/components/updateinfodialog.h"
#include "blackgui/components/aboutdialog.h"
#include "blackgui/components/setuploadingdialog.h"
#include "blackgui/splashscreen.h"
#include "blackgui/guiapplication.h"
#include "blackgui/guiutility.h"
#include "blackgui/registermetadata.h"
#include "blackcore/context/contextnetwork.h"
#include "blackcore/data/globalsetup.h"
#include "blackcore/db/networkwatchdog.h"
#include "blackcore/db/infodatareader.h"
#include "blackcore/webdataservices.h"
#include "blackcore/setupreader.h"
#include "blackmisc/slot.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/swiftdirectories.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/datacache.h"
#include "blackmisc/logcategories.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/loghandler.h"
#include "blackmisc/metadatautils.h"
#include "blackmisc/registermetadata.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/verify.h"
#include "blackconfig/buildconfig.h"

#include <QAction>
#include <QCloseEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QCommandLineParser>
#include <QDesktopServices>
#include <QDir>
#include <QEventLoop>
#include <QApplication>
#include <QGuiApplication>
#include <QIcon>
#include <QFont>
#include <QKeySequence>
#include <QMenu>
#include <QMessageBox>
#include <QProcess>
#include <QRegularExpression>
#include <QSettings>
#include <QSplashScreen>
#include <QMessageBox>
#include <QStyleFactory>
#include <QStringList>
#include <QStringBuilder>
#include <QStyle>
#include <QSysInfo>
#include <QToolBar>
#include <QUrl>
#include <QWidget>
#include <QWindow>
#include <QMainWindow>
#include <QMessageBox>
#include <QtGlobal>
#include <QWhatsThis>
#include <Qt>

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

    const QStringList &CGuiApplication::getLogCategories()
    {
        static const QStringList l(CApplication::getLogCategories() + QStringList { CLogCategories::guiComponent() });
        return l;
    }

    const QString &CGuiApplication::settingsOrganization()
    {
        static const QString o("swift-project.org");
        return o;
    }

    bool CGuiApplication::removeAllWindowsSwiftRegistryEntries()
    {
        if (!CBuildConfig::isRunningOnWindowsNtPlatform()) { return false; }

        // On Windows, NativeFormat settings are stored in the following registry paths:
        // HKEY_CURRENT_USER\Software\MySoft\Star Runner.
        // HKEY_CURRENT_USER\Software\MySoft\OrganizationDefaults.
        // HKEY_LOCAL_MACHINE\Software\MySoft\Star Runner.
        // HKEY_LOCAL_MACHINE\Software\MySoft\OrganizationDefaults.

        QSettings s1("HKEY_CURRENT_USER\\Software\\" + settingsOrganization(), QSettings::NativeFormat);
        s1.remove("");

        QSettings s2("HKEY_LOCAL_MACHINE\\Software\\" + settingsOrganization(), QSettings::NativeFormat);
        s2.remove("");

        return true;
    }

    CGuiApplication::CGuiApplication(const QString &applicationName, CApplicationInfo::Application application, const QPixmap &icon) : CApplication(applicationName, application, false)
    {
        this->addWindowModeOption();
        this->addWindowResetSizeOption();
        this->addWindowScaleSizeOption();

        // notify when app goes down
        connect(qGuiApp, &QGuiApplication::lastWindowClosed, this, &CGuiApplication::gracefulShutdown);

        // follow up on web data services
        connect(this, &CApplication::webDataServicesStarted, this, &CGuiApplication::onWebDataServicesStarted);

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

            connect(&m_styleSheetUtility, &CStyleSheetUtility::styleSheetsChanged, this, &CGuiApplication::onStyleSheetsChanged, Qt::QueuedConnection);
            connect(this, &CGuiApplication::startUpCompleted, this, &CGuiApplication::superviseWindowMinSizes, Qt::QueuedConnection);

            // splash screen
            connect(this->getSetupReader(), &CSetupReader::setupLoadingMessages, this, &CGuiApplication::displaySplashMessages, Qt::QueuedConnection);
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
        m_cmdWindowMode = QCommandLineOption({ "w", "window" }, QCoreApplication::translate("main", "Windows: (n)ormal, (f)rameless, (t)ool."), "windowtype");
        this->addParserOption(m_cmdWindowMode);
    }

    void CGuiApplication::addWindowResetSizeOption()
    {
        m_cmdWindowSizeReset = QCommandLineOption({ { "r", "resetsize" }, QCoreApplication::translate("main", "Reset window size (ignore saved values).") });
        this->addParserOption(m_cmdWindowSizeReset);
    }

    void CGuiApplication::addWindowScaleSizeOption()
    {
        // just added here to display it in help
        // parseScaleFactor() is used since it is needed upfront (before application is created)
        m_cmdWindowScaleSize = QCommandLineOption("scale", QCoreApplication::translate("main", "Scale: number."), "scalevalue");
        this->addParserOption(m_cmdWindowScaleSize);
    }

    void CGuiApplication::addWindowStateOption()
    {
        m_cmdWindowStateMinimized = QCommandLineOption({ { "m", "minimized" }, QCoreApplication::translate("main", "Start minimized in system tray.") });
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

        QFont splashFont;
        splashFont.setFamily("Arial");
        // splashFont.setBold(true);
        splashFont.setPointSize(10);
        splashFont.setStretch(125);

        m_splashScreen.reset(new CSplashScreen(pixmap.scaled(256, 256)));
        m_splashScreen->show();
        m_splashScreen->showStatusMessage(CBuildConfig::getVersionString());
        m_splashScreen->setSplashFont(splashFont);

        this->processEventsToRefreshGui();
    }

    void CGuiApplication::displaySplashMessage(const CStatusMessage &msg)
    {
        if (msg.isEmpty()) { return; }
        if (!m_splashScreen) { return; }
        if (this->isShuttingDown()) { return; }
        if (!m_splashScreen->isVisible()) { return; }
        m_splashScreen->showStatusMessage(msg);
    }

    void CGuiApplication::displaySplashMessages(const CStatusMessageList &msgs)
    {
        if (msgs.isEmpty()) { return; }
        for (const CStatusMessage &m : msgs)
        {
            if (!m_splashScreen) { return; }
            this->displaySplashMessage(m);
            this->processEventsToRefreshGui();
            if (!sGui) { return; }
        }
    }

    void CGuiApplication::processEventsToRefreshGui() const
    {
        if (this->isShuttingDown()) { return; }
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

    bool CGuiApplication::hasMinimumMappingVersion() const
    {
        if (this->getGlobalSetup().isSwiftVersionMinimumMappingVersion()) { return true; }

        const QString msg = QStringLiteral("Your are using swift version: '%1'.\nCreating mappings requires at least '%2'.").arg(CBuildConfig::getVersionString(), this->getGlobalSetup().getMappingMinimumVersionString());
        QMessageBox::warning(this->mainApplicationWindow(), "Version check", msg, QMessageBox::Close);
        return false;
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

        const QString name = this->setExtraWindowTitle("", mainWidget);
        mainWidget->setObjectName(QCoreApplication::applicationName());
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
            QPointer<CGuiApplication> myself(this);
            connectOnce(this, &CGuiApplication::uiObjectTreeReady, this, [=] {
                if (!myself) { return; }
                this->addWindowFlags(flags);
            });
        }
    }

    QString CGuiApplication::setExtraWindowTitle(const QString &extraInfo, QWidget *mainWindowWidget) const
    {
        QString name(this->getApplicationNameVersionDetailed());
        if (!extraInfo.isEmpty()) { name = extraInfo % u' ' % name; }
        if (!mainWindowWidget) { return name; }
        mainWindowWidget->setWindowTitle(name);
        return name;
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

    void CGuiApplication::highDpiScreenSupport(const QString &scaleFactor)
    {
        // https://lists.qt-project.org/pipermail/development/2019-September/037434.html
        // QSize s = CGuiUtility::physicalScreenSizeOs();
        QString sf = scaleFactor.trimmed().isEmpty() ? defaultScaleFactorString() : scaleFactor;
        if (sf.contains('/'))
        {
            const double sfd = parseFraction(scaleFactor, -1);
            sf = sfd < 0 ? "1.0" : QString::number(sfd, 'f', 8);
        }

        sf = cleanNumber(sf);

        QApplication::setAttribute(Qt::AA_EnableHighDpiScaling); // DPI support
        QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps); // HiDPI pixmaps

        // qputenv("QT_ENABLE_HIGHDPI_SCALING", "1");
        QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Floor);

        const QByteArray sfa = sf.toLatin1();
        qputenv("QT_SCALE_FACTOR", sfa);
    }

    bool CGuiApplication::isUsingHighDpiScreenSupport()
    {
        return CGuiUtility::isUsingHighDpiScreenSupport();
    }

    QScreen *CGuiApplication::currentScreen()
    {
        const QWidget *w = CGuiApplication::mainApplicationWidget();
        const int s = QApplication::desktop()->screenNumber(w);
        if (s >= 0 && s < QGuiApplication::screens().size()) { return QGuiApplication::screens().at(s); }
        return QGuiApplication::primaryScreen();
    }

    QRect CGuiApplication::currentScreenGeometry()
    {
        const QScreen *s = currentScreen();
        if (s) return s->geometry();
        return {};
    }

    void CGuiApplication::modalWindowToFront()
    {
        if (!QGuiApplication::modalWindow()) { return; }
        QGuiApplication::modalWindow()->raise();
    }

    const QString &CGuiApplication::fileForWindowGeometryAndStateSettings()
    {
        static const QString filename = [] {
            QString dir = CFileUtils::appendFilePaths(CSwiftDirectories::normalizedApplicationDataDirectory(), "settings/qgeom");
            return CFileUtils::appendFilePaths(dir, QFileInfo(QCoreApplication::applicationFilePath()).completeBaseName() + ".ini");
        }();
        return filename;
    }

    int CGuiApplication::hashForStateSettingsSchema(const QMainWindow *window)
    {
        unsigned int hash = 0;
        for (auto obj : window->findChildren<QToolBar *>(QString(), Qt::FindDirectChildrenOnly))
        {
            hash ^= qHash(obj->objectName());
        }
        for (auto obj : window->findChildren<QDockWidget *>(QString(), Qt::FindDirectChildrenOnly))
        {
            hash ^= qHash(obj->objectName());
        }
        return static_cast<int>((hash & 0xffff) ^ (hash >> 16));
    }

    bool CGuiApplication::saveWindowGeometryAndState(const QMainWindow *window) const
    {
        if (!window) { return false; }
        QSettings settings(fileForWindowGeometryAndStateSettings(), QSettings::IniFormat);
        settings.setValue("geometry", window->saveGeometry());
        settings.setValue("windowState", window->saveState(hashForStateSettingsSchema(window)));
        return true;
    }

    void CGuiApplication::resetWindowGeometryAndState()
    {
        QByteArray ba;
        QSettings settings(fileForWindowGeometryAndStateSettings(), QSettings::IniFormat);
        settings.setValue("geometry", ba);
        settings.setValue("windowState", ba);
    }

    bool CGuiApplication::restoreWindowGeometryAndState(QMainWindow *window)
    {
        if (!window) { return false; }
        const QSettings settings(fileForWindowGeometryAndStateSettings(), QSettings::IniFormat);
        const QString location = settings.fileName();
        CLogMessage(this).info(u"GUI settings are here: '%1'") << location;

        const QByteArray g = settings.value("geometry").toByteArray();
        const QByteArray s = settings.value("windowState").toByteArray();
        if (g.isEmpty() || s.isEmpty()) { return false; }

        // block for subscriber
        {
            const auto pattern = CLogPattern().withSeverity(CStatusMessage::SeverityError);
            const QString parameter = m_cmdWindowSizeReset.names().first();
            CLogSubscriber logSub(this, [&](const CStatusMessage &message) {
                // handles an error in restoreGeometry/State
                const int ret = QMessageBox::critical(sGui->mainApplicationWidget(), sGui->getApplicationNameAndVersion(),
                                                      QStringLiteral(
                                                          "Restoring the window state/geometry failed!\n"
                                                          "You need to reset the window size (command -%1).\n\n"
                                                          "Original msg: %2\n\n"
                                                          "We can try to reset the values and restart\n"
                                                          "Do you want to try?")
                                                          .arg(parameter, message.getMessage()),
                                                      QMessageBox::Yes | QMessageBox::No);
                if (ret == QMessageBox::Yes)
                {
                    this->resetWindowGeometryAndState();
                    this->restartApplication();
                }
                // most likely crashing if we do nothing
            });
            logSub.changeSubscription(pattern);

            window->restoreGeometry(g);
            window->restoreState(s, hashForStateSettingsSchema(window));
        }
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
            const QSizeF fontMetricEstSize = CGuiUtility::fontMetricsEstimateSize(m_minWidthChars, m_minHeightChars);
            QWidget *mw = CGuiUtility::mainApplicationWidget();
            if (mw)
            {
                // setMinimumSizeInCharacters sets m_minHeightChars/m_minWidthChars
                QSize cs = mw->size();
                if (m_minWidthChars > 0) { cs.setWidth(qRound(fontMetricEstSize.width())); }
                if (m_minHeightChars > 0) { cs.setHeight(qRound(fontMetricEstSize.height())); }
                mw->resize(cs);
            }
        }
        if (m_saveMainWidgetState && !this->isSet(m_cmdWindowSizeReset))
        {
            const Qt::KeyboardModifiers km = QGuiApplication::queryKeyboardModifiers();
            const bool shiftAlt = km.testFlag(Qt::ShiftModifier) && km.testFlag(Qt::AltModifier);
            if (!shiftAlt) { this->restoreWindowGeometryAndState(); }
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
        if (helpText.isEmpty()) { return {}; }
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

    double CGuiApplication::parseScaleFactor(int argc, char *argv[])
    {
        for (int i = 1; i < argc; ++i)
        {
            if (qstrcmp(argv[i], "--scale") == 0 || qstrcmp(argv[i], "-scale") == 0)
            {
                if (i + 1 >= argc) { return -1.0; } // no value
                const QString factor(argv[i + 1]);
                bool ok;
                const double f = factor.toDouble(&ok);
                return ok ? f : -1.0;
            }
        }
        return -1.0;
    }

    QString CGuiApplication::scaleFactor(int argc, char *argv[])
    {
        for (int i = 1; i < argc; ++i)
        {
            if (qstrcmp(argv[i], "--scale") == 0 || qstrcmp(argv[i], "-scale") == 0)
            {
                if (i + 1 >= argc) { return QString(); } // no value
                const QString factor(argv[i + 1]);
                return factor.trimmed();
            }
        }
        return QString();
    }

    QString CGuiApplication::defaultScaleFactorString()
    {
        if (!CBuildConfig::isRunningOnWindowsNtPlatform()) { return "1.0"; }

        // On windows
        // Qt 5.14.1 default is device ratio 3
        // Qt 5.14.0 default device ratio was 2

        // 2/3 (0.66667) => device ratio 3
        // 0.75 => device ratio 2.25
        // 0.8  => device ratio 2.4
        // 1.00 => device ratio 3

        // currently NOT used
        return "1.0";
    }

    bool CGuiApplication::cmdLineWarningMessage(const QString &text, const QString &informativeText) const
    {
        QMessageBox warningBox(QMessageBox::Warning, QGuiApplication::applicationDisplayName(), "<b>" + text + "</b>");
        warningBox.setInformativeText(informativeText);
        return warningBox.exec();
    }

    bool CGuiApplication::cmdLineErrorMessage(const QString &text, const QString &informativeText, bool retry) const
    {
        QMessageBox errorBox(QMessageBox::Critical, QGuiApplication::applicationDisplayName(), "<b>" + text + "</b>");
        if (informativeText.length() < 300)
            errorBox.setInformativeText(informativeText);
        else
            errorBox.setDetailedText(informativeText);

        errorBox.addButton(QMessageBox::Abort);
        if (retry)
            errorBox.addButton(QMessageBox::Retry);

        const int r = errorBox.exec();

        return (r == QMessageBox::Retry);
    }

    bool CGuiApplication::cmdLineErrorMessage(const CStatusMessageList &msgs, bool retry) const
    {
        if (msgs.isEmpty()) { return false; }
        if (!msgs.hasErrorMessages()) { return false; }
        static const CPropertyIndexList propertiesSingle({ CStatusMessage::IndexMessage });
        static const CPropertyIndexList propertiesMulti({ CStatusMessage::IndexSeverityAsString, CStatusMessage::IndexMessage });
        const QString msgsHtml = msgs.toHtml(msgs.size() > 1 ? propertiesMulti : propertiesSingle);
        const int r = QMessageBox::critical(nullptr,
                                            QGuiApplication::applicationDisplayName(),
                                            "<html><head><body>" + msgsHtml + "</body></html>", QMessageBox::Abort, retry ? QMessageBox::Retry : QMessageBox::NoButton);
        return (r == QMessageBox::Retry);
    }

    bool CGuiApplication::isCmdWindowSizeResetSet() const
    {
        return this->isParserOptionSet(m_cmdWindowSizeReset);
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
        if (message.isEmpty()) { return false; }
        IMainWindowAccess *m = mainWindowAccess();
        BLACK_VERIFY_X(m, Q_FUNC_INFO, "No access interface");
        if (!m) { return IMainWindowAccess::displayInOverlayWindow(message, timeOutMs); }
        return m->displayInOverlayWindow(message, timeOutMs);
    }

    bool CGuiApplication::displayInOverlayWindow(const CStatusMessageList &messages, int timeOutMs)
    {
        if (messages.isEmpty()) { return false; }
        IMainWindowAccess *m = mainWindowAccess();
        BLACK_VERIFY_X(m, Q_FUNC_INFO, "No access interface");
        if (!m) { return IMainWindowAccess::displayInOverlayWindow(messages, timeOutMs); }
        return m->displayInOverlayWindow(messages, timeOutMs);
    }

    bool CGuiApplication::displayInOverlayWindow(const QString &html, int timeOutMs)
    {
        if (html.isEmpty()) { return false; }
        IMainWindowAccess *m = mainWindowAccess();
        BLACK_VERIFY_X(m, Q_FUNC_INFO, "No access interface");
        if (!m) { return IMainWindowAccess::displayInOverlayWindow(html, timeOutMs); }
        return m->displayInOverlayWindow(html, timeOutMs);
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
        bool c = connect(a, &QAction::triggered, this, [=]() {
            if (!sGui || sGui->isShuttingDown()) { return; }
            const QString path(QDir::toNativeSeparators(CSettingsCache::persistentStore()));
            if (QDir(path).exists())
            {
                QDesktopServices::openUrl(QUrl::fromLocalFile(path));
            }
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");

        a = sm->addAction("Reset settings");
        c = connect(a, &QAction::triggered, this, [=] {
            if (!sGui || sGui->isShuttingDown()) { return; }
            CSettingsCache::instance()->clearAllValues();
            this->displayTextInConsole("Cleared all settings!");
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");

        a = sm->addAction("List settings files");
        c = connect(a, &QAction::triggered, this, [=]() {
            if (!sGui || sGui->isShuttingDown()) { return; }
            const QStringList files(CSettingsCache::instance()->enumerateStore());
            this->displayTextInConsole(files.join("\n"));
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");

        sm = menu.addMenu("Cache");
        sm->setIcon(CIcons::appSettings16());
        a = sm->addAction(CIcons::disk16(), "Cache directory");
        c = connect(a, &QAction::triggered, this, [=]() {
            const QString path(QDir::toNativeSeparators(CDataCache::persistentStore()));
            if (QDir(path).exists())
            {
                QDesktopServices::openUrl(QUrl::fromLocalFile(path));
            }
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");

        a = sm->addAction("Reset cache");
        c = connect(a, &QAction::triggered, this, [=]() {
            if (!sGui || sGui->isShuttingDown()) { return; }
            const QStringList files = CApplication::clearCaches();
            this->displayTextInConsole(u"Cleared caches! " % QString::number(files.size()) + " files");
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");

        a = sm->addAction("List cache files");
        c = connect(a, &QAction::triggered, this, [=]() {
            if (!sGui || sGui->isShuttingDown()) { return; }
            const QStringList files(CDataCache::instance()->enumerateStore());
            this->displayTextInConsole(files.join("\n"));
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");

        a = menu.addAction(CIcons::disk16(), "Log directory");
        c = connect(a, &QAction::triggered, this, [=]() {
            if (!sGui || sGui->isShuttingDown()) { return; }
            this->openStandardLogDirectory();
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");

        a = menu.addAction(CIcons::disk16(), "Crash dumps directory");
        c = connect(a, &QAction::triggered, this, [=]() {
            if (!sGui || sGui->isShuttingDown()) { return; }
            this->openStandardCrashDumpDirectory();
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");

        a = menu.addAction(CIcons::swift24(), "Check for updates");
        c = connect(a, &QAction::triggered, this, &CGuiApplication::checkNewVersionMenu);
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");

        a = menu.addAction(CIcons::monitorError16(), "Network config. (console)");
        c = connect(a, &QAction::triggered, this, [=]() {
            if (!sGui || sGui->isShuttingDown()) { return; }
            const QString r = CNetworkUtils::createNetworkConfigurationReport(this->getNetworkConfigurationManager(), this->getNetworkAccessManager());
            this->displayTextInConsole(r);

            if (this->getNetworkWatchdog())
            {
                const QString w = this->getNetworkWatchdog()->getCheckInfo();
                this->displayTextInConsole(w);
            }
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
        Q_UNUSED(c)
    }

    void CGuiApplication::addMenuForStyleSheets(QMenu &menu)
    {
        QMenu *sm = menu.addMenu("Style sheet");
        QAction *aReload = sm->addAction(CIcons::refresh16(), "Reload");
        bool c = connect(aReload, &QAction::triggered, this, [=]() {
            if (!sGui || sGui->isShuttingDown()) { return; }
            this->reloadStyleSheets();
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");

        QAction *aOpen = sm->addAction(CIcons::text16(), "Open qss file");
        c = connect(aOpen, &QAction::triggered, this, [=]() {
            if (!sGui || sGui->isShuttingDown()) { return; }
            this->openStandardWidgetStyleSheet();
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
        Q_UNUSED(c)
    }

    void CGuiApplication::addMenuFile(QMenu &menu)
    {
        addMenuForSettingsAndCache(menu);
        addMenuForStyleSheets(menu);
        QAction *a = nullptr;
        bool c = false;
        if (this->getApplicationInfo().getApplication() != CApplicationInfo::Laucher)
        {
            menu.addSeparator();
            a = menu.addAction(CIcons::swiftLauncher24(), "Start swift launcher");
            c = connect(a, &QAction::triggered, this, &CGuiApplication::startLauncher);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
        }

        menu.addSeparator();
        a = menu.addAction("E&xit");
        // a->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Q)); // avoid accidentally closing
        c = connect(
            a, &QAction::triggered, this, [=]() {
                // a close event might already trigger a shutdown
                if (!sGui || sGui->isShuttingDown()) { return; }
                if (!this->mainApplicationWidget()) { return; }
                this->mainApplicationWidget()->close();

                // T596, do not shutdown here, as close can be canceled
                // if shutdown is called, there is no way back
                // this->gracefulShutdown();
            },
            Qt::QueuedConnection);
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
        Q_UNUSED(c)
    }

    void CGuiApplication::addMenuInternals(QMenu &menu)
    {
        QMenu *sm = menu.addMenu("JSON files/Templates");
        QAction *a = sm->addAction("JSON bootstrap");
        bool c = connect(
            a, &QAction::triggered, this, [=]() {
                if (!sGui || sGui->isShuttingDown()) { return; }
                const CGlobalSetup s = this->getGlobalSetup();
                this->displayTextInConsole(s.toJsonString());
            },
            Qt::QueuedConnection);
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");

        a = sm->addAction("JSON version update info (for info only)");
        c = connect(
            a, &QAction::triggered, this, [=]() {
                if (!sGui || sGui->isShuttingDown()) { return; }
                const CUpdateInfo info = this->getUpdateInfo();
                this->displayTextInConsole(info.toJsonString());
            },
            Qt::QueuedConnection);
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");

        if (this->hasWebDataServices())
        {
            a = menu.addAction("Services log.(console)");
            c = connect(
                a, &QAction::triggered, this, [=]() {
                    if (!sGui || sGui->isShuttingDown()) { return; }
                    this->displayTextInConsole(this->getWebDataServices()->getReadersLog());
                    CLogMessage(this).info(u"Displayed services log.");
                },
                Qt::QueuedConnection);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");

            a = sm->addAction("JSON DB info (for info only)");
            c = connect(
                a, &QAction::triggered, this, [=]() {
                    if (!sGui || sGui->isShuttingDown()) { return; }
                    if (!this->getWebDataServices()->getDbInfoDataReader()) { return; }
                    const CDbInfoList info = this->getWebDataServices()->getDbInfoDataReader()->getInfoObjects();
                    this->displayTextInConsole(u"DB info:\n" % info.toJsonString());
                },
                Qt::QueuedConnection);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");

            a = sm->addAction("JSON shared info (for info only)");
            c = connect(
                a, &QAction::triggered, this, [=]() {
                    if (!sGui || sGui->isShuttingDown()) { return; }
                    if (!this->getWebDataServices()->getDbInfoDataReader()) { return; }
                    const CDbInfoList info = this->getWebDataServices()->getSharedInfoDataReader()->getInfoObjects();
                    this->displayTextInConsole(u"Shared info:\n" % info.toJsonString());
                },
                Qt::QueuedConnection);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
        }

        a = menu.addAction("Metadata (slow)");
        c = connect(
            a, &QAction::triggered, this, [=]() {
                if (!sGui || sGui->isShuttingDown()) { return; }
                this->displayTextInConsole(getAllUserMetatypesTypes());
            },
            Qt::QueuedConnection);
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
        Q_UNUSED(c)
    }

    void CGuiApplication::addMenuWindow(QMenu &menu)
    {
        QPointer<QWidget> w = CGuiApplication::mainApplicationWidget();
        if (!w) { return; }
        const QSize iconSize = CIcons::empty16().size();
        static QPixmap iconEmpty;

        QPixmap icon = w->style()->standardIcon(QStyle::SP_TitleBarMaxButton).pixmap(iconSize);
        QAction *a = menu.addAction(icon.isNull() ? iconEmpty : icon.scaled(iconSize), "Fullscreen");
        bool c = connect(a, &QAction::triggered, this, [=]() {
            if (!w) { return; }
            w->showFullScreen();
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");

        icon = w->style()->standardIcon(QStyle::SP_TitleBarMinButton).pixmap(iconSize);
        a = menu.addAction(icon.isNull() ? iconEmpty : icon.scaled(iconSize), "Minimize");
        c = connect(a, &QAction::triggered, this, [=]() {
            if (!w) { return; }
            w->showMinimized();
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");

        icon = w->style()->standardIcon(QStyle::SP_TitleBarNormalButton).pixmap(iconSize);
        a = menu.addAction(icon.isNull() ? iconEmpty : icon.scaled(iconSize), "Normal");
        c = connect(a, &QAction::triggered, this, [=]() {
            if (!w) { return; }
            w->showNormal();
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");

        a = menu.addAction("Toggle stay on top");
        c = connect(a, &QAction::triggered, this, [=]() {
            if (!w) { return; }
            this->toggleStayOnTop();
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");

        a = menu.addAction("Toggle to front or back");
        c = connect(a, &QAction::triggered, this, &CGuiApplication::windowToFrontBackToggle);
        Q_ASSERT_X(c, Q_FUNC_INFO, "connect failed");

        a = menu.addAction("Window to front");
        c = connect(a, &QAction::triggered, this, &CGuiApplication::windowToFront);
        Q_ASSERT_X(c, Q_FUNC_INFO, "connect failed");

        a = menu.addAction("Window to back");
        c = connect(a, &QAction::triggered, this, &CGuiApplication::windowToBack);
        Q_ASSERT_X(c, Q_FUNC_INFO, "connect failed");

        a = menu.addAction("Toggle normal or minimized");
        c = connect(a, &QAction::triggered, this, [=]() {
            if (!w) { return; }
            this->windowMinimizeNormalToggle();
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
        Q_UNUSED(c)
    }

    void CGuiApplication::openUrl(const CUrl &url)
    {
        if (url.isEmpty() || this->isShuttingDown()) { return; }
        QDesktopServices::openUrl(url);
    }

    void CGuiApplication::addMenuHelp(QMenu &menu)
    {
        QPointer<QWidget> w = mainApplicationWidget();
        if (!w) { return; }
        QAction *a = menu.addAction(w->style()->standardIcon(QStyle::SP_TitleBarContextHelpButton), "Online help");

        bool c = connect(a, &QAction::triggered, this, [=]() {
            if (!sGui || sGui->isShuttingDown()) { return; }
            this->showHelp();
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");

        a = menu.addAction(QApplication::windowIcon(), "About swift");
        c = connect(a, &QAction::triggered, this, [=]() {
            if (!w) { return; }
            CAboutDialog dialog(w);
            dialog.exec();
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
        Q_UNUSED(c)

        // https://joekuan.wordpress.com/2015/09/23/list-of-qt-icons/
        a = menu.addAction(QApplication::style()->standardIcon(QStyle::SP_TitleBarMenuButton), "About Qt");
        c = connect(a, &QAction::triggered, this, []() {
            QApplication::aboutQt();
        });
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
        Q_UNUSED(c)
    }

    void CGuiApplication::showHelp(const QString &context) const
    {
        if (this->isShuttingDown()) { return; }
        const CGlobalSetup gs = this->getGlobalSetup();
        const CUrl helpPage = gs.getHelpPageUrl(context);
        if (helpPage.isEmpty())
        {
            CLogMessage(this).warning(u"No help page");
            return;
        }
        QDesktopServices::openUrl(helpPage);
    }

    void CGuiApplication::showHelp(const QObject *qObject) const
    {
        if (this->isShuttingDown()) { return; }
        if (!qObject || qObject->objectName().isEmpty()) { this->showHelp(); }
        else { this->showHelp(qObject->objectName()); }
    }

    bool CGuiApplication::triggerShowHelp(const QWidget *widget, QEvent *event)
    {
        if (!widget) { return false; }
        if (!event) { return false; }
        const QEvent::Type t = event->type();
        if (t != QEvent::EnterWhatsThisMode) { return false; }
        QWhatsThis::leaveWhatsThisMode();
        event->accept();
        if (!widget->isVisible()) { return true; } // ignore invisble ones
        const QPointer<const QWidget> wp(widget);
        QTimer::singleShot(0, sGui, [=] {
            if (!wp || !sGui || sGui->isShuttingDown()) { return; }
            sGui->showHelp(widget);
        });
        return true;
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

    bool CGuiApplication::openStandardLogDirectory()
    {
        const QString path(QDir::toNativeSeparators(CSwiftDirectories::logDirectory()));
        if (!QDir(path).exists()) { return false; }
        return QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    }

    bool CGuiApplication::openStandardCrashDumpDirectory()
    {
        const QString path(QDir::toNativeSeparators(CSwiftDirectories::crashpadDatabaseDirectory()));
        if (!QDir(path).exists()) { return false; }
        return QDesktopServices::openUrl(QUrl::fromLocalFile(path));
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
                CSetupLoadingDialog dialog(msgs, this->mainApplicationWidget());
                if (sGui)
                {
                    static const QString style = sGui->getStyleSheetUtility().styles(
                        { CStyleSheetUtility::fileNameFonts(),
                          CStyleSheetUtility::fileNameStandardWidget() });
                    dialog.setStyleSheet(style);
                }

                const int r = dialog.exec();
                if (r == QDialog::Rejected)
                {
                    break; // exit with false state, as file was not loaded
                }
                else
                {
                    // run loop again and sync again
                }
            }
            else
            {
                // setup loaded
                ok = true;
                break;
            }
        }
        while (!ok);
        return ok;
    }

    bool CGuiApplication::parseAndSynchronizeSetup(int timeoutMs)
    {
        if (!this->parseAndStartupCheck()) { return false; }
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

        if (onlyIfNew && !m_updateDialog->isNewVersionAvailable()) { return; }
        const int result = m_updateDialog->exec();
        if (result != QDialog::Accepted) { return; }
    }

    QString CGuiApplication::getFontInfo() const
    {
        const QWidget *w = this->mainApplicationWidget();
        if (!w) { return QStringLiteral("Font info not available"); }
        return QStringLiteral("Family: '%1', average width: %2").arg(w->font().family()).arg(w->fontMetrics().averageCharWidth());
    }

    bool CGuiApplication::toggleStayOnTop()
    {
        QMainWindow *w = CGuiApplication::mainApplicationWindow();
        if (!w) { return false; }
        const bool onTop = CGuiUtility::toggleStayOnTop(w);
        CLogMessage(w).info(onTop ? QStringLiteral("Window on top") : QStringLiteral("Window not always on top"));
        emit this->alwaysOnTop(onTop);
        m_frontBack = onTop;
        return onTop;
    }

    void CGuiApplication::windowToFront()
    {
        if (this->isShuttingDown()) { return; }
        QMainWindow *w = sGui->mainApplicationWindow();
        if (!w) { return; }

        m_frontBack = true;
        w->showNormal(); // bring window to top on OSX
        w->raise(); // bring window from minimized state on OSX

        // if (!CGuiUtility::staysOnTop(w)) { CGuiUtility::stayOnTop(true, w); emit this->alwaysOnTop(true); }
        w->activateWindow(); // bring window to front/unminimize on windows
    }

    void CGuiApplication::windowToBack()
    {
        if (this->isShuttingDown()) { return; }
        QMainWindow *w = this->mainApplicationWindow();
        if (!w) { return; }

        m_frontBack = false;
        if (CGuiUtility::staysOnTop(w))
        {
            CGuiUtility::stayOnTop(false, w);
            emit this->alwaysOnTop(false);
        }
        w->lower();
    }

    void CGuiApplication::windowToFrontBackToggle()
    {
        if (this->isShuttingDown()) { return; }
        QMainWindow *w = sGui->mainApplicationWindow();
        if (!w) { return; }
        if (w->isMinimized())
        {
            this->windowToFront();
            return;
        }
        if (w->isMaximized())
        {
            this->windowToBack();
            return;
        }
        if (CGuiUtility::staysOnTop(w))
        {
            this->windowToBack();
            return;
        }

        if (m_frontBack)
        {
            this->windowToBack();
        }
        else
        {
            this->windowToFront();
        }
    }

    void CGuiApplication::windowMinimizeNormalToggle()
    {
        if (this->isShuttingDown()) { return; }
        QMainWindow *w = sGui->mainApplicationWindow();
        if (!w) { return; }
        if (m_normalizeMinimize)
        {
            w->showMinimized();
        }
        else
        {
            // trick here is to minimize first and the normalize from minimized state
            w->showMinimized();
            w->showNormal();
        }
        m_normalizeMinimize = !m_normalizeMinimize;
    }

    void CGuiApplication::triggerNewVersionCheck(int delayedMs)
    {
        if (!m_updateSetting.get()) { return; }
        QTimer::singleShot(delayedMs, this, [=] {
            if (!sGui || sGui->isShuttingDown()) { return; }
            if (m_updateDialog) { return; } // already checked elsewhere
            this->checkNewVersion(true);
        });
    }

    void CGuiApplication::gracefulShutdown()
    {
        if (m_shutdown) { return; }
        if (m_shutdownInProgress) { return; }

        CLogMessage(this).info(u"Graceful shutdown of GUI application started");
        if (m_saveMainWidgetState)
        {
            CLogMessage(this).info(u"Graceful shutdown, saving geometry");
            this->saveWindowGeometryAndState();
        }

        // shut down whole infrastructure
        CApplication::gracefulShutdown();

        // precautions to avoid hanging closing swift
        const QStringList modals = CGuiUtility::closeAllModalWidgetsGetTitles();
        if (modals.count() > 0)
        {
            // that is a pretty normal situation
            CLogMessage(this).info(u"Graceful shutdown, still %1 modal widget(s), closed: %2") << modals.count() << modals.join(", ");
        }

        //! \todo KB 3-2020 remove as soon as the info status bar blocks shutdown bug is fixed
        //! ref: https://discordapp.com/channels/539048679160676382/539846348275449887/693848134811517029
        const QStringList docks = CGuiUtility::deleteLaterAllDockWidgetsGetTitles(this->mainApplicationWidget(), true);
        if (docks.count() > 0)
        {
            // that should not happen
            CLogMessage(this).warning(u"Graceful shutdown, still %1 floating dock widget(s), closed: %2") << docks.count() << docks.join(", ");
        }
    }

    void CGuiApplication::settingsChanged()
    {
        // changing widget style is slow, so I try to prevent setting it when nothing changed
        const QString widgetStyle = m_guiSettings.get().getWidgetStyle();
        const QString currentWidgetStyle(this->getWidgetStyle());
        Q_ASSERT_X(CThreadUtils::thisIsMainThread(), Q_FUNC_INFO, "Wrong thread");
        if (!stringCompare(widgetStyle, currentWidgetStyle, Qt::CaseInsensitive))
        {
            const QStringList availableStyles = QStyleFactory::keys();
            if (availableStyles.contains(widgetStyle))
            {
                // changing style freezes the application, so it must not be done in flight mode
                if (this->getIContextNetwork() && this->getIContextNetwork()->isConnected())
                {
                    CLogMessage(this).validationError(u"Cannot change style while connected to network");
                }
                else
                {
                    // QStyle *style = QApplication::setStyle(widgetStyle);
                    QStyle *style = QStyleFactory::create(widgetStyle);
                    // That can crash
                    QApplication::setStyle(style); // subject of crash
                    if (style)
                    {
                        CLogMessage(this).info(u"Changed style to '%1', req.: '%2'") << style->objectName() << widgetStyle;
                    }
                    else
                    {
                        CLogMessage(this).error(u"Unable to set requested style '%1'") << widgetStyle;
                    }
                }
            } // valid style
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
        const QFont f = CGuiUtility::currentFont();
        if (f.pointSize() != m_fontPointSize || f.family() != m_fontFamily)
        {
            emit this->fontChanged();
            CLogMessage(this).info(this->getFontInfo());
        }
        emit this->styleSheetsChanged();
    }

    void CGuiApplication::setCurrentFontValues()
    {
        const QFont font = CGuiUtility::currentFont();
        m_fontFamily = font.family();
        m_fontPointSize = font.pointSize();
    }

    void CGuiApplication::onWebDataServicesStarted(bool success)
    {
        if (success)
        {
            connect(this->getWebDataServices(), &CWebDataServices::databaseReaderMessages, this, &CGuiApplication::displaySplashMessages, Qt::QueuedConnection);
        }
    }

    void CGuiApplication::superviseWindowMinSizes()
    {
        CGuiUtility::superviseMainWindowMinSizes();
    }
} // ns
