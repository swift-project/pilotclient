// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "infobarwebreadersstatussmallcomponent.h"
#include "ui_infobarwebreadersstatussmallcomponent.h"

namespace BlackGui::Components
{
    CInfoBarWebReadersStatusSmallComponent::CInfoBarWebReadersStatusSmallComponent(QWidget *parent) : CInfoBarWebReadersStatusBase(parent),
                                                                                                      ui(new Ui::CInfoBarWebReadersStatusSmallComponent)
    {
        ui->setupUi(this);
        this->setLeds(ui->led_SwiftDb, ui->led_DataReady, nullptr, ui->led_IcaoAircraft, ui->led_IcaoAirline, ui->led_Countries, ui->led_Distributors, ui->led_Liveries, ui->led_Models);
        this->init();
    }

    CInfoBarWebReadersStatusSmallComponent::~CInfoBarWebReadersStatusSmallComponent()
    {}
} // ns
