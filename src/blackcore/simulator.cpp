/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/simulator.h"
#include "blackcore/application.h"

#include <QFlag>
#include <Qt>
#include <QtGlobal>

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Network;
using namespace BlackMisc::Weather;

namespace BlackCore
{
    ISimulator::SimulatorStatus ISimulator::getSimulatorStatus() const
    {
        if (!this->isConnected()) { return ISimulator::Disconnected; }
        const SimulatorStatus status =
            Connected
            | (this->isSimulating() ? ISimulator::Simulating : static_cast<ISimulator::SimulatorStatusFlag>(0))
            | (this->isPaused() ? ISimulator::Paused : static_cast<ISimulator::SimulatorStatusFlag>(0));
        return status;
    }

    CInterpolationAndRenderingSetupPerCallsign ISimulator::getInterpolationSetupConsolidated(const CCallsign &callsign) const
    {
        CInterpolationAndRenderingSetupPerCallsign setup = this->getInterpolationSetupPerCallsignOrDefault(callsign);
        const CClient client = this->getClientOrDefaultForCallsign(callsign);
        setup.consolidateWithClient(client);
        return setup;
    }

    bool ISimulator::requestElevation(const Geo::ICoordinateGeodetic &reference) const
    {
        if (this->isShuttingDown()) { return false; }
        if (reference.isNull()) { return false; }
        Q_UNUSED(reference);
        return false;
    }

    void ISimulator::registerHelp()
    {
        if (CSimpleCommandParser::registered("BlackCore::ISimulator")) { return; }
        CSimpleCommandParser::registerCommand({".drv", "alias: .driver .plugin"});
        CSimpleCommandParser::registerCommand({".drv unload", "unload driver"});
        if (CBuildConfig::isCompiledWithFsuipcSupport())
        {
            CSimpleCommandParser::registerCommand({".drv fsuipc on|off", "enable/disable FSUIPC (if applicable)"});
        }
    }

    QString ISimulator::statusToString(SimulatorStatus status)
    {
        QStringList s;
        if (status.testFlag(Unspecified))  { s << QStringLiteral("Unspecified"); }
        if (status.testFlag(Disconnected)) { s << QStringLiteral("Disconnected"); }
        if (status.testFlag(Connected))    { s << QStringLiteral("Connected"); }
        if (status.testFlag(Simulating))   { s << QStringLiteral("Simulating"); }
        if (status.testFlag(Paused))       { s << QStringLiteral("Paused"); }
        return s.join(", ");
    }

    bool ISimulator::isAnyConnectedStatus(SimulatorStatus status)
    {
        return (status.testFlag(Connected) || status.testFlag(Simulating) || status.testFlag(Paused));
    }

    ISimulator::ISimulator(
        const CSimulatorPluginInfo &pluginInfo, IOwnAircraftProvider *ownAircraftProvider,
        IRemoteAircraftProvider *remoteAircraftProvider, IWeatherGridProvider *weatherGridProvider, QObject *parent) :
        QObject(parent),
        COwnAircraftAware(ownAircraftProvider),
        CRemoteAircraftAware(remoteAircraftProvider),
        CWeatherGridAware(weatherGridProvider),
        ISimulationEnvironmentProvider(pluginInfo),
        IInterpolationSetupProvider(),
        CIdentifiable(this)
    {
        ISimulator::registerHelp();
    }

    void ISimulator::rememberElevationAndCG(const CCallsign &callsign, const Geo::CElevationPlane &elevation, const CLength &cg)
    {
        if (callsign.isEmpty()) { return; }
        if (!elevation.isNull()) { this->rememberGroundElevation(elevation); }
        if (!cg.isNull() && !this->hasSameCG(cg, callsign)) { this->insertCG(cg, callsign); }
    }

    void ISimulator::emitSimulatorCombinedStatus(SimulatorStatus oldStatus)
    {
        const SimulatorStatus newStatus = this->getSimulatorStatus();
        if (oldStatus != newStatus)
        {
            // decouple, follow up of signal can include unloading
            // simulator so this should happen strictly asyncronously (which is like forcing Qt::QueuedConnection)
            QTimer::singleShot(0, this, [ = ]
            {
                emit this->simulatorStatusChanged(newStatus);
            });
        }
    }

    ISimulatorListener::ISimulatorListener(const CSimulatorPluginInfo &info) :
        QObject(), m_info(info)
    {
        this->setObjectName("ISimulatorListener:" + info.toQString());

        // stop listener after it reports simulator ready
        const bool s = connect(this, &ISimulatorListener::simulatorStarted, this, &ISimulatorListener::stop, Qt::QueuedConnection);
        Q_ASSERT_X(s, Q_FUNC_INFO, "connect failed");
        Q_UNUSED(s)
    }

    QString ISimulatorListener::backendInfo() const
    {
        return m_info.toQString();
    }

    bool ISimulatorListener::isShuttingDown() const
    {
        return (!sApp || sApp->isShuttingDown());
    }

    void ISimulatorListener::start()
    {
        if (m_isRunning) { return; }
        m_isRunning = true;
        this->startImpl();
    }

    void ISimulatorListener::stop()
    {
        if (!m_isRunning) { return; }
        this->stopImpl();
        m_isRunning = false;
    }
} // namespace
