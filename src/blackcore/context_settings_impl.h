/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_CONTEXTSETTINGS_IMPL_H
#define BLACKCORE_CONTEXTSETTINGS_IMPL_H

#include "context_settings.h"
#include "dbus_server.h"
#include "coreruntime.h"

#include "blackmisc/setnetwork.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/hwkeyboardkeylist.h"

namespace BlackCore
{

    /*!
     * \brief Settings context implementation
     */
    class CContextSettings : public IContextSettings
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTSETTINGS_INTERFACENAME)

    public:

        //! \brief Constructor
        CContextSettings(QObject *runtime = nullptr);

        //! Destructor
        virtual ~CContextSettings() {}

        //! \brief Register myself in DBus
        void registerWithDBus(CDBusServer *server)
        {
            server->addObject(IContextSettings::ObjectPath(), this);
        }

        //! \brief Runtime
        CCoreRuntime *getRuntime()
        {
            return static_cast<CCoreRuntime *>(this->parent());
        }

        //! \brief Runtime
        const CCoreRuntime *getRuntime() const
        {
            return static_cast<CCoreRuntime *>(this->parent());
        }

    public slots:
        //! \copydoc IContextSettings::getNetworkSettings()
        virtual BlackMisc::Settings::CSettingsNetwork getNetworkSettings() const override;

        //! \copydoc IContextSettings::getHotkeys()
        virtual BlackMisc::Hardware::CKeyboardKeyList getHotkeys() const override;

        //! \copydoc IContextSettings::value()
        virtual BlackMisc::CStatusMessageList value(const QString &path, const QString &command, const QVariant &value) override;

    private:
        BlackMisc::Settings::CSettingsNetwork m_settingsNetwork;
        BlackMisc::Hardware::CKeyboardKeyList m_hotkeys;
    };
}

#endif // guard
