// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackcore/application.h"
#include "blackcore/pluginmanagersimulator.h"
#include "blackcore/simulator.h"
#include "misc/swiftdirectories.h"
#include "misc/directoryutils.h"
#include "misc/logmessage.h"

#include <QJsonObject>
#include <QJsonValue>
#include <QStringBuilder>
#include <QtGlobal>

namespace BlackCore
{
    using namespace swift::misc;
    using namespace swift::misc::simulation;

    CPluginManagerSimulator::CPluginManagerSimulator(QObject *parent) : IPluginManager(parent)
    {
    }

    ISimulatorFactory *CPluginManagerSimulator::getFactory(const QString &pluginId)
    {
        return getPluginById<ISimulatorFactory>(pluginId);
    }

    ISimulatorListener *CPluginManagerSimulator::createListener(const QString &pluginId)
    {
        if (!m_plugins.contains(pluginId)) { return nullptr; }
        PluginExtended &plugin = m_plugins[pluginId];
        if (!plugin.listener)
        {
            ISimulatorFactory *factory = this->getFactory(pluginId);
            if (!factory)
            {
                CLogMessage(this).warning(u"Could not load plugin '%1'.") << pluginId;
                m_plugins.remove(pluginId);
                return nullptr;
            }

            ISimulatorListener *listener = factory->createListener(plugin.info);
            connect(qApp, &QCoreApplication::aboutToQuit, listener, &QObject::deleteLater);
            plugin.listener = listener;
        }

        return plugin.listener;
    }

    ISimulatorListener *CPluginManagerSimulator::getListener(const QString &pluginId)
    {
        if (!m_plugins.contains(pluginId)) { return nullptr; }
        PluginExtended &plugin = m_plugins[pluginId];
        return plugin.listener;
    }

    CSimulatorPluginInfoList CPluginManagerSimulator::getAvailableSimulatorPlugins() const
    {
        CSimulatorPluginInfoList list;
        for (const auto &i : m_plugins)
        {
            list.push_back(i.info);
        }
        return list;
    }

    int CPluginManagerSimulator::checkAvailableListeners()
    {
        if (m_plugins.isEmpty()) { return 0; }

        int c = 0;
        for (PluginExtended &pi : m_plugins)
        {
            if (!pi.listener) { continue; }
            if (!pi.listener->isRunning()) { continue; }
            pi.listener->check();
            c++;
        }
        return c;
    }

    void CPluginManagerSimulator::collectPlugins()
    {
        IPluginManager::collectPlugins();

        const CSequence<QJsonObject> &plugins = getPlugins();
        for (const QJsonObject &json : plugins)
        {
            const QString iid = json["IID"].toString();
            if (iid == QStringLiteral("org.swift-project.blackcore.simulatorinterface"))
            {
                auto it = m_plugins.insert(pluginIdentifier(json), {});
                it->info.convertFromJson(json);
            }
        }
    }

    CSequence<QString> CPluginManagerSimulator::acceptedIids() const
    {
        return {
            QStringLiteral("org.swift-project.blackcore.simulatorinterface"),
            QStringLiteral("org.swift-project.blackgui.pluginconfiginterface")
        };
    }

    const QString &CPluginManagerSimulator::pluginDirectory() const
    {
        static const QString d(CFileUtils::appendFilePaths(CSwiftDirectories::pluginsDirectory(), "simulator"));
        return d;
    }
}
