/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef CPLUGINMANAGER_H
#define CPLUGINMANAGER_H

#include "blackmisc/sequence.h"
#include <QObject>
#include <QMultiMap>
#include <QJsonObject>

namespace BlackCore
{

    /*!
     * \brief Manages all installed plugins, provides easy access to all of them.
     * Plugin loading works as follows:
     *  1. Collect all interesting plugins by their IID (\ref plugins());
     *      all plugins are guaranteed to be valid, i.e. they have valid IID and identifier.
     *  2. Load specified plugin (\ref getPluginById()).
     */
    class CPluginManager : public QObject
    {
        Q_OBJECT

    public:
        //! Retrieves plugin metadata list for the given IID.
        //! If no implementations are found, the empty list is returned.
        BlackMisc::CSequence<QJsonObject> plugins(const QString &iid) const;

        //! Loads the given plugin (if necessary) and returns its instance.
        //! Returns _nullptr_ on failure.
        QObject *getPluginById(const QString &identifier);

        //! Gets the singleton
        static CPluginManager *getInstance();

    protected:
        //! Constructor
        explicit CPluginManager(QObject *parent = nullptr);

    private:
        //! Looks (recursively) for all installed plugins
        void collectPlugins();

        //! Checks whether the provided metadata is valid
        //! \return The plugin identifier
        QString pluginIdentifier(const QJsonObject &metadata);

        QMultiMap<QString, QJsonObject> m_metadatas; //!< IID <-> metadata pairs
        QMap<QString, QString> m_paths; //!< identifier <-> file path pairs
        QMap<QString, QObject*> m_instances; //!< identifier <-> instance pairs

    };
}

#endif // CPLUGINMANAGER_H
