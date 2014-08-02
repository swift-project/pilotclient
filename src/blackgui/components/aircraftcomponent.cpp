/* Copyright (C) 2013
 * Swift Project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircraftcomponent.h"
#include "ui_aircraftcomponent.h"

namespace BlackGui
{
    namespace Components
    {

        CAircraftComponent::CAircraftComponent(QWidget *parent) :
            QTabWidget(parent), CRuntimeBasedComponent(nullptr, false), ui(new Ui::CAircraftComponent), m_timerComponent(nullptr)
        {
            ui->setupUi(this);
            m_timerComponent = new CTimerBasedComponent(SLOT(update()), this);
        }

        CAircraftComponent::~CAircraftComponent()
        {
            delete ui;
        }

        void CAircraftComponent::update()
        {
            Q_ASSERT(this->ui->tvp_AircraftsInRange);
            Q_ASSERT(this->getIContextNetwork());
            Q_ASSERT(this->getIContextSimulator());

            if (this->getIContextNetwork()->isConnected())
            {
                this->ui->tvp_AircraftsInRange->updateContainer(this->getIContextNetwork()->getAircraftsInRange());
            }
            if (this->getIContextSimulator()->isConnected())
            {
                this->ui->tvp_AirportsInRange->updateContainer(this->getIContextSimulator()->getAirportsInRange());
            }
        }
    }
}
