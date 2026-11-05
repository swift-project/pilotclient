// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "minisimulatorsettingsdialog.h"

#include <QVBoxLayout>

#include "gui/components/settingssimulatorcomponent.h"

using namespace swift::gui::components;

CMiniSimulatorSettingsDialog::CMiniSimulatorSettingsDialog(QWidget *parent) : QDialog(parent)
{
    this->setWindowTitle(tr("Simulator-Konfiguration"));
    this->resize(700, 600);

    m_simSettingsComponent = new CSettingsSimulatorComponent(this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->addWidget(m_simSettingsComponent);
    this->setLayout(layout);
}

CMiniSimulatorSettingsDialog::~CMiniSimulatorSettingsDialog() {}