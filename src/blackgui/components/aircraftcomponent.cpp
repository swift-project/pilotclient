/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/context/contextnetwork.h"
#include "blackcore/context/contextsimulator.h"
#include "blackcore/network.h"
#include "blackgui/components/aircraftcomponent.h"
#include "blackgui/guiapplication.h"
#include "blackgui/guiutility.h"
#include "blackgui/infoarea.h"
#include "blackgui/models/simulatedaircraftlistmodel.h"
#include "blackgui/views/airportview.h"
#include "blackgui/views/simulatedaircraftview.h"
#include "blackgui/views/viewbase.h"
#include "ui_aircraftcomponent.h"

#include <QString>
#include <QTabBar>
#include <QTimer>

using namespace BlackGui;
using namespace BlackGui::Views;
using namespace BlackGui::Models;
using namespace BlackGui::Settings;
using namespace BlackCore;
using namespace BlackCore::Context;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackGui
{
    namespace Components
    {
        CAircraftComponent::CAircraftComponent(QWidget *parent) :
            QTabWidget(parent),
            ui(new Ui::CAircraftComponent)
        {
            ui->setupUi(this);
            this->setCurrentIndex(0);
            this->tabBar()->setExpanding(false);
            this->tabBar()->setUsesScrollButtons(true);
            ui->tvp_AirportsInRange->setResizeMode(CAirportView::ResizingOnce);

            ui->tvp_AircraftInRange->setAircraftMode(CSimulatedAircraftListModel::NetworkMode);
            ui->tvp_AircraftInRange->configureMenu(true, false, false);

            connect(ui->tvp_AircraftInRange, &CSimulatedAircraftView::modelDataChangedDigest, this, &CAircraftComponent::ps_onRowCountChanged);
            connect(ui->tvp_AircraftInRange, &CSimulatedAircraftView::requestTextMessageWidget, this, &CAircraftComponent::requestTextMessageWidget);
            connect(ui->tvp_AircraftInRange, &CSimulatedAircraftView::requestHighlightInSimulator, this, &CAircraftComponent::ps_onMenuHighlightInSimulator);
            connect(ui->tvp_AirportsInRange, &CSimulatedAircraftView::modelDataChangedDigest, this, &CAircraftComponent::ps_onRowCountChanged);
            connect(sGui->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this, &CAircraftComponent::ps_connectionStatusChanged);
            connect(&m_updateTimer, &QTimer::timeout, this, &CAircraftComponent::update);

            this->ps_settingsChanged();
            m_updateTimer.start();
        }

        CAircraftComponent::~CAircraftComponent()
        { }

        int CAircraftComponent::countAircraftInView() const
        {
            Q_ASSERT(ui->tvp_AircraftInRange);
            return ui->tvp_AircraftInRange->rowCount();
        }

        int CAircraftComponent::countAirportsInRangeInView() const
        {
            Q_ASSERT(ui->tvp_AirportsInRange);
            return ui->tvp_AirportsInRange->rowCount();
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
            if (sGui->isShuttingDown()) { return; }

            Q_ASSERT(ui->tvp_AircraftInRange);
            Q_ASSERT(sGui->getIContextNetwork());
            Q_ASSERT(sGui->getIContextSimulator());

            // count < 1 checks if view already has been updated
            if (sGui->getIContextNetwork()->isConnected())
            {
                const bool visible = (this->isVisibleWidget() && this->currentWidget() == ui->tb_AircraftInRange);
                if (this->countAircraftInView() < 1 || visible)
                {
                    ui->tvp_AircraftInRange->updateContainer(sGui->getIContextNetwork()->getAircraftInRange());
                }
            }
            if (sGui->getIContextSimulator()->getSimulatorStatus() > 0)
            {
                const bool visible = (this->isVisibleWidget() && this->currentWidget() == ui->tb_AirportsInRange);
                if (this->countAirportsInRangeInView() < 1 || visible)
                {
                    ui->tvp_AirportsInRange->updateContainer(sGui->getIContextSimulator()->getAirportsInRange());
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
            int ac = this->indexOf(ui->tb_AircraftInRange);
            int ap = this->indexOf(ui->tb_AirportsInRange);
            QString acs = this->tabBar()->tabText(ac);
            QString aps = this->tabBar()->tabText(ap);
            acs = CGuiUtility::replaceTabCountValue(acs, this->countAircraftInView());
            aps = CGuiUtility::replaceTabCountValue(aps, this->countAirportsInRangeInView());
            this->tabBar()->setTabText(ac, acs);
            this->tabBar()->setTabText(ap, aps);
        }

        void CAircraftComponent::ps_connectionStatusChanged(BlackCore::INetwork::ConnectionStatus from, BlackCore::INetwork::ConnectionStatus to)
        {
            Q_UNUSED(from);
            if (INetwork::isDisconnectedStatus(to))
            {
                ui->tvp_AircraftInRange->clear();
            }
            else if (INetwork::isConnectedStatus(to))
            {
                // void
            }
        }

        void CAircraftComponent::ps_onMenuHighlightInSimulator(const CSimulatedAircraft &aircraft)
        {
            if (sGui->getIContextSimulator())
            {
                sGui->getIContextSimulator()->highlightAircraft(aircraft, true, IContextSimulator::HighlightTime());
            }
        }

        void CAircraftComponent::ps_settingsChanged()
        {
            const CViewUpdateSettings settings = this->m_settings.get();
            const int ms = settings.getAircraftUpdateTime().toMs();
            this->m_updateTimer.setInterval(ms);
        }
    } // namespace
} // namespace
