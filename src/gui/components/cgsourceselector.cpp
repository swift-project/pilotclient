// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "cgsourceselector.h"

#include "ui_cgsourceselector.h"

using namespace swift::misc::simulation::settings;

namespace swift::gui::components
{
    CCGSourceSelector::CCGSourceSelector(QWidget *parent) : QFrame(parent), ui(new Ui::CCGSourceSelector)
    {
        ui->setupUi(this);
        this->initComboBox();
    }

    CCGSourceSelector::~CCGSourceSelector() = default;

    CSimulatorSettings::CGSource CCGSourceSelector::getValue() const
    {
        const int d = ui->cb_CGSourceSelector->currentData().toInt();
        return static_cast<CSimulatorSettings::CGSource>(d);
    }

    void CCGSourceSelector::setValue(const CSimulatorSettings &settings) { this->setValue(settings.getCGSource()); }

    void CCGSourceSelector::setValue(CSimulatorSettings::CGSource source)
    {
        ui->cb_CGSourceSelector->setCurrentText(CSimulatorSettings::cgSourceAsString(source));
    }

    void CCGSourceSelector::initComboBox()
    {
        ui->cb_CGSourceSelector->clear();
        ui->cb_CGSourceSelector->addItem(CSimulatorSettings::cgSourceAsString(CSimulatorSettings::CGFromSimulatorFirst),
                                         CSimulatorSettings::CGFromSimulatorFirst);
        ui->cb_CGSourceSelector->addItem(CSimulatorSettings::cgSourceAsString(CSimulatorSettings::CGFromDBFirst),
                                         CSimulatorSettings::CGFromDBFirst);
        ui->cb_CGSourceSelector->addItem(CSimulatorSettings::cgSourceAsString(CSimulatorSettings::CGFromSimulatorOnly),
                                         CSimulatorSettings::CGFromSimulatorOnly);
        ui->cb_CGSourceSelector->addItem(CSimulatorSettings::cgSourceAsString(CSimulatorSettings::CGFromDBOnly),
                                         CSimulatorSettings::CGFromDBOnly);
        this->setValue(CSimulatorSettings::CGFromSimulatorFirst);
    }
} // namespace swift::gui::components
