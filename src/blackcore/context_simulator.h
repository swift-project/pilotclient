/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_CONTEXTSIMULATOR_H
#define BLACKCORE_CONTEXTSIMULATOR_H

#define BLACKCORE_CONTEXTSIMULATOR_INTERFACENAME "net.vatsim.PilotClient.BlackCore.ContextSimulator"
#define BLACKCORE_CONTEXTSIMULATOR_OBJECTPATH "/Simulator"

#include "blackcore/dbus_server.h"
#include "blackcore/coreruntime.h"
#include "blackmisc/avaircraft.h"
#include <QObject>

namespace BlackCore
{
    //! \brief Network context
    class IContextSimulator : public QObject
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTSIMULATOR_INTERFACENAME)

    public:

        /*!
         * \brief Service name
         * \return
         */
        static const QString &InterfaceName()
        {
            static QString s(BLACKCORE_CONTEXTSIMULATOR_INTERFACENAME);
            return s;
        }

        /*!
         * \brief Service path
         * \return
         */
        static const QString &ObjectPath()
        {
            static QString s(BLACKCORE_CONTEXTSIMULATOR_OBJECTPATH);
            return s;
        }

        //! \brief Constructor
        IContextSimulator(QObject *parent = nullptr) : QObject(parent) {}

        //! Destructor
        virtual ~IContextSimulator() {}

        /*!
         * \brief Using local objects?
         * \return
         */
        virtual bool usingLocalObjects() const = 0;

        /*!
         * \brief Get user aircraft value object
         * \return
         */
        virtual BlackMisc::Aviation::CAircraft ownAircraft() const = 0;

    signals:
        /*!
         * \brief Emitted when the simulator connection changes
         * \param value
         */
        void connectionChanged(bool value);

    public slots:

        //! Returns true when simulator is connected and available
        virtual bool isConnected() const = 0;
    };

} // namespace BlackCore

#endif // BLACKCORE_CONTEXTSIMULATOR_H
