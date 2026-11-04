// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFTGUIMINI_MINISIMULATORSETTINGSDIALOG_H
#define SWIFTGUIMINI_MINISIMULATORSETTINGSDIALOG_H

#include <QDialog>

namespace swift::gui::components
{
    class CSettingsSimulatorComponent;
}

//! Einfacher Dialog, der CSettingsSimulatorComponent (wie in swiftguistandard) kapselt,
//! damit Simulator-Plugins auch in der Mini-GUI aktiviert/konfiguriert werden koennen.
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