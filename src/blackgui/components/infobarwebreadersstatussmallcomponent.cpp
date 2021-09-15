/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "infobarwebreadersstatussmallcomponent.h"
#include "ui_infobarwebreadersstatussmallcomponent.h"

namespace BlackGui::Components
{
    CInfoBarWebReadersStatusSmallComponent::CInfoBarWebReadersStatusSmallComponent(QWidget *parent) :
        CInfoBarWebReadersStatusBase(parent),
        ui(new Ui::CInfoBarWebReadersStatusSmallComponent)
    {
        ui->setupUi(this);
        this->setLeds(ui->led_SwiftDb, ui->led_DataReady, nullptr, ui->led_IcaoAircraft, ui->led_IcaoAirline, ui->led_Countries, ui->led_Distributors, ui->led_Liveries, ui->led_Models);
        this->init();
    }

    CInfoBarWebReadersStatusSmallComponent::~CInfoBarWebReadersStatusSmallComponent()
    { }
} // ns
