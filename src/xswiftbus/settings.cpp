/* Copyright (C) 2019
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/simulation/settings/xswiftbussettingsqtfree.inc"
#include "settings.h"
#include <string>

using namespace BlackMisc::Simulation::Settings;

namespace XSwiftBus
{
    CSettings::CSettings() : CXSwiftBusSettingsQtFree()
    {
        // void
    }

    CSettings CSettingsProvider::getSettings() const
    {
        std::lock_guard<std::mutex> l(m_settingsMutex);
        return m_pluginSettings;
    }

    void CSettingsProvider::setSettings(const CSettings &settings)
    {
        std::lock_guard<std::mutex> l(m_settingsMutex);
        m_pluginSettings = settings;
    }

    CSettingsAware::CSettingsAware(CSettingsProvider *provider) : m_provider(provider)
    {
        // void
    }

    CSettings CSettingsAware::getSettings() const
    {
        return m_provider->getSettings();
    }

    void CSettingsAware::setSettings(const CSettings &settings)
    {
        m_provider->setSettings(settings);
    }

    const CConfig &CSettingsAware::getConfig() const
    {
        return m_provider->getConfig();
    }

    bool CSettingsAware::writeConfig(bool tcas, bool debug)
    {
        return m_provider->writeConfig(tcas, debug);
    }

} // ns
