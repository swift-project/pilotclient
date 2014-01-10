/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_CONTEXTSETTINGS_H
#define BLACKCORE_CONTEXTSETTINGS_H

#include "blackcore/dbus_server.h"
#include "blackcore/context_settings_interface.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/statusmessagelist.h"
#include "blackcore/coreruntime.h"
#include <QObject>

#define BLACKCORE_CONTEXTSETTINGS_INTERFACENAME "blackcore.contextsettings"

namespace BlackCore
{
    /*!
     * \brief Network context
     */
    class CContextSettings : public IContextSettings
    {
        // Register by same name, make signals sender independent
        // http://dbus.freedesktop.org/doc/dbus-faq.html#idp48032144
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTSETTINGS_INTERFACENAME)
        Q_OBJECT

    public:

        /*!
         * Context
         * \param parent
         */
        CContextSettings(CCoreRuntime *parent);

        /*!
         * Destructor
         */
        virtual ~CContextSettings() {}

        /*!
         * \brief Register myself in DBus
         * \param server
         */
        void registerWithDBus(CDBusServer *server)
        {
            server->addObject(IContextSettings::ServicePath(), this);
        }

        /*!
         * \brief Runtime
         * \return
         */
        const CCoreRuntime *getRuntime() const
        {
            return static_cast<CCoreRuntime *>(this->parent());
        }

    public slots:
        /*!
         * \brief Network settings
         * \return
         */
        virtual BlackMisc::Settings::CSettingsNetwork getNetworkSettings() const;

        /*!
         * \brief Validate
         * \param path
         * \param value
         * \return
         */
        virtual BlackMisc::CStatusMessageList value(const QString &path, const QString &command, const QVariant &value);

    private:
        BlackMisc::Settings::CSettingsNetwork m_settingsNetwork;

    };
}

#endif // guard
