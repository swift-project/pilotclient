/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_CONTEXTSETTINGS_H
#define BLACKCORE_CONTEXTSETTINGS_H

#include "blackcore/context.h"
#include "blackcore/dbus_server.h"
#include "blackcore/keyboard.h"
#include "blackmisc/hwkeyboardkeylist.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/settingutilities.h"
#include "blackmisc/setnetwork.h"
#include "blackmisc/dbus.h"
#include <QObject>
#include <QVariant>

#define BLACKCORE_CONTEXTSETTINGS_INTERFACENAME "net.vatsim.PilotClient.BlackCore.ContextSettings"
#define BLACKCORE_CONTEXTSETTINGS_OBJECTPATH "/Settings"

namespace BlackCore
{
    /*!
     * Context settings interface
     */
    class IContextSettings : public CContext
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTSETTINGS_INTERFACENAME)

    public:
        //! Settings type
        enum SettingsType
        {
            SettingsHotKeys,
            SettingsNetwork
        };

    protected:
        //! DBus version constructor
        IContextSettings(CRuntimeConfig::ContextMode mode, CRuntime *runtime = nullptr) : CContext(mode, runtime)
        {}

    public:

        //! Service name
        static const QString &InterfaceName()
        {
            static QString s(BLACKCORE_CONTEXTSETTINGS_INTERFACENAME);
            return s;
        }

        //! Service path
        static const QString &ObjectPath()
        {
            static QString s(BLACKCORE_CONTEXTSETTINGS_OBJECTPATH);
            return s;
        }

        /*!
         * \brief Path for network settings
         * \remarks no to be confused with DBus paths
         */
        static const QString &PathNetworkSettings()
        {
            static QString s("network");
            return s;
        }

        /*!
         * \brief Path for network settings
         * \remarks no to be confused with DBus paths
         */
        static const QString &PathRoot()
        {
            static QString s("root");
            return s;
        }

        /*!
         * \brief Path for hotkeys
         * \remarks no to be confused with DBus paths
         */
        static const QString &PathHotkeys()
        {
            static QString s("hotkeys");
            return s;
        }

        //! Destructor
        virtual ~IContextSettings() {}

        /*!
         * Handle value
         * \param path      where value belongs to
         * \param command   what to do with value
         * \param value
         * \return          messages generated during handling
         * \remarks Do not make this a slot, no DBus XML signature shall be created. The QVariant
         *  will be send a tailored value method using QDBusVariant
         *  @see value(const QString &, const QString &, QDBusVariant, int)
         */
        virtual BlackMisc::CStatusMessageList value(const QString &path, const QString &command, const QVariant &value) = 0;

    signals:
        //! Settings have been changed
        void changedSettings(uint type);

    public slots:

        //! Network settings
        virtual BlackMisc::Settings::CSettingsNetwork getNetworkSettings() const = 0;

        //! Hotkeys
        virtual BlackMisc::Hardware::CKeyboardKeyList getHotkeys() const = 0;

        //! Save settings
        virtual BlackMisc::CStatusMessage write() const = 0;

        //! Read settings
        virtual BlackMisc::CStatusMessage read() = 0;

        //! Reset settings
        virtual BlackMisc::CStatusMessage reset(bool write = true) = 0;

        //! Read settings
        virtual QString getSettingsFileName() const = 0;

        //! Settings as JSON string
        virtual QString getSettingsAsJsonString() const = 0;
    };
}

#endif // guard
