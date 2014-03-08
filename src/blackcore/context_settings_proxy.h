/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_CONTEXTSETTINGS_PROXY_H
#define BLACKCORE_CONTEXTSETTINGS_PROXY_H

#include "context_settings.h"

#include "blackmisc/statusmessagelist.h"
#include "blackmisc/genericdbusinterface.h"
#include "blackmisc/settingutilities.h"
#include "blackmisc/setnetwork.h"
#include "blackmisc/hwkeyboardkeylist.h"

#include <QVariant>

namespace BlackCore
{

    /*!
     * \brief Settings context proxy
     */
    class CContextSettingsProxy : public IContextSettings
    {
        Q_OBJECT

    public:

        /*!
         * \brief DBus version constructor
         */
        CContextSettingsProxy(const QString &serviceName, QDBusConnection &connection, QObject *parent = nullptr);

        /*!
         * Destructor
         */
        virtual ~CContextSettingsProxy() {}


        /*!
         * \brief Handle value
         * \param path      where value belongs to
         * \param command   what to do with value
         * \param value
         * \return          messages generated during handling
         * \remarks Do not make this a slot, no DBus XML signature shall be created. The QVariant
         *  will be send a tailored value method using QDBusVariant
         *  @see value(const QString &, const QString &, QDBusVariant, int)
         */
        virtual BlackMisc::CStatusMessageList value(const QString &path, const QString &command, const QVariant &value) override;

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
        CContextSettingsProxy(QObject *parent = nullptr) : IContextSettings(parent), m_dBusInterface(nullptr) {}

    public slots:

        //! \copydoc IContextSettings::getNetworkSettings()
        virtual BlackMisc::Settings::CSettingsNetwork getNetworkSettings() const override;

        //! \copydoc IContextSettings::getHotkeys()
        virtual BlackMisc::Hardware::CKeyboardKeyList getHotkeys() const override;

        /*!
         * \brief DBus version of value method.
         * \remarks Basically an unwanted signature as this is different from the "local" signature and
         * contains explicit DBus types (a: QDbusArgument, b: type for conversion).
         * If this can be removed, fine.
         */
        BlackMisc::CStatusMessageList value(const QString &path, const QString &command, QDBusVariant value, int unifiedBlackMetaType);
    };
}

#endif // guard
