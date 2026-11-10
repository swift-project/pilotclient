// SPDX-FileCopyrightText: Copyright (C) 2026 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFTGUIMINI_MINILOGINSETTINGSDIALOG_H
#define SWIFTGUIMINI_MINILOGINSETTINGSDIALOG_H

#include <QDialog>

class CMiniLoginWidget;

//! "Login configuration" dialog of the Mini-GUI (accessible via Settings -> Login configuration).
//!
//! This dialog takes over (via reparenting) the "Network" and "Pilot's info" groups from the
//! existing CMiniLoginWidget (CLoginComponent) shown on the Connect tab, so that the Connect tab
//! itself only shows "Own Aircraft" (and, while connected, the server info). Additionally (analogous
//! to swiftguistandard, Settings -> Servers) a CSettingsNetworkServersComponent is embedded, allowing
//! new server connections to be created, edited and saved.
//!
//! \remarks CLoginComponent and CSettingsNetworkServersComponent themselves remain unchanged
//! (also used by swiftguistandard); see CONTRIBUTING.md.
class CMiniLoginSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    //! Constructor
    //! \param loginWidget The CMiniLoginWidget instance shown on the Connect tab, whose
    //!                    Network/Pilot groups are embedded here.
    //! \param parent      Parent widget
    explicit CMiniLoginSettingsDialog(CMiniLoginWidget *loginWidget, QWidget *parent = nullptr);

    //! Destructor
    ~CMiniLoginSettingsDialog() override;
};

#endif // SWIFTGUIMINI_MINILOGINSETTINGSDIALOG_H