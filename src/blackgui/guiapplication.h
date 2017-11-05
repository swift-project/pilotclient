/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_GUIAPPLICATION_H
#define BLACKGUI_GUIAPPLICATION_H

#include "blackcore/application.h"
#include "blackgui/blackguiexport.h"
#include "blackgui/enableforframelesswindow.h"
#include "blackgui/mainwindowaccess.h"
#include "blackgui/settings/guisettings.h"
#include "blackgui/settings/updatenotification.h"
#include "blackgui/stylesheetutility.h"
#include "blackmisc/network/networkutils.h"
#include "blackmisc/icons.h"
#include "blackmisc/statusmessage.h"

#include <QCommandLineOption>
#include <QDialog>
#include <QObject>
#include <QPixmap>
#include <QScopedPointer>
#include <QString>
#include <Qt>

class QMenu;
class QSplashScreen;
class QWidget;
class QMainWindow;

namespace BlackMisc { class CLogCategoryList; }
namespace BlackGui
{
    namespace Components
    {
        class CApplicationCloseDialog;
        class CDownloadAndInstallDialog;
    }
}
namespace BlackGui
{
    /*!
     * \brief GUI application, a specialized version of BlackCore::CApplication for GUI applications.
     *
     * \details Analog to QCoreApplication and QApplication this class provides more details for swift
     * GUI applications. It is normally used via the global sGui pointer. As an example of how to extend this
     * class see CSwiftGuiStdApplication.
     *
     *  - style sheet handling
     *  - standard menus
     *  - splash screen support
     *
     *  Simple example
     *  \snippet swiftlauncher/main.cpp SwiftApplicationDemo
     *
     *  Derived class example, hence very short (logic in CSwiftGuiStdApplication)
     *  \snippet swiftguistandard/main.cpp SwiftApplicationDemo
     *
     *  Longer example
     *  \snippet swiftcore/main.cpp SwiftApplicationDemo
     */
    class BLACKGUI_EXPORT CGuiApplication :
        public BlackCore::CApplication,
        public BlackGui::IMainWindowAccess
    {
        Q_OBJECT
        Q_INTERFACES(BlackGui::IMainWindowAccess)

    public:
        //! Similar to \sa QCoreApplication::instance() returns the single instance
        static CGuiApplication *instance();

        //! Own log categories
        static const BlackMisc::CLogCategoryList &getLogCategories();

        //! Constructor
        CGuiApplication(const QString &applicationName = executable(), BlackMisc::CApplicationInfo::Application application = BlackMisc::CApplicationInfo::Unknown, const QPixmap &icon = BlackMisc::CIcons::swift64());

        //! Destructor
        virtual ~CGuiApplication();

        //! CMD line arguments
        void addWindowStateOption();

        //! CMD line arguments
        void addWindowModeOption();

        //! Window state
        Qt::WindowState getWindowState() const;

        //! Window mode (window flags)
        CEnableForFramelessWindow::WindowMode getWindowMode() const;

        //! Add a splash screen based on resource, empty means remove splash screen
        void splashScreen(const QString &resource);

        //! Add a splash screen based on resource, empty means remove splash screen
        void splashScreen(const QPixmap &pixmap);

        //! Allow the GUI to refresh by processing events, call the event loop
        void processEventsToRefreshGui() const;

        //! Init the main application window based on information in this application
        void initMainApplicationWindow(QWidget *mainWindow);

        //! Set window flag on main application window
        void addWindowFlags(Qt::WindowFlags flags);

        //! \name print messages generated during parsing / cmd handling
        //! @{
        virtual bool cmdLineErrorMessage(const QString &cmdLineErrorMessage, bool retry = false) const override;
        virtual bool cmdLineErrorMessage(const BlackMisc::CStatusMessageList &msgs, bool retry = false) const override;
        //! @}

        //! \name direct access to main application window
        //! @{
        virtual bool displayInStatusBar(const BlackMisc::CStatusMessage &message) override;
        virtual bool displayInOverlayWindow(const BlackMisc::CStatusMessage &message, int timeOutMs = -1) override;
        virtual bool displayTextInConsole(const QString &text) override;
        //! @}

        //! Add menu items for settings and cache
        void addMenuForSettingsAndCache(QMenu &menu);

        //! Add menu for style sheets
        void addMenuForStyleSheets(QMenu &menu);

        //! File menu
        void addMenuFile(QMenu &menu);

        //! Internals menu
        void addMenuInternals(QMenu &menu);

        //! Window operations
        void addMenuWindow(QMenu &menu);

        //! Help operations
        void addMenuHelp(QMenu &menu);

        //! Show help page (online help)
        void showHelp(const QString &context = {}) const;

        //! Show help page (online help), use QObject::objectName as 2nd level context
        void showHelp(const QObject *qObject) const;

        //! Style sheet handling
        const CStyleSheetUtility &getStyleSheetUtility() const;

        //! Current widget style
        QString getWidgetStyle() const;

        //! Reload style sheets
        bool reloadStyleSheets();

        //! Opens the standard stylesheet
        bool openStandardWidgetStyleSheet();

        //! Update the fonts
        bool updateFont(const QString &fontFamily, const QString &fontSize, const QString &fontStyle, const QString &fontWeight, const QString &fontColor);

        //! Update the fonts
        bool updateFont(const QString &qss);

        //! Reset the font to default
        bool resetFont();

        //! Wait for setup, in case it fails display a dialog how to continue
        bool interactivelySynchronizeSetup(int timeoutMs = BlackMisc::Network::CNetworkUtils::getLongTimeoutMs());

        //! Combined function
        //! \see parseAndStartupCheck
        //! \see interactivelySynchronizeSetup
        virtual bool parseAndSynchronizeSetup(int timeoutMs = BlackMisc::Network::CNetworkUtils::getLongTimeoutMs()) override;

        //! Show close dialog
        QDialog::DialogCode showCloseDialog(QMainWindow *mainWindow, QCloseEvent *closeEvent);

        //! Trigger new version check
        void triggerNewVersionCheck(int delayedMs);

        //! Set icon
        //! \note Pixmap requires a valid QApplication, so it cannot be passed as constructor parameter
        static void setWindowIcon(const QPixmap &icon);

        //! Main application window
        static QWidget *mainApplicationWindow();

        //! Main window access interface
        static BlackGui::IMainWindowAccess *mainWindowAccess();

        //! Exit application, perform graceful shutdown and exit
        static void exit(int retcode = 0);

        //! Support for high DPI screens
        //! \note Needs to be at the beginning of main
        static void highDpiScreenSupport();

    signals:
        //! Style sheet changed
        void styleSheetsChanged();

        //! Object tree ready (means ui->setupUi() completed)
        void uiObjectTreeReady();

    protected slots:
        //! Startup competed
        virtual void onStartUpCompleted() override;

    protected:
        //! \name print messages generated during parsing / cmd handling
        //! @{
        virtual void cmdLineHelpMessage() override;
        virtual void cmdLineVersionMessage() const override;
        //! @}

        //! Handle parsing of special GUI cmd arguments
        virtual bool parsingHookIn() override;

        //! \copydoc BlackCore::CApplication::onCoreFacadeStarted
        virtual void onCoreFacadeStarted() override;

        //! Check for a new version (update)
        void checkNewVersion(bool onlyIfNew);

        //! Register metadata
        static void registerMetadata();

    private:
        QPixmap m_windowIcon;
        QCommandLineOption m_cmdWindowStateMinimized { "empty" }; //!< window state (minimized)
        QCommandLineOption m_cmdWindowMode { "empty" };           //!< window mode (flags: frameless ...)
        CStyleSheetUtility m_styleSheetUtility {{}, this};        //!< style sheet utility
        bool m_uiSetupCompleted = false;                          //!< ui setup completed
        QScopedPointer<QSplashScreen> m_splashScreen;             //!< splash screen
        Components::CDownloadAndInstallDialog *m_installDialog = nullptr; //!< software installation dialog
        Components::CApplicationCloseDialog *m_closeDialog = nullptr;     //!< close dialog (no QScopedPointer because I need to set parent)
        BlackMisc::CSettingReadOnly<Settings::TGeneralGui> m_guiSettings { this, &CGuiApplication::settingsChanged };
        BlackMisc::CSettingReadOnly<Settings::TUpdateNotificationSettings> m_updateSetting { this }; //!< update notification settings

        //! Qt help message to formatted HTML
        static QString beautifyHelpMessage(const QString &helpText);

        //! Reload widget style from settings
        void settingsChanged();

        //! Check new from menu
        void checkNewVersionMenu();

        //! Fix the palette for better readibility
        void adjustPalette();
    };
} // ns

//! Single instance of GUI application object
extern BLACKGUI_EXPORT BlackGui::CGuiApplication *sGui;

#endif // guard
