// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFTLAUNCHER_H
#define SWIFTLAUNCHER_H

#include "core/coremodeenums.h"
#include "core/data/globalsetup.h"
#include "core/data/launchersetup.h"
#include "gui/enableforframelesswindow.h"
#include "gui/mainwindowaccess.h"
#include "gui/overlaymessagesframe.h"
#include "misc/db/artifact.h"
#include "misc/identifiable.h"
#include "misc/loghistory.h"
#include "misc/simulation/data/modelcaches.h"

#ifdef Q_OS_MAC
#    include "misc/macos/microphoneaccess.h"
#endif

#include <QMainWindow>
#include <QNetworkReply>
#include <QScopedPointer>
#include <QTimer>

namespace Ui
{
    class CSwiftLauncher;
}
namespace swift::gui::components
{
    class CConfigurationWizard;
    class CTextEditDialog;
} // namespace swift::gui::components

/*!
 * swift launcher tool
 * \note Besides the fact the launcher makes it easy to start our applications it also pre-fetches some
 *       cache files, hence reducing load times in the subsequent applications. Therefor starting via the launcher
 *       is preferable, but not mandatory.
 */
class CSwiftLauncher :
    public QMainWindow,
    public swift::gui::CEnableForFramelessWindow,
    public swift::gui::IMainWindowAccess,
    public swift::misc::simulation::data::CCentralMultiSimulatorModelSetCachesAware,
    public swift::misc::CIdentifiable
{
    Q_OBJECT
    Q_INTERFACES(swift::gui::IMainWindowAccess)

public:
    //! Pages
    enum Pages
    {
        PageNews = 0,
        PageWindowType,
        PageCoreMode,
        PageUpdates
    };

    //! Constructor
    CSwiftLauncher(bool installerMode, QWidget *parent = nullptr);

    //! Destructor
    virtual ~CSwiftLauncher() override;

    //! Executable (to be started)
    const QString &getExecutable() const { return m_executable; }

    //! Arguments
    const QStringList &getExecutableArgs() const { return m_executableArgs; }

    //! Current command line
    QString getCmdLine() const;

    //! Start currently set application detached
    //! \remark simplifies debugging
    bool startDetached();

    //! Check if an app is set that should be started detached
    bool shouldStartAppDetached() const;

protected:
    //! Mouse events for frameless window
    //! @{
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    //! @}

private slots:
    //! Show the log page
    void showLogPage();

private:
    QScopedPointer<Ui::CSwiftLauncher> ui;
    QScopedPointer<swift::gui::components::CConfigurationWizard> m_wizard;
    QScopedPointer<swift::gui::components::CTextEditDialog> m_textEditDialog;
    swift::misc::CData<swift::core::data::TLauncherSetup> m_setup { this }; //!< setup, i.e. last user selection
    swift::misc::CLogHistoryReplica m_logHistory { this }; //!< for the overlay
#ifdef Q_OS_MAC
    swift::misc::CMacOSMicrophoneAccess m_micAccess;
#endif

    QString m_executable;
    QStringList m_executableArgs;
    QTimer m_checkTimer { this };
    int m_startCoreWaitCycles = 0;
    int m_startMappingToolWaitCycles = 0;
    int m_startGuiWaitCycles = 0;

    //! Get core mode
    swift::core::CoreModes::CoreMode getCoreMode() const;

    //! Selected window mode
    swift::gui::CEnableForFramelessWindow::WindowMode getWindowMode() const;

    //! Init
    void init();

    //! style sheets
    void initStyleSheet();

    //! Log display
    void initLogDisplay();

    //! Set header info
    void setHeaderInfo(const swift::misc::db::CArtifact &latestArtifact);

    //! Executaable for core
    bool setSwiftCoreExecutable();

    //! Set executable for swift data
    bool setSwiftDataExecutable();

    //! Set executable for swift GUI
    bool setSwiftGuiExecutable();

    //! Can DBus server be connected
    bool canConnectSwiftOnDBusServer(const QString &dbusAddress, QString &msg) const;

    //! Standalone GUI selected
    bool isStandaloneGuiSelected() const;

    //! Set default
    void setDefaults();

    //! Save state
    void saveSetup();

    //! Check for other swift applications, if so show message box
    bool warnAboutOtherSwiftApplications();

    //! Distribution info is available
    void updateInfoAvailable();

    //! Start button pressed
    void startButtonPressed();

    //! DBus server mode selected
    void dbusServerModeSelected(bool selected);

    //! Display status message as overlay
    void showStatusMessage(const swift::misc::CStatusMessage &msg);

    //! Display status message as overlay
    void showStatusMessage(const QString &htmlMsg);

    //! Show set main page
    void showMainPage();

    //! Tab changed
    void tabChanged(int current);

    //! Check if applications are already running
    void checkRunningApplicationsAndCore();

    //! Start the configuration wizard
    void startWizard();

    //! Style sheet has been changed
    void onStyleSheetsChanged();

    //! DBus editing completed
    void onDBusEditingFinished();

    //! Core mode has been changed
    void onCoreModeReleased();

    //! Display a popup with the cmd line args
    void popupExecutableArgs();

    //! Show the FSX/P3D config simulator directories
    void showSimulatorConfigDirs();

    //! Request MacOS microphone request
    void requestMacMicrophoneAccess();

    //! Run in installer mode
    void installerMode();

    //! Clear Windows registry
    void clearWindowsRegistry();

    //! Command line
    static QString toCmdLine(const QString &exe, const QStringList &exeArgs);
};

#endif // SWIFTLAUNCHER_H
