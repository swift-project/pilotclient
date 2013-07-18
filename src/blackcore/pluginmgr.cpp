/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackcore/pluginmgr.h"
#include "blackmisc/plugins.h"
#include "blackmisc/context.h"
#include <QDirIterator>
#include <iostream>
#include <stdexcept>

namespace BlackCore
{

    CPluginManager::~CPluginManager()
    {
        for (QVector<PluginEntry>::iterator it = m_plugins.begin(); it != m_plugins.end(); ++it)
        {
            Q_ASSERT_X(it->loader, "Plugin manager", "Previously loaded plugin's loader ptr is null");

            it->loader->unload();
        }
    }

    void CPluginManager::loadAllPlugins(const QString &pluginsPath)
    {
        //TODO should we use a custom extension, so we don't attempt to load non-plugin .dll/.so files?
        #ifdef Q_OS_WIN
            static const QStringList pluginsFilter("*.dll");
        #else
            static const QStringList pluginsFilter("*.so");
        #endif

        QDirIterator iter (pluginsPath, pluginsFilter, QDir::Files);
        while (iter.hasNext())
        {
            QString filename = iter.next();

            QSharedPointer<QPluginLoader> loader (new QPluginLoader (filename));
            try
            {
                if (! loader->load())
                {
                    throw std::runtime_error(QString("Failed loading plugin from %1").arg(filename).toStdString());
                }

                PluginEntry entry;
                entry.loader = loader;
                entry.factory = qobject_cast<BlackMisc::IPluginFactory*>(loader->instance());

                if (! entry.factory)
                {
                    throw std::runtime_error(QString("Plugin loaded from %1 is not compatible").arg(filename).toStdString());
                }

                m_plugins.push_back(entry);
            }
            catch (...)
            {
                //TODO warning?
            }
        }
    }

    const CPluginManager::PluginEntry &CPluginManager::getEntry(size_t index) const
    {
        Q_ASSERT_X(index < getPluginCount(), "Plugin manager", "Plugin index out of bounds");

        const PluginEntry &entry = m_plugins[index];

        Q_ASSERT_X(entry.factory, "Plugin manager", "Previously loaded plugin's factory ptr is null");
        Q_ASSERT_X(entry.loader, "Plugin manager", "Previously loaded plugin's loader ptr is null");

        if (! entry.loader->isLoaded())
        {
            //TODO throw plugin loader unexpectedly unloaded
        }

        return entry;
    }

    const QString CPluginManager::getName(size_t index) const
    {
        return getFactory(index)->getName();
    }

    const QString CPluginManager::getDescription(size_t index) const
    {
        return getFactory(index)->getDescription();
    }

    BlackMisc::IPlugin *CPluginManager::constructPlugin(size_t index)
    {
        BlackMisc::IPlugin *plugin = getFactory(index)->create(BlackMisc::IContext::getInstance());
        if (! plugin->isValid())
        {
            delete plugin;
            plugin = 0;
            //TODO throw plugin construction failed
        }
        return plugin;
    }

} // namespace BlackCore