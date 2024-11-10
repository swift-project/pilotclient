// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "networkaircraftvaluescomponent.h"
#include "ui_networkaircraftvaluescomponent.h"

#include "blackgui/guiapplication.h"
#include "core/context/contextownaircraft.h"
#include "core/context/contextnetwork.h"

using namespace swift::core::context;

namespace BlackGui::Components
{
    CNetworkAircraftValuesComponent::CNetworkAircraftValuesComponent(QWidget *parent) : QFrame(parent),
                                                                                        ui(new Ui::CNetworkAircraftValuesComponent)
    {
        ui->setupUi(this);
    }

    CNetworkAircraftValuesComponent::~CNetworkAircraftValuesComponent()
    {
        // void
    }

    void CNetworkAircraftValuesComponent::showValues()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }
        if (!sGui->getIContextOwnAircraft() || !sGui->getIContextNetwork()) { return; }

        ui->le_SimModel->setText(sGui->getIContextOwnAircraft()->getOwnAircraft().getModelString());
        const QStringList values = sGui->getIContextNetwork()->getNetworkPresetValues();
        if (values.size() >= 5)
        {
            ui->le_SentModel->setText(values.at(0));
            ui->le_SentLivery->setText(values.at(1));
            ui->le_Aircraft->setText(values.at(2));
            ui->le_Airline->setText(values.at(3));
            ui->le_Callsign->setText(values.at(4));
        }
    }
} // ns
