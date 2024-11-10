// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef SWIFT_MISC_XSWIFTBUS_CSETTINGS_H
#define SWIFT_MISC_XSWIFTBUS_CSETTINGS_H

#include "misc/simulation/settings/xswiftbussettingsqtfree.h"
#include "config.h"
#include <string>
#include <mutex>

namespace XSwiftBus
{
    /*!
     * xswiftbus/swift side settings class, JSON capable, shared among all services
     */
    class CSettings final : public swift::misc::simulation::settings::CXSwiftBusSettingsQtFree
    {
    public:
        //! Constructor.
        CSettings();

    protected:
        //! \copydoc swift::misc::simulation::settings::CXSwiftBusSettingsQtFree::objectUpdated
        virtual void objectUpdated() override final {}
    };

    //! Something owning the settings
    class CSettingsProvider
    {
    public:
        //! Get settings
        //! \threadsafe
        //! \fixme Does this really need to be threadsafe?
        CSettings getSettings() const;

        //! Set settings
        //! \threadsafe
        //! \fixme Does this really need to be threadsafe?
        void setSettings(const CSettings &settings);

        //! Get settings from xswiftbus.conf (needed during plugin initialization)
        virtual const CConfig &getConfig() const = 0;

        //! Write a config file with these new values
        virtual bool writeConfig(bool tcas, bool debug) = 0;

    protected:
        //! Destructor
        ~CSettingsProvider() = default;

    private:
        mutable std::mutex m_settingsMutex;
        CSettings m_pluginSettings; //!< owner of the settings
    };

    //! Something having access to the settings
    class CSettingsAware
    {
    protected:
        //! Constructor
        CSettingsAware(CSettingsProvider *provider);

        //! Destructor
        ~CSettingsAware() = default;

        //! \copydoc CSettingsProvider::getSettings
        CSettings getSettings() const;

        //! \copydoc CSettingsProvider::setSettings
        void setSettings(const CSettings &settings);

        //! \copydoc CSettingsProvider::getConfig
        const CConfig &getConfig() const;

        //! \copydoc CSettingsProvider::writeConfig
        bool writeConfig(bool tcas, bool debug);

    private:
        CSettingsProvider *m_provider = nullptr;
    };
} // ns

#endif
