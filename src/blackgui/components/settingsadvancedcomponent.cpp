/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "ui_settingsadvancedcomponent.h"
#include "blackgui/components/settingsadvancedcomponent.h"
#include "blackmisc/crashsettings.h"

using namespace BlackMisc;
using namespace BlackMisc::Settings;

namespace BlackGui::Components
{
    CSettingsAdvancedComponent::CSettingsAdvancedComponent(QWidget *parent) :
        QFrame(parent),
        ui(new Ui::CSettingsAdvancedComponent)
    {
        ui->setupUi(this);

        const CCrashSettings settings = m_crashDumpSettings.getThreadLocal();
        ui->cb_crashDumpsUpload->setChecked(settings.isEnabled());
        connect(ui->cb_crashDumpsUpload, &QCheckBox::stateChanged, this, &CSettingsAdvancedComponent::crashDumpUploadEnabledChanged);
    }

    CSettingsAdvancedComponent::~CSettingsAdvancedComponent()
    { }

    void CSettingsAdvancedComponent::crashDumpUploadEnabledChanged(int state)
    {
        auto text = ui->cb_crashDumpsUpload->text();
        if (!text.endsWith("(restart needed)"))
        {
            ui->cb_crashDumpsUpload->setText(ui->cb_crashDumpsUpload->text() + " (restart needed)");
        }

        CCrashSettings settings = m_crashDumpSettings.getThreadLocal();
        settings.setEnabled(state == Qt::Checked);
        m_crashDumpSettings.set(settings);
    }

} // ns
