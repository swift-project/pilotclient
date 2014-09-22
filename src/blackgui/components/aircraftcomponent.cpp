/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircraftcomponent.h"
#include "ui_aircraftcomponent.h"
#include "dockwidgetinfoareacomponent.h"
#include "../guiutility.h"

namespace BlackGui
{
    namespace Components
    {

        CAircraftComponent::CAircraftComponent(QWidget *parent) :
            QTabWidget(parent),
            CDockWidgetInfoAreaComponent(this),
            CRuntimeBasedComponent(nullptr, false),
            ui(new Ui::CAircraftComponent), m_timerComponent(nullptr)
        {
            ui->setupUi(this);
            m_timerComponent = new CTimerBasedComponent(SLOT(update()), this);
        }

        CAircraftComponent::~CAircraftComponent()
        {
            delete ui;
        }

        int CAircraftComponent::countAircrafts() const
        {
            Q_ASSERT(this->ui->tvp_AircraftsInRange);
            return this->ui->tvp_AircraftsInRange->rowCount();
        }

        int CAircraftComponent::countAirportsInRange() const
        {
            Q_ASSERT(this->ui->tvp_AirportsInRange);
            return this->ui->tvp_AirportsInRange->rowCount();
        }

        void CAircraftComponent::update()
        {
            Q_ASSERT(this->ui->tvp_AircraftsInRange);
            Q_ASSERT(this->getIContextNetwork());
            Q_ASSERT(this->getIContextSimulator());

            if (this->getIContextNetwork()->isConnected())
            {
                if (this->countAircrafts() < 1 || this->isVisibleWidget())
                {
                    this->ui->tvp_AircraftsInRange->updateContainer(this->getIContextNetwork()->getAircraftsInRange());
                }
                else
                {
                    // KWB remove: qDebug() will be removed soo
                    qDebug() << this->objectName() << "Skipping update (aircrafts)";
                }
            }
            if (this->getIContextSimulator()->isConnected())
            {
                if (this->countAirportsInRange() < 1 || this->isVisibleWidget())
                {
                    this->ui->tvp_AirportsInRange->updateContainer(this->getIContextSimulator()->getAirportsInRange());
                }
                else
                {
                    qDebug() << this->objectName() << "Skipping update (airports)";
                }
            }
        }

        void CAircraftComponent::runtimeHasBeenSet()
        {
            connect(this->getParentInfoArea(), &CInfoArea::tabBarCurrentChanged, this, &CAircraftComponent::ps_infoAreaTabBarChanged);
        }

        void CAircraftComponent::ps_infoAreaTabBarChanged(int index)
        {
            // ignore in those cases
            if (!this->isVisibleWidget()) return;
            if (this->isParentDockWidgetFloating()) return;
            if (!this->getIContextNetwork()->isConnected()) return;

            // here I know I am the selected widget, update, but keep GUI responsive (hence
            QTimer::singleShot(1000, this, SLOT(update()));
            Q_UNUSED(index);
        }

    } // namespace
} // namespace
