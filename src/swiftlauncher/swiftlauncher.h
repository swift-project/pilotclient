/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef SWIFTLAUNCHER_H
#define SWIFTLAUNCHER_H

#include "blackgui/overlaymessagesframe.h"
#include "blackgui/enableforframelesswindow.h"
#include "blackgui/mainwindowaccess.h"
#include "blackcore/data/globalsetup.h"
#include "blackcore/data/launchersetup.h"
#include "blackcore/coremodeenums.h"
#include "blackmisc/db/artifact.h"
#include "blackmisc/identifiable.h"
#include <QDialog>
#include <QTimer>
#include <QScopedPointer>
#include <QNetworkReply>

namespace Ui { class CSwiftLauncher; }
namespace BlackGui { namespace Components { class CConfigurationWizard; }}

/*!
 * swift launcher tool
 * \note Besides the fact the launcher makes it easy to start our applications it also pre-fetches some
 *       cache files, hence reducing load times in the subsequent applications. Therefor starting via the launcher
 *       is preferable, but not mandatory.
 * \fixme it was better if launcher was a QMainWindow, because we have to deal with main widgets QDialog and QMainWindow because of launcher
 */
class CSwiftLauncher :
    public QDialog,
    public BlackGui::CEnableForFramelessWindow,
    public BlackGui::IMainWindowAccess,
    public BlackMisc::CIdentifiable
{
    Q_OBJECT
    Q_INTERFACES(BlackGui::IMainWindowAccess)

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
    explicit CSwiftLauncher(QWidget *parent = nullptr);

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

protected:
    //! \copydoc QDialog::mousePressEvent
    virtual void mousePressEvent(QMouseEvent *event) override;

    //! \copydoc QDialog::mouseMoveEvent
    void mouseMoveEvent(QMouseEvent *event) override;

private slots:
    //! Show the log page
    void showLogPage();

private:
    QScopedPointer<Ui::CSwiftLauncher> ui;
    QScopedPointer<BlackGui::Components::CConfigurationWizard> m_wizard;
    BlackMisc::CData<BlackCore::Data::TLauncherSetup>          m_setup { this }; //!< setup, i.e. last user selection

    QString     m_executable;
    QStringList m_executableArgs;
    QTimer      m_checkTimer { this };
    int         m_startCoreWaitCycles = 0;
    int         m_startMappingToolWaitCycles = 0;
    int         m_startGuiWaitCycles = 0;

    //! Get core mode
    BlackCore::CoreModes::CoreMode getCoreMode() const;

    //! Selected window mode
    BlackGui::CEnableForFramelessWindow::WindowMode getWindowMode() const;

    //! Init
    void init();

    //! style sheets
    void initStyleSheet();

    //! Log display
    void initLogDisplay();

    //! Set header info
    void setHeaderInfo(const BlackMisc::Db::CArtifact &latestArtifact);

    //! Latest news
    //! \sa CSwiftLauncher::displayLatestNews
    void loadLatestNews();

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

    //! Display latest news
    void displayLatestNews(QNetworkReply *reply);

    //! Distribution info is available
    void updateInfoAvailable();

    //! Start button pressed
    void startButtonPressed();

    //! DBus server mode selected
    void dbusServerModeSelected(bool selected);

    //! Display status message as overlay
    void showStatusMessage(const BlackMisc::CStatusMessage &msg);

    //! Display status message as overlay
    void showStatusMessage(const QString &htmlMsg);

    //! Append status message
    void appendLogMessage(const BlackMisc::CStatusMessage &message);

    //! Append status messages
    void appendLogMessages(const BlackMisc::CStatusMessageList &messages);

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

    //! Command line
    static QString toCmdLine(const QString &exe, const QStringList &exeArgs);
};

#endif // guard
