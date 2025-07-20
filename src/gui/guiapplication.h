// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_GUIAPPLICATION_H
#define SWIFT_GUI_GUIAPPLICATION_H

#include <QCommandLineOption>
#include <QDialog>
#include <QEvent>
#include <QObject>
#include <QPixmap>
#include <QScopedPointer>
#include <QScreen>
#include <QString>
#include <Qt>

#include "core/application.h"
#include "gui/enableforframelesswindow.h"
#include "gui/mainwindowaccess.h"
#include "gui/settings/guisettings.h"
#include "gui/settings/updatenotification.h"
#include "gui/splashscreen.h"
#include "gui/stylesheetutility.h"
#include "gui/swiftguiexport.h"
#include "misc/icons.h"
#include "misc/statusmessage.h"

class QMenu;
class QWidget;
class QMainWindow;

namespace swift::misc
{
    class CLogCategoryList;
}
namespace swift::gui::components
{
    class CApplicationCloseDialog;
    class CUpdateInfoDialog;
} // namespace swift::gui::components
namespace swift::gui
{
    /*!
     * \brief GUI application, a specialized version of swift::core::CApplication for GUI applications.
     *
     * \details Analog to QCoreApplication and QApplication this class provides more details for swift
     * GUI applications. It is normally used via the global sGui pointer. As an example of how to extend this
     * class see CSwiftGuiStdApplication.
     *
     *  - style sheet handling
     *  - standard menus
     *  - splash screen support
     *
     */
    class SWIFT_GUI_EXPORT CGuiApplication : public swift::core::CApplication, public IMainWindowAccess
    {
        Q_OBJECT
        Q_INTERFACES(swift::gui::IMainWindowAccess)

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
        explicit CGuiApplication(
            const QString &applicationName = executable(),
            swift::misc::CApplicationInfo::Application application = swift::misc::CApplicationInfo::Unknown,
            const QPixmap &icon = swift::misc::CIcons::swift64());

        //! Destructor
        ~CGuiApplication() override;

        //! CMD line arguments
        void addWindowStateOption();

        //! CMD line arguments
        void addWindowModeOption();

        //! CMD line arguments (reset size store)
        void addWindowResetSizeOption();

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
        QString setExtraWindowTitle(const QString &extraInfo,
                                    QWidget *mainWindowWidget = mainApplicationWidget()) const;

        //! @{
        //! print messages generated during parsing / cmd handling
        void cmdLineErrorMessage(const QString &text, const QString &informativeText) const override;
        void cmdLineErrorMessage(const swift::misc::CStatusMessageList &msgs) const override;
        //! @}

        //! Window size reset mode set
        bool isCmdWindowSizeResetSet() const;

        //! @{
        //! direct access to main application window
        bool displayInStatusBar(const swift::misc::CStatusMessage &message) override;
        bool displayInOverlayWindow(const swift::misc::CStatusMessage &message,
                                    std::chrono::milliseconds timeout = std::chrono::milliseconds(0)) override;
        bool displayInOverlayWindow(const swift::misc::CStatusMessageList &messages,
                                    std::chrono::milliseconds timeout = std::chrono::milliseconds(0)) override;
        bool displayInOverlayWindow(const QString &html,
                                    std::chrono::milliseconds timeout = std::chrono::milliseconds(0)) override;
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
        void openUrl(const swift::misc::network::CUrl &url);

        //! Help operations
        void addMenuHelp(QMenu &menu);

        //! Show help page (online help)
        void showHelp(const QString &subpath = {}) const;

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
        bool updateFont(const QString &fontFamily, const QString &fontSize, const QString &fontStyle,
                        const QString &fontWeight, const QString &fontColor);

        //! Update the fonts
        bool updateFont(const QString &qss);

        //! Reset the font to default
        bool resetFont();

        //! Set minimum width/height in characters
        //! \deprecated kept for experimental tests
        void setMinimumSizeInCharacters(int widthChars, int heightChars);

        //! \copydoc swift::core::CApplication::displaySetupLoadFailure
        void displaySetupLoadFailure(swift::misc::CStatusMessageList msgs) override;

        //! Show close dialog
        //! \remark will modify CApplication::saveSettingsOnShutdown
        QDialog::DialogCode showCloseDialog(QMainWindow *mainWindow, QCloseEvent *closeEvent);

        //! Trigger new version check
        void triggerNewVersionCheck(int delayedMs);

        //! \copydoc swift::core::CApplication::gracefulShutdown
        void gracefulShutdown() override;

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

        //! \copydoc swift::gui::CGuiUtility::mainApplicationWidget
        static QWidget *mainApplicationWidget();

        //! \copydoc swift::gui::CGuiUtility::registerMainApplicationWidget
        void registerMainApplicationWidget(QWidget *mainWidget);

        //! \copydoc swift::core::CApplication::hasMinimumMappingVersion
        bool hasMinimumMappingVersion() const override;

        //! Main application window
        static QMainWindow *mainApplicationWindow();

        //! Main window access interface
        static swift::gui::IMainWindowAccess *mainWindowAccess();

        //! Exit application, perform graceful shutdown and exit
        static void exit(int retcode = 0);

        //! Current screen
        static QScreen *currentScreen();

        //! Current screen resolution
        //! \remark logical resolution, not physical resolution
        static QRect currentScreenGeometry();

        //! Bring any modal dialog to front
        static void modalWindowToFront();

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
        bool parsingHookIn() override;

        //! \copydoc swift::core::CApplication::onCoreFacadeStarted
        void onCoreFacadeStarted() override;

        //! \copydoc swift::core::CApplication::onStartUpCompleted
        void onStartUpCompleted() override;

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
        CStyleSheetUtility m_styleSheetUtility { this }; //!< style sheet utility
        bool m_uiSetupCompleted = false; //!< ui setup completed
        bool m_saveMainWidgetState = true; //!< save/restore main widget's state
        bool m_frontBack = true; //!< for front/back toggle
        bool m_normalizeMinimize = true; //!< for normalize/minimize toggle
        QScopedPointer<CSplashScreen> m_splashScreen; //!< splash screen
        components::CUpdateInfoDialog *m_updateDialog = nullptr; //!< software installation dialog
        components::CApplicationCloseDialog *m_closeDialog =
            nullptr; //!< close dialog (no QScopedPointer because I need to set parent)
        swift::misc::CSettingReadOnly<settings::TGeneralGui> m_guiSettings { this, &CGuiApplication::settingsChanged };
        swift::misc::CSettingReadOnly<settings::TUpdateNotificationSettings> m_updateSetting {
            this
        }; //!< update notification settings

        //! Reload widget style from settings
        void settingsChanged();

        //! Check new from menu
        void checkNewVersionMenu();

        //! Fix the palette for better readability
        void adjustPalette();

        //! Style sheets have been changed
        void onStyleSheetsChanged();

        //! Set current font values
        void setCurrentFontValues();

        //! \copydoc swift::gui::CGuiUtility::superviseMainWindowMinSizes
        void superviseWindowMinSizes();

        //! Settings ini filename
        static const QString &fileForWindowGeometryAndStateSettings();

        //! Hash of names of toolbars and dockwidgets
        static int hashForStateSettingsSchema(const QMainWindow *window);
    };
} // namespace swift::gui

//! Single instance of GUI application object
extern SWIFT_GUI_EXPORT swift::gui::CGuiApplication *sGui;

#endif // SWIFT_GUI_GUIAPPLICATION_H
