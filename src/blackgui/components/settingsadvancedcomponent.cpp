/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/components/settingsadvancedcomponent.h"
#include "ui_settingsadvancedcomponent.h"

using namespace BlackMisc;

namespace BlackGui
{
    namespace Components
    {
        CSettingsAdvancedComponent::CSettingsAdvancedComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CSettingsAdvancedComponent)
        {
            ui->setupUi(this);
            ui->cb_crashDumpsUpload->setChecked(m_crashDumpUploadEnabled.getThreadLocal());
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
            m_crashDumpUploadEnabled.set(state == Qt::Checked);
        }

    } // ns
} // ns
