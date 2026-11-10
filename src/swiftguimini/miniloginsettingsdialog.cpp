// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "miniloginsettingsdialog.h"

#include <QVBoxLayout>

#include "miniloginwidget.h"

#include "gui/components/settingsnetworkserverscomponent.h"

using namespace swift::gui::components;

CMiniLoginSettingsDialog::CMiniLoginSettingsDialog(CMiniLoginWidget *loginWidget, QWidget *parent) : QDialog(parent)
{
    this->setWindowTitle(tr("Login Configuration"));
    this->resize(500, 700);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Take over connection parameters (Network/Pilot) from the Connect tab
    if (loginWidget) { loginWidget->moveNetworkAndPilotGroupsTo(mainLayout); }

    // Server management (saving new connections), analogous to swiftguistd: Settings -> Servers
    CSettingsNetworkServersComponent *serversComponent = new CSettingsNetworkServersComponent(this);
    mainLayout->addWidget(serversComponent, 1);

    this->setLayout(mainLayout);
}

CMiniLoginSettingsDialog::~CMiniLoginSettingsDialog() {}