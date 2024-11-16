// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "airportdialog.h"

#include "ui_airportdialog.h"

using namespace swift::misc::aviation;

namespace swift::gui::components
{
    CAirportDialog::CAirportDialog(QWidget *parent) : QDialog(parent), ui(new Ui::CAirportDialog)
    {
        ui->setupUi(this);
        this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    }

    CAirportDialog::~CAirportDialog() {}

    void CAirportDialog::setAirport(const CAirport &airport) { ui->comp_AirportCompleter->setAirport(airport); }

    CAirport CAirportDialog::getAirport() const { return ui->comp_AirportCompleter->getAirport(); }

    void CAirportDialog::clear() { ui->comp_AirportCompleter->clear(); }
} // namespace swift::gui::components
