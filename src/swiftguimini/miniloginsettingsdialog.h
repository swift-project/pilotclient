// SPDX-FileCopyrightText: Copyright (C) 2026 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFTGUIMINI_MINILOGINSETTINGSDIALOG_H
#define SWIFTGUIMINI_MINILOGINSETTINGSDIALOG_H

#include <QDialog>

namespace swift::gui::components
{
    class CSettingsNetworkComponent;
}

//! Einfacher Dialog fuer die Login-/Netzwerk-Konfiguration (analog swiftguistandard Settings > Network)
class CMiniLoginSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    //! Constructor
    explicit CMiniLoginSettingsDialog(QWidget *parent = nullptr);

    //! Destructor
    ~CMiniLoginSettingsDialog() override;

private:
    swift::gui::components::CSettingsNetworkComponent *m_networkSettings = nullptr;
};

#endif // SWIFTGUIMINI_MINILOGINSETTINGSDIALOG_H