/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_CONTEXTSIMULATOR_PROXY_H
#define BLACKCORE_CONTEXTSIMULATOR_PROXY_H

#include "blackcore/context_simulator.h"
#include "blackmisc/genericdbusinterface.h"

namespace BlackCore
{
    //! \brief DBus proxy for Simulator Context
    //! \ingroup dbus
    class CContextSimulatorProxy : public IContextSimulator
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

        //! \copydoc IContextSimulator::getSimulatorPluginList()
        virtual BlackSim::CSimulatorInfoList getAvailableSimulatorPlugins() const override;

        //! \copydoc IContextSimulator::isConnected()
        virtual bool isConnected() const override;

        //! \copydoc IContextSimulator::canConnect
        virtual bool canConnect() override;

        //! \copydoc IContextSimulator::connectTo
        virtual bool connectTo() override;

        //! \copydoc IContextSimulator::asyncConnectTo
        virtual void asyncConnectTo() override;

        //! \copydoc IContextSimulator::disconnectFrom
        virtual bool disconnectFrom() override;

        //! \copydoc IContextSimulator::getAircraftModel()
        virtual BlackMisc::Network::CAircraftModel getOwnAircraftModel() const override;

        //! \copydoc IContextSimulator::getAirportsInRange()
        virtual BlackMisc::Aviation::CAirportList getAirportsInRange() const override;

        //! \copydoc IContextSimulator::getSimulatorInfo
        virtual BlackSim::CSimulatorInfo getSimulatorInfo() const override;

        //! \copydoc IContextSimulator::setTimeSynchronization
        virtual void setTimeSynchronization(bool enable, BlackMisc::PhysicalQuantities::CTime offset) override;

        //! \copydoc IContextSimulator::isTimeSynchronized
        virtual bool isTimeSynchronized() const override;

        //! \copydoc IContextSimulator::getTimeSynchronizationOffset
        virtual BlackMisc::PhysicalQuantities::CTime getTimeSynchronizationOffset() const override;

        //! \copydoc IContextSimulator::loadSimulatorPlugin
        virtual bool loadSimulatorPlugin(const BlackSim::CSimulatorInfo &simulatorInfo) override;

        //! \copydoc IContextSimulator::loadSimulatorPluginFromSettings()
        virtual bool loadSimulatorPluginFromSettings();

        //! \copydoc IContextSimulator::unloadSimulatorPlugin()
        virtual void unloadSimulatorPlugin() override;

        //! \copydoc IContextSimulator::settingsChanged
        virtual void settingsChanged(uint type) override;

        //! \copydoc IContextSimulator::isSimulatorPaused
        virtual bool isSimulatorPaused() const override;

    };

} // namespace BlackCore

#endif // guard
