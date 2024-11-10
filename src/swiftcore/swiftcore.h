// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFTCORE_H
#define SWIFTCORE_H

#include "core/coremodeenums.h"
#include "blackgui/mainwindowaccess.h"
#include "blackgui/systemtraywindow.h"
#include "misc/identifiable.h"
#include "misc/statusmessage.h"

#include <QObject>
#include <QScopedPointer>
#include <QString>

class QWidget;
namespace BlackGui::Components
{
    class CCoreSettingsDialog;
    class CRawFsdMessagesDialog;
    class CCockpitComAudioDialog;
    class CAudioAdvancedDistributedDialog;
}
namespace Ui
{
    class CSwiftCore;
}

/*!
 * swift core control GUI
 */
class CSwiftCore :
    public BlackGui::CSystemTrayWindow,
    public BlackGui::IMainWindowAccess,
    public swift::misc::CIdentifiable
{
    Q_OBJECT
    Q_INTERFACES(BlackGui::IMainWindowAccess)

public:
    //! Constructor
    CSwiftCore(QWidget *parent = nullptr);

    //! Destructor
    virtual ~CSwiftCore() override;

private:
    //! @{
    //! Init
    void initLogDisplay();
    void initStyleSheet();
    void initMenus();
    //! @}

    //! Toggled P2P mode
    void p2pModeToggled(bool checked);

    //! Show the settings dialog
    void showSettingsDialog();

    //! Show the raw FSD messages dialog
    void showRawFsdMessageDialog();

    //! Style sheet has changed
    virtual void onStyleSheetsChanged();

    //! Restart with new arguments
    void restart();

    //! Disconnect from network
    void disconnectFromNetwork();

    //! Audio dialog
    void audioDialog();

    //! Audio adv. dialog
    void audioAdvancedDialog();

    //! Restart CMD args
    QStringList getRestartCmdArgs() const;

    QScopedPointer<BlackGui::Components::CCoreSettingsDialog> m_settingsDialog;
    QScopedPointer<BlackGui::Components::CRawFsdMessagesDialog> m_rawFsdMessageDialog;
    QScopedPointer<BlackGui::Components::CCockpitComAudioDialog> m_audioDialog;
    QScopedPointer<BlackGui::Components::CAudioAdvancedDistributedDialog> m_audioAdvDialog;

    QScopedPointer<Ui::CSwiftCore> ui;
};

#endif // guard
