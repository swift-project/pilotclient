/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_CONTEXTSIMULATOR_PROXY_H
#define BLACKCORE_CONTEXTSIMULATOR_PROXY_H

#include "blackcoreexport.h"
#include "blackcore/context_simulator.h"
#include "blackmisc/genericdbusinterface.h"
#include "blackmisc/pixmap.h"

namespace BlackCore
{
    //! \brief DBus proxy for Simulator Context
    //! \ingroup dbus
    class BLACKCORE_EXPORT CContextSimulatorProxy : public IContextSimulator
    {
        Q_OBJECT
        friend class IContextSimulator;

    public:
        //! Destructor
        ~CContextSimulatorProxy() {}

    private:
        BlackMisc::CGenericDBusInterface *m_dBusInterface;

        //! Relay connection signals to local signals
        void relaySignals(const QString &serviceName, QDBusConnection &connection);

    protected:
        //! Constructor
        CContextSimulatorProxy(CRuntimeConfig::ContextMode mode, CRuntime *runtime) : IContextSimulator(mode, runtime), m_dBusInterface(0) {}

        //! DBus version constructor
        CContextSimulatorProxy(const QString &serviceName, QDBusConnection &connection, CRuntimeConfig::ContextMode mode, CRuntime *runtime);

    public slots:

        //! \copydoc IContextSimulator::getSimualtorStatus()
        virtual int getSimulatorStatus() const override;

        //! \copydoc IContextSimulator::disconnectFrom
        virtual bool disconnectFromSimulator() override;

        //! \copydoc IContextSimulator::getAirportsInRange()
        virtual BlackMisc::Aviation::CAirportList getAirportsInRange() const override;

        //! \copydoc IContextSimulator::getInstalledModels()
        virtual BlackMisc::Simulation::CAircraftModelList getInstalledModels() const override;

        //! \copydoc IContextSimulator::getInstalledModelsStartingWith
        virtual BlackMisc::Simulation::CAircraftModelList getInstalledModelsStartingWith(const QString modelString) const override;

        //! \copydoc IContextSimulator::getInstalledModelsCount
        virtual int getInstalledModelsCount() const override;

        //! \copydoc IContextSimulator::reloadInstalledModels
        virtual void reloadInstalledModels() override;

        //! \copydoc IContextSimulator::getIcaoForModelString
        virtual BlackMisc::Aviation::CAircraftIcaoData getIcaoForModelString(const QString &modelString) const override;

        //! \copydoc IContextSimulator::getSimulatorPluginInfo
        virtual BlackMisc::Simulation::CSimulatorPluginInfo getSimulatorPluginInfo() const override;

        //! \copydoc IContextSimulator::getSimulatorSetup
        virtual BlackMisc::Simulation::CSimulatorSetup getSimulatorSetup() const override;

        //! \copydoc IContextSimulator::getSimulatorPluginList()
        virtual BlackMisc::Simulation::CSimulatorPluginInfoList getAvailableSimulatorPlugins() const override;

        //! \copydoc IContextSimulator::setTimeSynchronization
        virtual bool setTimeSynchronization(bool enable, BlackMisc::PhysicalQuantities::CTime offset) override;

        //! \copydoc IContextSimulator::isTimeSynchronized
        virtual bool isTimeSynchronized() const override;

        //! \copydoc IContextSimulator::getMaxRenderedAircraft
        virtual int getMaxRenderedAircraft() const override;

        //! \copydoc IContextSimulator::setMaxRenderedRemoteAircraft
        virtual void setMaxRenderedAircraft(int number) override;

        //! \copydoc IContextSimulator::setMaxRenderedDistance
        virtual void setMaxRenderedDistance(BlackMisc::PhysicalQuantities::CLength &distance);

        //! \copydoc IContextSimulator::setMaxRenderedDistance
        virtual void deleteAllRenderingRestrictions() override;

        //! \copydoc IContextSimulator::isRenderingRestricted
        virtual bool isRenderingRestricted() const override;

        //! \copydoc IContextSimulator::isRenderingEnabled
        virtual bool isRenderingEnabled() const override;

        //! \copydoc IContextSimulator::getMaxRenderedDistance
        virtual BlackMisc::PhysicalQuantities::CLength getMaxRenderedDistance() const override;

        //! \copydoc IContextSimulator::getRenderedDistanceBoundary
        virtual BlackMisc::PhysicalQuantities::CLength getRenderedDistanceBoundary() const override;

        //! \copydoc IContextSimulator::getRenderRestrictionText
        virtual QString getRenderRestrictionText() const override;

        //! \copydoc IContextSimulator::getTimeSynchronizationOffset
        virtual BlackMisc::PhysicalQuantities::CTime getTimeSynchronizationOffset() const override;

        //! \copydoc IContextSimulator::loadSimulatorPlugin
        virtual bool loadSimulatorPlugin(const BlackMisc::Simulation::CSimulatorPluginInfo &simulatorInfo) override;

        //! \copydoc IContextSimulator::loadSimulatorPluginFromSettings()
        virtual bool loadSimulatorPluginFromSettings();

        //! \copydoc IContextSimulator::listenForSimulator()
        virtual void listenForSimulator(const BlackMisc::Simulation::CSimulatorPluginInfo &simulatorInfo) override;

        //! \copydoc IContextSimulator::listenForAllSimulators()
        virtual void listenForAllSimulators() override;

        //! \copydoc IContextSimulator::listenForSimulatorFromSettings()
        virtual void listenForSimulatorFromSettings() override;

        //! \copydoc IContextSimulator::unloadSimulatorPlugin()
        virtual void unloadSimulatorPlugin() override;

        //! \copydoc IContextSimulator::settingsChanged
        virtual void settingsChanged(uint type) override;

        //! \copydoc IContextSimulator::iconForModel
        virtual BlackMisc::CPixmap iconForModel(const QString &modelString) const override;

        //! \copydoc IContextSimulator::highlightAircraft
        virtual void highlightAircraft(const BlackMisc::Simulation::CSimulatedAircraft &aircraftToHighlight, bool enableHighlight, const BlackMisc::PhysicalQuantities::CTime &displayTime) override;

        //! \copydoc ISimulator::enableDebuggingMessages
        virtual void enableDebugMessages(bool driver, bool interpolator) override;

    };

} // namespace BlackCore

#endif // guard
