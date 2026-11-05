// SPDX-FileCopyrightText: Copyright (C) 2026 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "miniflightplandialog.h"

#include <QVBoxLayout>

#include "gui/components/flightplancomponent.h"

using namespace swift::gui::components;

CMiniFlightPlanDialog::CMiniFlightPlanDialog(QWidget *parent) : QDialog(parent)
{
    this->setWindowTitle(tr("Flugplanverwaltung"));
    this->resize(700, 600);

    m_flightPlanComponent = new CFlightPlanComponent(this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->addWidget(m_flightPlanComponent);
    this->setLayout(layout);
}

CMiniFlightPlanDialog::~CMiniFlightPlanDialog() {}