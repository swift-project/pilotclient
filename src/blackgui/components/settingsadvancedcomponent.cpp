// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "ui_settingsadvancedcomponent.h"
#include "blackgui/components/settingsadvancedcomponent.h"

using namespace BlackMisc;

namespace BlackGui::Components
{
    CSettingsAdvancedComponent::CSettingsAdvancedComponent(QWidget *parent) : QFrame(parent),
                                                                              ui(new Ui::CSettingsAdvancedComponent)
    {
        ui->setupUi(this);

        const bool crashDumpUploadEnabled = m_crashDumpUploadEnabled.getThreadLocal();
        ui->cb_crashDumpsUpload->setChecked(crashDumpUploadEnabled);
        connect(ui->cb_crashDumpsUpload, &QCheckBox::stateChanged, this, &CSettingsAdvancedComponent::crashDumpUploadEnabledChanged);
    }

    CSettingsAdvancedComponent::~CSettingsAdvancedComponent()
    {}

    void CSettingsAdvancedComponent::crashDumpUploadEnabledChanged(int state)
    {
        auto text = ui->cb_crashDumpsUpload->text();
        if (!text.endsWith("(restart needed)"))
        {
            ui->cb_crashDumpsUpload->setText(ui->cb_crashDumpsUpload->text() + " (restart needed)");
        }

        m_crashDumpUploadEnabled.set(state == Qt::Checked);
    }

} // ns
