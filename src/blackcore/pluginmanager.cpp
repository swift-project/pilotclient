// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackcore/pluginmanager.h"
#include "blackcore/application.h"
#include "misc/swiftdirectories.h"
#include "misc/directoryutils.h"
#include "misc/logmessage.h"
#include "misc/statusmessage.h"

#include <QDir>
#include <QDirIterator>
#include <QJsonValue>
#include <QJsonValueRef>
#include <QLibrary>
#include <QPluginLoader>
#include <QStringBuilder>
#include <QtGlobal>

using namespace swift::misc;

namespace BlackCore
{
    IPluginManager::IPluginManager(QObject *parent) : QObject(parent)
    {}

    void IPluginManager::collectPlugins()
    {
        QDir pluginDir(pluginDirectory());
        if (!pluginDir.exists())
        {
            CLogMessage(this).warning(u"No such directory: %1") << pluginDir.path();
            return;
        }

        QDirIterator it(pluginDir, QDirIterator::FollowSymlinks);
        while (it.hasNext())
        {
            tryLoad(it.next());
        }
    }

    QString IPluginManager::getPluginConfigId(const QString &identifier)
    {
        return m_configs.contains(identifier) ? m_configs.value(identifier) : QString();
    }

    const QString &IPluginManager::pluginDirectory() const
    {
        return CSwiftDirectories::pluginsDirectory();
    }

    bool IPluginManager::isValid(const QJsonObject &metadata) const
    {
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

    bool IPluginManager::tryLoad(const QString &path)
    {
        if (!QLibrary::isLibrary(path)) { return false; }

        CLogMessage(this).debug() << "Try loading plugin:" << path;
        QPluginLoader loader(path);
        const QJsonObject json = loader.metaData();
        if (!isValid(json))
        {
            CLogMessage(this).warning(u"Plugin '%1' invalid, not loading it") << path;
            return false;
        }

        const QString identifier = pluginIdentifier(json);
        m_paths.insert(identifier, path);
        m_metadata.push_back(json);

        if (json.value("MetaData").toObject().contains("config"))
        {
            const QString configId = json.value("MetaData").toObject().value("config").toString();
            if (!configId.isEmpty())
            {
                m_configs.insert(identifier, configId);
            }
        }

        return true;
    }

    QObject *IPluginManager::getPluginByIdImpl(const QString &identifier)
    {
        if (m_instances.contains(identifier))
        {
            return m_instances.value(identifier);
        }

        if (!m_paths.contains(identifier))
        {
            CLogMessage(this).warning(u"Plugin with id '%1' does not exist") << identifier;
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
