/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_PLUGIN_MANAGER_H
#define BLACKCORE_PLUGIN_MANAGER_H

#include "blackcoreexport.h"
#include "blackmisc/sequence.h"
#include <QObject>
#include <QMultiMap>
#include <QJsonObject>

namespace BlackCore
{

    /*!
     * Base class for all contexts that provide plugin support.
     * It is responsible for locating, validating and loading plugins.
     */
    class BLACKCORE_EXPORT IPluginManager : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor
        IPluginManager(QObject *parent = nullptr);

        //! Looks for all available plugins
        virtual void collectPlugins();

    protected:
        //! Returns the list of valid IIDs for the implementation
        virtual BlackMisc::CSequence<QString> acceptedIids() const = 0;

        //! Where to look for plugins, absolute path.
        //! Default implementation returns `plugins` in the application dir.
        virtual QString pluginDirectory() const;

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

        //! Loads the given plugin (if necessary), casts it to the desired
        //! type and returns its instance. Returns `nullptr` on failure.
        template <class T>
        T *getPluginById(const QString &identifier)
        {
            return qobject_cast<T *>(getPluginByIdImpl(identifier));
        }

        //! Gets direct access to all plugins' metadata
        const BlackMisc::CSequence<QJsonObject> &getPlugins()
        {
            return m_metadatas;
        }

    private:

        //! Loads the given plugin (if necessary) and returns its instance.
        //! Returns `nullptr` on failure.
        QObject *getPluginByIdImpl(const QString &identifier);

        BlackMisc::CSequence<QJsonObject> m_metadatas;
        QMap<QString, QString> m_paths; //!< identifier <-> file path pairs
        QMap<QString, QObject *> m_instances; //!< identifier <-> instance pairs
        QMap<const QObject *, QString> m_instanceIds; //!< instance <-> identifier pairs

    };
}

#endif // BLACKCORE_PLUGIN_MANAGER_H
