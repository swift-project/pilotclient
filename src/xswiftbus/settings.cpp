// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "settings.h"

#include <string>

#include "misc/simulation/settings/xswiftbussettingsqtfree.inc"

using namespace swift::misc::simulation::settings;

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

} // namespace XSwiftBus
