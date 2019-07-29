/* Copyright (C) 2019
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

// include whole code in this translation unit, as this code is used on swift/and XSWiftBus side
// BLACKMISC_EXPORT cannot be used
#include "blackmisc/simulation/settings/xswiftbussettingsqtfree.cpp"

#include "settings.h"
#include <string>

using namespace BlackMisc::Simulation::Settings;

namespace XSwiftBus
{
    CSettings::CSettings() : CXSwiftBusSettingsQtFree()
    {
        // void
    }

    CSettings::CSettings(const std::string &json) : CXSwiftBusSettingsQtFree(json)
    {
        // void
    }

    CSettings ISettingsProvider::getSettings() const
    {
        std::lock_guard<std::mutex> l(m_settingsMutex);
        return m_pluginSettings;
    }

    void ISettingsProvider::setSettings(const CSettings &settings)
    {
        std::lock_guard<std::mutex> l(m_settingsMutex);
        m_pluginSettings = settings;
    }

} // ns

