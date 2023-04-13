/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "cgsourceselector.h"
#include "ui_cgsourceselector.h"

using namespace BlackMisc::Simulation::Settings;

namespace BlackGui::Components
{
    CCGSourceSelector::CCGSourceSelector(QWidget *parent) : QFrame(parent),
                                                            ui(new Ui::CCGSourceSelector)
    {
        ui->setupUi(this);
        this->initComboBox();
    }

    CCGSourceSelector::~CCGSourceSelector()
    {}

    CSimulatorSettings::CGSource CCGSourceSelector::getValue() const
    {
        const int d = ui->cb_CGSourceSelector->currentData().toInt();
        return static_cast<CSimulatorSettings::CGSource>(d);
    }

    void CCGSourceSelector::setValue(const CSimulatorSettings &settings)
    {
        this->setValue(settings.getCGSource());
    }

    void CCGSourceSelector::setValue(CSimulatorSettings::CGSource source)
    {
        ui->cb_CGSourceSelector->setCurrentText(CSimulatorSettings::cgSourceAsString(source));
    }

    void CCGSourceSelector::initComboBox()
    {
        ui->cb_CGSourceSelector->clear();
        ui->cb_CGSourceSelector->addItem(CSimulatorSettings::cgSourceAsString(CSimulatorSettings::CGFromSimulatorFirst), CSimulatorSettings::CGFromSimulatorFirst);
        ui->cb_CGSourceSelector->addItem(CSimulatorSettings::cgSourceAsString(CSimulatorSettings::CGFromDBFirst), CSimulatorSettings::CGFromDBFirst);
        ui->cb_CGSourceSelector->addItem(CSimulatorSettings::cgSourceAsString(CSimulatorSettings::CGFromSimulatorOnly), CSimulatorSettings::CGFromSimulatorOnly);
        ui->cb_CGSourceSelector->addItem(CSimulatorSettings::cgSourceAsString(CSimulatorSettings::CGFromDBOnly), CSimulatorSettings::CGFromDBOnly);
        this->setValue(CSimulatorSettings::CGFromSimulatorFirst);
    }
} // ns
