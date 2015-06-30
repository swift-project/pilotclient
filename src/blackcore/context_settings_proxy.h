/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_CONTEXTSETTINGS_PROXY_H
#define BLACKCORE_CONTEXTSETTINGS_PROXY_H

#include "blackcoreexport.h"
#include "context_settings.h"

#include "blackmisc/statusmessagelist.h"
#include "blackmisc/genericdbusinterface.h"
#include "blackmisc/settingutilities.h"
#include "blackmisc/setnetwork.h"
#include "blackmisc/hardware/keyboardkeylist.h"

namespace BlackCore
{

    //! \brief Settings context proxy
    //! \ingroup dbus
    class BLACKCORE_EXPORT CContextSettingsProxy : public IContextSettings
    {
        Q_OBJECT
        friend class IContextSettings;

    public:
        //! Destructor
        virtual ~CContextSettingsProxy() {}

    private:
        BlackMisc::CGenericDBusInterface *m_dBusInterface;

        //! Relay connection signals to local signals
        //! No idea why this has to be wired and is not done automatically
        void relaySignals(const QString &serviceName, QDBusConnection &connection);

    protected:
        //! DBus version constructor
        CContextSettingsProxy(const QString &serviceName, QDBusConnection &connection, CRuntimeConfig::ContextMode mode, CRuntime *runtime);

        //! Constructor
        CContextSettingsProxy(CRuntimeConfig::ContextMode mode, CRuntime *runtime) : IContextSettings(mode, runtime), m_dBusInterface(nullptr) {}

    public slots:
        //! \copydoc IContextSettings::getNetworkSettings()
        virtual BlackMisc::Settings::CSettingsNetwork getNetworkSettings() const override;

        //! \copydoc IContextSettings::getAudioSettings()
        virtual BlackMisc::Settings::CSettingsAudio getAudioSettings() const override;

        //! \copydoc IContextSettings::getHotkeys()
        virtual BlackMisc::Settings::CSettingKeyboardHotkeyList getHotkeys() const override;

        //! \copydoc IContextSettings::value
        virtual BlackMisc::CStatusMessageList value(const QString &path, const QString &command, const BlackMisc::CVariant &value) override;

        //! \copydoc IContextSettings::write
        BlackMisc::CStatusMessage write() const override;

        //! read settings
        virtual BlackMisc::CStatusMessage read() override;

        //! \copydoc IContextSettings::reset
        virtual BlackMisc::CStatusMessage reset(bool write = true) override;

        //! settings file name
        virtual QString getSettingsFileName() const override;

        //! as JSON string
        virtual QString getSettingsAsJsonString() const override;

    };
}

#endif // guard
