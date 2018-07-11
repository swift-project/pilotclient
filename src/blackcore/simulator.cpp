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
#include "blackmisc/logmessage.h"

#include <QFlag>
#include <Qt>
#include <QtGlobal>
#include <QPointer>

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
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

    bool ISimulator::requestElevation(const ICoordinateGeodetic &reference, const CCallsign &callsign)
    {
        Q_UNUSED(reference);
        Q_UNUSED(callsign);
        return false;
    }

    void ISimulator::callbackReceivedRequestedElevation(const CElevationPlane &plane, const CCallsign &callsign)
    {
        if (this->isShuttingDown()) { return; }

        ISimulationEnvironmentProvider::rememberGroundElevation(callsign, plane); // in simulator
        const int updated = CRemoteAircraftAware::updateAircraftGroundElevation(callsign, plane, CAircraftSituation::FromProvider);
        if (updated < 1) { return; }
        emit this->receivedRequestedElevation(plane, callsign);
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

    ISimulator::ISimulator(const CSimulatorPluginInfo &pluginInfo,
                           IOwnAircraftProvider *ownAircraftProvider,
                           IRemoteAircraftProvider *remoteAircraftProvider,
                           IWeatherGridProvider *weatherGridProvider,
                           IClientProvider *clientProvider,
                           QObject *parent) :
        QObject(parent),
        COwnAircraftAware(ownAircraftProvider),
        CRemoteAircraftAware(remoteAircraftProvider),
        CWeatherGridAware(weatherGridProvider),
        CClientAware(clientProvider),
        ISimulationEnvironmentProvider(pluginInfo),
        IInterpolationSetupProvider(),
        CIdentifiable(this)
    {
        ISimulator::registerHelp();
    }

    void ISimulator::rememberElevationAndCG(const CCallsign &callsign, const QString &modelString, const Geo::CElevationPlane &elevation, const CLength &cg)
    {
        if (callsign.isEmpty()) { return; }
        if (!elevation.isNull())
        {
            const int aircraftCount = this->getAircraftInRangeCount();
            this->setMaxElevationsRemembered(aircraftCount * 3); // at least 3 elevations per aircraft, even better as not all are requesting elevations
            this->rememberGroundElevation(callsign, elevation);
        }
        if (!cg.isNull() && !this->hasSameCG(cg, callsign)) { this->insertCG(cg, modelString, callsign); }
    }

    void ISimulator::emitSimulatorCombinedStatus(SimulatorStatus oldStatus)
    {
        const SimulatorStatus newStatus = this->getSimulatorStatus();
        if (oldStatus != newStatus)
        {
            // decouple, follow up of signal can include unloading
            // simulator so this should happen strictly asyncronously (which is like forcing Qt::QueuedConnection)
            QPointer<ISimulator> myself(this);
            QTimer::singleShot(0, this, [ = ]
            {
                if (!myself) { return; }
                emit myself->simulatorStatusChanged(newStatus);
            });
        }
    }

    void ISimulator::emitInterpolationSetupChanged()
    {
        QPointer<ISimulator> myself(this);
        QTimer::singleShot(0, this, [ = ]
        {
            if (!myself) { return; }
            emit this->interpolationAndRenderingSetupChanged();
        });
    }


    bool ISimulator::setInterpolationSetupGlobal(const CInterpolationAndRenderingSetupGlobal &setup)
    {
        if (!IInterpolationSetupProvider::setInterpolationSetupGlobal(setup)) { return false; }
        const bool r = setup.isRenderingRestricted();
        const bool e = setup.isRenderingEnabled();

        emit this->renderRestrictionsChanged(r, e, setup.getMaxRenderedAircraft(), setup.getMaxRenderedDistance());
        return true;
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
