/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_CONTEXTSETTINGS_IMPL_H
#define BLACKCORE_CONTEXTSETTINGS_IMPL_H

#include "context_settings.h"
#include "dbus_server.h"
#include "context_runtime.h"

#include "blackcore/settingsallclasses.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/hwkeyboardkeylist.h"

namespace BlackCore
{

    /*!
     * Settings context implementation
     */
    class CContextSettings : public IContextSettings
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTSETTINGS_INTERFACENAME)
        friend class CRuntime;
        friend class IContextSettings;

    protected:
        //! Constructor
        CContextSettings(CRuntimeConfig::ContextMode mode, CRuntime *runtime = nullptr);

        //! Register myself in DBus
        CContextSettings *registerWithDBus(CDBusServer *server)
        {
            if (!server || this->m_mode != CRuntimeConfig::LocalInDbusServer) return this;
            server->addObject(IContextSettings::ObjectPath(), this);
            return this;
        }

    public:
        //! Destructor
        virtual ~CContextSettings() {}

        //! \copydoc IContextSettings::value()
        virtual BlackMisc::CStatusMessageList value(const QString &path, const QString &command, const BlackMisc::CVariant &value) override;

    public slots:
        //! \copydoc IContextSettings::getNetworkSettings()
        virtual BlackMisc::Settings::CSettingsNetwork getNetworkSettings() const override;

        //! \copydoc IContextSettings::getAudioSettings()
        virtual BlackMisc::Settings::CSettingsAudio getAudioSettings() const override;

        //! \copydoc IContextSettings::getSimulatorSettings()
        virtual BlackMisc::Simulation::Settings::CSettingsSimulator getSimulatorSettings() const override;

        //! \copydoc IContextSettings::getHotkeys()
        virtual BlackMisc::Settings::CSettingKeyboardHotkeyList getHotkeys() const override;

        //! read settings
        virtual BlackMisc::CStatusMessage read() override;

        //! \copydoc IContextSettings::write
        virtual BlackMisc::CStatusMessage write() const override;

        //! \copydoc IContextSettings::reset
        virtual BlackMisc::CStatusMessage reset(bool write = true) override;

        //! Settings file name
        virtual QString getSettingsFileName() const override { return BlackMisc::Settings::CSettingUtilities::getSettingsFile(); }

        //! JSON represenation
        virtual QString getSettingsAsJsonString() const override;

    private:
        //! settings directory
        const QString &getSettingsDirectory() const { return BlackMisc::Settings::CSettingUtilities::getSettingsDirectory(); }

        BlackMisc::Settings::CSettingsNetwork m_settingsNetwork;
        BlackMisc::Settings::CSettingsAudio m_settingsAudio;
        BlackMisc::Simulation::Settings::CSettingsSimulator m_settingsSimulator;
        BlackMisc::Settings::CSettingKeyboardHotkeyList m_hotkeys;
        QJsonDocument toJsonDocument() const;
        void emitCompletelyChanged();
    };
}

#endif // guard
