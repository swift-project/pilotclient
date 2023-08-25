// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackcore/application.h"
#include "blackcore/pluginmanagerweatherdata.h"
#include "blackcore/weatherdata.h"
#include "blackmisc/swiftdirectories.h"
#include "blackmisc/directoryutils.h"

#include <QJsonObject>
#include <QJsonValue>
#include <QStringBuilder>

namespace BlackCore
{
    using namespace BlackMisc;
    using namespace BlackMisc::Weather;

    CPluginManagerWeatherData::CPluginManagerWeatherData(QObject *parent) : IPluginManager(parent)
    {}

    IWeatherDataFactory *CPluginManagerWeatherData::getFactory(const QString &pluginId)
    {
        return getPluginById<IWeatherDataFactory>(pluginId);
    }

    Weather::CWeatherDataPluginInfoList CPluginManagerWeatherData::getAvailableWeatherDataPlugins() const
    {
        CWeatherDataPluginInfoList list;
        for (const auto &i : m_plugins)
        {
            list.push_back(i.info);
        }
        return list;
    }

    void CPluginManagerWeatherData::collectPlugins()
    {
        IPluginManager::collectPlugins();

        const CSequence<QJsonObject> &plugins = getPlugins();
        for (const QJsonObject &json : plugins)
        {
            QString iid = json["IID"].toString();
            if (iid == QStringLiteral("org.swift-project.blackcore.weatherdata"))
            {
                auto it = m_plugins.insert(pluginIdentifier(json), {});
                it->info.convertFromJson(json);
            }
        }
    }

    BlackMisc::CSequence<QString> CPluginManagerWeatherData::acceptedIids() const
    {
        return {
            QStringLiteral("org.swift-project.blackcore.weatherdata")
        };
    }

    const QString &CPluginManagerWeatherData::pluginDirectory() const
    {
        static const QString d(CFileUtils::appendFilePaths(CSwiftDirectories::pluginsDirectory(), "weatherdata"));
        return d;
    }
}
