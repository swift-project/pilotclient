/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_CONTEXTSIMULATOR_IMPL_H
#define BLACKCORE_CONTEXTSIMULATOR_IMPL_H

#include "blackcore/context_simulator.h"
#include "blackcore/context_network.h"
#include "blackcore/simulator.h"

#include <QTimer>
#include <QDir>

namespace BlackCore
{
    /*!
     * \brief Network simulator concrete implementation
     */
    class CContextSimulator : public IContextSimulator
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTSIMULATOR_INTERFACENAME)
        friend class CRuntime;

    public:
        //! \brief Destructor
        virtual ~CContextSimulator();

    public slots:
        //! \copydoc IContextSimulator::isConnected()
        virtual bool isConnected() const override;

        //! \copydoc IContextSimulator::getOwnAircraft()
        virtual BlackMisc::Aviation::CAircraft getOwnAircraft() const override;

    protected:
        //! \brief Constructor
        CContextSimulator(CRuntimeConfig::ContextMode, CRuntime *runtime);

        //! Register myself in DBus
        CContextSimulator *registerWithDBus(CDBusServer *server)
        {
            Q_ASSERT(server);
            server->addObject(CContextSimulator::ObjectPath(), this);
            return this;
        }

    private slots:
        //! \copydoc IContextSimulator::updateOwnAircraft()
        virtual void updateOwnAircraft();

        //! Set new connection status
        void setConnectionStatus(bool value);

    private:
        /*!
         * \brief Load any kind of plugins
         * \todo Currently it goes through the plugins folder and creates an instance for any plugin it may find
         *       In case an FSX and an X-Plane are in that folder, m_simulator will always point to X-Plane in the end.
         */
        void loadPlugins();
        BlackMisc::Aviation::CAircraft m_ownAircraft;
        BlackCore::ISimulator *m_simulator;
        QTimer *m_updateTimer;
        QDir m_pluginsDir;
    };

} // namespace BlackCore

#endif // guard
