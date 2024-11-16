// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_PLUGIN_MANAGER_H
#define SWIFT_CORE_PLUGIN_MANAGER_H

#include <QJsonObject>
#include <QMap>
#include <QObject>
#include <QString>

#include "core/swiftcoreexport.h"
#include "misc/sequence.h"

namespace swift::core
{
    /*!
     * Base class for all contexts that provide plugin support.
     * It is responsible for locating, validating and loading plugins.
     */
    class SWIFT_CORE_EXPORT IPluginManager : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor
        IPluginManager(QObject *parent = nullptr);

        //! Looks for all available plugins
        virtual void collectPlugins();

        //! If the plugin specifies its config plugin, its identifier can be
        //! obtained using this method. You can get the plugin config instance
        //! later, using `getPluginById()`.
        QString getPluginConfigId(const QString &identifier);

        //! Loads the given plugin (if necessary), casts it to the desired
        //! type and returns its instance. Returns `nullptr` on failure.
        template <class T>
        T *getPluginById(const QString &identifier)
        {
            return qobject_cast<T *>(getPluginByIdImpl(identifier));
        }

    protected:
        //! Returns the list of valid IIDs for the implementation
        virtual swift::misc::CSequence<QString> acceptedIids() const = 0;

        //! Where to look for plugins, absolute path.
        //! Default implementation returns `plugins` in the application dir.
        virtual const QString &pluginDirectory() const;

        //! Defines whether the given plugin is valid or not, based on its metadata.
        //! The default implementation checks if all values common for all plugins
        //! (i.e. MetaData and identifier) are present and valid. It also checks
        //! if each plugin meets one of the specified in \ref acceptedIids() IIDs.
        //! Override custom plugin validation in the subclass if needed.
        virtual bool isValid(const QJsonObject &metadata) const;

        //! Gets the plugin identifier from the metadata.
        QString pluginIdentifier(const QJsonObject &metadata) const;

        //! Gets plugin identifier by its instance
        QString getIdByPlugin(const QObject *instance) const;

        //! Gets direct access to all plugins' metadata
        const swift::misc::CSequence<QJsonObject> &getPlugins() const
        {
            return m_metadata;
        }

    private:
        //! Tries to load the given plugin.
        bool tryLoad(const QString &path);

        //! Loads the given plugin (if necessary) and returns its instance.
        //! Returns `nullptr` on failure.
        QObject *getPluginByIdImpl(const QString &identifier);

        swift::misc::CSequence<QJsonObject> m_metadata; //!< plugin metadata
        QMap<QString, QString> m_paths; //!< identifier <-> file path pairs
        QMap<QString, QObject *> m_instances; //!< identifier <-> instance pairs
        QMap<QString, QString> m_configs; //!< identifier <-> identifier pairs
        QMap<const QObject *, QString> m_instanceIds; //!< instance <-> identifier pairs
    };
} // namespace swift::core

#endif // guard
