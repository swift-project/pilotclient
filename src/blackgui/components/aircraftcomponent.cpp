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
#include "blackgui/guiutility.h"
#include "blackgui/guiapplication.h"
#include "blackcore/contextnetwork.h"
#include "blackcore/contextsimulator.h"
#include "blackcore/network.h"

using namespace BlackGui;
using namespace BlackGui::Views;
using namespace BlackGui::Models;
using namespace BlackCore;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackGui
{
    namespace Components
    {

        CAircraftComponent::CAircraftComponent(QWidget *parent) :
            QTabWidget(parent),
            ui(new Ui::CAircraftComponent),
            m_updateTimer(new CUpdateTimer("CAircraftComponent", &CAircraftComponent::update, this))
        {
            ui->setupUi(this);
            this->tabBar()->setExpanding(false);
            this->ui->tvp_AirportsInRange->setResizeMode(CAirportView::ResizingOnce);

            this->ui->tvp_AircraftInRange->setAircraftMode(CSimulatedAircraftListModel::InfoMode);
            this->ui->tvp_AircraftInRange->configureMenu(true, false, false);

            connect(this->ui->tvp_AircraftInRange, &CSimulatedAircraftView::modelDataChanged, this, &CAircraftComponent::ps_onRowCountChanged);
            connect(this->ui->tvp_AircraftInRange, &CSimulatedAircraftView::requestTextMessageWidget, this, &CAircraftComponent::requestTextMessageWidget);
            connect(this->ui->tvp_AircraftInRange, &CSimulatedAircraftView::requestHighlightInSimulator, this, &CAircraftComponent::ps_onMenuHighlightInSimulator);
            connect(this->ui->tvp_AirportsInRange, &CSimulatedAircraftView::modelDataChanged, this, &CAircraftComponent::ps_onRowCountChanged);
            connect(sGui->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this, &CAircraftComponent::ps_connectionStatusChanged);
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

        bool CAircraftComponent::setParentDockWidgetInfoArea(CDockWidgetInfoArea *parentDockableWidget)
        {
            CEnableForDockWidgetInfoArea::setParentDockWidgetInfoArea(parentDockableWidget);
            bool c = connect(this->getParentInfoArea(), &CInfoArea::changedInfoAreaTabBarIndex, this, &CAircraftComponent::ps_infoAreaTabBarChanged);
            Q_ASSERT_X(c, Q_FUNC_INFO, "failed connect");
            Q_ASSERT_X(parentDockableWidget, Q_FUNC_INFO, "missing parent");
            return c && parentDockableWidget;
        }

        void CAircraftComponent::update()
        {
            Q_ASSERT(this->ui->tvp_AircraftInRange);
            Q_ASSERT(sGui->getIContextNetwork());
            Q_ASSERT(sGui->getIContextSimulator());

            if (sGui->getIContextNetwork()->isConnected())
            {
                bool visible = (this->isVisibleWidget() && this->currentWidget() == this->ui->tb_AircraftInRange);
                if (this->countAircraft() < 1 || visible)
                {
                    this->ui->tvp_AircraftInRange->updateContainer(sGui->getIContextNetwork()->getAircraftInRange());
                }
            }
            if (sGui->getIContextSimulator()->getSimulatorStatus() > 0)
            {
                bool visible = (this->isVisibleWidget() && this->currentWidget() == this->ui->tb_AirportsInRange);
                if (this->countAirportsInRange() < 1 || visible)
                {
                    this->ui->tvp_AirportsInRange->updateContainer(sGui->getIContextSimulator()->getAirportsInRange());
                }
            }
        }

        void CAircraftComponent::ps_infoAreaTabBarChanged(int index)
        {
            // ignore in those cases
            if (!this->isVisibleWidget()) return;
            if (this->isParentDockWidgetFloating()) return;
            if (!sGui->getIContextNetwork()->isConnected()) return;

            // here I know I am the selected widget, update, but keep GUI responsive (hence I use a timer)
            QTimer::singleShot(1000, this, &CAircraftComponent::update);
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

        void CAircraftComponent::ps_connectionStatusChanged(BlackCore::INetwork::ConnectionStatus from, BlackCore::INetwork::ConnectionStatus to)
        {
            Q_UNUSED(from);
            if (INetwork::isDisconnectedStatus(to))
            {
                this->ui->tvp_AircraftInRange->clear();
            }
        }

        void CAircraftComponent::ps_onMenuHighlightInSimulator(const CSimulatedAircraft &aircraft)
        {
            if (sGui->getIContextSimulator())
            {
                sGui->getIContextSimulator()->highlightAircraft(aircraft, true, IContextSimulator::HighlightTime());
            }
        }

    } // namespace
} // namespace
