/* Copyright (C) 2019
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#ifndef BLACKMISC_XSWIFTBUS_CSETTINGS_H
#define BLACKMISC_XSWIFTBUS_CSETTINGS_H

#include "blackmisc/simulation/settings/xswiftbussettingsqtfree.h"
#include "config.h"
#include <string>
#include <mutex>

namespace XSwiftBus
{
    /*!
     * XSwiftBus/swift side settings class, JSON capable, shared among all services
     */
    class CSettings final : public BlackMisc::Simulation::Settings::CXSwiftBusSettingsQtFree
    {
    public:
        //! Constructor.
        CSettings();

    protected:
        //! \copydoc BlackMisc::Simulation::Settings::CXSwiftBusSettingsQtFree::objectUpdated
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
