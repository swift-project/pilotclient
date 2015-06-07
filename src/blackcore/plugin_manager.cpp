/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "plugin_manager.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/loghandler.h"
#include <QCoreApplication>
#include <QDir>
#include <QDirIterator>
#include <QLibrary>

using namespace BlackMisc;

namespace BlackCore {

    BlackMisc::CSequence<QJsonObject> CPluginManager::plugins(const QString &iid) const
    {
        return m_metadatas.values(iid);
    }

    QObject *CPluginManager::getPluginById(const QString &identifier)
    {
        if (m_instances.contains(identifier))
        {
            return m_instances.value(identifier);
        }

        if (!m_paths.contains(identifier))
        {
            CLogMessage(this).warning("Plugin with id %1 does not exist") << identifier;
            return nullptr;
        }

        QString path = m_paths.value(identifier);
        QPluginLoader loader(path);
        QObject *instance = loader.instance();
        if (instance)
        {
            m_instances.insert(identifier, instance);
            return instance;
        }
        else
        {
            CLogMessage(this).error(loader.errorString());
            return nullptr;
        }
    }

    CPluginManager *CPluginManager::getInstance()
    {
        static CPluginManager *instance = nullptr;
        if (!instance)
        {
            instance = new CPluginManager();
        }
        return instance;
    }

    CPluginManager::CPluginManager(QObject *parent) : QObject(parent)
    {
        collectPlugins();

        connect(qApp, &QCoreApplication::aboutToQuit, this, &CPluginManager::deleteLater);
    }

    void CPluginManager::collectPlugins()
    {
        QDir pluginDir(qApp->applicationDirPath().append("/plugins"));
        if (!pluginDir.exists())
        {
            CLogMessage(this).warning("No plugins directory: %1") << pluginDir.path();
            return;
        }

        QDirIterator it(pluginDir, QDirIterator::Subdirectories);
        while (it.hasNext())
        {
            if (!QLibrary::isLibrary(it.next()))
            {
                continue;
            }

            CLogMessage(this).debug() << "Loading plugin: " << it.filePath();
            QPluginLoader loader(it.filePath());
            QJsonObject json = loader.metaData();
            QString identifier = pluginIdentifier(json);
            if (identifier.isEmpty())
            {
                CLogMessage(this).warning("Plugin %1 invalid, not loading it") << it.filePath();
                continue;
            }

            m_paths.insert(identifier, it.filePath());
            m_metadatas.insert(json["IID"].toString(), json);
        }
    }

    QString CPluginManager::pluginIdentifier(const QJsonObject &metadata)
    {
        if (!metadata.contains("IID") || !metadata["IID"].isString())
        {
            return QString();
        }

        if (!metadata["MetaData"].isObject())
        {
            return QString();
        }

        QJsonObject data = metadata["MetaData"].toObject();
        if (!data.contains("identifier"))
        {
            return QString();
        }

        QJsonValue identifier = data["identifier"];
        if (!identifier.isString())
        {
            return QString();
        }

        return identifier.toString();
    }

} // namespace
