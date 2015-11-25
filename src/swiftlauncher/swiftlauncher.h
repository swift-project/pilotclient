/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef SWIFTLAUNCHER_H
#define SWIFTLAUNCHER_H

#include <QDialog>
#include <QScopedPointer>
#include "blackcore/data/globalsetup.h"
#include "blackcore/data/updateinfo.h"
#include "blackgui/enableforframelesswindow.h"
#include "blackgui/overlaymessagesframe.h"
#include "swiftguistandard/guimodeenums.h"

namespace Ui { class CSwiftLauncher; }

/*!
 * swift launcher tool
 * \note Besides the fact the launcher makes it easy to start our applications it also pre-fetches some
 *       cache files, hence reducing load times in the subsequent applications. Therefor starting via the launcher
 *       is preferable, but not mandatory.
 */
class CSwiftLauncher :
    public QDialog,
    public BlackGui::CEnableForFramelessWindow
{
    Q_OBJECT

public:
    //! Constructor
    explicit CSwiftLauncher(QWidget *parent = nullptr);

    //! Destructor
    ~CSwiftLauncher();

    //! Executable
    const QString &getExecutable() const { return m_executable; }

    //! Arguments
    const QStringList &getExecutableArgs() const { return m_executableArgs; }

    //! Current command line
    QString getCmdLine() const;

protected:
    //! \copydoc QDialog::mousePressEvent
    virtual void mousePressEvent(QMouseEvent *event) override;

    //! \copydoc QDialog::mouseMoveEvent
    void mouseMoveEvent(QMouseEvent *event);

private:
    QScopedPointer<Ui::CSwiftLauncher>             ui;
    BlackCore::CData<BlackCore::Data::GlobalSetup> m_setup    { this, &CSwiftLauncher::ps_changedCache };   //!< setup cache
    BlackCore::CData<BlackCore::Data::UpdateInfo>  m_updateInfo { this, &CSwiftLauncher::ps_changedCache }; //!< version cache
    QString     m_executable;
    QStringList m_executableArgs;

    //! Get core mode
    GuiModes::CoreMode getCoreMode() const;

    //! select DBus address/mode
    QString getDBusAddress() const;

    //! Selected window mode
    BlackGui::CEnableForFramelessWindow::WindowMode getWindowMode() const;

    //! Init
    void init();

    //! style sheets
    void initStyleSheet();

    //! combobox for DBus
    void initDBusGui();

    //! Version string
    void initVersion();

    //! Log display
    void initLogDisplay();

    //! latest news
    void displayLatestNews();

    //! Start the core
    void startSwiftCore();

    //! Set executable for swift data
    void setSwiftDataExecutable();

    //! Set executable for swift GUI
    bool setSwiftGuiExecutable();

    //! Can DBus server be connected
    bool canConnectDBusServer(QString &msg) const;

    //! Standalone GUI selected
    bool isStandaloneGuiSelected() const;

    //! Command line
    static QString toCmdLine(const QString &exe, const QStringList &exeArgs);

private slots:
    //! Load latest version
    void ps_loadSetup();

    //! Loaded latest version
    void ps_loadedSetup(bool success);

    //! Cache values have been changed
    void ps_changedCache();

    //! Start button pressed
    void ps_startButtonPressed();

    //! Changed selection
    void ps_dbusServerAddressSelectionChanged(const QString &currentText);

    //! DBus server mode selected
    void ps_dbusServerModeSelected(bool selected);

    //! Display status message as overlay
    void ps_showStatusMessage(const BlackMisc::CStatusMessage &msg);

    //! Append status message
    void ps_appendLogMessage(const BlackMisc::CStatusMessage &message);

    //! Show set main page
    void ps_showMainPage();

    //! Show the log page
    void ps_showLogPage();
};

#endif // guard
