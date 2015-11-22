/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "pluginmanagersimulator.h"
#include "simulator.h"
#include <QStringBuilder>

namespace BlackCore
{

    using namespace BlackMisc;
    using namespace BlackMisc::Simulation;

    CPluginManagerSimulator::CPluginManagerSimulator(QObject *parent) : IPluginManager(parent)
    {
    }

    CVariant CPluginManagerSimulator::getPluginData(const QObject *obj, const QString &key) const
    {
        const QObject *p = obj;
        while (p && !p->inherits("BlackCore::ISimulatorFactory"))
        {
            p = p->parent();
        }

        if (!p) return CVariant();

        QString id = getIdByPlugin(p);
        Q_ASSERT(!id.isEmpty());
        const PluginExtended &pe = m_plugins[id];
        return pe.storage.value(key);
    }

    void CPluginManagerSimulator::setPluginData(const QObject *obj, const QString &key, const CVariant &value)
    {
        const QObject *p = obj;
        while (p && !p->inherits("BlackCore::ISimulatorFactory"))
        {
            p = p->parent();
        }

        if (!p) return;

        QString id = getIdByPlugin(p);
        Q_ASSERT(!id.isEmpty());
        PluginExtended &pe = m_plugins[id];
        pe.storage.insert(key, value);
    }

    ISimulatorFactory *CPluginManagerSimulator::getFactory(const QString &pluginId)
    {
        return getPluginById<ISimulatorFactory>(pluginId);
    }

    ISimulatorListener *CPluginManagerSimulator::getListener(const QString &pluginId)
    {
        if (!m_plugins.contains(pluginId))
        {
            return nullptr;
        }

        PluginExtended &plugin = m_plugins[pluginId];
        if (!plugin.listener)
        {
            ISimulatorFactory *factory = getPluginById<ISimulatorFactory>(pluginId);
            if (!factory)
            {
                m_plugins.remove(pluginId);
                return nullptr;
            }

            ISimulatorListener *listener = factory->createListener(plugin.info);
            connect(qApp, &QCoreApplication::aboutToQuit, listener, &QObject::deleteLater);
            plugin.listener = listener;
        }

        return plugin.listener;
    }

    CSimulatorPluginInfoList CPluginManagerSimulator::getAvailableSimulatorPlugins() const
    {
        CSimulatorPluginInfoList list;
        for (const auto &i : m_plugins.values())
        {
            list.push_back(i.info);
        }
        return list;
    }

    void CPluginManagerSimulator::collectPlugins()
    {
        IPluginManager::collectPlugins();

        const CSequence<QJsonObject> &plugins = getPlugins();
        for (const QJsonObject &json : plugins)
        {
            QString iid = json["IID"].toString();
            if (iid == QStringLiteral("org.swift-project.blackcore.simulatorinterface")) {
                auto it = m_plugins.insert(pluginIdentifier(json), {});
                it->info.convertFromJson(json);
            }
        }
    }

    BlackMisc::CSequence<QString> CPluginManagerSimulator::acceptedIids() const
    {
        return {
            QStringLiteral("org.swift-project.blackcore.simulatorinterface"),
            QStringLiteral("org.swift-project.blackgui.pluginconfiginterface")
        };
    }

    QString CPluginManagerSimulator::pluginDirectory() const
    {
        return qApp->applicationDirPath() % QStringLiteral("/plugins/simulator");
    }

}
