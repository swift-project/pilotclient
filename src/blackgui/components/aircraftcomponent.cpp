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
#include "enablefordockwidgetinfoarea.h"
#include "../guiutility.h"
#include "blackcore/context_network.h"
#include "blackcore/context_simulator.h"
#include "blackcore/network.h"

using namespace BlackGui;
using namespace BlackGui::Views;
using namespace BlackCore;


namespace BlackGui
{
    namespace Components
    {

        CAircraftComponent::CAircraftComponent(QWidget *parent) :
            QTabWidget(parent),
            ui(new Ui::CAircraftComponent)
        {
            ui->setupUi(this);
            this->tabBar()->setExpanding(false);
            this->ui->tvp_AirportsInRange->setResizeMode(CAirportView::ResizingOnce);
            m_updateTimer = new CUpdateTimer(&CAircraftComponent::update, this);

            connect(this->ui->tvp_AircraftInRange, &CAircraftView::rowCountChanged, this, &CAircraftComponent::ps_onRowCountChanged);
            connect(this->ui->tvp_AirportsInRange, &CAircraftView::rowCountChanged, this, &CAircraftComponent::ps_onRowCountChanged);
        }

        CAircraftComponent::~CAircraftComponent()
        { }

        int CAircraftComponent::countAircraft() const
        {
            Q_ASSERT(this->ui->tvp_AircraftInRange);
            return this->ui->tvp_AircraftInRange->rowCount();
        }

        int CAircraftComponent::countAirportsInRange() const
        {
            Q_ASSERT(this->ui->tvp_AirportsInRange);
            return this->ui->tvp_AirportsInRange->rowCount();
        }

        void CAircraftComponent::update()
        {
            Q_ASSERT(this->ui->tvp_AircraftInRange);
            Q_ASSERT(this->getIContextNetwork());
            Q_ASSERT(this->getIContextSimulator());

            if (this->getIContextNetwork()->isConnected())
            {
                bool visible = (this->isVisibleWidget() && this->currentWidget() == this->ui->tb_AircraftInRange);
                if (this->countAircraft() < 1 || visible)
                {
                    this->ui->tvp_AircraftInRange->updateContainer(this->getIContextNetwork()->getAircraftInRange().toAircraftList());
                }
            }
            if (this->getIContextSimulator()->isConnected())
            {
                bool visible = (this->isVisibleWidget() && this->currentWidget() == this->ui->tb_AirportsInRange);
                if (this->countAirportsInRange() < 1 || visible)
                {
                    this->ui->tvp_AirportsInRange->updateContainer(this->getIContextSimulator()->getAirportsInRange());
                }
            }
        }

        void CAircraftComponent::runtimeHasBeenSet()
        {
            Q_ASSERT(this->getIContextNetwork());
            connect(this->getParentInfoArea(), &CInfoArea::changedInfoAreaTabBarIndex, this, &CAircraftComponent::ps_infoAreaTabBarChanged);
            connect(this->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this, &CAircraftComponent::ps_connectionStatusChanged);
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

        void CAircraftComponent::ps_onRowCountChanged(int count, bool withFilter)
        {
            Q_UNUSED(count);
            Q_UNUSED(withFilter);
            int ac = this->indexOf(this->ui->tb_AircraftInRange);
            int ap = this->indexOf(this->ui->tb_AirportsInRange);
            QString acs = this->tabBar()->tabText(ac);
            QString aps = this->tabBar()->tabText(ap);
            acs = CGuiUtility::replaceTabCountValue(acs, this->countAircraft());
            aps = CGuiUtility::replaceTabCountValue(aps, this->countAirportsInRange());
            this->tabBar()->setTabText(ac, acs);
            this->tabBar()->setTabText(ap, aps);
        }

        void CAircraftComponent::ps_connectionStatusChanged(int from, int to)
        {
            INetwork::ConnectionStatus fromStatus = static_cast<INetwork::ConnectionStatus>(from);
            INetwork::ConnectionStatus toStatus = static_cast<INetwork::ConnectionStatus>(to);
            Q_UNUSED(fromStatus);
            if (INetwork::isDisconnectedStatus(toStatus))
            {
                this->ui->tvp_AircraftInRange->clear();
            }
        }

    } // namespace
} // namespace
