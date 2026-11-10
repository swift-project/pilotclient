// SPDX-FileCopyrightText: Copyright (C) 2026 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "miniflightplandialog.h"

#include <QCloseEvent>
#include <QVBoxLayout>

#include "gui/components/flightplancomponent.h"

using namespace swift::gui::components;
using namespace swift::misc::aviation;

CMiniFlightPlanDialog::CMiniFlightPlanDialog(QWidget *parent) : QDialog(parent)
{
    this->setWindowTitle(tr("Flight Plan Management"));
    this->resize(700, 600);

    m_flightPlanComponent = new CFlightPlanComponent(this);
    connect(m_flightPlanComponent, &CFlightPlanComponent::flightPlanDataChanged, this,
            &CMiniFlightPlanDialog::flightPlanDataChanged);
    connect(m_flightPlanComponent, &CFlightPlanComponent::flightPlanDataChanged, this,
            &CMiniFlightPlanDialog::onFlightPlanDataChanged);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->addWidget(m_flightPlanComponent);
    this->setLayout(layout);

    // restore the flight plan last entered/saved locally for the Mini-GUI (independent from swiftguistandard)
    const CFlightPlan lastFlightPlan = m_lastFlightPlanSetting.get();
    if (!lastFlightPlan.getRoute().isEmpty() || !lastFlightPlan.getOriginAirportIcao().isEmpty())
    {
        m_flightPlanComponent->fillWithFlightPlanData(lastFlightPlan);
    }
}

CMiniFlightPlanDialog::~CMiniFlightPlanDialog() {}

void CMiniFlightPlanDialog::sendFlightPlan()
{
    if (m_flightPlanComponent) { m_flightPlanComponent->sendFlightPlan(); }
}

CFlightPlan CMiniFlightPlanDialog::getCurrentFlightPlanData() const
{
    return m_flightPlanComponent ? m_flightPlanComponent->getCurrentFlightPlanData() : CFlightPlan();
}

void CMiniFlightPlanDialog::closeEvent(QCloseEvent *event)
{
    if (m_flightPlanComponent)
    {
        m_lastFlightPlanSetting.setAndSave(m_flightPlanComponent->getCurrentFlightPlanData());
    }
    QDialog::closeEvent(event);
}

void CMiniFlightPlanDialog::onFlightPlanDataChanged(bool available)
{
    m_hasFlightPlan = available;
    if (m_flightPlanComponent)
    {
        emit this->flightPlanContentChanged(m_flightPlanComponent->getCurrentFlightPlanData());
    }
}