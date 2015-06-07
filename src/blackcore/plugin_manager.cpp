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

namespace BlackCore
{

    IPluginManager::IPluginManager(QObject *parent) : QObject(parent)
    {

    }

    void IPluginManager::collectPlugins()
    {
        QDir pluginDir(pluginDirectory());
        if (!pluginDir.exists())
        {
            CLogMessage(this).warning("No such directory: %1") << pluginDir.path();
            return;
        }

        QDirIterator it(pluginDir, QDirIterator::FollowSymlinks);
        while (it.hasNext())
        {
            if (!QLibrary::isLibrary(it.next()))
            {
                continue;
            }

            CLogMessage(this).debug() << "Loading plugin: " << it.filePath();
            QPluginLoader loader(it.filePath());
            QJsonObject json = loader.metaData();
            if (!isValid(json))
            {
                CLogMessage(this).warning("Plugin %1 invalid, not loading it") << it.filePath();
                continue;
            }

            QString identifier = pluginIdentifier(json);
            m_paths.insert(identifier, it.filePath());
            m_metadatas.push_back(json);
        }
    }

    QString IPluginManager::pluginDirectory() const
    {
        return qApp->applicationDirPath() % QStringLiteral("/plugins");
    }

    bool IPluginManager::isValid(const QJsonObject &metadata) const
    {
        if (!metadata.contains("IID") || !metadata["IID"].isString())
        {
            return false;
        }

        if (!metadata["MetaData"].isObject())
        {
            return false;
        }

        QJsonObject data = metadata["MetaData"].toObject();
        if (!data.contains("identifier") || !data["identifier"].isString())
        {
            return false;
        }

        auto iids = acceptedIids();
        for (const QString &iid : iids)
        {
            if (metadata["IID"].toString() == iid)
            {
                return true;
            }
        }

        return false;
    }

    QString IPluginManager::pluginIdentifier(const QJsonObject &metadata) const
    {
        Q_ASSERT(isValid(metadata));
        return metadata.value("MetaData").toObject().value("identifier").toString();
    }

    QString IPluginManager::getIdByPlugin(const QObject *instance) const
    {
        return m_instanceIds.value(instance, QString());
    }

    QObject *IPluginManager::getPluginByIdImpl(const QString &identifier)
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
            m_instanceIds.insert(instance, identifier);
            return instance;
        }
        else
        {
            CLogMessage(this).error(loader.errorString());
            return nullptr;
        }
    }

} // namespace
