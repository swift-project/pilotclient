/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "configurationwizard.h"
#include "ui_configurationwizard.h"

namespace BlackGui
{
    namespace Components
    {
        CConfigurationWizard::CConfigurationWizard(QWidget *parent) :
            QWizard(parent),
            ui(new Ui::CConfigurationWizard)
        {
            ui->setupUi(this);
            ui->wp_CopyCaches->setConfigComponent(ui->comp_CopyCaches);
            ui->wp_CopySettings->setConfigComponent(ui->comp_CopySettings);
            ui->wp_Simulator->setConfigComponent(ui->comp_Simulator);
            ui->wp_DataLoad->setConfigComponent(ui->comp_DataLoad);

            // no other versions, skip copy pages
            if (!ui->comp_CopySettings->hasOtherVersionData())
            {
                this->setStartId(ConfigSimulator);
            }
            connect(this, &QWizard::currentIdChanged, this, &CConfigurationWizard::wizardCurrentIdChanged);
        }

        CConfigurationWizard::~CConfigurationWizard()
        { }

        void CConfigurationWizard::wizardCurrentIdChanged(int id)
        {
            const int lastId = m_lastId;
            m_lastId = id; // update
            const bool backward = id < lastId;
            Q_UNUSED(backward);

            const QWizardPage *page = this->currentPage();
            if (page == ui->wp_CopyCaches)
            {
                ui->comp_CopyCaches->setCacheMode();
            }
            else if (page == ui->wp_CopySettings)
            {
                ui->comp_CopySettings->setSettingsMode();
            }
        }
    } // ns
} // ns
