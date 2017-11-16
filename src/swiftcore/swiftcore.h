
/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef SWIFTCORE_H
#define SWIFTCORE_H

#include "blackcore/coremodeenums.h"
#include "blackgui/mainwindowaccess.h"
#include "blackgui/systemtraywindow.h"
#include "blackmisc/identifiable.h"
#include "blackmisc/statusmessage.h"

#include <QObject>
#include <QScopedPointer>
#include <QString>

class QWidget;
namespace BlackGui { namespace Components { class CCoreSettingsDialog; }}
namespace Ui { class CSwiftCore; }

/*!
 * swift core control GUI
 */
class CSwiftCore :
    public BlackGui::CSystemTrayWindow,
    public BlackGui::IMainWindowAccess,
    public BlackMisc::CIdentifiable
{
    Q_OBJECT
    Q_INTERFACES(BlackGui::IMainWindowAccess)

public:
    //! Constructor
    CSwiftCore(QWidget *parent = nullptr);

    //! Destructor
    virtual ~CSwiftCore();

private:
    //! \name Init
    //! @{
    void initLogDisplay();
    void initStyleSheet();
    void initMenus();
    void initAudio();
    //! @}

    //! Add a message to log UI
    void appendLogMessage(const BlackMisc::CStatusMessage &message);

    //! Toggled P2P mode
    void p2pModeToggled(bool checked);

    //! Show the settings dialog
    void showSettingsDialog();

    //! Style sheet has changed
    virtual void onStyleSheetsChanged();

    //! Restart with new arguments
    void restart();

    //! Core mode from radio buttons
    QString getAudioCmdFromRadioButtons() const;

    //! Restart CMD args
    QStringList getRestartCmdArgs() const;

    QScopedPointer<BlackGui::Components::CCoreSettingsDialog> m_settingsDialog;
    QScopedPointer<Ui::CSwiftCore> ui;
};

#endif // guard
