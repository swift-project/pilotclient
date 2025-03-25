// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/components/aircraftcomponent.h"

#include <QPointer>
#include <QString>
#include <QTabBar>
#include <QTimer>

#include "ui_aircraftcomponent.h"

#include "core/context/contextnetwork.h"
#include "core/context/contextownaircraft.h"
#include "core/context/contextsimulator.h"
#include "gui/guiapplication.h"
#include "gui/guiutility.h"
#include "gui/infoarea.h"
#include "gui/models/simulatedaircraftlistmodel.h"
#include "gui/views/simulatedaircraftview.h"
#include "gui/views/viewbase.h"
#include "misc/network/fsdsetup.h"
#include "misc/network/server.h"

using namespace swift::gui;
using namespace swift::gui::views;
using namespace swift::gui::models;
using namespace swift::gui::settings;
using namespace swift::core;
using namespace swift::core::context;
using namespace swift::misc::network;
using namespace swift::misc::simulation;
using namespace swift::misc::physical_quantities;

namespace swift::gui::components
{
    CAircraftComponent::CAircraftComponent(QWidget *parent) : QTabWidget(parent), ui(new Ui::CAircraftComponent)
    {
        ui->setupUi(this);

        Q_ASSERT(sGui->getIContextNetwork());
        Q_ASSERT(sGui->getIContextSimulator());
        Q_ASSERT(sGui->getIContextOwnAircraft());

        this->setCurrentIndex(0);
        this->tabBar()->setExpanding(false);
        this->tabBar()->setUsesScrollButtons(true);
        ui->tvp_AircraftInRange->setAircraftMode(CSimulatedAircraftListModel::NetworkMode);
        ui->tvp_AircraftInRange->configureMenu(true, true, false, true, true, true);

        connect(ui->tvp_AircraftInRange, &CSimulatedAircraftView::modelDataChangedDigest, this,
                &CAircraftComponent::onRowCountChanged);
        connect(ui->tvp_AircraftInRange, &CSimulatedAircraftView::requestTextMessageWidget, this,
                &CAircraftComponent::requestTextMessageWidget);
        connect(sGui->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this,
                &CAircraftComponent::onConnectionStatusChanged, Qt::QueuedConnection);
        connect(sGui->getIContextOwnAircraft(), &IContextOwnAircraft::movedAircraft, this,
                &CAircraftComponent::onOwnAircraftMoved, Qt::QueuedConnection);
        connect(&m_updateTimer, &QTimer::timeout, this, &CAircraftComponent::update);

        using namespace std::chrono_literals;
        m_updateTimer.setInterval(10s);

        m_updateTimer.start();
    }

    CAircraftComponent::~CAircraftComponent() {}

    int CAircraftComponent::countAircraftInView() const
    {
        Q_ASSERT(ui->tvp_AircraftInRange);
        return ui->tvp_AircraftInRange->rowCount();
    }

    bool CAircraftComponent::setParentDockWidgetInfoArea(CDockWidgetInfoArea *parentDockableWidget)
    {
        CEnableForDockWidgetInfoArea::setParentDockWidgetInfoArea(parentDockableWidget);
        const bool c = connect(this->getParentInfoArea(), &CInfoArea::changedInfoAreaTabBarIndex, this,
                               &CAircraftComponent::onInfoAreaTabBarChanged, Qt::QueuedConnection);
        Q_ASSERT_X(c, Q_FUNC_INFO, "failed connect");
        Q_ASSERT_X(parentDockableWidget, Q_FUNC_INFO, "missing parent");
        return c && parentDockableWidget;
    }

    void CAircraftComponent::update()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }

        // count < 1 checks if view already has been updated
        if (sGui->getIContextNetwork()->isConnected())
        {
            const bool visible = (this->isVisibleWidget() && this->currentWidget() == ui->tb_AircraftInRange);
            if (this->countAircraftInView() < 1 || visible)
            {
                ui->tvp_AircraftInRange->updateContainerMaybeAsync(sGui->getIContextNetwork()->getAircraftInRange());
            }
        }

        m_updateCounter++;
    }

    void CAircraftComponent::setTab(CAircraftComponent::AircraftTab tab)
    {
        const int tabIndex = static_cast<int>(tab);
        this->setCurrentIndex(tabIndex);
    }

    void CAircraftComponent::updateViews()
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextNetwork() || !sGui->getIContextSimulator()) { return; }
        ui->tvp_AircraftInRange->updateContainerMaybeAsync(sGui->getIContextNetwork()->getAircraftInRange());
    }

    void CAircraftComponent::onInfoAreaTabBarChanged(int index)
    {
        // ignore in those cases
        if (!this->isVisibleWidget()) return;
        if (this->isParentDockWidgetFloating()) return;
        if (!sGui->getIContextNetwork()->isConnected()) return;

        // here I know I am the selected widget, update, but keep GUI responsive (hence I use a timer)
        QPointer<CAircraftComponent> myself(this);
        QTimer::singleShot(1000, this, [=] {
            if (!myself) { return; }
            myself->update();
        });
        Q_UNUSED(index)
    }

    void CAircraftComponent::onRowCountChanged(int count, bool withFilter)
    {
        Q_UNUSED(count)
        Q_UNUSED(withFilter)
        const int ac = this->indexOf(ui->tb_AircraftInRange);
        QString acs = this->tabBar()->tabText(ac);
        acs = CGuiUtility::replaceTabCountValue(acs, this->countAircraftInView());
    }

    void CAircraftComponent::onConnectionStatusChanged(const CConnectionStatus &from, const CConnectionStatus &to)
    {
        Q_UNUSED(from)
        if (to.isDisconnected()) { ui->tvp_AircraftInRange->clear(); }
        else if (to.isConnected())
        {
            if (sGui && sGui->getIContextNetwork())
            {
                const CServer server = sGui->getIContextNetwork()->getConnectedServer();
                const bool sendFast = server.getFsdSetup().sendInterimPositions();
                ui->tvp_AircraftInRange->configureMenuFastPositionUpdates(sendFast);
            }
        }
    }

    void CAircraftComponent::onOwnAircraftMoved(const CLength &distance)
    {
        Q_UNUSED(distance)
        this->updateViews();
    }
} // namespace swift::gui::components
