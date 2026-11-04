// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "ministatusbarwidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QStringBuilder>

#include "core/context/contextnetwork.h"
#include "core/context/contextsimulator.h"
#include "core/simulator.h"
#include "gui/guiapplication.h"
#include "gui/led.h"
#include "misc/network/server.h"
#include "misc/simulation/simulatorplugininfo.h"

using namespace swift::core;
using namespace swift::core::context;
using namespace swift::gui;
using namespace swift::misc;
using namespace swift::misc::network;

CMiniStatusBarWidget::CMiniStatusBarWidget(QWidget *parent) : QFrame(parent)
{
    this->setFrameShape(QFrame::StyledPanel);

    m_ledSimulator = new CLedWidget(this);
    m_ledSimulator->setValues(CLedWidget::Yellow, CLedWidget::Black, CLedWidget::Blue, CLedWidget::Rounded,
                              "Simulator running", "Simulator disconnected", "Simulator connected", 14);

    m_ledNetwork = new CLedWidget(this);
    m_ledNetwork->setValues(CLedWidget::Yellow, CLedWidget::Black, CLedWidget::Rounded, "Network connected",
                            "Network disconnected", 14);

    QLabel *lblSim = new QLabel("Simulator", this);
    QLabel *lblNet = new QLabel("FSD-Server", this);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(6, 2, 6, 2);
    layout->addWidget(lblSim);
    layout->addWidget(m_ledSimulator);
    layout->addSpacing(16);
    layout->addWidget(lblNet);
    layout->addWidget(m_ledNetwork);
    layout->addStretch(1);
    this->setLayout(layout);

    Q_ASSERT_X(sGui, Q_FUNC_INFO, "Need sGui");
    if (sGui->getIContextSimulator())
    {
        connect(sGui->getIContextSimulator(), &IContextSimulator::simulatorStatusChanged, this,
                &CMiniStatusBarWidget::onSimulatorStatusChanged, Qt::QueuedConnection);
    }

    if (sGui->getIContextNetwork())
    {
        connect(sGui->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this,
                &CMiniStatusBarWidget::onNetworkStatusChanged, Qt::QueuedConnection);
    }

    this->updateValues();
}

CMiniStatusBarWidget::~CMiniStatusBarWidget() {}

void CMiniStatusBarWidget::onSimulatorStatusChanged(int status)
{
    if (!sGui || sGui->isShuttingDown() || !sGui->getIContextSimulator()) { return; }
    const auto simStatus = static_cast<swift::core::ISimulator::SimulatorStatus>(status);

    if (simStatus.testFlag(swift::core::ISimulator::Connected))
    {
        // kurzer Typ-Name, z.B. P3D, MSFS2020, MSFS2024, XPlane
        const QString shortName =
            sGui->getIContextSimulator()->getSimulatorPluginInfo().getSimulatorInfo().convertToQString();
        const QString tooltip = shortName % u": " % swift::core::ISimulator::statusToString(simStatus);

        if (simStatus.testFlag(swift::core::ISimulator::Simulating))
        {
            m_ledSimulator->setOn(true);
            m_ledSimulator->setOnToolTip(tooltip);
        }
        else
        {
            // only connected, not yet simulating
            m_ledSimulator->setTriState();
            m_ledSimulator->setTriStateToolTip(tooltip);
        }
    }
    else { m_ledSimulator->setOn(false); }
}

void CMiniStatusBarWidget::onNetworkStatusChanged(const CConnectionStatus &from, const CConnectionStatus &to)
{
    Q_UNUSED(from)
    if (!sGui || sGui->isShuttingDown() || !sGui->getIContextNetwork()) { return; }

    switch (to.getConnectionStatus())
    {
    case CConnectionStatus::Connected:
        m_ledNetwork->setOn(true);
        m_ledNetwork->setOnToolTip(u"Verbunden mit: " % sGui->getIContextNetwork()->getConnectedServer().getName());
        break;
    case CConnectionStatus::Connecting: m_ledNetwork->setTriStateColor(CLedWidget::Yellow); break;
    default: m_ledNetwork->setOn(false); break;
    }
}

void CMiniStatusBarWidget::updateValues()
{
    if (!sGui || sGui->isShuttingDown()) { return; }
    if (sGui->getIContextSimulator())
    {
        this->onSimulatorStatusChanged(sGui->getIContextSimulator()->getSimulatorStatus());
    }
    if (sGui->getIContextNetwork()) { m_ledNetwork->setOn(sGui->getIContextNetwork()->isConnected()); }
}