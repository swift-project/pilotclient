/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "networkaircraftvaluescomponent.h"
#include "ui_networkaircraftvaluescomponent.h"

#include "blackgui/guiapplication.h"
#include "blackcore/context/contextownaircraft.h"
#include "blackcore/context/contextnetwork.h"

using namespace BlackCore::Context;

namespace BlackGui::Components
{
    CNetworkAircraftValuesComponent::CNetworkAircraftValuesComponent(QWidget *parent) :
        QFrame(parent),
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
