/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_CONTEXTAPPLICATION_INTERFACE_H
#define BLACKCORE_CONTEXTAPPLICATION_INTERFACE_H

#include "blackmisc/genericdbusinterface.h"
#include "blackmisc/settingutilities.h"
#include "blackmisc/setnetwork.h"
#include "blackmisc/statusmessagelist.h"
#include <QObject>
#include <QVariant>
#include <QDBusAbstractInterface>

#define BLACKCORE_CONTEXTAPPLICATION_INTERFACENAME "blackcore.contextapplication"
#define BLACKCORE_CONTEXTAPPLICATION_SERVICEPATH "/application"

// SERVICENAME must contain at least one ".", otherwise generation fails
// as this is interpreted in the way comain.somename

namespace BlackCore
{

    /*!
     * \brief The interface context settings
     */
    class IContextApplication : public QObject
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTAPPLICATION_INTERFACENAME)

    public:

        /*!
         * \brief Service name
         * \return
         */
        static const QString &InterfaceName()
        {
            static QString s(BLACKCORE_CONTEXTAPPLICATION_INTERFACENAME);
            return s;
        }

        /*!
         * \brief Service path
         * \return
         */
        static const QString &ServicePath()
        {
            static QString s(BLACKCORE_CONTEXTAPPLICATION_SERVICEPATH);
            return s;
        }

        /*!
          * \brief DBus version constructor
          * \param serviceName
          * \param connection
          * \param parent
          */
        IContextApplication(const QString &serviceName, QDBusConnection &connection, QObject *parent = 0);

        /*!
         * Destructor
         */
        ~IContextApplication() {}

    private:
        BlackMisc::CGenericDBusInterface *m_dBusInterface;

        /*!
         * Relay connection signals to local signals
         * No idea why this has to be wired and is not done automatically
         * \param connection
         */
        void relaySignals(const QString &serviceName, QDBusConnection &connection);

    protected:
        /*!
         * \brief IContextApplication
         * \param parent
         */
        IContextApplication(QObject *parent = nullptr) : QObject(parent), m_dBusInterface(nullptr) {}

    signals:
        //! \brief Status message
        void statusMessage(const BlackMisc::CStatusMessage &message);

        //! Widget GUI is about to start
        void widgetGuiStarting() const;

        //! Widget GUI is about to terminate
        void widgetGuiTerminating() const;

    public slots:

        /*!
         * \brief Ping
         * \param token
         * \return
         */
        virtual qint64 ping(qint64 token) const;

    };
}

#endif // guard
