// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFTGUIMINI_MINISIMULATORSETTINGSDIALOG_H
#define SWIFTGUIMINI_MINISIMULATORSETTINGSDIALOG_H

#include <QDialog>

namespace swift::gui::components
{
    class CSettingsSimulatorComponent;
}

//! Simple dialog that wraps CSettingsSimulatorComponent (as in swiftguistandard), so simulator
//! plugins can also be enabled/configured in the Mini-GUI.
class CMiniSimulatorSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    //! Constructor
    explicit CMiniSimulatorSettingsDialog(QWidget *parent = nullptr);

    //! Destructor
    ~CMiniSimulatorSettingsDialog() override;

private:
    swift::gui::components::CSettingsSimulatorComponent *m_simSettingsComponent = nullptr;
};

#endif // SWIFTGUIMINI_MINISIMULATORSETTINGSDIALOG_H