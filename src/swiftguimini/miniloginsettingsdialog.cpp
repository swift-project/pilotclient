// SPDX-FileCopyrightText: Copyright (C) 2026 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "miniloginsettingsdialog.h"

#include <QDialogButtonBox>
#include <QVBoxLayout>

#include "gui/components/settingsnetworkcomponent.h"

using namespace swift::gui::components;

CMiniLoginSettingsDialog::CMiniLoginSettingsDialog(QWidget *parent) : QDialog(parent)
{
    this->setWindowTitle("Login-Konfiguration");
    this->resize(500, 400);

    m_networkSettings = new CSettingsNetworkComponent(this);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::close);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::close);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_networkSettings);
    layout->addWidget(buttons);
    this->setLayout(layout);
}

CMiniLoginSettingsDialog::~CMiniLoginSettingsDialog() {}