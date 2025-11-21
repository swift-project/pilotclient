// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFTCORE_H
#define SWIFTCORE_H

#include <QObject>
#include <QScopedPointer>
#include <QString>

#include "core/coremodeenums.h"
#include "gui/mainwindowaccess.h"
#include "gui/systemtraywindow.h"
#include "misc/identifiable.h"
#include "misc/statusmessage.h"

class QWidget;
namespace swift::gui::components
{
    class CCoreSettingsDialog;
    class CRawFsdMessagesDialog;
    class CCockpitComAudioDialog;
    class CAudioAdvancedDistributedDialog;
} // namespace swift::gui::components
namespace Ui
{
    class CSwiftCore;
}

/*!
 * swift core control GUI
 */
class CSwiftCore :
    public swift::gui::CSystemTrayWindow,
    public swift::gui::IMainWindowAccess,
    public swift::misc::CIdentifiable
{
    Q_OBJECT
    Q_INTERFACES(swift::gui::IMainWindowAccess)

public:
    //! Constructor
    CSwiftCore(QWidget *parent = nullptr);

    //! Destructor
    ~CSwiftCore() override;

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

    QScopedPointer<swift::gui::components::CCoreSettingsDialog> m_settingsDialog;
    QScopedPointer<swift::gui::components::CRawFsdMessagesDialog> m_rawFsdMessageDialog;
    QScopedPointer<swift::gui::components::CCockpitComAudioDialog> m_audioDialog;
    QScopedPointer<swift::gui::components::CAudioAdvancedDistributedDialog> m_audioAdvDialog;

    QScopedPointer<Ui::CSwiftCore> ui;
};

#endif // SWIFTCORE_H
