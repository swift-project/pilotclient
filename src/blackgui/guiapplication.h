// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_GUIAPPLICATION_H
#define BLACKGUI_GUIAPPLICATION_H

#include "blackgui/settings/guisettings.h"
#include "blackgui/settings/updatenotification.h"
#include "blackgui/enableforframelesswindow.h"
#include "blackgui/mainwindowaccess.h"
#include "blackgui/splashscreen.h"
#include "blackgui/stylesheetutility.h"
#include "blackgui/blackguiexport.h"
#include "blackcore/application.h"
#include "blackmisc/network/networkutils.h"
#include "blackmisc/icons.h"
#include "blackmisc/statusmessage.h"

#include <QCommandLineOption>
#include <QDialog>
#include <QObject>
#include <QEvent>
#include <QScreen>
#include <QPixmap>
#include <QScopedPointer>
#include <QString>
#include <Qt>

class QMenu;
class QWidget;
class QMainWindow;

namespace BlackMisc
{
    class CLogCategoryList;
}
namespace BlackGui::Components
{
    class CApplicationCloseDialog;
    class CUpdateInfoDialog;
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
        public IMainWindowAccess
    {
        Q_OBJECT
        Q_INTERFACES(BlackGui::IMainWindowAccess)

    public:
        //! Similar to \sa QCoreApplication::instance() returns the single instance
        static CGuiApplication *instance();

        //! Own log categories
        static const QStringList &getLogCategories();

        //! Organization string used for settings, is the registry key under windows
        static const QString &settingsOrganization();

        //! Remove all registry entries
        static bool removeAllWindowsSwiftRegistryEntries();

        //! Constructor
        CGuiApplication(const QString &applicationName = executable(), BlackMisc::CApplicationInfo::Application application = BlackMisc::CApplicationInfo::Unknown, const QPixmap &icon = BlackMisc::CIcons::swift64());

        //! Destructor
        virtual ~CGuiApplication() override;

        //! CMD line arguments
        void addWindowStateOption();

        //! CMD line arguments
        void addWindowModeOption();

        //! CMD line arguments (reset size store)
        void addWindowResetSizeOption();

        //! CMD line arguments (scale size on DPI screens)
        void addWindowScaleSizeOption();

        //! Window state
        Qt::WindowState getWindowState() const;

        //! Window mode (window flags)
        CEnableForFramelessWindow::WindowMode getWindowMode() const;

        //! Allow the GUI to refresh by processing events, call the event loop
        void processEventsToRefreshGui() const;

        //! Init the main application window based on information in this application
        //! \remark can be QDialog, QMainWindow
        void initMainApplicationWidget(QWidget *mainWidget);

        //! Set window flag on main application window
        void addWindowFlags(Qt::WindowFlags flags);

        //! Set window title
        QString setExtraWindowTitle(const QString &extraInfo, QWidget *mainWindowWidget = mainApplicationWidget()) const;

        //! print warning message
        virtual bool cmdLineWarningMessage(const QString &text, const QString &informativeText) const override;

        //! @{
        //! print messages generated during parsing / cmd handling
        virtual bool cmdLineErrorMessage(const QString &text, const QString &informativeText = "", bool retry = false) const override;
        virtual bool cmdLineErrorMessage(const BlackMisc::CStatusMessageList &msgs, bool retry = false) const override;
        //! @}

        //! Window size reset mode set
        bool isCmdWindowSizeResetSet() const;

        //! @{
        //! direct access to main application window
        virtual bool displayInStatusBar(const BlackMisc::CStatusMessage &message) override;
        virtual bool displayTextInConsole(const QString &text) override;
        virtual bool displayInOverlayWindow(const BlackMisc::CStatusMessage &message, int timeOutMs = -1) override;
        virtual bool displayInOverlayWindow(const BlackMisc::CStatusMessageList &messages, int timeOutMs = -1) override;
        virtual bool displayInOverlayWindow(const QString &html, int timeOutMs = -1) override;
        //! @}

        // -------- Splash screen related ---------
        //! Add a splash screen based on resource, empty means remove splash screen
        void splashScreen(const QPixmap &pixmap);

        // -------- Splash screen related ---------

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

        //! Open a given URL
        void openUrl(const BlackMisc::Network::CUrl &url);

        //! Help operations
        void addMenuHelp(QMenu &menu);

        //! Show help page (online help)
        void showHelp(const QString &context = {}) const;

        //! Show help page (online help), use QObject::objectName as 2nd level context
        void showHelp(const QObject *qObject) const;

        //! Static version used with dialogs
        static bool triggerShowHelp(const QWidget *widget, QEvent *event);

        //! Style sheet handling
        const CStyleSheetUtility &getStyleSheetUtility() const;

        //! Current widget style
        QString getWidgetStyle() const;

        //! Reload style sheets
        bool reloadStyleSheets();

        //! Opens the standard stylesheet
        bool openStandardWidgetStyleSheet();

        //! Opens the standard log directory
        bool openStandardLogDirectory();

        //! Opens the standard dumps directory
        bool openStandardCrashDumpDirectory();

        //! Update the fonts
        bool updateFont(const QString &fontFamily, const QString &fontSize, const QString &fontStyle, const QString &fontWeight, const QString &fontColor);

        //! Update the fonts
        bool updateFont(const QString &qss);

        //! Reset the font to default
        bool resetFont();

        //! Set minimum width/height in characters
        //! \deprecated kept for experimental tests
        void setMinimumSizeInCharacters(int widthChars, int heightChars);

        //! \copydoc BlackCore::CApplication::displaySetupLoadFailure
        void displaySetupLoadFailure(BlackMisc::CStatusMessageList msgs) override;

        //! Show close dialog
        //! \remark will modify CApplication::saveSettingsOnShutdown
        QDialog::DialogCode showCloseDialog(QMainWindow *mainWindow, QCloseEvent *closeEvent);

        //! Trigger new version check
        void triggerNewVersionCheck(int delayedMs);

        //! \copydoc BlackCore::CApplication::gracefulShutdown
        virtual void gracefulShutdown() override;

        //! Toggle stay on top
        bool toggleStayOnTop();

        //! @{
        //! Window to front/back
        void windowToFront();
        void windowToBack();
        void windowToFrontBackToggle();
        //! @}

        //! Window minimize/normalize
        void windowMinimizeNormalToggle();

        //! Save the main widget state?
        void setSaveMainWidgetState(bool save) { m_saveMainWidgetState = save; }

        //! Save widget's geometry and state
        bool saveWindowGeometryAndState(const QMainWindow *window = CGuiApplication::mainApplicationWindow()) const;

        //! Reset the saved values
        void resetWindowGeometryAndState();

        //! Restore widget's geometry and state
        bool restoreWindowGeometryAndState(QMainWindow *window = CGuiApplication::mainApplicationWindow());

        //! Set icon
        //! \note Pixmap requires a valid QApplication, so it cannot be passed as constructor parameter
        static void setWindowIcon(const QPixmap &icon);

        //! \copydoc BlackGui::CGuiUtility::mainApplicationWidget
        static QWidget *mainApplicationWidget();

        //! \copydoc BlackGui::CGuiUtility::registerMainApplicationWidget
        void registerMainApplicationWidget(QWidget *mainWidget);

        //! \copydoc BlackCore::CApplication::hasMinimumMappingVersion
        virtual bool hasMinimumMappingVersion() const override;

        //! Main application window
        static QMainWindow *mainApplicationWindow();

        //! Main window access interface
        static BlackGui::IMainWindowAccess *mainWindowAccess();

        //! Exit application, perform graceful shutdown and exit
        static void exit(int retcode = 0);

        //! Support for high DPI screens
        //! \note Needs to be at the beginning of main
        static void highDpiScreenSupport(const QString &scaleFactor = {});

        //! Uses the high DPI support?
        static bool isUsingHighDpiScreenSupport();

        //! Current screen
        static QScreen *currentScreen();

        //! Current screen resolution
        //! \remark logical resolution, not physical resolution
        static QRect currentScreenGeometry();

        //! Bring any modal dialog to front
        static void modalWindowToFront();

        //! Parse scale factor if any
        //! \deprecated using scaleFactor now
        static double parseScaleFactor(int argc, char *argv[]);

        //! Get the scale factor
        static QString scaleFactor(int argc, char *argv[]);

        //! Get a default scale factor
        static QString defaultScaleFactorString();

    signals:
        //! Style sheet changed
        void styleSheetsChanged();

        //! Object tree ready (means ui->setupUi() completed)
        void uiObjectTreeReady();

        //! Font has been changed
        void fontChanged();

        //! always on top
        void alwaysOnTop(bool onTop);

    protected:
        //! Handle parsing of special GUI cmd arguments
        virtual bool parsingHookIn() override;

        //! \copydoc BlackCore::CApplication::onCoreFacadeStarted
        virtual void onCoreFacadeStarted() override;

        //! \copydoc BlackCore::CApplication::onStartUpCompleted
        virtual void onStartUpCompleted() override;

        //! Check for a new version (update)
        void checkNewVersion(bool onlyIfNew);

        //! Info about font
        QString getFontInfo() const;

        //! Register metadata
        static void registerMetadata();

    private:
        QPixmap m_windowIcon; //!< the window icon
        QString m_fontFamily; //!< current font family
        int m_fontPointSize; //!< current font size
        int m_minWidthChars = -1; //!< min. width characters (based on current font metrics)
        int m_minHeightChars = -1; //!< min. height characters (based on current font metrics)
        QCommandLineOption m_cmdWindowStateMinimized { "emptyMinimized" }; //!< window state (minimized)
        QCommandLineOption m_cmdWindowMode { "emptyWindowMode" }; //!< window mode (flags: frameless ...)
        QCommandLineOption m_cmdWindowSizeReset { "emptySizeReset" }; //!< window size reset
        QCommandLineOption m_cmdWindowScaleSize { "emptyScale" }; //!< window scale size
        CStyleSheetUtility m_styleSheetUtility { {}, this }; //!< style sheet utility
        bool m_uiSetupCompleted = false; //!< ui setup completed
        bool m_saveMainWidgetState = true; //!< save/restore main widget's state
        bool m_frontBack = true; //!< for front/back toggle
        bool m_normalizeMinimize = true; //!< for normalize/minimize toggle
        QScopedPointer<CSplashScreen> m_splashScreen; //!< splash screen
        Components::CUpdateInfoDialog *m_updateDialog = nullptr; //!< software installation dialog
        Components::CApplicationCloseDialog *m_closeDialog = nullptr; //!< close dialog (no QScopedPointer because I need to set parent)
        BlackMisc::CSettingReadOnly<Settings::TGeneralGui> m_guiSettings { this, &CGuiApplication::settingsChanged };
        BlackMisc::CSettingReadOnly<Settings::TUpdateNotificationSettings> m_updateSetting { this }; //!< update notification settings

        //! Reload widget style from settings
        void settingsChanged();

        //! Check new from menu
        void checkNewVersionMenu();

        //! Fix the palette for better readibility
        void adjustPalette();

        //! Style sheets have been changed
        void onStyleSheetsChanged();

        //! Set current font values
        void setCurrentFontValues();

        //! \copydoc BlackGui::CGuiUtility::superviseMainWindowMinSizes
        void superviseWindowMinSizes();

        //! Settings ini filename
        static const QString &fileForWindowGeometryAndStateSettings();

        //! Hash of names of toolbars and dockwidgets
        static int hashForStateSettingsSchema(const QMainWindow *window);
    };
} // ns

//! Single instance of GUI application object
extern BLACKGUI_EXPORT BlackGui::CGuiApplication *sGui;

#endif // guard
