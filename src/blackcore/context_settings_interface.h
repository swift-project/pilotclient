/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_CONTEXTSETTINGS_INTERFACE_H
#define BLACKCORE_CONTEXTSETTINGS_INTERFACE_H

#include "blackmisc/statusmessagelist.h"
#include "blackmisc/genericdbusinterface.h"
#include "blackmisc/settingutilities.h"
#include "blackmisc/setnetwork.h"
#include <QObject>
#include <QVariant>
#include <QDBusAbstractInterface>

#define BLACKCORE_CONTEXTSETTINGS_INTERFACENAME "blackcore.contextsettings"
#define BLACKCORE_CONTEXTSETTINGS_SERVICEPATH "/settings"

// SERVICENAME must contain at least one ".", otherwise generation fails
// as this is interpreted in the way comain.somename

namespace BlackCore
{

    /*!
     * \brief The interface context settings
     */
    class IContextSettings : public QObject
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTSETTINGS_INTERFACENAME)

    public:

        /*!
         * \brief Service name
         */
        static const QString &InterfaceName()
        {
            static QString s(BLACKCORE_CONTEXTSETTINGS_INTERFACENAME);
            return s;
        }

        /*!
         * \brief Service path
         */
        static const QString &ServicePath()
        {
            static QString s(BLACKCORE_CONTEXTSETTINGS_SERVICEPATH);
            return s;
        }

        /*!
         * \brief Path for network settings
         * \remark no to be confused with DBus paths
         */
        static const QString &PathNetworkSettings()
        {
            static QString s("network");
            return s;
        }

        /*!
         * \brief DBus version constructor
         */
        IContextSettings(const QString &serviceName, QDBusConnection &connection, QObject *parent = nullptr);

        /*!
         * Destructor
         */
        ~IContextSettings() {}

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
         * \brief IContextSettings
         * \param parent
         */
        IContextSettings(QObject *parent = 0) : QObject(parent), m_dBusInterface(0) {}

    signals:

        /*!
         * \brief Settings have been changed
         */
        void changedNetworkSettings();

    public slots:

        /*!
         * \brief Network settings
         */
        virtual BlackMisc::Settings::CSettingsNetwork getNetworkSettings() const;

        /*!
         * \brief Handle value
         * \param path      where value belongs to
         * \param command   what to do with value
         * \param value
         * \return          messages generated during handling
         */
        virtual BlackMisc::CStatusMessageList value(const QString &path, const QString &command, const QVariant &value);

        /*!
         * \brief DBus version of value method.
         * \remarks Basically an unwanted signature as this is different from the "local" signature and
         * contains explicit DBus types (a: QDbusArgument, b: type for conversion).
         * If this can be removed, fine.
         *
         */
        virtual BlackMisc::CStatusMessageList value(const QString &path, const QString &command, QDBusVariant value, int unifiedBlackMetaType);
    };
}

#endif // guard
