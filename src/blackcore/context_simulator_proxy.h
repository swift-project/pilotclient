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
    class CContextSimulatorProxy : public IContextSimulator
    {
        Q_OBJECT
        friend class CRuntime;

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

        //! \brief DBus version constructor
        CContextSimulatorProxy(const QString &serviceName, QDBusConnection &connection, CRuntimeConfig::ContextMode mode, CRuntime *runtime);

    public slots:

        //! \copydoc IContextSimulator::isConnected()
        virtual bool isConnected() const override;

        //! \brief Can we connect?
        virtual bool canConnect() override;

        //! \brief Connect to simulator
        virtual bool connectTo() override;

        //! \brief Disconnect from simulator
        virtual bool disconnectFrom() override;

        //! \copydoc IContextSimulator::getOwnAircraft()
        virtual BlackMisc::Aviation::CAircraft getOwnAircraft() const override;

        //! \copydoc IContextSimulator::getSimulatorInfo
        virtual BlackSim::CSimulatorInfo getSimulatorInfo() const override;
    };

} // namespace BlackCore

#endif // guard
